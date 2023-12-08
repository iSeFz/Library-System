#ifndef AUTHORS_UPDATING_H
#define AUTHORS_UPDATING_H

#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsUpdating
{
public:
    // Update author name using author ID
    void updateAuthorName(map<long long, short> &authorsPrimaryIndex, map<string, short> &authorsSecondaryIndex)
    {
        long long authorID;
        short offset;
        char newName[30];
        cout << "Enter author ID: ";
        cin >> authorID;
        cin.ignore();
        // Searching for the record which will be updated in runtime
        auto id = authorsPrimaryIndex.lower_bound(authorID);
        if (id != authorsPrimaryIndex.end() && id->first == authorID)
        {
            offset = id->second;
            cout << "Enter new author name: ";
            cin.getline(newName, 30);
            updateAuthorNameInDataFile(authorID, newName, offset, authorsSecondaryIndex);
            cout << "\tAuthor name updated successfully!\n";
        }
        else // If the author is not found, print a warning message
            cerr << "\tAuthor does NOT exist!\n";
    }

    // applying the updates in the authors data file
    void updateAuthorNameInDataFile(long long authorID, char newAuthorName[30], int byteOffset, map<string, short> &authorsSecondaryIndex)
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

            for (int i = strlen(newAuthorName) + 1; i < maxSize; i++)
                newAuthorName[i] = '_';

            dataFile.write(newAuthorName, maxSize);
        }
        // case 2 & 3  --> length of new name == length of old name,
        // length of new name > length of old name
        else
            dataFile.write(newAuthorName, maxSize);

        dataFile.close();
        // Update the secondary index & inverted list files
        short targetRecordRRN = updateAuthorNameInSecondaryIndex(authorID, authorsSecondaryIndex[oldName], newAuthorName, authorsSecondaryIndex);

        // Check if the name is unique and is not duplicate
        if (authorsSecondaryIndex.count(newAuthorName) == 0)
        {
            // Insert into the secondary index with pointer = -1 because it does not point to any primary key
            authorsSecondaryIndex[newAuthorName] = targetRecordRRN;
        }
        else
        {
            fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);
            // reaching the record which will be updated in runtime
            invertedList.seekg(targetRecordRRN * (sizeof(long long) + sizeof(short)), ios::beg);
            // reaching the beginning of pointer_to_previous field
            invertedList.ignore(sizeof(long long));
            // storing previous RRN
            invertedList.write((char *)&authorsSecondaryIndex[newAuthorName], sizeof(short));
            invertedList.close();
            authorsSecondaryIndex[newAuthorName] = targetRecordRRN;
        }
    }

    // applying the updates in the authors secondary index
    short updateAuthorNameInSecondaryIndex(long long authorID, int firstPosition, char newAuthorName[30], map<string, short> &authorsSecondaryIndex)
    {
        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);
        short previousRecord = -1;
        int recordSize = sizeof(long long) + sizeof(short);
        short currentRecord = firstPosition;
        invertedList.seekg(currentRecord * recordSize, ios::beg);
        while (true)
        {
            long long currentPrimaryKey;
            invertedList.read((char *)&currentPrimaryKey, sizeof(long long));
            if (currentPrimaryKey == authorID)
            {
                short nextPointer;
                invertedList.read((char *)&nextPointer, sizeof(short));
                // update the previous record to point to the next record of the current record
                invertedList.seekp(previousRecord * recordSize, ios::beg);
                invertedList.ignore(sizeof(long long));
                invertedList.write((char *)&nextPointer, sizeof(short));
                invertedList.close();
                return currentRecord; // return RRN of the updated record
            }
            else
            {
                previousRecord = currentRecord;
                invertedList.read((char *)&currentRecord, sizeof(short)); // make the current to bve the next record
                invertedList.seekg(currentRecord * recordSize, ios::beg);
            }
        }
    }
};

#endif // AUTHORS_UPDATING_H