#include <bits/stdc++.h>
using namespace std;

// Library class to manage all methods
class Library
{
private:
    // Save author information
    struct Author
    {
        char authorID[15];
        char authorName[30];
        char address[30];
    };
    // Save book information
    struct Book
    {
        char ISBN[15];
        char bookTitle[30];
        char authorID[15];
    };

    const string authorsFile = "authors.txt";                                                 // Authors data file
    const string authorsPrimaryIndexFile = "authorsPrimaryIndex.txt";                         // Authors primary index file
    const string authorsSecondaryIndexFile = "authorsSecondaryIndex.txt";                     // Authors secondary index file
    const string authorsSecondaryIndexLinkedListFile = "authorsSecondaryIndexLinkedList.txt"; // Authors secondary index linked list file

    const string booksFile = "books.txt";                                                 // Books data file
    const string booksPrimaryIndexFile = "booksPrimaryIndex.txt";                         // Books primary index file
    const string booksSecondaryIndexFile = "booksSecondaryIndex.txt";                     // Books secondary index file
    const string booksSecondaryIndexLinkedListFile = "booksSecondaryIndexLinkedList.txt"; // Books secondary index linked list file

    const char lengthDelimiter = '|'; // Delimiter to separate length indicator from record data

    map<char[15], int> authorsPrimaryIndex;   // (Author ID, byte offset) Authors primary index
    map<char[30], int> authorsSecondaryIndex; // (Author Name, First record in linked list) Authors secondary index

    map<char[15], int> booksPrimaryIndex;   // (ISBN, byte offset) Books primary index
    map<char[15], int> booksSecondaryIndex; // (Author ID, First record in linked list) Books secondary index

public:
    // Add a new author helper function
    void addAuthor()
    {
        // Get author data from the user & store it in an Author object
        Author newAuthor;
        cout << "Enter author ID: ";
        cin.getline(newAuthor.authorID, 15);
        cout << "Enter author Name: ";
        cin.getline(newAuthor.authorName, 30);
        cout << "Enter address: ";
        cin.getline(newAuthor.address, 30);
        // Add the new author to the data file
        addAuthorToDataFile(newAuthor);
    }

    // Add the new author to the main authors data file
    void addAuthorToDataFile(Author author)
    {
        short header, recordSize, idSize, nameSize, addSize;

        // Open the file in multiple modes
        fstream authors(authorsFile, ios::in | ios::out | ios::app | ios::binary);
        
        // Read the header of the file
        authors.seekg(0);
        authors.read((char *)&header, sizeof(header));

        // Get the sizes of the id, name & address
        idSize = strlen(author.authorID);
        nameSize = strlen(author.authorName);
        addSize = strlen(author.address);

        // Calculate the total record length
        // 5 refers to 2 bytes for the record length indicator
        // + 3 bytes for 3 delimeters between fields
        recordSize = idSize + nameSize + addSize + 5;

        // If the avail list is empty, insert at the end of the file
        if (header == -1)
        {
            authors.seekp(0, ios::end); // Seek to the end of file
            authors.write((char *)&recordSize, sizeof(recordSize));
            short offset = authors.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            authors.write(author.authorID, idSize);
            authors.write((char *)&lengthDelimiter, 1);
            authors.write(author.authorName, nameSize);
            authors.write((char *)&lengthDelimiter, 1);
            authors.write(author.address, addSize);
            authors.write((char *)&lengthDelimiter, 1);
            // Add the new author to the primary index file
            // addAuthorToPrimaryIndexFile(author, offset);
            cout << "\tNew Author Added Successfully!\n";
            authors.close(); // Close the authors data file
        }
    }

    void addAuthorToPrimaryIndexFile(Author author, short byteOffest) {}

    void addAuthorToSecondaryIndexFile(Author author) {}

    void addAuthorToSecondaryIndexLinkedListFile(Author author) {}

    // Add a new book helper function
    void addBook()
    {
        // Get book data from the user & store it in a Book object
        Book newBook;
        cout << "Enter book ISBN: ";
        cin.getline(newBook.ISBN, 15);
        cout << "Enter book title: ";
        cin.getline(newBook.bookTitle, 30);
        cout << "Enter author ID: ";
        cin.getline(newBook.authorID, 15);
        // Add the new book to the data file
        addBookToDataFile(newBook);
    }

    // Add the new book to the main books data file
    void addBookToDataFile(Book book)
    {
        short header, recordSize, isbnSize, titleSize, authIdSize;

        // Open the file in multiple modes
        fstream books(booksFile, ios::in | ios::out | ios::app | ios::binary);
        
        // Read the header of the file
        books.seekg(0);
        books.read((char *)&header, sizeof(header));

        // Get the sizes of the isbn, title & author id
        isbnSize = strlen(book.ISBN);
        titleSize = strlen(book.bookTitle);
        authIdSize = strlen(book.authorID);

        // Calculate the total record length
        // 5 refers to 2 bytes for the record length indicator
        // + 3 bytes for 3 delimeters between fields
        recordSize = isbnSize + titleSize + authIdSize + 5;

        // If the avail list is empty, insert at the end of the file
        if (header == -1)
        {
            books.seekp(0, ios::end); // Seek to the end of file
            books.write((char *)&recordSize, sizeof(recordSize));
            short offset = books.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            books.write(book.ISBN, isbnSize);
            books.write((char *)&lengthDelimiter, 1);
            books.write(book.bookTitle, titleSize);
            books.write((char *)&lengthDelimiter, 1);
            books.write(book.authorID, authIdSize);
            books.write((char *)&lengthDelimiter, 1);
            // Add the new book to the primary index file
            // addBookToPrimaryIndexFile(book, offset);
            cout << "\tNew Book Added Successfully!\n";
            books.close(); // Close the books data file
        }
    }

    void addBookToPrimaryIndexFile(Book book, short byteOffest) {}

    void addBookToSecondaryIndexFile(Book book) {}

    void addBookToSecondaryIndexLinkedListFile(Book book) {}

    // Update author name using author ID
    void updateAuthorName() {}

    void updateAuthorNameInDataFile(char newAuthorName[3], int byteOffset) {}

    void updateAuthorNameInSecondayIndex(char oldAuthorName[30], char newAuthorName[30]) {}

    // Update book title using ISBN
    void updateBookTitle() {}

    void updateBookTitleInDataFile(char newBookTitle[30], int byteOffset) {}

    // Delete an author using author ID
    void deleteAuthor() {}

    void deleteFromAuthorsDataFile(int byteOffset) {}

    void deleteFromAuthorsPrimaryIndexFile(char authorId[30]) {}

    void deleteFromAuthorsSecondaryIndexFile(char authorName[30]) {}

    void deleteFromAuthorsSecondaryIndexLinkedListFile(int firstPosition, char authorId[30]) {}

    // Delete a book using ISBN
    void deleteBook() {}

    void deleteFromBooksDataFile(int byteOffset) {}

    void deleteFromBooksPrimaryIndexFile(char ISBN[15]) {}

    void deleteFromBooksSecondaryIndexFile(char authorId[30]) {}

    void deleteFromBooksSecondaryIndexLinkedListFile(int firstPosition, char ISBN[15]) {}

    // Print author using author ID
    void printAuthor() {}

    // Print book using ISBN
    void printBook() {}

    // Handle the select query
    void writeQuery() {}

    // Main method to start the program
    void start()
    {
        cout << "\tWelcome to the Library Catalog System!\n";
        // Loop until user exits
        while (true)
        {
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
            if (choice == "1")
            {
                cout << "\tAdding New Author...\n";
                addAuthor();
            }
            else if (choice == "2")
            {
                cout << "\tAdding New Book...\n";
                addBook();
            }
            else if (choice == "10")
            {
                cout << "\tThank you for using our Library System!\n";
                break;
            }
            else
            {
                cerr << "\n\tINVALID CHOICE!!\n";
            }
        }
    }
};

// Start the program
int main()
{
    Library lib;
    lib.start();
    return 0;
}