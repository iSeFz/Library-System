#ifndef BOOKS_TESTING_H
#define BOOKS_TESTING_H

#include "LibraryUtilities.h"
#include "Book.h"

class BooksTesting
{
public:
    void printBooksFile()
    {
        cout << "----------------------------\n";
        fstream books(LibraryUtilities::booksFile, ios::in | ios::binary);

        books.seekg(0, ios::end);
        int fileEndOffset = books.tellg();
        books.seekg(0, ios::beg);

        short header;
        books.read((char *)&header, sizeof(header));
        cout << "Avail list header: " << header << "\n";
        while (books)
        {
            int recordOffset = books.tellg();
            if (recordOffset == fileEndOffset or recordOffset == -1)
                break;
            cout << "\n-------- "
                 << "Record Offset: " << recordOffset << " --------\n";
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
                cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
                cout << "Redcord deleted\n";
                cout << "Previous deleted record: " << previousRecord << "\n";
                cout << "Record Size: " << recordSize << "\n";
                cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
                continue;
            }
            else
            {
                books.seekg(-1, ios::cur);
            }
            cout << "############################\n";
            short recordSize;
            books.read((char *)&recordSize, sizeof(short));
            if (recordSize == -1)
                break;
            cout << "Record Size: " << recordSize << "\n";
            Book book;
            books.getline(book.ISBN, 15, '|');
            books.getline(book.bookTitle, 30, '|');
            books.getline(book.authorID, 15, '|');
            if (books.peek() == '|')
            {
                books.seekg(1, ios::cur);
            }
            cout << "\tBook with ISBN #" << book.ISBN << " - ";
            cout << "Book Title: " << book.bookTitle << " - ";
            cout << "Author ID: " << book.authorID << "\n";
            cout << "############################\n";
            books.seekg(recordOffset + recordSize, ios::beg);   // jump to the next record
        }
        books.close();
    }

    void printBooksFileAsPlainText()
    {
        cout << "--------------\n";
        cout << "Books whole File:\n";
        fstream books(LibraryUtilities::booksFile, ios::in | ios::binary);
        short header;
        books.read((char *)&header, sizeof(header));
        cout << "Header: " << header << "\n";
        string wholeText;
        getline(books, wholeText);
        cout << wholeText << "\n";
        cout << "--------------\n";
        books.close();
    }

    void printBooksSecondaryIndex(map<long long, short> &booksSecondaryIndex)
    {
        cout << "--------------\n";
        cout << "Books Secondary Index:\n";
        for (auto record : booksSecondaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }

    void printBooksInvertedList()
    {
        fstream invertedFile(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::binary);
        invertedFile.seekg(0, ios::end);
        int endOffset = invertedFile.tellg();
        invertedFile.seekg(0, ios::beg);

        while (invertedFile)
        {
            if (invertedFile.tellg() == endOffset)
                break;
            long long ISBN;
            invertedFile.read((char *)&ISBN, sizeof(long long));
            short nextRecordPointer;
            invertedFile.read((char *)&nextRecordPointer, sizeof(short));
            cout << ISBN << " " << nextRecordPointer << "\n";
        }
    }

    void printBooksPrimaryIndex(map<long long, short> &booksPrimaryIndex)
    {
        cout << "--------------\n";
        cout << "Books Primary Index:\n";
        for (auto record : booksPrimaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }
};

#endif // BOOKS_TESTING_H