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
    void addAuthor(map<long long, short> &authorsPrimaryIndex, map<long long, short> &authorsSecondaryIndex)
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
    void addAuthorToDataFile(Author &author, map<long long, short> &authorsPrimaryIndex, map<long long, short> &authorsSecondaryIndex)
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

    void addAuthorToSecondaryIndexFile(Author author, map<long long, short> &authorsSecondaryIndex) {}

    void addAuthorToSecondaryIndexLinkedListFile(char authorName[], short nextRecordPointer) {}
};