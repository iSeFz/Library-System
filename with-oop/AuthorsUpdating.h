#ifndef AUTHORS_UPDATING_H
#define AUTHORS_UPDATING_H

#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsUpdating
{
public:
    // Update author name using author ID
    void updateAuthorName(map<long long, short> &authorsPrimaryIndex,map<string, short> &authorsSecondaryIndex) {
        int authorID;
        int offset;
        char newName[30];
        cout << "Enter author ID: ";
        cin >> authorID;
        cout << "Enter new author name: ";
        cin.ignore();
        cin.getline(newName, 30);
        // searching for the record which will be updated in runtime
        auto id = authorsPrimaryIndex.lower_bound(authorID);
        if (id != authorsPrimaryIndex.end() && id->first == authorID)
        {
            offset = id->second;
            updateAuthorNameInDataFile(newName, offset,authorsSecondaryIndex);
        }
    }

    // applying the updates in the authors data file
    void updateAuthorNameInDataFile(char newAuthorName[30], int byteOffset,map<string, short> &authorsSecondaryIndex)
    {
        fstream dataFile(LibraryUtilities::authorsFile, ios::in | ios::out | ios::binary);
        // reaching desired record
        dataFile.seekg(byteOffset, ios::beg);
        // skipping the length indicator(2 bytes) and ID field(15 bytes or finding |)
        dataFile.ignore(17, '|');

        // storing the length of the old name (before updating) to not exceed the allocated size
        char oldName[30];
        dataFile.getline(oldName, 30, '|');
        int maxSize = strlen(oldName);

        // reaching the name field beginning
        dataFile.seekg(-(maxSize + 1), ios::cur);

        // case 1 --> length of new name < length of old name
        if (strlen(newAuthorName) < maxSize)
        {
            newAuthorName[strlen(newAuthorName)] = '\0';

            for (int i = strlen(newAuthorName)+1; i < maxSize; i++)
            {
                newAuthorName[i] = '_';
            }
            dataFile.write(newAuthorName, maxSize);
        }
            // case 2 & 3  --> length of new name == length of old name,
            // length of new name > length of old name
        else
        {
            dataFile.write(newAuthorName, maxSize);
        }

        dataFile.close();
        updateAuthorNameInSecondaryIndex(oldName,newAuthorName,authorsSecondaryIndex);
        cout << "\tAuthor name is updated successfully\n";
    }
    // applying the updates in the authors secondary index
    void updateAuthorNameInSecondaryIndex(char oldAuthorName[30], char newAuthorName[30], map<string, short> &authorsSecondaryIndex) {

        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);
        // checking if the inverted list file isn't opened
        if (invertedList.fail())
        {
            cout << "Failed to open the file.\n";
        }
        else {
            // step 1 in updating --> modify RRN that is related to old author name, RRN=previousRRN in inverted list
//______________________________________________________________________________________________________________________
            // storing RRN value pointed by old name secondary key
            short RRN = authorsSecondaryIndex[oldAuthorName];
            // storing  record size in invertedListFile (fixed size --> long long + short)
            int recordSize = sizeof(long long) + sizeof(short);
            // reaching last added record (in old author name linked list)
            invertedList.seekg(RRN * recordSize, ios::beg);
            // reaching the beginning of pointer_to_previous field
            invertedList.ignore(sizeof(long long));
            // storing previous RRN
            short previousRRN;
            invertedList.read((char *) &previousRRN, sizeof(short));

            if(previousRRN=='-1')
            {
                authorsSecondaryIndex[oldAuthorName]=-1;
            }
            else{
                authorsSecondaryIndex[oldAuthorName] = previousRRN;
            }
//______________________________________________________________________________________________________________________
            // step 2 in updating --> handling newAuthorName
//______________________________________________________________________________________________________________________
            // case 1 in step 2 --> if newAuthorName does not exist in secondary index , then add it to secondary key with its RRN
            // (RRN of primary key (authorID) related to the updated authorName)
            if (authorsSecondaryIndex.count(newAuthorName) == 0) {
                authorsSecondaryIndex[newAuthorName] = RRN;

                invertedList.seekg(RRN*recordSize,ios::beg);
                invertedList.ignore(sizeof(long long));

                // pointer_to_previous of that primary key (authorID)= -1 (first entry)
                short firstEntry=-1;
                invertedList.write((char *)&firstEntry, sizeof(short));


            }
            // case 2 in step 2 --> if newAuthorName exists in secondary index , update:
            // secondary key's RRN=RRN of primary key (authorID) related to updated authorName
            // pointer_to_previous of that primary key (authorID) =  secondary key's RRN before updating
            else {
                short RRN_before_updating = authorsSecondaryIndex[newAuthorName];
                authorsSecondaryIndex[newAuthorName] = RRN;
                invertedList.seekg(RRN*recordSize,ios::beg);
                invertedList.ignore(sizeof(long long));
                invertedList.write((char *) &RRN_before_updating, sizeof(RRN_before_updating));


            }
        }
//______________________________________________________________________________________________________________________
        invertedList.close();

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markAuthorsSecondaryIndexFlag('0');

    }
};

#endif // AUTHORS_UPDATING_H