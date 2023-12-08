#ifndef BOOKS_ADDING_SECONDARY_INDEX_H
#define BOOKS_ADDING_SECONDARY_INDEX_H
#include "LibraryUtilities.h"
#include "Book.h"

class BooksAddingSecondaryIndex
{
private:
    // Add the new book isbn to the inverted list of primary keys & return the RRN of the added record
    static short addToInvertedList(long long &ISBN, short nextRecordPointer)
    {
        // Open the file in multiple modes
        fstream invertedList(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);

        short bestOffset = getBestOffsetInInvertedList();
        invertedList.seekp(bestOffset, ios::beg);

        // Write the ISBN & the next record pointer
        invertedList.write((char *)&ISBN, sizeof(long long));
        invertedList.write((char *)&nextRecordPointer, sizeof(short));

        invertedList.close();

        // Return the RRN of the new record
        return bestOffset / (sizeof(long long) + sizeof(short)); // RRN
    }

    // Return the byte offset to insert the new record into the inverted list file
    static short getBestOffsetInInvertedList()
    {
        // Open the file in input mode
        fstream invertedList(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::binary);

        // Get the file size
        invertedList.seekg(0, ios::end);
        int endOffset = invertedList.tellg();

        invertedList.seekg(0, ios::beg);
        // Search to find the offset of the first deleted record to insert into
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

public:
    // Add the new book to the secondary index file
    static void add(Book &book, map<long long, short> &booksSecondaryIndex)
    {
        // Convert char array into long long
        long long authorId = LibraryUtilities::convertCharArrToLongLong(book.authorID);
        long long ISBN = LibraryUtilities::convertCharArrToLongLong(book.ISBN);

        // Check if the author is unique and is not duplicate
        if (booksSecondaryIndex.count(authorId) == 0)
            // Insert into the secondary index with pointer = -1 because it does not point to any primary key
            booksSecondaryIndex[authorId] = addToInvertedList(ISBN, -1); // RRN
        else // Otherwise, update the value of the RRN of this name to point to the next RRN
            booksSecondaryIndex[authorId] = addToInvertedList(ISBN, booksSecondaryIndex[authorId]);

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markBooksSecondaryIndexFlag('0');
    }
};

#endif