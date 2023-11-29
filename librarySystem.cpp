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

    const string authorsFile = "authors.txt";                                                 // Authors data file
    const string authorsPrimaryIndexFile = "authorsPrimaryIndex.txt";                         // Authors primary index file
    const string authorsSecondaryIndexFile = "authorsSecondaryIndex.txt";                     // Authors secondary index file
    const string authorsSecondaryIndexLinkedListFile = "authorsSecondaryIndexLinkedList.txt"; // Authors secondary index linked list file

    const string booksFile = "books.txt";                                                 // Books data file
    const string booksPrimaryIndexFile = "booksPrimaryIndex.txt";                         // Books primary index file
    const string booksSecondaryIndexFile = "booksSecondaryIndex.txt";                     // Books secondary index file
    const string booksSecondaryIndexLinkedListFile = "booksSecondaryIndexLinkedList.txt"; // Books secondary index linked list file

    const char lengthDelimiter = '|';  // Delimiter to separate length indicator from record data
    short isAuthorPrimIdxUpToDate = 0; // Flag to track the status of the authors primary index file
    short isBookPrimIdxUpToDate = 0;   // Flag to track the status of the books primary index file

    map<int, short> authorsPrimaryIndex;      // (Author ID, byte offset) Authors primary index
    map<char[30], int> authorsSecondaryIndex; // (Author Name, First record in linked list) Authors secondary index

    map<int, short> booksPrimaryIndex;      // (ISBN, byte offset) Books primary index
    map<char[15], int> booksSecondaryIndex; // (Author ID, First record in linked list) Books secondary index

public:
    // Create the author primary index file
    void createAuthorPrimaryIndex() {
        // Open the data file in input mode
        fstream authors(authorsFile, ios::in | ios::binary);

        // Skip the header value
        authors.seekg(2, ios::beg);

        // Create the index file
        while (authors) { // Read the record size to be able to jump to the end of the record
            short recordSize;
            authors.read((char *) &recordSize, sizeof(short));
            // Store the byte offset of the current record
            short tempOffset = authors.tellg();
            if (tempOffset == -1) // If the file has no records, just the header
                break;
            // Read the value of author id
            int tempID = 0;
            char authorID[15];
            authors.read((char *) &authorID, sizeof(authorID));
            // Convert the authorID from character array into integer
            for (int i = 0; authorID[i] != '\0' and authorID[i] != lengthDelimiter; i++) {
                tempID *= 10;
                tempID += (authorID[i] - '0');
            }
            // Jump to the next record, minus 17 to compensate the read of the record size itself (2 bytes)
            // And the rest (15 bytes) to compensate the dummy read of authorID character array
            authors.seekg(recordSize - 17, ios::cur);
            // Insert the record into the map to be sorted in memory by the author id
            authorsPrimaryIndex.insert({tempID, tempOffset});
        }
        authors.close();

        // Open the file in output mode
        fstream authorPrimIdx(authorsPrimaryIndexFile, ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        // Before appending any records in it
        isAuthorPrimIdxUpToDate = 0;
        authorPrimIdx.seekp(0, ios::beg);
        authorPrimIdx.write((char *) &isAuthorPrimIdxUpToDate, sizeof(short));
        authorPrimIdx.close();

        // Save the index file to disk
        saveAuthorPrimaryIndex();
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveAuthorPrimaryIndex() {
        // Open the file in multiple modes
        fstream authorPrimIdx(authorsPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        authorPrimIdx.seekg(0, ios::beg);
        authorPrimIdx.read((char *) &isAuthorPrimIdxUpToDate, sizeof(short));

        // If the file is already up to date, do not write & exit
        if (isAuthorPrimIdxUpToDate == 1)
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record: authorsPrimaryIndex) {
            authorPrimIdx.write((char *) &record.first, sizeof(int));    // Write the authorID
            authorPrimIdx.write((char *) &record.second, sizeof(short)); // Write the byte offset
        }

        // Update the file status to be up to date, by setting isAuthorPrimIdxUpToDate to 1
        isAuthorPrimIdxUpToDate = 1;
        authorPrimIdx.seekp(0, ios::beg);
        authorPrimIdx.write((char *) &isAuthorPrimIdxUpToDate, sizeof(short));
        authorPrimIdx.close();
    }

    // Load authors primary index file into memory
    void loadAuthorPrimaryIndex() {
        // Open the index file in input mode
        fstream authorPrimIdx(authorsPrimaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        authorPrimIdx.seekg(0, ios::end);
        short endOffset = authorPrimIdx.tellg();

        // Check the status field
        authorPrimIdx.seekg(0, ios::beg);
        authorPrimIdx.read((char *) &isAuthorPrimIdxUpToDate, sizeof(short));

        // If the file is outdated, recreate it
        if (isAuthorPrimIdxUpToDate != 1) {
            createAuthorPrimaryIndex();
            return;
        }

        // Insert all records into the authors primary index map
        while (authorPrimIdx) { // If reached the end of file, exit
            if (authorPrimIdx.tellg() == endOffset)
                break;
            int tempID;
            short tempOffset;
            authorPrimIdx.read((char *) &tempID, sizeof(int));
            authorPrimIdx.read((char *) &tempOffset, sizeof(short));
            // Insert the record into the map to be sorted in memory by the author id
            authorsPrimaryIndex.insert({tempID, tempOffset});
        }
        authorPrimIdx.close();
    }

    // Add a new author helper function
    void addAuthor() {
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
    void addAuthorToDataFile(Author &author) {
        short header, recordSize, idSize, nameSize, addSize;
        // Open the file in multiple modes
        fstream authors(authorsFile, ios::in | ios::out | ios::app | ios::binary);

        // Read the header of the file
        authors.seekg(0);
        authors.read((char *) &header, sizeof(header));

        // Get the sizes of the id, name & address
        idSize = strlen(author.authorID);
        nameSize = strlen(author.authorName);
        addSize = strlen(author.address);

        // Calculate the total record length
        // 5 refers to 2 bytes for the record length indicator
        // + 3 bytes for 3 delimeters between fields
        recordSize = idSize + nameSize + addSize + 5;

        // If the avail list is empty, insert at the end of the file
        if (header == -1) {
            authors.seekp(0, ios::end); // Seek to the end of file
            authors.write((char *) &recordSize, sizeof(recordSize));
            short offset = authors.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            authors.write(author.authorID, idSize);
            authors.write((char *) &lengthDelimiter, 1);
            authors.write(author.authorName, nameSize);
            authors.write((char *) &lengthDelimiter, 1);
            authors.write(author.address, addSize);
            authors.write((char *) &lengthDelimiter, 1);
            // Add the new author to the primary index file
            addAuthorToPrimaryIndexFile(author.authorID, offset);
            cout << "\tNew Author Added Successfully!\n";
            authors.close();
        }
    }

    // Add the new author to the primary index file
    void addAuthorToPrimaryIndexFile(char authorID[], short byteOffest) {
        int authID = 0;
        // Convert the authorID from character array into integer
        for (int i = 0; authorID[i] != '\0'; i++) {
            authID *= 10;
            authID += (authorID[i] - '0');
        }

        // Insert the new record into the map to be automatically sorted by authorID
        authorsPrimaryIndex.insert({authID, byteOffest});

        // Open the index file in output mode
        fstream authorPrimIdx(authorsPrimaryIndexFile, ios::out | ios::binary);
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        isAuthorPrimIdxUpToDate = 0;
        authorPrimIdx.seekp(0);
        authorPrimIdx.write((char *) &isAuthorPrimIdxUpToDate, sizeof(short));
        authorPrimIdx.close();
    }

    void addAuthorToSecondaryIndexFile(Author author) {}

    void addAuthorToSecondaryIndexLinkedListFile(Author author) {}

    // Create the book primary index file
    void createBookPrimaryIndex() {
        // Open the data file in input mode
        fstream books(booksFile, ios::in | ios::binary);

        // Skip the header value
        books.seekg(2, ios::beg);

        // Create the index file
        while (books) { // Read the record size to be able to jump to the end of the record
            short recordSize;
            books.read((char *) &recordSize, sizeof(short));
            // Store the byte offset of the current record
            short tempOffset = books.tellg();
            if (tempOffset == -1) // If the file has no records, just the header
                break;
            // Read the value of book isbn
            int tempISBN = 0;
            char ISBN[15];
            books.read((char *) &ISBN, sizeof(ISBN));
            // Convert the ISBN from character array into integer
            for (int i = 0; ISBN[i] != '\0' and ISBN[i] != lengthDelimiter; i++) {
                tempISBN *= 10;
                tempISBN += (ISBN[i] - '0');
            }
            // Jump to the next record, minus 17 to compensate the read of the record size itself (2 bytes)
            // And the rest (15 bytes) to compensate the dummy read of ISBN character array
            books.seekg(recordSize - 17, ios::cur);
            // Insert the record into the map to be sorted in memory by the author id
            booksPrimaryIndex.insert({tempISBN, tempOffset});
        }
        books.close();

        // Open the file in output mode
        fstream bookPrimIdx(booksPrimaryIndexFile, ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        // Before appending any records in it
        isBookPrimIdxUpToDate = 0;
        bookPrimIdx.seekp(0, ios::beg);
        bookPrimIdx.write((char *) &isBookPrimIdxUpToDate, sizeof(short));
        bookPrimIdx.close();

        // Save the index file to disk
        saveBookPrimaryIndex();
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveBookPrimaryIndex() {
        // Open the file in multiple modes
        fstream bookPrimIdx(booksPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        bookPrimIdx.seekg(0, ios::beg);
        bookPrimIdx.read((char *) &isBookPrimIdxUpToDate, sizeof(short));

        // If the file is already up to date, do not write & exit
        if (isBookPrimIdxUpToDate == 1)
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record: booksPrimaryIndex) {
            bookPrimIdx.write((char *) &record.first, sizeof(int));    // Write the ISBN
            bookPrimIdx.write((char *) &record.second, sizeof(short)); // Write the byte offset
        }

        // Update the file status to be up to date, by setting isBookPrimIdxUpToDate to 1
        isBookPrimIdxUpToDate = 1;
        bookPrimIdx.seekp(0, ios::beg);
        bookPrimIdx.write((char *) &isBookPrimIdxUpToDate, sizeof(short));
        bookPrimIdx.close();
    }

    // Load books primary index file into memory
    void loadBookPrimaryIndex() {
        // Open the index file in input mode
        fstream bookPrimIdx(booksPrimaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        bookPrimIdx.seekg(0, ios::end);
        short endOffset = bookPrimIdx.tellg();

        // Check the status field
        bookPrimIdx.seekg(0, ios::beg);
        bookPrimIdx.read((char *) &isBookPrimIdxUpToDate, sizeof(short));

        // If the file is outdated, recreate it
        if (isBookPrimIdxUpToDate != 1) {
            createBookPrimaryIndex();
            return;
        }

        // Insert all records into the books primary index map
        while (bookPrimIdx) { // If reached the end of file, exit
            if (bookPrimIdx.tellg() == endOffset)
                break;
            int tempISBN;
            short tempOffset;
            bookPrimIdx.read((char *) &tempISBN, sizeof(int));
            bookPrimIdx.read((char *) &tempOffset, sizeof(short));
            // Insert the record into the map to be sorted in memory by the book isbn
            booksPrimaryIndex.insert({tempISBN, tempOffset});
            cout << "Book ISBN ==> " << tempISBN << ", Offset ==> " << tempOffset << "\n";
        }
        bookPrimIdx.close();
    }

    // Add a new book helper function
    void addBook() {
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
    void addBookToDataFile(Book &book) {
        short header, recordSize, isbnSize, titleSize, authIdSize;
        // Open the file in multiple modes
        fstream books(booksFile, ios::in | ios::out | ios::app | ios::binary);

        // Read the header of the file
        books.seekg(0);
        books.read((char *) &header, sizeof(header));

        // Get the sizes of the isbn, title & author id
        isbnSize = strlen(book.ISBN);
        titleSize = strlen(book.bookTitle);
        authIdSize = strlen(book.authorID);

        // Calculate the total record length
        // 5 refers to 2 bytes for the record length indicator
        // + 3 bytes for 3 delimeters between fields
        recordSize = isbnSize + titleSize + authIdSize + 5;

        // If the avail list is empty, insert at the end of the file
        if (header == -1) {
            books.seekp(0, ios::end); // Seek to the end of file
            books.write((char *) &recordSize, sizeof(recordSize));
            short offset = books.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            books.write(book.ISBN, isbnSize);
            books.write((char *) &lengthDelimiter, 1);
            books.write(book.bookTitle, titleSize);
            books.write((char *) &lengthDelimiter, 1);
            books.write(book.authorID, authIdSize);
            books.write((char *) &lengthDelimiter, 1);
            // Add the new book to the primary index file
            addBookToPrimaryIndexFile(book.ISBN, offset);
            cout << "\tNew Book Added Successfully!\n";
            books.close();
        }
    }

    // Add the new book to the primary index file
    void addBookToPrimaryIndexFile(char ISBN[], short byteOffest) {
        int bookISBN = 0;
        // Convert the ISBN from character array into integer
        for (int i = 0; ISBN[i] != '\0'; i++) {
            bookISBN *= 10;
            bookISBN += (ISBN[i] - '0');
        }

        // Insert the new record into the map to be automatically sorted by ISBN
        booksPrimaryIndex.insert({bookISBN, byteOffest});

        // Open the index file in output mode
        fstream bookPrimIdx(booksPrimaryIndexFile, ios::out | ios::binary);
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        isBookPrimIdxUpToDate = 0;
        bookPrimIdx.seekp(0);
        bookPrimIdx.write((char *) &isBookPrimIdxUpToDate, sizeof(short));
        bookPrimIdx.close();
    }

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
    void printAuthor() {
        int authorID;
        cout << "Enter author ID: ";
        cin >> authorID;
        auto id = authorsPrimaryIndex.lower_bound(authorID);
        if (id != authorsPrimaryIndex.end() && id->first == authorID) {
            int offset = id->second;
            fstream authors(authorsFile, ios::in | ios::binary);
            authors.seekg(offset, ios::beg);
            Author author{};
            authors.getline(author.authorID, 15, '|');
            authors.getline(author.authorName, 30, '|');
            authors.getline(author.address, 30, '|');
            cout << "Author ID: " << author.authorID << "\n";
            cout << "Author Name: " << author.authorName << "\n";
            cout << "Author Address: " << author.address << "\n";
        }
        else cout << "Author does not exist\n";
    }

    // Print book using ISBN
    void printBook() {
        int ISBN;
        cout << "Enter Book ISBN: ";
        cin >> ISBN;
        auto id = booksPrimaryIndex.lower_bound(ISBN);
        if (id != booksPrimaryIndex.end() && id->first == ISBN) {
            int offset = id->second;
            fstream books(booksFile, ios::in | ios::binary);
            books.seekg(offset + 1, ios::beg);
            Book book{};
            books.getline(book.ISBN, 15, '|');
            books.getline(book.bookTitle, 30, '|');
            books.getline(book.authorID, 15, '|');
            cout << "Book ISBN: " << book.ISBN << "\n";
            cout << "Book Title: " << book.bookTitle << "\n";
            cout << "Author ID: " << book.authorID << "\n";
        }
        else cout << "Book does not exist\n";
    }

    // Handle the select query
    void writeQuery() {
//        string selectedColumns, tables, condition;
//        int fromPointer = (int)query.find("from");
//        int wherePointer = (int)query.find("where");
//        selectedColumns = query.substr(7, fromPointer - 7);
//        tables
    }

    // Main method to start the program
    void start() {
        cout << "\tWelcome to the Library Catalog System!\n";
        // Load index files into memory
        loadAuthorPrimaryIndex();
        loadBookPrimaryIndex();
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
            }
            else if (choice == "7")printAuthor();
            else if (choice == "8")printBook();
            else if (choice == "9")writeQuery();
            else if (choice == "10") {
                terminate();
                break;
            } else {
                cerr << "\n\tINVALID CHOICE!!\n";
            }
        }
    }

    // Terminate the program correctly with saved changes
    void terminate() {
        // Save the primary index files to the disk with the updated indices
        saveAuthorPrimaryIndex();
        saveBookPrimaryIndex();
        cout << "\tThank you for using our Library System!\n";
    }
};

// Start the program
int main() {
    Library lib;
    lib.start();
    return 0;
}