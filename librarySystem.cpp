#include <bits/stdc++.h>
using namespace std;

// Library class to manage all methods
class Library {
private:
    // Save author information
    struct Author {
        char authorID[15];
        char authorName[30];
        char address[30];
    };
    // Save book information
    struct Book {
        char ISBN[15];
        char bookTitle[30];
        char authorID[15];
    };

public:
    // Add a new author
    void addAuthor() {
        Author newAuthor;
        // Open the authors data file to add a new author
        fstream authors("authors.txt", ios::app | ios::binary);
        // Check if the file has successfully opened without errors
        if(authors.fail()) {
            cerr << "\tError opening the file!\n";
            return;
        }
        string record;     // Store the record data to write
        int recordLen = 0; // Track the length of the record
        // Get the actual author data from the user
        cout << "Enter author ID: ";
        cin.getline(newAuthor.authorID, 15);
        record += newAuthor.authorID;
        record += "|";
        cout << "Enter author Name: ";
        cin.getline(newAuthor.authorName, 30);
        record += newAuthor.authorName;
        record += "|";
        cout << "Enter address: ";
        cin.getline(newAuthor.address, 30);
        record += newAuthor.address;
        record += "|";
        // Get the record length to use as length indicator
        recordLen = record.length();
        // Write the record to the file
        authors << recordLen << record;
        cout << "\tNew Author Added Successfully!\n";
        authors.close();
    }

    // Add a new book
    void addBook() {
        Book newBook;
        // Open the books data file to add a new book
        fstream books("books.txt", ios::app | ios::binary);
        // Check if the file has successfully opened without errors
        if(books.fail()) {
            cerr << "\tError opening the file!\n";
            return;
        }
        string record;     // Store the record data to write
        int recordLen = 0; // Track the length of the record
        // Get the actual book data from the user
        cout << "Enter book ISBN: ";
        cin.getline(newBook.ISBN, 15);
        record += newBook.ISBN;
        record += "|";
        cout << "Enter book title: ";
        cin.getline(newBook.bookTitle, 30);
        record += newBook.bookTitle;
        record += "|";
        cout << "Enter author ID: ";
        cin.getline(newBook.authorID, 15);
        record += newBook.authorID;
        record += "|";
        // Get the record length to use as length indicator
        recordLen = record.length();
        // Write the record to the file
        books << recordLen << record;
        cout << "\tNew Book Added Successfully!\n";
        books.close();
    }

    // Main method to start the program
    void start() {
        cout << "\tWelcome to the Library Catalog System!\n";
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
                    "10. Exit\n";
            cout << "Please Enter Choice (1-10) ==> ";
            string choice;
            getline(cin, choice);
            if (choice == "1") {
                cout << "\tAdding New Author...\n";
                addAuthor();
            } else if (choice == "2") {
                cout << "\tAdding New Book...\n";
                addBook();
            } else if (choice == "10") {
                cout << "\tThank you for using our Library System!\n";
                break;
            } else {
                cerr << "\n\tINVALID CHOICE!!\n";
            }
        }
    }
};

// Start the program
int main() {
    Library lib;
    lib.start();
    return 0;
}