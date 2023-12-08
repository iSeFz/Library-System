#ifndef BOOKS_ADDING_H
#define BOOKS_ADDING_H

#include "LibraryUtilities.h"
#include "Book.h"
#include "BooksAddingSecondaryIndex.h"

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
            BooksAddingSecondaryIndex::add(book, booksSecondaryIndex);
            cout << "\tNew Book Added Successfully!\n";
            books.close();
        }
        else
        { // Otherwise, if the avail list is NOT empty
            short currentOffset = header, prevOffset = -1;
            // First fit algorithm to find the first large enough record to insert the new record into
            while (currentOffset != -1)
            {
                short nextOffset, currentDeletedSize;
                // Seek to the offset of the last deleted record
                books.seekg(currentOffset, ios::beg);

                // Parse the record to save its information
                books.ignore(2); // Ignore the two characters '*' & the length delimiter '|'
                // Read the offset of the previous deleted record (next pointer)
                books.read((char *)&nextOffset, sizeof(nextOffset));
                books.ignore(1); // Ignore the delimiter '|'
                // Read the size of the current deleted record
                books.read((char *)&currentDeletedSize, sizeof(currentDeletedSize));

                // If the size of the new record is smaller than the deleted record
                // Means the current deleted record is suitable to insert in
                if (currentDeletedSize >= recordSize)
                {
                    // Check for to see if there are any records point to the current record
                    // If there are no records that point to the current record
                    if (prevOffset == -1)
                    { // Update the header to point to the next pointer of the current record
                        books.seekp(0, ios::beg);
                        books.write((char *)&nextOffset, sizeof(short));
                    }
                    else
                    { // Otherwise, seek to the record that points to the current record
                        // Update its next pointer to be the current record's next pointer
                        books.seekp(prevOffset, ios::beg);
                        books.ignore(2); // Skip the two characters '*' & the length delimiter '|'
                        books.write((char *)&nextOffset, sizeof(short));
                    }

                    // Seek to the deleted record offset to write the new record
                    books.seekp(currentOffset, ios::beg);
                    // Write the deleted record size to apply internal fragmentation afterward
                    books.write((char *)&currentDeletedSize, sizeof(currentDeletedSize));
                    short PKOffset = books.tellp(); // Store the byteoffset of the new record
                    // Write the rest of record fields separated by delimeters
                    books.write(book.ISBN, isbnSize);
                    books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
                    books.write(book.bookTitle, titleSize);
                    books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
                    books.write(book.authorID, authIdSize);
                    books.write((char *)&LibraryUtilities::lengthDelimiter, 1);

                    // Apply internal fragmentation and fill the rest of record space with telda '~'
                    short iterator = books.tellp();
                    while (iterator++ < (currentOffset + currentDeletedSize - 1))
                        books.write("~", 1);
                    // Write the last delimiter after the telda to indicate the end of this record
                    books.write((char *)&LibraryUtilities::lengthDelimiter, 1);

                    // Add the new book to index files
                    addBookToPrimaryIndexFile(book.ISBN, PKOffset, booksPrimaryIndex);
                    BooksAddingSecondaryIndex::add(book, booksSecondaryIndex);
                    cout << "\tNew Book Added Successfully!\n";
                    books.close();
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
            books.seekp(0, ios::end); // Seek to the end of file
            books.write((char *)&recordSize, sizeof(recordSize));
            short PKOffset = books.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            books.write(book.ISBN, isbnSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            books.write(book.bookTitle, titleSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            books.write(book.authorID, authIdSize);
            books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
            // Add the new book to index files
            addBookToPrimaryIndexFile(book.ISBN, PKOffset, booksPrimaryIndex);
            BooksAddingSecondaryIndex::add(book, booksSecondaryIndex);
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
};

#endif // BOOKS_ADDING_H