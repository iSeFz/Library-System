#ifndef AUTHORS_ADDING_SECONDARY_INDEX_H
#define AUTHORS_ADDING_SECONDARY_INDEX_H
#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsAddingSecondaryIndex
{
private:
    // Add the new book isbn to the inverted list of primary keys & return the RRN of the added record
    static short addToInvertedList(long long &authorId, short nextRecordPointer)
    {
        // Open the file in multiple modes
        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);

        short bestOffset = getBestOffsetInInvertedList();
        cout << "Best palce to insert: " << bestOffset << "\n";
        invertedList.seekp(bestOffset, ios::beg);
        if (invertedList.fail())
        {
            cout << "Failed to open the file.\n";
            return -1; // or handle the error appropriately
        }
        cout << "Writing to the inverted list file......\n";
        cout << "Author ID = " << authorId << "\n";
        cout << "nextRecordPointer = " << nextRecordPointer << "\n";
        // Write the Author ID & the next record pointer
        invertedList.write((char *)&authorId, sizeof(long long));
        invertedList.write((char *)&nextRecordPointer, sizeof(short));

        invertedList.close();

        // Return the RRN of the new record
        return bestOffset / (sizeof(long long) + sizeof(short)); // RRN
    }

    // Return the byte offset to insert the new record into the inverted list file
    static short getBestOffsetInInvertedList()
    {
        // Open the file in input mode
        fstream invertedList(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::binary);

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
    // Add the new author to the secondary index file
    static void add(Author &author, map<string, short> &authorsSecondaryIndex)
    {
        // Convert char array into long long
        long long authorId = LibraryUtilities::convertCharArrToLongLong(author.authorID);

        // Check if the name is unique and is not duplicate
        if (authorsSecondaryIndex.count(author.authorName) == 0)
        {
            cout << "Empty list\n";
            // Insert into the secondary index with pointer = -1 because it does not point to any primary key
            authorsSecondaryIndex[author.authorName] = addToInvertedList(authorId, -1); // RRN
            cout << "Added value: " << authorsSecondaryIndex[author.authorName] << "\n";
        }
        else // Otherwise, update the value of the RRN of this name to point to the next RRN
            authorsSecondaryIndex[author.authorName] = addToInvertedList(authorId, authorsSecondaryIndex[author.authorName]);

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        LibraryUtilities::markAuthorsSecondaryIndexFlag('0');
    }
};

#endif