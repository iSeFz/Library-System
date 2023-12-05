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
    void addBook(map<long long, short> &booksPrimaryIndex, map<long long, short> &booksSecondaryIndex)
    {
        // Get book data from the user & store it in a Book object
        Book newBook;
        cout << "Enter book ISBN: ";
        cin.getline(newBook.ISBN, 15);
        cout << "Enter book title: ";
        cin.getline(newBook.bookTitle, 30);
        cout << "Enter author ID: ";
        cin.getline(newBook.authorID, 15);
        // Add the new book to the data file
        addBookToDataFile(newBook, booksPrimaryIndex, booksSecondaryIndex);
    }

    // Add the new book to the main books data file
    void addBookToDataFile(Book &book, map<long long, short> &booksPrimaryIndex, map<long long, short> &booksSecondaryIndex)
    {
        short header, recordSize, isbnSize, titleSize, authIdSize;
        // Open the file in multiple modes
        fstream books(LibraryUtilities::booksFile, ios::in | ios::out | ios::app | ios::binary);

        // Read the header of the file
        books.seekg(0);
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
        // if (header == -1)
        // {
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
        // Add the new book to the primary index file
        addBookToPrimaryIndexFile(book.ISBN, offset, booksPrimaryIndex);
        addBookToSecondaryIndexFile(book, booksSecondaryIndex);
        cout << "\tNew Book Added Successfully!\n";
        books.close();
        // }
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
        return endOffset;
    }
};