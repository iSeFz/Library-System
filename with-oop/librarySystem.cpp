#include <bits/stdc++.h>
#include "Authors.h"
#include "Books.h"
#include "Queries.h"

using namespace std;

// Library class to manage all methods
class Library {
private:
    Authors authors = Authors();
    Books books = Books();
    Queries queries = Queries();

public:
    // Main method to start the program
    void start() {
        cout << "\tWelcome to the Library Catalog System!\n";
        LibraryUtilities::checkFilesExist();
        // Load index files into memory
        authors.loadAuthorPrimaryIndex();
        authors.loadAuthorsSecondaryIndex();
        books.loadBookPrimaryIndex();
        books.loadBookSecondaryIndex();
        // Loop until user exits
        while (true) {
            cout << "\n1. Add New Author\n"
                    "2. Add New Book\n"
                    "3. Update Author Name (Author ID)\n"
                    "4. Update Book Title (ISBN)\n"
                    "5. Delete Book (ISBN)\n"
                    "6. Delete Author (Author ID)\n"
                    "7. Print Author (Author ID)\n"
                    "8. Print Book (ISBN)\n"
                    "9. Write Query\n"
                    "10. Exit\n"
                    "11. Print Books File\n"
                    "12. Print Books Primary Index\n"
                    "13. Print Books Secondary Index\n"
                    "14. Print Books Secondary Index Inverted list\n"
                    "15. Print Authors File\n"
                    "16. Print Authors Primary Index\n"
                    "17. Print Authors Secondary Index\n"
                    "18. Print Authors Secondary Index Inverted list\n";
            cout << "Please Enter Choice (1-10) ==> ";
            string choice;
            getline(cin, choice);
            if (choice == "1") {
                cout << "\tAdding New Author\n";
                authors.addAuthor();
            } else if (choice == "2") {
                cout << "\tAdding New Book\n";
                books.addBook(authors.getPrimaryIndex());
            } else if (choice == "3") {
                cout << "\tUpdating Author Name\n";
                authors.updateAuthorName();
            } else if (choice == "4") {
                cout << "\tUpdating Book Title\n";
                books.updateBookTitle();
            } else if (choice == "5") {
                books.deleteBook();
            } else if (choice == "6") {
                authors.deleteAuthor();
            } else if (choice == "7") {
                cout << "\tPrinting Author Using ID\n";
                authors.printAuthor();
            } else if (choice == "8") {
                cout << "\tPrinting Book Using ISBN\n";
                books.printBook();
            } else if (choice == "9") {
                queries.setIndexes(authors, books);
                queries.writeQuery();
            } else if (choice == "10") {
                terminate();
                break;
            } else if (choice == "11") {
                books.printBooksFile();
            } else if (choice == "12") {
                books.printBooksPrimaryIndex();
            } else if (choice == "13") {
                books.printBooksSecondaryIndex();
            } else if (choice == "14") {
                books.printBooksInvertedList();
            } else if (choice == "15") {
                authors.printAuthorsFile();
            } else if (choice == "16") {
                authors.printAuthorsPrimaryIndex();
            } else if (choice == "17") {
                authors.printAuthorsSecondaryIndex();
            } else if (choice == "18") {
                authors.printAuthorsInvertedList();
            } else {
                cerr << "\n\tINVALID CHOICE!!\n";
            }
        }
    }

    // Terminate the program correctly with saved changes
    void terminate() {
        // Save the primary index files to the disk with the updated indices
        books.saveBookPrimaryIndex();
        books.saveBookSecondaryIndex();
        authors.saveAuthorPrimaryIndex();
        authors.saveAuthorsSecondaryIndex();
        cout << "\tThank you for using our Library System!\n";
    }
};

// Start the program
int main() {
    Library lib;
    lib.start();
    return 0;
}