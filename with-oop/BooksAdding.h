#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Book.h"

class BooksAdding
{
public:
    // Add a new book helper function
    void addBook(map<long long, short> &booksPrimaryIndex, map<long long, short> &booksSecondaryIndex, map<long long, short> &authorsPrimaryIndex)
    {
        // Get book data from the user & store it in a Book object
        Book newBook;
        while (true)
        { // Check for the ISBN to verify that it is unique
            cout << "Enter book ISBN: ";
            cin.getline(newBook.ISBN, 15);
            short isbn = LibraryUtilities::convertCharArrToLongLong(newBook.ISBN);
            // Search for the given ISBN
            auto result = booksPrimaryIndex.lower_bound(isbn);
            // If the isbn is found, print a warning message indicating that it is a duplicate
            if (result != booksPrimaryIndex.end() && result->first == isbn)
                cerr << "\tA Book with the same ISBN already exists\n";
            else // Otherwise, break the loop & continue data entry
                break;
        }
        cout << "Enter book title: ";
        cin.getline(newBook.bookTitle, 30);
        while (true)
        { // Check for the author id to verify that it exists
            cout << "Enter author ID: ";
            cin.getline(newBook.authorID, 15);
            long long authID = LibraryUtilities::convertCharArrToLongLong(newBook.authorID);
            // Search for the given author id
            auto result = authorsPrimaryIndex.lower_bound(authID);
            // If the id is found, break the loop & continue data entry
            if (result != authorsPrimaryIndex.end() && result->first == authID)
                break;
            // Otherwise, print a warning message indicating that it is not found
            cerr << "\tAuthor with ID#" << authID << " is NOT found!\n";
        }
        // Add the new book to the data file
        addBookToDataFile(newBook, booksPrimaryIndex, booksSecondaryIndex);
    }

    // Add the new book to the main books data file
    void addBookToDataFile(Book &book, map<long long, short> &booksPrimaryIndex, map<long long, short> &booksSecondaryIndex)
    {
        short header, recordSize, isbnSize, titleSize, authIdSize;
        // Open the file in multiple modes
        fstream books(LibraryUtilities::booksFile, ios::in | ios::out | ios::binary);

        // Read the header of the file
        books.seekg(0, ios::beg);
        books.read((char *)&header, sizeof(header));

        // Get the sizes of the isbn, title & author id
        isbnSize = strlen(book.ISBN);
        titleSize = strlen(book.bookTitle);
        authIdSize = strlen(book.authorID);

        // Calculate the total record length
        // 5 refers to 2 bytes for the record length indicator
        // + 3 bytes for 3 delimeters between fields
        recordSize = isbnSize + titleSize + authIdSize + 5;

        // If the avail list is empty, insert at the end of the file
        if (header == -1)
        {
            books.seekp(0, ios::end); // Seek to the end of file
            books.write((char *)&recordSize, sizeof(recordSize));
            short offset = books.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            books.write(book.ISBN, isbnSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            books.write(book.bookTitle, titleSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            books.write(book.authorID, authIdSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            // Add the new book to index files
            addBookToPrimaryIndexFile(book.ISBN, offset, booksPrimaryIndex);
            addBookToSecondaryIndexFile(book, booksSecondaryIndex);
            cout << "\tNew Book Added Successfully!\n";
            books.close();
        }
        else
        { // Otherwise, if the avail list is NOT empty
            short prevOffset = header, deletedSize, differSize, tempOffset;
            bool fragmentation = false;
            char deleteChar;

            // Seek to the offset of the last deleted record
            books.seekg(header, ios::beg);
            // First fit algorithm to find the first large enough record to insert the new record into
            while (true)
            {
                tempOffset = prevOffset; // Store the previous offset before overriding its value
                // Read the first character of the record to check if it's REALLY a deleted record or not
                books.read((char *)&deleteChar, 1);
                // If it's NOT a deleted record, break the loop
                if (deleteChar != '*')
                { // Fake the differSize, in order to force insert the new record at the end of the file
                    differSize = -1;
                    header = -1;
                    // Seek to the beginning of file & update the header to be -1
                    books.seekp(0, ios::beg);
                    books.write((char *)&header, sizeof(header));
                    break;
                }
                // Otherwise, continue parsing the record
                books.ignore(1);                                       // Ignore the delimiter '|'
                books.read((char *)&prevOffset, sizeof(prevOffset));   // Read the offset of the previous deleted record
                books.ignore(1);                                       // Ignore the delimiter '|'
                books.read((char *)&deletedSize, sizeof(deletedSize)); // Read the size of the current deleted record

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
                        books.seekp(0, ios::beg);
                        books.write((char *)&prevOffset, sizeof(prevOffset));
                    }
                    break;
                }

                // If this is the first deleted record & no other records before it satisfy the condition of size
                // Break the loop to insert at the end of the file and keep the header as it is
                if (prevOffset == -1)
                    break;

                // Otherwise, continue the loop
                books.seekg(prevOffset, ios::beg);
            }

            // If the difference is negative, then the new record is bigger than the deleted one
            if (differSize < 0)
                // Insert at the end of the file and keep the header value as it is
                books.seekp(0, ios::end);
            else
            { // Otherwise, the difference is positive, then the new record size is smaller than the deleted one
                // Seek to the deleted record offset to write the new record
                books.seekp(tempOffset, ios::beg);
                // Add the difference in size to the current record size in order to apply internal fragmentation
                recordSize += differSize;
                // Flag to indicate that internal fragmentation is required
                // Check if the sizes are equal (meaning the differSize is zero), then do NOT apply fragmentation
                fragmentation = (differSize == 0) ? false : true;
            }

            // Write the record size of the new record
            books.write((char *)&recordSize, sizeof(recordSize));
            short offset = books.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            books.write(book.ISBN, isbnSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            books.write(book.bookTitle, titleSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            books.write(book.authorID, authIdSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);

            // Check for internal fragmentation
            if (fragmentation)
            { // Apply internal fragmentation and fill the rest of record space with telda '~'
                short iterator = books.tellp();
                while (iterator < (tempOffset + recordSize - 2) && tempOffset != -1)
                {
                    books.write("~", 1);
                    iterator++;
                }
                // Write the last delimiter after the telda to indicate the end of this record
                books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            }

            // Read the header of the file again to get the latest update
            books.seekg(0, ios::beg);
            books.read((char *)&header, sizeof(header));
            // Fix the avail list pointers to avoid corrupted pointers
            LibraryUtilities::fixAvailList(header, LibraryUtilities::booksFile);
            // Add the new book to index files
            addBookToPrimaryIndexFile(book.ISBN, offset, booksPrimaryIndex);
            addBookToSecondaryIndexFile(book, booksSecondaryIndex);
            cout << "\tNew Book Added Successfully!\n";
            books.close();
        }
    }

    // Add the new book to the primary index file
    void addBookToPrimaryIndexFile(char ISBN[], short byteOffset, map<long long, short> &booksPrimaryIndex)
    {
        // Convert the ISBN from character array into long long
        long long bookISBN = LibraryUtilities::convertCharArrToLongLong(ISBN);

        // Insert the new record into the map to be automatically sorted by ISBN
        booksPrimaryIndex.insert({bookISBN, byteOffset});

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markBooksPrimaryIndexFlag('0');
    }

    void addBookToSecondaryIndexFile(Book book, map<long long, short> &booksSecondaryIndex)
    {
        long long authorId = LibraryUtilities::convertCharArrToLongLong(book.authorID);
        long long ISBN = LibraryUtilities::convertCharArrToLongLong(book.ISBN);

        if (booksSecondaryIndex.count(authorId) == 0)
        {
            cout << "Empty list\n";
            booksSecondaryIndex[authorId] = addToInvertedList(ISBN, -1); // RRN
            cout << "Added value: " << booksSecondaryIndex[authorId] << "\n";
        }
        else
        {
            booksSecondaryIndex[authorId] = addToInvertedList(ISBN, booksSecondaryIndex[authorId]);
        }
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markBooksSecondaryIndexFlag('0');
    }

    short addToInvertedList(long long ISBN, short nextRecordPointer)
    {
        // Open the file in multiple modes
        fstream invertedList(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);

        short bestOffset = getBestOffsetInInvertedList();
        if (invertedList.fail())
        {
            cout << "First failing...............\n";
            return -1; // or handle the error appropriately
        }
        cout << "Best palce to insert: " << bestOffset << "\n";
        invertedList.seekp(bestOffset, ios::beg);
        if (invertedList.fail())
        {
            cout << "Failed to open the file.\n";
            return -1; // or handle the error appropriately
        }
        cout << "Writing to the inverted list file......\n";
        cout << "ISBN = " << ISBN << "\n";
        cout << "nextRecordPointer = " << nextRecordPointer << "\n";
        // Write the ISBN & the next record pointer
        invertedList.write((char *)&ISBN, sizeof(long long));
        invertedList.write((char *)&nextRecordPointer, sizeof(short));

        invertedList.close();

        // Return the RRN of the new record
        return bestOffset / (sizeof(long long) + sizeof(short)); // RRN
    }

    short getBestOffsetInInvertedList()
    {
        fstream invertedList(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::binary);

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