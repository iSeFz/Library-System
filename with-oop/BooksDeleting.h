#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Book.h"

class BooksDeleting
{
public:
    void deleteBook(map<long long, short> &booksPrimaryIndex, map<long long, short> &booksSecondaryIndex)
    {
        long long ISBN;
        cout << "Enter Book ISBN: ";
        cin >> ISBN;
        cin.ignore();
        auto isbn = booksPrimaryIndex.lower_bound(ISBN);
        if (isbn != booksPrimaryIndex.end() && isbn->first == ISBN)
        {
            short recordStartoffset = isbn->second - 2;
            deleteFromBooksSecondaryIndexFile(booksSecondaryIndex, ISBN, getBookAuthorIdAt(recordStartoffset));
            deleteFromBooksPrimaryIndexFile(booksPrimaryIndex, ISBN);
            deleteFromBooksDataFile(recordStartoffset);
        }
        else
        {
            cout << "\tBook does not exist\n";
        }
    }

    void deleteFromBooksDataFile(int recordStartoffset)
    {
        fstream books(LibraryUtilities::booksFile, ios::in | ios::out | ios::binary);
        short header, recordSize;
        books.read((char *)&header, sizeof(header));
        books.seekg(recordStartoffset, ios::beg);
        books.read((char *)&recordSize, sizeof(short));
        // string overrideText = "*|" + to_string(header) + "|" + to_string(recordSize) + "|";
        books.seekp(recordStartoffset, ios::beg);
        books.write((char *)"*", 1);
        books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
        books.write((char *)&header, sizeof(header));
        books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
        books.write((char *)&recordSize, sizeof(recordSize));
        books.write((char *)&LibraryUtilities::lengthDelimiter, 1);
        // update the header
        books.seekp(0, ios::beg);
        books.write((char *)&recordStartoffset, sizeof(short));
        books.close();
    }

    void deleteFromBooksPrimaryIndexFile(map<long long, short> &booksPrimaryIndex, long long ISBN)
    {
        booksPrimaryIndex.erase(ISBN);
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markBooksPrimaryIndexFlag('0');
    }

    long long getBookAuthorIdAt(short startOffset)
    {
        fstream books(LibraryUtilities::booksFile, ios::in | ios::binary);
        Book tmpBook;
        books.seekg(startOffset, ios::beg);
        books.ignore(2); // ignore the record size
        books.getline(tmpBook.ISBN, 15, '|');
        books.getline(tmpBook.bookTitle, 30, '|');
        books.getline(tmpBook.authorID, 15, '|');
        books.close();
        return LibraryUtilities::convertCharArrToLongLong(tmpBook.authorID);
    }

    void deleteFromBooksSecondaryIndexFile(map<long long, short> &booksSecondaryIndex, long long ISBN, long long authorId)
    {
        short oldPointer = booksSecondaryIndex[authorId];
        short newPointer = deleteFromBooksSecondaryIndexLinkedListFile(oldPointer, ISBN);
        if (newPointer == -1)
            booksSecondaryIndex.erase(authorId);
        else
            booksSecondaryIndex[authorId] = newPointer;
    }

    short deleteFromBooksSecondaryIndexLinkedListFile(int firstPosition, long long ISBN)
    {
        // Keep the rpevious record offset (Not the RRN)
        short previousRecordOffset = -1;

        fstream invertedList(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);
        // char flagLength = 1;
        short deletedSymbol = '#';

        // Size of each record in the linked list is the ISBN + the next record pointer
        short recordSize = sizeof(long long) + sizeof(short);

        // Go to the first record of the linked list
        invertedList.seekg(firstPosition * recordSize, ios::beg);

        while (true)
        {
            // cout << "current offset: " << invertedList.tellg() << "\n";
            // Read the ISBN & the next record pointer from the current record
            long long tmpISBN;
            invertedList.read((char *)&tmpISBN, sizeof(long long));
            short nextRecordPointer;
            invertedList.read((char *)&nextRecordPointer, sizeof(short));

            if (tmpISBN == ISBN)
            {
                // If there is no previous record, then we want to delete this first record
                if (previousRecordOffset == -1)
                {
                    invertedList.seekp(invertedList.tellg() - sizeof(nextRecordPointer), ios::beg);
                    invertedList.write((char *)&deletedSymbol, sizeof(deletedSymbol));
                    invertedList.close();
                    if (nextRecordPointer == -1)
                    {
                        // There are no longer any records in the linked list that belongs to the same author
                        return -1;
                    }
                    else
                    {
                        // Make the secondary index point to the next record of the current deleted record
                        return nextRecordPointer;
                    }
                }
                else
                {
                    // mark the current record as deleted
                    // Go back to the next record pointer field to update it
                    invertedList.seekp(invertedList.tellg() - sizeof(nextRecordPointer), ios::beg);
                    invertedList.write((char *)&deletedSymbol, sizeof(deletedSymbol));

                    // update the previous record to point to the next record of this deleted record
                    invertedList.seekp(previousRecordOffset, ios::beg);
                    invertedList.ignore(sizeof(long long)); // ignore the ISBN
                    invertedList.write((char *)&nextRecordPointer, sizeof(nextRecordPointer));
                    invertedList.close();

                    // The record that the secondary index refers to still exists
                    return firstPosition;
                }
            }
            else
            {
                previousRecordOffset = invertedList.tellg() - recordSize;
                // Go to the next record
                invertedList.seekg(nextRecordPointer * recordSize, ios::beg);
            }
        }
    }
};