#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Book.h"

class BooksPrinting
{
public:
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