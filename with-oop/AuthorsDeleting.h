#ifndef AUTHORS_DELETING_H
#define AUTHORS_DELETING_H

#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsDeleting
{
public:
    // Delete an author using author ID
    void deleteAuthor(map<long long, short> &authorsPrimaryIndex, map<string, short> &authorsSecondaryIndex)
    {
        long long authorId;
        cout << "Enter author ID: ";
        cin >> authorId;
        cin.ignore();
        auto authorInMap = authorsPrimaryIndex.lower_bound(authorId);
        if (authorInMap != authorsPrimaryIndex.end() && authorInMap->first == authorId)
        {
            short recordStartoffset = authorInMap->second - 2;
            deleteFromAuthorsSecondaryIndexFile(authorsSecondaryIndex, authorId, getAuthorNameAt(recordStartoffset));
            deleteFromAuthorsPrimaryIndexFile(authorsPrimaryIndex, authorId);
            deleteFromAuthorsDataFile(recordStartoffset);
            cout << "\tAuthor Deleted Successfully\n";
        }
        else
            cout << "\tAuthor does not exist\n";
    }

    string getAuthorNameAt(short startOffset)
    {
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);
        Author tmpAuthor;
        authors.seekg(startOffset, ios::beg);
        authors.ignore(2); // ignore the record size
        authors.getline(tmpAuthor.authorID, 15, '|');
        authors.getline(tmpAuthor.authorName, 30, '|');
        authors.getline(tmpAuthor.address, 30, '|');
        authors.close();
        return tmpAuthor.authorName;
    }

    void deleteFromAuthorsDataFile(int recordStartoffset)
    {
        fstream books(LibraryUtilities::authorsFile, ios::in | ios::out | ios::binary);
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

    void deleteFromAuthorsPrimaryIndexFile(map<long long, short> &authorsPrimaryIndex, long long authorId)
    {
        authorsPrimaryIndex.erase(authorId);
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markAuthorsPrimaryIndexFlag('0');
    }

    void deleteFromAuthorsSecondaryIndexFile(map<string, short> &authorsSecondaryIndex, long long authorId, string authorName)
    {
        short oldPointer = authorsSecondaryIndex[authorName];
        short newPointer = deleteFromAuthorsSecondaryIndexLinkedListFile(oldPointer, authorId);
        if (newPointer == -1)
            authorsSecondaryIndex.erase(authorName);
        else
            authorsSecondaryIndex[authorName] = newPointer;
    }

    short deleteFromAuthorsSecondaryIndexLinkedListFile(int firstPosition, long long authorId)
    {
        // Keep the rpevious record offset (Not the RRN)
        short previousRecordOffset = -1;

        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);
        // char flagLength = 1;
        short deletedSymbol = '#';

        // Size of each record in the linked list is the ISBN + the next record pointer
        short recordSize = sizeof(long long) + sizeof(short);

        // Go to the first record of the linked list
        invertedList.seekg(firstPosition * recordSize, ios::beg);

        while (true)
        {
            // cout << "current offset: " << invertedList.tellg() << "\n";
            // Read the authorId & the next record pointer from the current record
            long long tmpAuthorId;
            invertedList.read((char *)&tmpAuthorId, sizeof(long long));
            short nextRecordPointer;
            invertedList.read((char *)&nextRecordPointer, sizeof(short));

            if (tmpAuthorId == authorId)
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
                    invertedList.ignore(sizeof(long long)); // ignore the authorID
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

#endif // AUTHORS_DELETING_H