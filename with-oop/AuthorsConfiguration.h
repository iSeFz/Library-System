#ifndef AUTHORS_CONF_H
#define AUTHORS_CONF_H

#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsConfiguration
{
public:
    // Create the author primary index file
    void createAuthorPrimaryIndex(map<long long, short> &authorsPrimaryIndex)
    {
        // Open the data file in input mode
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);

        // Skip the header value
        authors.seekg(2, ios::beg);

        // Create the index file
        while (authors)
        {
            int recordOffset = authors.tellg();
            char firstChar;
            authors.read((char *)&firstChar, sizeof(char));
            // If the record is deleted, skip to the next record
            if (firstChar == '*')
            {
                short previousRecord, recordSize;
                authors.ignore(1);                                    // Ignore the delimiter
                authors.read((char *)&previousRecord, sizeof(short)); // Read the previous record
                authors.ignore(1);                                    // Ignore the delimiter
                authors.read((char *)&recordSize, sizeof(short));     // Read the record size
                authors.seekg(recordOffset + recordSize, ios::beg);   // Jump to the next record
                continue;
            }
            else // Return the cursor back one character that was read
                authors.seekg(-1, ios::cur);

            // Read the record size to be able to jump to the end of the record
            short recordSize;
            authors.read((char *)&recordSize, sizeof(short));
            // Store the byte offset of the current record
            short tempOffset = authors.tellg();
            if (tempOffset == -1) // If the file has no records, just the header
                break;

            // Read the value of author id
            char authorID[15];
            authors.read((char *)&authorID, sizeof(authorID));

            // Convert the authorID from character array into long long
            long long tempID = LibraryUtilities::convertCharArrToLongLong(authorID);

            // Jump to the next record, minus 17 to compensate the read of the record size itself (2 bytes)
            // And the rest (15 bytes) to compensate the dummy read of authorID character array
            authors.seekg(recordSize - 17, ios::cur);
            // Insert the record into the map to be sorted in memory by the author id
            authorsPrimaryIndex.insert({tempID, tempOffset});
        }
        authors.close();

        // Write the status flag at the beginning of the file
        LibraryUtilities::markAuthorsPrimaryIndexFlag('0');

        // Save the index file to disk
        saveAuthorPrimaryIndex(authorsPrimaryIndex);
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveAuthorPrimaryIndex(map<long long, short> &authorsPrimaryIndex)
    {
        // Open the file in multiple modes
        fstream authorPrimIdx(LibraryUtilities::authorsPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        char isAuthorPrimIdxUpToDate;
        authorPrimIdx.seekg(0, ios::beg);
        authorPrimIdx.read((char *)&isAuthorPrimIdxUpToDate, sizeof(char));

        // If the file is already up to date, do not write & exit
        if (isAuthorPrimIdxUpToDate == '1')
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record : authorsPrimaryIndex)
        {
            authorPrimIdx.write((char *)&record.first, sizeof(long long)); // Write the authorID
            authorPrimIdx.write((char *)&record.second, sizeof(short));    // Write the byte offset
        }
        authorPrimIdx.close();

        // Update the file status to be up to date
        LibraryUtilities::markAuthorsPrimaryIndexFlag('1');
    }

    // Load authors primary index file into memory
    void loadAuthorPrimaryIndex(map<long long, short> &authorsPrimaryIndex)
    {
        // Open the index file in input mode
        fstream authorPrimIdx(LibraryUtilities::authorsPrimaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        authorPrimIdx.seekg(0, ios::end);
        short endOffset = authorPrimIdx.tellg();

        // Check the status field
        char isAuthorPrimIdxUpToDate;
        authorPrimIdx.seekg(0, ios::beg);
        authorPrimIdx.read((char *)&isAuthorPrimIdxUpToDate, sizeof(char));

        // If the file is outdated, recreate it
        if (isAuthorPrimIdxUpToDate != '1')
        {
            createAuthorPrimaryIndex(authorsPrimaryIndex);
            return;
        }

        // Insert all records into the authors primary index map
        while (authorPrimIdx)
        { // If reached the end of file, exit
            if (authorPrimIdx.tellg() == endOffset)
                break;
            long long tempID;
            short tempOffset;
            authorPrimIdx.read((char *)&tempID, sizeof(long long));
            authorPrimIdx.read((char *)&tempOffset, sizeof(short));
            // Insert the record into the map to be sorted in memory by the author id
            authorsPrimaryIndex.insert({tempID, tempOffset});
        }
        authorPrimIdx.close();
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveAuthorsSecondaryIndex(map<string, short> &authorsSecondaryIndex)
    {
        // Open the file in multiple modes
        fstream authorsSecondaryIndexFstream(LibraryUtilities::authorsSecondaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        char isAuthorsSecondaryIdxUpToDate;
        authorsSecondaryIndexFstream.seekg(0, ios::beg);
        authorsSecondaryIndexFstream.read((char *)&isAuthorsSecondaryIdxUpToDate, sizeof(char));

        // If the file is already up to date, do not write & exit
        if (isAuthorsSecondaryIdxUpToDate == '1')
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record : authorsSecondaryIndex)
        {
            authorsSecondaryIndexFstream.write(record.first.c_str(), sizeof(char[30])); // Write the author ID
            authorsSecondaryIndexFstream.write((char *)&record.second, sizeof(short));  // Write the first record pointer (RRN) in the inverted list file
        }
        authorsSecondaryIndexFstream.close();

        // Update the file status to be up to date
        LibraryUtilities::markAuthorsSecondaryIndexFlag('1');
    }

    // Load authors secondary index file into memory
    void loadAuthorsSecondaryIndex(map<string, short> &authorsSecondaryIndex)
    {
        // Open the index file in input mode
        fstream authorsSecondaryIndexFileFstream(LibraryUtilities::authorsSecondaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        authorsSecondaryIndexFileFstream.seekg(0, ios::end);
        short endOffset = authorsSecondaryIndexFileFstream.tellg();

        // Check the status field
        char isAuthorsSecIdxUpToDate;
        authorsSecondaryIndexFileFstream.seekg(0, ios::beg);
        authorsSecondaryIndexFileFstream.read((char *)&isAuthorsSecIdxUpToDate, sizeof(char));

        // If the file is outdated, recreate it
        if (isAuthorsSecIdxUpToDate != '1')
        {
            createAuthorSecondaryIndex(authorsSecondaryIndex); // Create the secondary index
            return;
        }

        // Insert all records into the authors secondary index map
        while (authorsSecondaryIndexFileFstream)
        { // If reached the end of file, exit
            if (authorsSecondaryIndexFileFstream.tellg() == endOffset)
                break;
            char tempAuthorName[30];
            short tempRecordPointer;
            authorsSecondaryIndexFileFstream.read((char *)&tempAuthorName, sizeof(char[30]));
            authorsSecondaryIndexFileFstream.read((char *)&tempRecordPointer, sizeof(short));
            // Insert the record into the map to be sorted in memory by the author name
            authorsSecondaryIndex.insert({tempAuthorName, tempRecordPointer});
        }
        authorsSecondaryIndexFileFstream.close();
    }

    // Create the author primary index file
    void createAuthorSecondaryIndex(map<string, short> &authorsSecondaryIndex)
    {
        // clear the inverted list file
        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::out | ios::binary);
        invertedList.close();
        
        // Open the data file in input mode
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);

        // Skip the header value
        authors.seekg(2, ios::beg);

        // Create the index file
        while (authors)
        {
            int recordOffset = authors.tellg();
            char firstChar;
            authors.read((char *)&firstChar, sizeof(char));
            // If the record is deleted, skip to the next record
            if (firstChar == '*')
            {
                short previousRecord, recordSize;
                authors.ignore(1);                                    // Ignore the delimiter
                authors.read((char *)&previousRecord, sizeof(short)); // Read the previous record
                authors.ignore(1);                                    // Ignore the delimiter
                authors.read((char *)&recordSize, sizeof(short));     // Read the record size
                authors.seekg(recordOffset + recordSize, ios::beg);   // Jump to the next record
                continue;
            }
            else // Return the cursor back one character that was read
                authors.seekg(-1, ios::cur);

            // Read the record size to be able to jump to the end of the record
            short recordSize;
            authors.read((char *)&recordSize, sizeof(short));
            // Store the byte offset of the current record
            short tempOffset = authors.tellg();
            if (tempOffset == -1) // If the file has no records, just the header
                break;

            Author author;
            authors.getline(author.authorID, 15, '|');
            authors.getline(author.authorName, 30, '|');
            authors.getline(author.address, 30, '|');

            if (authors.peek() == '|')
            {
                authors.seekg(1, ios::cur);
            }

            // Insert the record into the map to be sorted in memory by the author id
            AuthorsAddingSecondaryIndex::add(author, authorsSecondaryIndex);
        }
        authors.close();

        // Write the status flag at the beginning of the file
        LibraryUtilities::markAuthorsSecondaryIndexFlag('0');

        // Save the index file to disk
        saveAuthorsSecondaryIndex(authorsSecondaryIndex);
    }

    // Print author using author ID
    void printAuthor(map<long long, short> &authorsPrimaryIndex)
    {
        long long authorID;
        cout << "Enter author ID: ";
        cin >> authorID;
        auto id = authorsPrimaryIndex.lower_bound(authorID);
        if (id != authorsPrimaryIndex.end() && id->first == authorID)
        {
            int offset = id->second;
            fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);
            authors.seekg(offset, ios::beg);
            Author author;
            authors.getline(author.authorID, 15, '|');
            authors.getline(author.authorName, 30, '|');
            authors.getline(author.address, 30, '|');
            cout << "\tAuthor #" << author.authorID << " Data\n";
            cout << "Author Name: " << author.authorName << "\n";
            cout << "Author Address: " << author.address << "\n";
        }
        else
            cout << "\tAuthor does NOT exist\n";
        cin.ignore(); // To keep input stream clean
    }
};

#endif // AUTHORS_CONF_H