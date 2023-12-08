#ifndef BOOKS_UPDATING_H
#define BOOKS_UPDATING_H

#include "LibraryUtilities.h"
#include "Book.h"

class BooksUpdating
{
public:
    // Update book title using ISBN
    void updateBookTitle(map<long long, short> &booksPrimaryIndex)
    {
        long long ISBN;
        short offset;
        char newTitle[30];
        cout << "Enter book ISBN: ";
        cin >> ISBN;
        cin.ignore();
        // Searching for the record which will be updated in runtime
        auto isbn = booksPrimaryIndex.lower_bound(ISBN);
        if (isbn != booksPrimaryIndex.end() && isbn->first == ISBN)
        {
            offset = isbn->second;
            cout << "Enter new book title: ";
            cin.getline(newTitle, 30);
            updateBookTitleInDataFile(newTitle, offset);
            cout << "\tBook title updated successfully!\n";
        }
        else // If the book is not found, print a warning message
            cerr << "\tBook does NOT exist!\n";
    }

    // applying the updates in the books data file
    void updateBookTitleInDataFile(char newBookTitle[30], int byteOffset)
    {
        fstream dataFile(LibraryUtilities::booksFile, ios::in | ios::out | ios::binary);
        // reaching desired record
        dataFile.seekg(byteOffset, ios::beg);
        // skipping the length indicator(2 bytes) and ISBN field(15 bytes or finding |)
        dataFile.ignore(17, '|');

        // storing the length of the old title (before updating) to not exceed the allocated size
        char oldTitle[30];
        dataFile.getline(oldTitle, 30, '|');
        int maxSize = strlen(oldTitle);

        // reaching the title field beginning
        dataFile.seekg(-(maxSize + 1), ios::cur);

        // case 1 --> length of new title < length of old title
        if (strlen(newBookTitle) < maxSize)
        {
            newBookTitle[strlen(newBookTitle)] = '\0';

            for (int i = strlen(newBookTitle) + 1; i < maxSize; i++)
                newBookTitle[i] = '_';

            dataFile.write(newBookTitle, maxSize);
        }
        // case 2 & 3  --> length of new title == length of old title,
        // length of new title > length of old title
        else
            dataFile.write(newBookTitle, maxSize);

        dataFile.close();
    }
};

#endif // BOOKS_UPDATING_H