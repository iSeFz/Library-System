#ifndef BOOKS_H
#define BOOKS_H
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Book.h"
#include "BooksTesting.h"
#include "BooksPrinting.h"
#include "BooksDeleting.h"
#include "BooksUpdating.h"
#include "BooksAdding.h"
#include "BooksConfiguration.h"

using namespace std;

class Books
{
private:
    map<long long, short> booksPrimaryIndex;   // (ISBN, byte offset) Books primary index
    map<long long, short> booksSecondaryIndex; // (Author ID, First record in linked list) Books secondary index
    BooksTesting booksTesting = BooksTesting();
    BooksPrinting booksPrinting = BooksPrinting();
    BooksDeleting booksDeleting = BooksDeleting();
    BooksUpdating booksUpdating = BooksUpdating();
    BooksAdding booksAdding = BooksAdding();
    BooksConfiguration booksConfiguration = BooksConfiguration();

public:
    // Retrieve data from the map & write it back to the physical file on disk
    void saveBookPrimaryIndex()
    {
        booksConfiguration.saveBookPrimaryIndex(booksPrimaryIndex);
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveBookSecondaryIndex()
    {
        booksConfiguration.saveBookSecondaryIndex(booksSecondaryIndex);
    }

    // Load books primary index file into memory
    void loadBookPrimaryIndex()
    {
        booksConfiguration.loadBookPrimaryIndex(booksPrimaryIndex);
    }

    // Load books secondary index file into memory
    void loadBookSecondaryIndex()
    {
        booksConfiguration.loadBookSecondaryIndex(booksSecondaryIndex);
    }

    // Add a new book helper function
    void addBook(map<long long, short> &authorsPrimaryIndex)
    {
        booksAdding.addBook(booksPrimaryIndex, booksSecondaryIndex, authorsPrimaryIndex);
    }

    // Update book title using ISBN
    void updateBookTitle()
    {
        booksUpdating.updateBookTitle(booksPrimaryIndex);
    }

    // Delete a book using ISBN
    void deleteBook()
    {
        booksDeleting.deleteBook(booksPrimaryIndex, booksSecondaryIndex);
    }

    // Print book using ISBN
    void printBook()
    {
        booksPrinting.printBook(booksPrimaryIndex);
    }

    // For testing only
    void printBooksFile()
    {
        booksTesting.printBooksFile();
    }

    void printBooksFileAsPlainText()
    {
        booksTesting.printBooksFileAsPlainText();
    }

    void printBooksSecondaryIndex()
    {
        booksTesting.printBooksSecondaryIndex(booksSecondaryIndex);
    }

    void printBooksInvertedList()
    {
        booksTesting.printBooksInvertedList();
    }

    void printBooksPrimaryIndex()
    {
        booksTesting.printBooksPrimaryIndex(booksPrimaryIndex);
    }
    map<long long, short> &getPrimaryIndex()
    {
        return booksPrimaryIndex;
    }
    map<long long, short> &getSecondaryIndex()
    {
        return booksSecondaryIndex;
    }
};
#endif