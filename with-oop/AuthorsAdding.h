#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsAdding
{
public:
    // Add a new author helper function
    void addAuthor(map<long long, short> &authorsPrimaryIndex, map<string, short> &authorsSecondaryIndex)
    {
        // Get author data from the user & store it in an Author object
        Author newAuthor;
        cout << "Enter author ID: ";
        cin.getline(newAuthor.authorID, 15);
        cout << "Enter author Name: ";
        cin.getline(newAuthor.authorName, 30);
        cout << "Enter address: ";
        cin.getline(newAuthor.address, 30);
        // Add the new author to the data file
        addAuthorToDataFile(newAuthor, authorsPrimaryIndex, authorsSecondaryIndex);
    }

    // Add the new author to the main authors data file
    void addAuthorToDataFile(Author &author, map<long long, short> &authorsPrimaryIndex, map<string, short> &authorsSecondaryIndex)
    {
        short header, recordSize, idSize, nameSize, addSize;
        // Open the file in multiple modes
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::out | ios::app | ios::binary);

        // Read the header of the file
        authors.seekg(0);
        authors.read((char *)&header, sizeof(header));

        // Get the sizes of the id, name & address
        idSize = strlen(author.authorID);
        nameSize = strlen(author.authorName);
        addSize = strlen(author.address);

        // Calculate the total record length
        // 5 refers to 2 bytes for the record length indicator
        // + 3 bytes for 3 delimeters between fields
        recordSize = idSize + nameSize + addSize + 5;

        // If the avail list is empty, insert at the end of the file
        if (header == -1)
        {
            authors.seekp(0, ios::end); // Seek to the end of file
            authors.write((char *)&recordSize, sizeof(recordSize));
            short offset = authors.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            authors.write(author.authorID, idSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            authors.write(author.authorName, nameSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            authors.write(author.address, addSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            // Add the new author to the primary index file
            addAuthorToPrimaryIndexFile(author.authorID, offset, authorsPrimaryIndex);
            addAuthorToSecondaryIndexFile(author, authorsSecondaryIndex);
            cout << "\tNew Author Added Successfully!\n";
            authors.close();
        }
    }

    // Add the new author to the primary index file
    void addAuthorToPrimaryIndexFile(char authorID[], short byteOffset, map<long long, short> &authorsPrimaryIndex)
    {
        // Convert the authorID from character array into long long
        long long authID = LibraryUtilities::convertCharArrToLongLong(authorID);

        // Insert the new record into the map to be automatically sorted by authorID
        authorsPrimaryIndex.insert({authID, byteOffset});

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markAuthorsPrimaryIndexFlag('0');
    }

    void addAuthorToSecondaryIndexFile(Author author, map<string, short> &authorsSecondaryIndex)
    {
        long long authorId = LibraryUtilities::convertCharArrToLongLong(author.authorID);

        if (authorsSecondaryIndex.count(author.authorName) == 0)
        {
            cout << "Empty list\n";
            authorsSecondaryIndex[author.authorName] = addToInvertedList(authorId, -1); // RRN
            cout << "Added value: " << authorsSecondaryIndex[author.authorName] << "\n";
        }
        else
        {
            authorsSecondaryIndex[author.authorName] = addToInvertedList(authorId, authorsSecondaryIndex[author.authorName]);
        }
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markAuthorsSecondaryIndexFlag('0');
    }

    short addToInvertedList(long long authorId, short nextRecordPointer)
    {
        // Open the file in multiple modes
        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);

        short bestOffset = getBestOffsetInInvertedList();
        cout << "Best palce to insert: " << bestOffset << "\n";
        invertedList.seekp(bestOffset, ios::beg);
        if (invertedList.fail())
        {
            cout << "Failed to open the file.\n";
            return -1; // or handle the error appropriately
        }
        cout << "Writing to the inverted list file......\n";
        cout << "Author ID = " << authorId << "\n";
        cout << "nextRecordPointer = " << nextRecordPointer << "\n";
        // Write the Author ID & the next record pointer
        invertedList.write((char *)&authorId, sizeof(long long));
        invertedList.write((char *)&nextRecordPointer, sizeof(short));

        invertedList.close();

        // Return the RRN of the new record
        return bestOffset / (sizeof(long long) + sizeof(short)); // RRN
    }

    short getBestOffsetInInvertedList()
    {
        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::binary);

        invertedList.seekg(0, ios::end);
        int endOffset = invertedList.tellg();
        invertedList.seekg(0, ios::beg);

        while (invertedList)
        {
            long long ISBN;
            invertedList.read((char *)&ISBN, sizeof(long long));
            short nextRecordPointer;
            invertedList.read((char *)&nextRecordPointer, sizeof(short));
            if (nextRecordPointer == '#')
            {
                int returned = invertedList.tellg() - (sizeof(long long) + sizeof(short));
                invertedList.close();
                return returned;
            }
        }

        invertedList.close();
        return max(0, endOffset);
    }
};