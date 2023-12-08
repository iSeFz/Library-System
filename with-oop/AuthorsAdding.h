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
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::out | ios::binary);

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
            short currentOffset = header, prevOffset = -1;
            // First fit algorithm to find the first large enough record to insert the new record into
            while (currentOffset != -1)
            {
                short nextOffset, currentDeletedSize;
                // Seek to the offset of the last deleted record
                authors.seekg(currentOffset, ios::beg);

                // Parse the record to save its information
                authors.ignore(2); // Ignore the two characters '*' & the length delimiter '|'
                // Read the offset of the previous deleted record (next pointer)
                authors.read((char *)&nextOffset, sizeof(nextOffset));
                authors.ignore(1); // Ignore the delimiter '|'
                // Read the size of the current deleted record
                authors.read((char *)&currentDeletedSize, sizeof(currentDeletedSize));

                // If the size of the new record is smaller than the deleted record
                // Means the current deleted record is suitable to insert in
                if (currentDeletedSize >= recordSize)
                {
                    // Check for to see if there are any records point to the current record
                    // If there are no records that point to the current record
                    if (prevOffset == -1)
                    { // Update the header to point to the next pointer of the current record
                        authors.seekp(0, ios::beg);
                        authors.write((char *)&nextOffset, sizeof(short));
                    }
                    else
                    { // Otherwise, seek to the record that points to the current record
                        // Update its next pointer to be the current record's next pointer
                        authors.seekp(prevOffset, ios::beg);
                        authors.ignore(2); // Skip the two characters '*' & the length delimiter '|'
                        authors.write((char *)&nextOffset, sizeof(short));
                    }

                    // Seek to the deleted record offset to write the new record
                    authors.seekp(currentOffset, ios::beg);
                    // Write the deleted record size to apply internal fragmentation afterward
                    authors.write((char *)&currentDeletedSize, sizeof(currentDeletedSize));
                    short PKOffset = authors.tellp(); // Store the byteoffset of the new record
                    // Write the rest of record fields separated by delimeters
                    authors.write(author.authorID, idSize);
                    authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
                    authors.write(author.authorName, nameSize);
                    authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
                    authors.write(author.address, addSize);
                    authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);

                    // Apply internal fragmentation and fill the rest of record space with telda '~'
                    short iterator = authors.tellp();
                    while (iterator++ < (currentOffset + currentDeletedSize - 1))
                        authors.write("~", 1);
                    // Write the last delimiter after the telda to indicate the end of this record
                    authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);

                    // Add the new author to index files
                    addAuthorToPrimaryIndexFile(author.authorID, PKOffset, authorsPrimaryIndex);
                    AuthorsAddingSecondaryIndex::add(author, authorsSecondaryIndex);
                    cout << "\tNew Author Added Successfully!\n";
                    authors.close();
                    return;
                }
                else
                { // Otherwise, continue the search for a suitable deleted record
                    // Update the previous pointer to be the current record offset
                    prevOffset = currentOffset;
                    // Update the current record offset to be the next pointer to continue searching
                    currentOffset = nextOffset;
                }
            }

            // If there were no records with suitable size to fit the new record, append at the end of the file
            authors.seekp(0, ios::end); // Seek to the end of file
            authors.write((char *)&recordSize, sizeof(recordSize));
            short PKOffset = authors.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            authors.write(author.authorID, idSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            authors.write(author.authorName, nameSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            authors.write(author.address, addSize);
            authors.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            // Add the new author to index files
            addAuthorToPrimaryIndexFile(author.authorID, PKOffset, authorsPrimaryIndex);
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