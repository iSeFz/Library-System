#ifndef BOOKS_CONF_H
#define BOOKS_CONF_H

#include "LibraryUtilities.h"
#include "Book.h"

class BooksConfiguration
{
public:
    // Create the book primary index file
    void createBookPrimaryIndex(map<long long, short> &booksPrimaryIndex)
    {
        // Open the data file in input mode
        fstream books(LibraryUtilities::booksFile, ios::in | ios::binary);

        // Skip the header value
        books.seekg(2, ios::beg);

        // Create the index file
        while (books)
        {
            int recordOffset = books.tellg();
            char firstChar;
            books.read((char *)&firstChar, sizeof(char));
            // if the record is deleted
            if (firstChar == '*')
            {
                short previousRecord, recordSize;
                books.ignore(1);                                    // ignore the delimiter
                books.read((char *)&previousRecord, sizeof(short)); // read the previous record
                books.ignore(1);                                    // ignore the delimiter
                books.read((char *)&recordSize, sizeof(short));     // read the record size
                books.seekg(recordOffset + recordSize, ios::beg);   // jump to the next record
                continue;
            }
            else // Return the cursor back one character that was read
                books.seekg(-1, ios::cur);

            // Read the record size to be able to jump to the end of the record
            short recordSize;
            books.read((char *)&recordSize, sizeof(short));
            // Store the byte offset of the current record
            short tempOffset = books.tellg();
            if (tempOffset == -1) // If the file has no records, just the header
                break;

            // Read the value of book isbn
            char ISBN[15];
            books.read((char *)&ISBN, sizeof(ISBN));

            // Convert the ISBN from character array into long long
            long long tempISBN = LibraryUtilities::convertCharArrToLongLong(ISBN);

            // Jump to the next record, minus 17 to compensate the read of the record size itself (2 bytes)
            // And the rest (15 bytes) to compensate the dummy read of ISBN character array
            books.seekg(recordSize - 17, ios::cur);
            // Insert the record into the map to be sorted in memory by the author id
            booksPrimaryIndex.insert({tempISBN, tempOffset});
        }
        books.close();

        // Write the status flag at the beginning of the file
        LibraryUtilities::markBooksPrimaryIndexFlag('0');

        // Save the index file to disk
        saveBookPrimaryIndex(booksPrimaryIndex);
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveBookPrimaryIndex(map<long long, short> &booksPrimaryIndex)
    {
        // Open the file in multiple modes
        fstream bookPrimIdx(LibraryUtilities::booksPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        char isBookPrimIdxUpToDate;
        bookPrimIdx.seekg(0, ios::beg);
        bookPrimIdx.read((char *)&isBookPrimIdxUpToDate, sizeof(char));

        // If the file is already up to date, do not write & exit
        if (isBookPrimIdxUpToDate == '1')
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record : booksPrimaryIndex)
        {
            bookPrimIdx.write((char *)&record.first, sizeof(long long)); // Write the ISBN
            bookPrimIdx.write((char *)&record.second, sizeof(short));    // Write the byte offset
        }
        bookPrimIdx.close();

        // Update the file status to be up to date
        LibraryUtilities::markBooksPrimaryIndexFlag('1');
    }

    // Load books primary index file into memory
    void loadBookPrimaryIndex(map<long long, short> &booksPrimaryIndex)
    {
        // Open the index file in input mode
        fstream bookPrimIdx(LibraryUtilities::booksPrimaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        bookPrimIdx.seekg(0, ios::end);
        short endOffset = bookPrimIdx.tellg();

        // Check the status field
        char isBookPrimIdxUpToDate;
        bookPrimIdx.seekg(0, ios::beg);
        bookPrimIdx.read((char *)&isBookPrimIdxUpToDate, sizeof(char));

        // If the file is outdated, recreate it
        if (isBookPrimIdxUpToDate != '1')
        {
            createBookPrimaryIndex(booksPrimaryIndex);
            return;
        }

        // Insert all records into the books primary index map
        while (bookPrimIdx)
        { // If reached the end of file, exit
            if (bookPrimIdx.tellg() == endOffset)
                break;
            long long tempISBN;
            short tempOffset;
            bookPrimIdx.read((char *)&tempISBN, sizeof(long long));
            bookPrimIdx.read((char *)&tempOffset, sizeof(short));
            // Insert the record into the map to be sorted in memory by the book isbn
            booksPrimaryIndex.insert({tempISBN, tempOffset});
        }
        bookPrimIdx.close();
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveBookSecondaryIndex(map<long long, short> &booksSecondaryIndex)
    {
        // Open the file in multiple modes
        fstream booksSecondaryIndexFstream(LibraryUtilities::booksSecondaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        char isBookSecondaryIdxUpToDate;
        booksSecondaryIndexFstream.seekg(0, ios::beg);
        booksSecondaryIndexFstream.read((char *)&isBookSecondaryIdxUpToDate, sizeof(char));

        // If the file is already up to date, do not write & exit
        if (isBookSecondaryIdxUpToDate == '1')
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record : booksSecondaryIndex)
        {
            booksSecondaryIndexFstream.write((char *)&record.first, sizeof(long long)); // Write the author ID
            booksSecondaryIndexFstream.write((char *)&record.second, sizeof(short));    // Write the first record pointer (RRN) in the inverted list file
        }
        booksSecondaryIndexFstream.close();

        // Update the file status to be up to date
        LibraryUtilities::markBooksSecondaryIndexFlag('1');
    }

    // Load books secondary index file into memory
    void loadBookSecondaryIndex(map<long long, short> &booksSecondaryIndex)
    {
        // Open the index file in input mode
        fstream booksSecondaryIndexFileFstream(LibraryUtilities::booksSecondaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        booksSecondaryIndexFileFstream.seekg(0, ios::end);
        short endOffset = booksSecondaryIndexFileFstream.tellg();

        // Check the status field
        char isBookPrimIdxUpToDate;
        booksSecondaryIndexFileFstream.seekg(0, ios::beg);
        booksSecondaryIndexFileFstream.read((char *)&isBookPrimIdxUpToDate, sizeof(char));

        // If the file is outdated, recreate it
        if (isBookPrimIdxUpToDate != '1')
        {
            booksSecondaryIndexFileFstream.seekp(0, ios::beg);
            char updatedSymbol = '0';
            booksSecondaryIndexFileFstream.write((char *)&updatedSymbol, sizeof(char));
            return;
        }

        // Insert all records into the books primary index map
        while (booksSecondaryIndexFileFstream)
        { // If reached the end of file, exit
            if (booksSecondaryIndexFileFstream.tellg() == endOffset)
                break;
            long long tempAuthorId;
            short tempRecordPointer;
            booksSecondaryIndexFileFstream.read((char *)&tempAuthorId, sizeof(long long));
            booksSecondaryIndexFileFstream.read((char *)&tempRecordPointer, sizeof(short));
            // Insert the record into the map to be sorted in memory by the book isbn
            booksSecondaryIndex.insert({tempAuthorId, tempRecordPointer});
        }
        booksSecondaryIndexFileFstream.close();
    }

    // Print book using ISBN
    void printBook(map<long long, short> &booksPrimaryIndex)
    {
        int ISBN;
        cout << "Enter Book ISBN: ";
        cin >> ISBN;
        auto isbn = booksPrimaryIndex.lower_bound(ISBN);
        if (isbn != booksPrimaryIndex.end() && isbn->first == ISBN)
        {
            int offset = isbn->second;
            fstream books(LibraryUtilities::booksFile, ios::in | ios::binary);
            books.seekg(offset, ios::beg);
            Book book;
            books.getline(book.ISBN, 15, '|');
            books.getline(book.bookTitle, 30, '|');
            books.getline(book.authorID, 15, '|');
            cout << "\tBook with ISBN #" << book.ISBN << " Data\n";
            cout << "Book Title: " << book.bookTitle << "\n";
            cout << "Author ID: " << book.authorID << "\n";
        }
        else
            cout << "\tBook does not exist\n";
        cin.ignore(); // To keep input stream clean
    }
};

#endif // BOOKS_CONF_H