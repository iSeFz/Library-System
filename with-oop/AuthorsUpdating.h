#ifndef AUTHORS_UPDATING_H
#define AUTHORS_UPDATING_H

#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsUpdating
{
public:
    // Update author name using author ID
    void updateAuthorName(map<long long, short> &authorsPrimaryIndex) {
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
            updateAuthorNameInDataFile(newName, offset);
        }
    }

    void updateAuthorNameInDataFile(char newAuthorName[30], int byteOffset)
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
        cout << "\tAuthor name is updated successfully\n";
    }

    void updateAuthorNameInSecondaryIndex(char oldAuthorName[30], char newAuthorName[30], map<string, short> &authorsSecondaryIndex) {}
};

#endif // AUTHORS_UPDATING_H