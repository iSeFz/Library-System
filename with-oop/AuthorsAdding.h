#ifndef AUTHORS_ADDING_H
#define AUTHORS_ADDING_H

#include "LibraryUtilities.h"
#include "Author.h"
#include "AuthorsAddingSecondaryIndex.h"

class AuthorsAdding
{
public:
    // Add a new author helper function
    void addAuthor(map<long long, short> &authorsPrimaryIndex, map<string, short> &authorsSecondaryIndex)
    {
        // Get author data from the user & store it in an Author object
        Author newAuthor;
        while (true)
        { // Check for the author id to verify that it is unique
            cout << "Enter author ID: ";
            cin.getline(newAuthor.authorID, 15);
            long long authID = LibraryUtilities::convertCharArrToLongLong(newAuthor.authorID);
            // Search for the given author id
            auto result = authorsPrimaryIndex.lower_bound(authID);
            // If the id is found, print a warning message indicating that it is a duplicate
            if (result != authorsPrimaryIndex.end() && result->first == authID)
                cerr << "\tAn Author with the same ID already exists\n";
            else // Otherwise, break the loop & continue data entry
                break;
        }
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
        authors.seekg(0, ios::beg);
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
            // Add the new author to index files
            addAuthorToPrimaryIndexFile(author.authorID, offset, authorsPrimaryIndex);
            AuthorsAddingSecondaryIndex::add(author, authorsSecondaryIndex);
            cout << "\tNew Author Added Successfully!\n";
            authors.close();
        }
        else
        { // Otherwise, if the avail list is NOT empty
            short prevOffset = header, deletedSize, differSize, tempOffset;
            bool fragmentation = false;
            char deleteChar;

            // Seek to the offset of the last deleted record
            authors.seekg(header, ios::beg);
            // First fit algorithm to find the first large enough record to insert the new record into
            while (true)
            {
                tempOffset = prevOffset; // Store the previous offset before overriding its value
                // Read the first character of the record to check if it's REALLY a deleted record or not
                authors.read((char *)&deleteChar, 1);
                // If it's NOT a deleted record, break the loop
                if (deleteChar != '*')
                { // Fake the differSize, in order to force insert the new record at the end of the file
                    differSize = -1;
                    header = -1;
                    // Seek to the beginning of file & update the header to be -1
                    authors.seekp(0, ios::beg);
                    authors.write((char *)&header, sizeof(header));
                    break;
                }
                // Otherwise, continue parsing the record
                authors.ignore(1);                                       // Ignore the delimiter '|'
                authors.read((char *)&prevOffset, sizeof(prevOffset));   // Read the offset of the previous deleted record
                authors.ignore(1);                                       // Ignore the delimiter '|'
                authors.read((char *)&deletedSize, sizeof(deletedSize)); // Read the size of the current deleted record

                // Calculate the difference in size between the deleted record & the new record to insert
                // In order to decide the exact position to insert the new record
                differSize = deletedSize - recordSize;

                // If the difference is positive, means the current deleted record is suitable to insert in
                // As the size of the new record is smaller than the deleted record
                if (differSize >= 0)
                { // If this is NOT the first deleted record
                    if (prevOffset != -1)
                    {
                        // Update the header to be the offset of the previous deleted record
                        authors.seekp(0, ios::beg);
                        authors.write((char *)&prevOffset, sizeof(prevOffset));
                    }
                    break;
                }

                // If this is the first deleted record & no other records before it satisfy the condition of size
                // Break the loop to insert at the end of the file and keep the header as it is
                if (prevOffset == -1)
                    break;

                // Otherwise, continue the loop
                authors.seekg(prevOffset, ios::beg);
            }

            // If the difference is negative, then the new record is bigger than the deleted one
            if (differSize < 0)
                // Insert at the end of the file and keep the header value as it is
                authors.seekp(0, ios::end);
            else
            { // Otherwise, the difference is positive, then the new record size is smaller than the deleted one
                // Seek to the deleted record offset to write the new record
                authors.seekp(tempOffset, ios::beg);
                // Add the difference in size to the current record size in order to apply internal fragmentation
                recordSize += differSize;
                // Flag to indicate that internal fragmentation is required
                // Check if the sizes are equal (meaning the differSize is zero), then do NOT apply fragmentation
                fragmentation = (differSize == 0) ? false : true;
            }

            // Write the record size of the new record
            authors.write((char *)&recordSize, sizeof(recordSize));
            short offset = authors.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            authors.write(author.authorID, idSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            authors.write(author.authorName, nameSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            authors.write(author.address, addSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);

            // Check for internal fragmentation
            if (fragmentation)
            { // Apply internal fragmentation and fill the rest of record space with telda '~'
                short iterator = authors.tellp();
                while (iterator < (tempOffset + recordSize - 2) && tempOffset != -1)
                {
                    authors.write("~", 1);
                    iterator++;
                }
                // Write the last delimiter after the telda to indicate the end of this record
                authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            }

            // Read the header of the file again to get the latest update
            authors.seekg(0, ios::beg);
            authors.read((char *)&header, sizeof(header));
            // Fix the avail list pointers to avoid corrupted pointers
            LibraryUtilities::fixAvailList(header, LibraryUtilities::authorsFile);
            // Add the new author to index files
            addAuthorToPrimaryIndexFile(author.authorID, offset, authorsPrimaryIndex);
            AuthorsAddingSecondaryIndex::add(author, authorsSecondaryIndex);
            cout << "\tNew Author Added Successfully!\n";
            authors.close();
        }
    }

    // Add the new author to the primary index file
    void addAuthorToPrimaryIndexFile(char authorID[], short &byteOffset, map<long long, short> &authorsPrimaryIndex)
    {
        // Convert the authorID from character array into long long
        long long authID = LibraryUtilities::convertCharArrToLongLong(authorID);

        // Insert the new record into the map to be automatically sorted by authorID
        authorsPrimaryIndex.insert({authID, byteOffset});

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markAuthorsPrimaryIndexFlag('0');
    }
};

#endif // AUTHORS_ADDING_H