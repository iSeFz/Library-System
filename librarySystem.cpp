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
                // addAuthor();
            } else if (choice == "2") {
                cout << "\tAdding New Book...\n";
                // addBook();
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