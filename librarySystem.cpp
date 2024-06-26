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

    map<long long, short> authorsPrimaryIndex;   // (Author ID, byte offset) Authors primary index
    map<long long, short> authorsSecondaryIndex; // (Author Name, First record in linked list) Authors secondary index

    map<long long, short> booksPrimaryIndex;   // (ISBN, byte offset) Books primary index
    map<long long, short> booksSecondaryIndex; // (Author ID, First record in linked list) Books secondary index

public:
    // Create the author primary index file
    void createAuthorPrimaryIndex()
    {
        // Open the data file in input mode
        fstream authors(authorsFile, ios::in | ios::binary);

        // Skip the header value
        authors.seekg(2, ios::beg);

        // Create the index file
        while (authors)
        { // Read the record size to be able to jump to the end of the record
            short recordSize;
            authors.read((char *)&recordSize, sizeof(short));
            // Store the byte offset of the current record
            short tempOffset = authors.tellg();
            if (tempOffset == -1) // If the file has no records, just the header
                break;

            // Read the value of author id
            char authorID[15];
            authors.read((char *)&authorID, sizeof(authorID));

            // Convert the authorID from character array into long long
            long long tempID = convertCharArrToLongLong(authorID);

            // Jump to the next record, minus 17 to compensate the read of the record size itself (2 bytes)
            // And the rest (15 bytes) to compensate the dummy read of authorID character array
            authors.seekg(recordSize - 17, ios::cur);
            // Insert the record into the map to be sorted in memory by the author id
            authorsPrimaryIndex.insert({tempID, tempOffset});
        }
        authors.close();

        // Write the status flag at the beginning of the file
        markAuthorsPrimaryIndexFlag('0');

        // Save the index file to disk
        saveAuthorPrimaryIndex();
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveAuthorPrimaryIndex()
    {
        // Open the file in multiple modes
        fstream authorPrimIdx(authorsPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        char isAuthorPrimIdxUpToDate;
        authorPrimIdx.seekg(0, ios::beg);
        authorPrimIdx.read((char *)&isAuthorPrimIdxUpToDate, sizeof(char));

        // If the file is already up to date, do not write & exit
        if (isAuthorPrimIdxUpToDate == '1')
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record : authorsPrimaryIndex)
        {
            authorPrimIdx.write((char *)&record.first, sizeof(long long)); // Write the authorID
            authorPrimIdx.write((char *)&record.second, sizeof(short));    // Write the byte offset
        }
        authorPrimIdx.close();

        // Update the file status to be up to date
        markAuthorsPrimaryIndexFlag('1');
    }

    // Load authors primary index file into memory
    void loadAuthorPrimaryIndex()
    {
        // Open the index file in input mode
        fstream authorPrimIdx(authorsPrimaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        authorPrimIdx.seekg(0, ios::end);
        short endOffset = authorPrimIdx.tellg();

        // Check the status field
        char isAuthorPrimIdxUpToDate;
        authorPrimIdx.seekg(0, ios::beg);
        authorPrimIdx.read((char *)&isAuthorPrimIdxUpToDate, sizeof(char));

        // If the file is outdated, recreate it
        if (isAuthorPrimIdxUpToDate != '1')
        {
            createAuthorPrimaryIndex();
            return;
        }

        // Insert all records into the authors primary index map
        while (authorPrimIdx)
        { // If reached the end of file, exit
            if (authorPrimIdx.tellg() == endOffset)
                break;
            long long tempID;
            short tempOffset;
            authorPrimIdx.read((char *)&tempID, sizeof(long long));
            authorPrimIdx.read((char *)&tempOffset, sizeof(short));
            // Insert the record into the map to be sorted in memory by the author id
            authorsPrimaryIndex.insert({tempID, tempOffset});
        }
        authorPrimIdx.close();
    }

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
    void addAuthorToDataFile(Author &author)
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
            addAuthorToPrimaryIndexFile(author.authorID, offset);
            cout << "\tNew Author Added Successfully!\n";
            authors.close();
        }
    }

    // Add the new author to the primary index file
    void addAuthorToPrimaryIndexFile(char authorID[], short byteOffset)
    {
        // Convert the authorID from character array into long long
        long long authID = convertCharArrToLongLong(authorID);

        // Insert the new record into the map to be automatically sorted by authorID
        authorsPrimaryIndex.insert({authID, byteOffset});

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        markAuthorsPrimaryIndexFlag('0');
    }

    void addAuthorToSecondaryIndexFile(Author author) {}

    void addAuthorToSecondaryIndexLinkedListFile(Author author) {}

    // Create the book primary index file
    void createBookPrimaryIndex()
    {
        // Open the data file in input mode
        fstream books(booksFile, ios::in | ios::binary);

        // Skip the header value
        books.seekg(2, ios::beg);

        // Create the index file
        while (books)
        {
            int recordOffset = books.tellg();
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
                continue;
            }
            else // Return the cursor back one character that was read
                books.seekg(-1, ios::cur);

            // Read the record size to be able to jump to the end of the record
            short recordSize;
            books.read((char *)&recordSize, sizeof(short));
            // Store the byte offset of the current record
            short tempOffset = books.tellg();
            if (tempOffset == -1) // If the file has no records, just the header
                break;

            // Read the value of book isbn
            char ISBN[15];
            books.read((char *)&ISBN, sizeof(ISBN));

            // Convert the ISBN from character array into long long
            long long tempISBN = convertCharArrToLongLong(ISBN);

            // Jump to the next record, minus 17 to compensate the read of the record size itself (2 bytes)
            // And the rest (15 bytes) to compensate the dummy read of ISBN character array
            books.seekg(recordSize - 17, ios::cur);
            // Insert the record into the map to be sorted in memory by the author id
            booksPrimaryIndex.insert({tempISBN, tempOffset});
        }
        books.close();

        // Write the status flag at the beginning of the file
        markBooksPrimaryIndexFlag('0');

        // Save the index file to disk
        saveBookPrimaryIndex();
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveBookPrimaryIndex()
    {
        // Open the file in multiple modes
        fstream bookPrimIdx(booksPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        char isBookPrimIdxUpToDate;
        bookPrimIdx.seekg(0, ios::beg);
        bookPrimIdx.read((char *)&isBookPrimIdxUpToDate, sizeof(char));

        // If the file is already up to date, do not write & exit
        if (isBookPrimIdxUpToDate == '1')
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record : booksPrimaryIndex)
        {
            bookPrimIdx.write((char *)&record.first, sizeof(long long)); // Write the ISBN
            bookPrimIdx.write((char *)&record.second, sizeof(short));    // Write the byte offset
        }
        bookPrimIdx.close();

        // Update the file status to be up to date
        markBooksPrimaryIndexFlag('1');
    }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveBookSecondaryIndex()
    {
        // Open the file in multiple modes
        fstream booksSecondaryIndexFstream(booksSecondaryIndexFile, ios::in | ios::out | ios::binary);

        // Read the status flag
        char isBookSecondaryIdxUpToDate;
        booksSecondaryIndexFstream.seekg(0, ios::beg);
        booksSecondaryIndexFstream.read((char *)&isBookSecondaryIdxUpToDate, sizeof(char));

        // If the file is already up to date, do not write & exit
        if (isBookSecondaryIdxUpToDate == '1')
            return;

        // Otherwise if the file is not up to date OR it is the first time to save it, write it to disk
        // Update the index file by rewriting it back to disk after inserting into the map
        for (auto record : booksSecondaryIndex)
        {
            booksSecondaryIndexFstream.write((char *)&record.first, sizeof(long long)); // Write the author ID
            booksSecondaryIndexFstream.write((char *)&record.second, sizeof(short));    // Write the first record pointer (RRN) in the inverted list file
        }
        booksSecondaryIndexFstream.close();

        // Update the file status to be up to date
        markBooksSecondaryIndexFlag('1');
    }

    // Load books primary index file into memory
    void loadBookPrimaryIndex()
    {
        // Open the index file in input mode
        fstream bookPrimIdx(booksPrimaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        bookPrimIdx.seekg(0, ios::end);
        short endOffset = bookPrimIdx.tellg();

        // Check the status field
        char isBookPrimIdxUpToDate;
        bookPrimIdx.seekg(0, ios::beg);
        bookPrimIdx.read((char *)&isBookPrimIdxUpToDate, sizeof(char));

        // If the file is outdated, recreate it
        if (isBookPrimIdxUpToDate != '1')
        {
            createBookPrimaryIndex();
            return;
        }

        // Insert all records into the books primary index map
        while (bookPrimIdx)
        { // If reached the end of file, exit
            if (bookPrimIdx.tellg() == endOffset)
                break;
            long long tempISBN;
            short tempOffset;
            bookPrimIdx.read((char *)&tempISBN, sizeof(long long));
            bookPrimIdx.read((char *)&tempOffset, sizeof(short));
            // Insert the record into the map to be sorted in memory by the book isbn
            booksPrimaryIndex.insert({tempISBN, tempOffset});
        }
        bookPrimIdx.close();
    }

    // Load books secondary index file into memory
    void loadBookSecondaryIndex()
    {
        // Open the index file in input mode
        fstream booksSecondaryIndexFileFstream(booksSecondaryIndexFile, ios::in | ios::binary);

        // Get the file size, store its offset in endOffset
        booksSecondaryIndexFileFstream.seekg(0, ios::end);
        short endOffset = booksSecondaryIndexFileFstream.tellg();

        // Check the status field
        char isBookPrimIdxUpToDate;
        booksSecondaryIndexFileFstream.seekg(0, ios::beg);
        booksSecondaryIndexFileFstream.read((char *)&isBookPrimIdxUpToDate, sizeof(char));

        // If the file is outdated, recreate it
        if (isBookPrimIdxUpToDate != '1')
        {
            booksSecondaryIndexFileFstream.seekp(0, ios::beg);
            char updatedSymbol = '0';
            booksSecondaryIndexFileFstream.write((char *)&updatedSymbol, sizeof(char));
            return;
        }

        // Insert all records into the books primary index map
        while (booksSecondaryIndexFileFstream)
        { // If reached the end of file, exit
            if (booksSecondaryIndexFileFstream.tellg() == endOffset)
                break;
            long long tempAuthorId;
            short tempRecordPointer;
            booksSecondaryIndexFileFstream.read((char *)&tempAuthorId, sizeof(long long));
            booksSecondaryIndexFileFstream.read((char *)&tempRecordPointer, sizeof(short));
            // Insert the record into the map to be sorted in memory by the book isbn
            booksSecondaryIndex.insert({tempAuthorId, tempRecordPointer});
        }
        booksSecondaryIndexFileFstream.close();
    }

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
    void addBookToDataFile(Book &book)
    {
        short header, recordSize, isbnSize, titleSize, authIdSize;
        // Open the file in multiple modes
        fstream books(booksFile, ios::in | ios::out | ios::binary);

        // Read the header of the file
        books.seekg(0, ios::beg);
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
            addBookToPrimaryIndexFile(book.ISBN, offset);
            addBookToSecondaryIndexFile(book);
            cout << "\tNew Book Added Successfully!\n";
            books.close();
        }
        else
        { // Otherwise, if the avail list is NOT empty
            short prevOffset = header, deletedSize, differSize, tempOffset;
            bool fragmentation = false;
            char deleteChar;

            // Seek to the offset of the last deleted record
            books.seekg(header, ios::beg);
            // First fit algorithm to find the first large enough record to insert the new record into
            while (true)
            {
                tempOffset = prevOffset; // Store the previous offset before overriding its value
                // Read the first character of the record to check if it's REALLY a deleted record or not
                books.read((char *)&deleteChar, 1);
                // If it's NOT a deleted record, break the loop
                if (deleteChar != '*')
                { // Fake the differSize, in order to force insert the new record at the end of the file
                    differSize = -1;
                    header = -1;
                    // Seek to the beginning of file & update the header to be -1
                    books.seekp(0, ios::beg);
                    books.write((char *)&header, sizeof(header));
                    break;
                }
                // Otherwise, continue parsing the record
                books.ignore(1);                                       // Ignore the delimiter '|'
                books.read((char *)&prevOffset, sizeof(prevOffset));   // Read the offset of the previous deleted record
                books.ignore(1);                                       // Ignore the delimiter '|'
                books.read((char *)&deletedSize, sizeof(deletedSize)); // Read the size of the current deleted record

                // Calculate the difference in size between the deleted record & the new record to insert
                // In order to decide the exact position to insert the new record
                differSize = deletedSize - recordSize;

                // If the difference is positive, means the current deleted record is suitable to insert in
                // As the size of the new record is smaller than the deleted record
                if (differSize >= 0)
                { // If this is NOT the first deleted record
                    if (prevOffset != -1)
                    {
                        // Update the header to be the offset of the previous deleted record
                        books.seekp(0, ios::beg);
                        books.write((char *)&prevOffset, sizeof(prevOffset));
                    }
                    break;
                }

                // If this is the first deleted record & no other records before it satisfy the condition of size
                // Break the loop to insert at the end of the file and keep the header as it is
                if (prevOffset == -1)
                    break;

                // Otherwise, continue the loop
                books.seekg(prevOffset, ios::beg);
            }

            // If the difference is negative, then the new record is bigger than the deleted one
            if (differSize < 0)
                // Insert at the end of the file and keep the header value as it is
                books.seekp(0, ios::end);
            else
            { // Otherwise, the difference is positive, then the new record size is smaller than the deleted one
                // Seek to the deleted record offset to write the new record
                books.seekp(tempOffset, ios::beg);
                // Add the difference in size to the current record size in order to apply internal fragmentation
                recordSize += differSize;
                // Flag to indicate that internal fragmentation is required
                // Check if the sizes are equal (meaning the differSize is zero), then do NOT apply fragmentation
                fragmentation = (differSize == 0) ? false : true;
            }

            // Write the record size of the new record
            books.write((char *)&recordSize, sizeof(recordSize));
            short offset = books.tellp(); // Store the byteoffset of the new record
            // Write the rest of record fields separated by delimeters
            books.write(book.ISBN, isbnSize);
            books.write((char *)&lengthDelimiter, 1);
            books.write(book.bookTitle, titleSize);
            books.write((char *)&lengthDelimiter, 1);
            books.write(book.authorID, authIdSize);
            books.write((char *)&lengthDelimiter, 1);

            // Check for internal fragmentation
            if (fragmentation)
            { // Apply internal fragmentation and fill the rest of record space with telda '~'
                short iterator = books.tellp();
                while (iterator < (tempOffset + recordSize - 2) && tempOffset != -1)
                {
                    books.write("~", 1);
                    iterator++;
                }
                // Write the last delimiter after the telda to indicate the end of this record
                books.write((char *)&lengthDelimiter, 1);
            }

            // Read the header of the file again to get the latest update
            books.seekg(0, ios::beg);
            books.read((char *)&header, sizeof(header));
            // Fix the avail list pointers to avoid corrupted pointers
            fixAvailList(header, booksFile);
            // Add the new book to the primary index file
            addBookToPrimaryIndexFile(book.ISBN, offset);
            cout << "\tNew Book Added Successfully!\n";
            books.close();
        }
    }

    // Add the new book to the primary index file
    void addBookToPrimaryIndexFile(char ISBN[], short byteOffset)
    {
        // Convert the ISBN from character array into long long
        long long bookISBN = convertCharArrToLongLong(ISBN);

        // Insert the new record into the map to be automatically sorted by ISBN
        booksPrimaryIndex.insert({bookISBN, byteOffset});

        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        markBooksPrimaryIndexFlag('0');
    }

    void addBookToSecondaryIndexFile(Book book)
    {
        long long authorId = convertCharArrToLongLong(book.authorID);
        long long ISBN = convertCharArrToLongLong(book.ISBN);

        if (booksSecondaryIndex.count(authorId) == 0)
        {
            cout << "Empty list\n";
            booksSecondaryIndex[authorId] = addToInvertedList(ISBN, -1); // RRN
            cout << "Added value: " << booksSecondaryIndex[authorId] << "\n";
        }
        else
        {
            booksSecondaryIndex[authorId] = addToInvertedList(ISBN, booksSecondaryIndex[authorId]);
        }
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        markBooksSecondaryIndexFlag('0');
    }

    short addToInvertedList(long long ISBN, short nextRecordPointer)
    {
        // Open the file in multiple modes
        fstream invertedList(booksSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);

        short bestOffset = getBestOffsetInInvertedList();
        cout << "Best palce to insert: " << bestOffset << "\n";
        invertedList.seekp(bestOffset, ios::beg);
        if (invertedList.fail())
        {
            cout << "Failed to open the file.\n";
            return -1; // or handle the error appropriately
        }
        cout << "Writing to the inverted list file......\n";
        cout << "ISBN = " << ISBN << "\n";
        cout << "nextRecordPointer = " << nextRecordPointer << "\n";
        // Write the ISBN & the next record pointer
        invertedList.write((char *)&ISBN, sizeof(long long));
        invertedList.write((char *)&nextRecordPointer, sizeof(short));

        invertedList.close();

        // Return the RRN of the new record
        return bestOffset / (sizeof(long long) + sizeof(short)); // RRN
    }

    short getBestOffsetInInvertedList()
    {
        fstream invertedList(booksSecondaryIndexLinkedListFile, ios::in | ios::binary);

        invertedList.seekg(0, ios::end);
        int endOffset = invertedList.tellg();
        invertedList.seekg(0, ios::beg);

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
        return endOffset;
    }

    // Update author name using author ID
    void updateAuthorName() {}

    void updateAuthorNameInDataFile(char newAuthorName[3], int byteOffset) {}

    void updateAuthorNameInSecondaryIndex(char oldAuthorName[30], char newAuthorName[30]) {}

    // Update book title using ISBN
    void updateBookTitle()
    {
        int ISBN;
        int offset;
        char newTitle[30];
        cout << "Enter book ISBN: ";
        cin >> ISBN;
        cout << "Enter new book title: ";
        cin.ignore();
        cin.getline(newTitle, 30);

        // searching for the record which will be updated in runtime
        auto isbn = booksPrimaryIndex.lower_bound(ISBN);
        if (isbn != booksPrimaryIndex.end() && isbn->first == ISBN)
        {
            offset = isbn->second;
            updateBookTitleInDataFile(newTitle, offset);
        }
    }

    // applying the updates in the books data file
    void updateBookTitleInDataFile(char newBookTitle[30], int byteOffset)
    {
        fstream dataFile(booksFile, ios::in | ios::out | ios::binary);
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

        // case 1 --> length of new title > length of old title
        if (strlen(newBookTitle) > maxSize)
        {
            newBookTitle[maxSize] = '\0';
            dataFile.write(newBookTitle, maxSize);
        }
        // case 2 --> length of new title < length of old title
        else if (strlen(newBookTitle) < maxSize)
        {
            for (int i = strlen(newBookTitle); i < maxSize; i++)
            {
                newBookTitle[i] = '_';
            }
            dataFile.write(newBookTitle, maxSize);
        }
        // case 3 --> length of new title == length of old title
        else
        {
            dataFile.write(newBookTitle, maxSize);
        }

        dataFile.close();
        cout << "\tBook title is updated successfully\n";
    }

    // Delete an author using author ID
    void deleteAuthor() {}

    void deleteFromAuthorsDataFile(int byteOffset) {}

    void deleteFromAuthorsPrimaryIndexFile(char authorId[30]) {}

    void deleteFromAuthorsSecondaryIndexFile(char authorName[30]) {}

    void deleteFromAuthorsSecondaryIndexLinkedListFile(int firstPosition, char authorId[30]) {}

    // Delete a book using ISBN
    void deleteBook()
    {
        long long ISBN;
        cout << "Enter Book ISBN: ";
        cin >> ISBN;
        cin.ignore();
        auto isbn = booksPrimaryIndex.lower_bound(ISBN);
        if (isbn != booksPrimaryIndex.end() && isbn->first == ISBN)
        {
            short recordStartoffset = isbn->second - 2;
            deleteFromBooksSecondaryIndexFile(ISBN, getBookAuthorIdAt(recordStartoffset));
            deleteFromBooksPrimaryIndexFile(ISBN);
            deleteFromBooksDataFile(recordStartoffset);
        }
        else
        {
            cout << "\tBook does not exist\n";
        }
    }

    void deleteFromBooksDataFile(int recordStartoffset)
    {
        fstream books(booksFile, ios::in | ios::out | ios::binary);
        short header, recordSize;
        books.read((char *)&header, sizeof(header));
        books.seekg(recordStartoffset, ios::beg);
        books.read((char *)&recordSize, sizeof(short));
        // string overrideText = "*|" + to_string(header) + "|" + to_string(recordSize) + "|";
        books.seekp(recordStartoffset, ios::beg);
        books.write((char *)"*", 1);
        books.write((char *)&lengthDelimiter, 1);
        books.write((char *)&header, sizeof(header));
        books.write((char *)&lengthDelimiter, 1);
        books.write((char *)&recordSize, sizeof(recordSize));
        books.write((char *)&lengthDelimiter, 1);
        // update the header
        books.seekp(0, ios::beg);
        books.write((char *)&recordStartoffset, sizeof(short));
        books.close();
    }

    void deleteFromBooksPrimaryIndexFile(int ISBN)
    {
        booksPrimaryIndex.erase(ISBN);
        // Update the status of the file to be NOT up to date, to save it to the disk afterward
        markBooksPrimaryIndexFlag('0');
    }

    long long getBookAuthorIdAt(short startOffset)
    {
        fstream books(booksFile, ios::in | ios::binary);
        Book tmpBook;
        books.seekg(startOffset, ios::beg);
        books.ignore(2); // ignore the record size
        books.getline(tmpBook.ISBN, 15, '|');
        books.getline(tmpBook.bookTitle, 30, '|');
        books.getline(tmpBook.authorID, 15, '|');
        books.close();
        return convertCharArrToLongLong(tmpBook.authorID);
    }

    void deleteFromBooksSecondaryIndexFile(long long ISBN, long long authorId)
    {
        short oldPointer = booksSecondaryIndex[authorId];
        short newPointer = deleteFromBooksSecondaryIndexLinkedListFile(oldPointer, ISBN);
        if (newPointer == -1)
            booksSecondaryIndex.erase(authorId);
        else
            booksSecondaryIndex[authorId] = newPointer;
    }

    short deleteFromBooksSecondaryIndexLinkedListFile(int firstPosition, long long ISBN)
    {
        // Keep the rpevious record offset (Not the RRN)
        short previousRecordOffset = -1;

        fstream invertedList(booksSecondaryIndexLinkedListFile, ios::in | ios::out | ios::binary);
        // char flagLength = 1;
        short deletedSymbol = '#';

        // Size of each record in the linked list is the ISBN + the next record pointer
        short recordSize = sizeof(long long) + sizeof(short);

        // Go to the first record of the linked list
        invertedList.seekg(firstPosition * recordSize, ios::beg);

        while (true)
        {
            // cout << "current offset: " << invertedList.tellg() << "\n";
            // Read the ISBN & the next record pointer from the current record
            long long tmpISBN;
            invertedList.read((char *)&tmpISBN, sizeof(long long));
            short nextRecordPointer;
            invertedList.read((char *)&nextRecordPointer, sizeof(short));

            if (tmpISBN == ISBN)
            {
                // If there is no previous record, then we want to delete this first record
                if (previousRecordOffset == -1)
                {
                    invertedList.seekp(invertedList.tellg() - sizeof(nextRecordPointer), ios::beg);
                    invertedList.write((char *)&deletedSymbol, sizeof(deletedSymbol));
                    invertedList.close();
                    if (nextRecordPointer == -1)
                    {
                        // There are no longer any records in the linked list that belongs to the same author
                        return -1;
                    }
                    else
                    {
                        // Make the secondary index point to the next record of the current deleted record
                        return nextRecordPointer;
                    }
                }
                else
                {
                    // mark the current record as deleted
                    // Go back to the next record pointer field to update it
                    invertedList.seekp(invertedList.tellg() - sizeof(nextRecordPointer), ios::beg);
                    invertedList.write((char *)&deletedSymbol, sizeof(deletedSymbol));

                    // update the previous record to point to the next record of this deleted record
                    invertedList.seekp(previousRecordOffset, ios::beg);
                    invertedList.ignore(sizeof(long long)); // ignore the ISBN
                    invertedList.write((char *)&nextRecordPointer, sizeof(nextRecordPointer));
                    invertedList.close();

                    // The record that the secondary index refers to still exists
                    return firstPosition;
                }
            }
            else
            {
                previousRecordOffset = invertedList.tellg() - recordSize;
                // Go to the next record
                invertedList.seekg(nextRecordPointer * recordSize, ios::beg);
            }
        }
    }

    // Print author using author ID
    void printAuthor()
    {
        int authorID;
        cout << "Enter author ID: ";
        cin >> authorID;
        auto id = authorsPrimaryIndex.lower_bound(authorID);
        if (id != authorsPrimaryIndex.end() && id->first == authorID)
        {
            int offset = id->second;
            fstream authors(authorsFile, ios::in | ios::binary);
            authors.seekg(offset, ios::beg);
            Author author;
            authors.getline(author.authorID, 15, '|');
            authors.getline(author.authorName, 30, '|');
            authors.getline(author.address, 30, '|');
            cout << "\tAuthor #" << author.authorID << " Data\n";
            cout << "Author Name: " << author.authorName << "\n";
            cout << "Author Address: " << author.address << "\n";
        }
        else
            cout << "\tAuthor does not exist\n";
        cin.ignore(); // To keep input stream clean
    }

    // Print book using ISBN
    void printBook()
    {
        int ISBN;
        cout << "Enter Book ISBN: ";
        cin >> ISBN;
        auto isbn = booksPrimaryIndex.lower_bound(ISBN);
        if (isbn != booksPrimaryIndex.end() && isbn->first == ISBN)
        {
            int offset = isbn->second;
            fstream books(booksFile, ios::in | ios::binary);
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

    // Handle the select query
    void writeQuery()
    {
        // Select all from Authors where Author ID=’xxx’;
        // Select all from Books where Author ID=’xxx’;
        // Select Author Name from Authors where Author ID=’xxx’;

        auto removeSpaces = [](string &s) -> void
        {
            while (s.back() == ' ')
                s.pop_back();
            int j = 0;
            while (s[j] == ' ')
                j++;
            s = s.substr(j);
        };

        string query;
        cout << "Enter the query: ";
        getline(cin, query);

        for (char &ch : query)
            if (isupper(ch))
                ch = (char)tolower(ch);

        int fromPointer = (int)query.find("from"), wherePointer = (int)query.find("where");
        // parsing table
        string table = query.substr(fromPointer + 5, wherePointer - fromPointer - 6);
        // parsing columns
        vector<string> columns;
        string selectedColumns = query.substr(7, fromPointer - 8);
        string column;
        for (int i = 0; i < selectedColumns.size(); ++i)
        {
            if (selectedColumns[i] == ',')
            {
                removeSpaces(column);
                columns.push_back(column);
                column.clear();
            }
            else
                column += selectedColumns[i];
            if (i == selectedColumns.size() - 1)
            {
                removeSpaces(column);
                columns.push_back(column);
            }
        }
        // parse conditions
        map<string, string> conditions;
        string totalConditions = query.substr(wherePointer + 5);
        string condition;
        for (int i = 0; i < totalConditions.size(); ++i)
        {
            if (totalConditions[i] == ',' || totalConditions[i] == ';')
            {
                int equalityPointer = (int)condition.find('=');
                string lhs = condition.substr(0, equalityPointer);
                string rhs = condition.substr(equalityPointer + 1);
                removeSpaces(rhs);
                removeSpaces(lhs);
                conditions[lhs] = rhs;
                condition.clear();
            }
            else
                condition += totalConditions[i];
            if (i == totalConditions.size() - 1 && totalConditions[i] != ';')
            {
                int equalityPointer = (int)condition.find('=');
                string lhs = condition.substr(0, equalityPointer);
                string rhs = condition.substr(equalityPointer + 1);
                removeSpaces(rhs);
                removeSpaces(lhs);
                conditions[lhs] = rhs;
                condition.clear();
            }
        }
        if (table == "authors")
        {
            vector<Author> authors;
        }
        else if (table == "books")
        {
            vector<Book> books;
        }
        else
            cerr << "Invalid Query: Table does not exist.\n";
    }

    // Utilities
    void checkFilesExist()
    {
        fstream authors(authorsFile, ios::in | ios::binary);
        // check if the file exists
        if (!authors)
        {
            // create the file
            authors.open(authorsFile, ios::out | ios::binary);
            // write the header
            short header = -1;
            authors.write((char *)&header, sizeof(header));
            authors.close();
        }

        fstream books(booksFile, ios::in | ios::binary);
        // check if the file exists
        if (!books)
        {
            // create the file
            books.open(booksFile, ios::out | ios::binary);
            // write the header
            short header = -1;
            books.write((char *)&header, sizeof(header));
            books.close();
        }

        fstream authorsPrimIdx(authorsPrimaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!authorsPrimIdx)
        {
            // create the file
            authorsPrimIdx.open(authorsPrimaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            authorsPrimIdx.write((char *)&flag, sizeof(char));
            authorsPrimIdx.close();
        }

        fstream authorsSeondaryIndexFileFstream(authorsSecondaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!authorsSeondaryIndexFileFstream)
        {
            // create the file
            authorsSeondaryIndexFileFstream.open(authorsSecondaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            authorsSeondaryIndexFileFstream.write((char *)&flag, sizeof(char));
            authorsSeondaryIndexFileFstream.close();
        }

        fstream booksPrimIdx(booksPrimaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!booksPrimIdx)
        {
            // create the file
            booksPrimIdx.open(booksPrimaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            booksPrimIdx.write((char *)&flag, sizeof(char));
            booksPrimIdx.close();
        }

        fstream booksSecondaryIndexFileFstream(booksSecondaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!booksSecondaryIndexFileFstream)
        {
            // create the file
            booksSecondaryIndexFileFstream.open(booksSecondaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            booksSecondaryIndexFileFstream.write((char *)&flag, sizeof(char));
            booksSecondaryIndexFileFstream.close();
        }

        fstream booksInvertedList(booksSecondaryIndexLinkedListFile, ios::in | ios::binary);
        // check if the file exists
        if (!booksInvertedList)
        {
            // create the file
            booksInvertedList.open(booksSecondaryIndexLinkedListFile, ios::out | ios::binary);
            booksInvertedList.close();
        }
    }

    // Update the status flag of the books primary index file
    void markBooksPrimaryIndexFlag(char value)
    {
        // Open the file in multiple modes
        fstream booksPrimaryFile(booksPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        booksPrimaryFile.seekp(0, ios::beg);
        booksPrimaryFile.write((char *)&value, sizeof(char));
        booksPrimaryFile.close();
    }

    void markBooksSecondaryIndexFlag(char value)
    {
        // Open the file in multiple modes
        fstream booksSecondaryIndexFstream(booksSecondaryIndexFile, ios::in | ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        booksSecondaryIndexFstream.seekp(0, ios::beg);
        booksSecondaryIndexFstream.write((char *)&value, sizeof(char));
        booksSecondaryIndexFstream.close();
    }

    void markBooksSecondaryIndexFlag(short value)
    {
        fstream booksSecondaryFile(booksSecondaryIndexFile, ios::out | ios::binary);
        booksSecondaryFile.write((char *)&value, sizeof(value));
        booksSecondaryFile.close();
    }

    // Update the status flag of the authors primary index file
    void markAuthorsPrimaryIndexFlag(char value)
    {
        // Open the file in multiple modes
        fstream authorsPrimaryFile(authorsPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        authorsPrimaryFile.seekp(0, ios::beg);
        authorsPrimaryFile.write((char *)&value, sizeof(char));
        authorsPrimaryFile.close();
    }

    void markAuthorsSecondaryIndexFlag(char value)
    {
        fstream authorsSecondaryFile(authorsSecondaryIndexFile, ios::out | ios::binary);
        authorsSecondaryFile.write((char *)&value, sizeof(value));
        authorsSecondaryFile.close();
    }

    // Convert array of characters into long long data type
    long long convertCharArrToLongLong(char arr[])
    {
        long long num = 0;
        // Loop over the length of the array or until the length delimiter
        for (int i = 0; arr[i] != '\0' && arr[i] != lengthDelimiter; i++)
        {
            num *= 10;
            num += (arr[i] - '0');
        }
        return num;
    }

    // Insure that avail list pointers are correct after the insertion of a new record
    void fixAvailList(short &headerOffset, string dataFile)
    {
        // Open the file in multiple modes
        fstream file(dataFile, ios::in | ios::out | ios::binary);

        // Read the offset of the previous deleted record
        short prevOffset;
        file.seekg(headerOffset + 2, ios::beg); // Skip the two characters '*' & the length delimiter '|'
        file.read((char *)&prevOffset, sizeof(prevOffset));

        // Seek to the previous offset to check if it points correctly to the previous deleted record
        char deleteChar;
        file.seekg(prevOffset, ios::beg);
        file.read((char *)&deleteChar, 1);
        // If it does NOT point to a deleted record, modify its pointer to be -1
        if (deleteChar != '*')
        {
            short newOffset = -1;
            file.seekp(headerOffset + 2, ios::beg);
            file.write((char *)&newOffset, sizeof(newOffset));
        }
        else // Otherwise, check for its previous offset recursively
        {
            short tempOffset;
            file.ignore(1); // Ignore the delimiter '|'
            file.read((char *)&tempOffset, sizeof(tempOffset));
            fixAvailList(tempOffset, dataFile);
        }
    }

    // For testing only
    void printBooksFile()
    {
        cout << "----------------------------\n";
        fstream books(booksFile, ios::in | ios::binary);

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
        fstream books(booksFile, ios::in | ios::binary);
        short header;
        books.read((char *)&header, sizeof(header));
        cout << "Header: " << header << "\n";
        string wholeText;
        getline(books, wholeText);
        cout << wholeText << "\n";
        cout << "--------------\n";
        books.close();
    }

    void printAuthorsFile()
    {
        cout << "--------------\n";
        fstream authors(authorsFile, ios::in | ios::binary);
        short header;
        authors.read((char *)&header, sizeof(header));
        cout << "Header: " << header << "\n";
        while (authors.eof() == false)
        {
            cout << "##############\n";
            short recordSize;
            authors.read((char *)&recordSize, sizeof(short));
            if (recordSize == -1)
                break;
            cout << "Record Size: " << recordSize << "\n";
            Author author;
            authors.getline(author.authorID, 15, '|');
            authors.getline(author.authorName, 30, '|');
            authors.getline(author.address, 30, '|');
            cout << "\tAuthor #" << author.authorID << " - ";
            cout << "Author Name: " << author.authorName << " - ";
            cout << "Author Address: " << author.address << "\n";
        }
        authors.close();
    }

    void printAuthorsPrimaryIndex()
    {
        cout << "--------------\n";
        cout << "Authors Primary Index:\n";
        for (auto record : authorsPrimaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }

    void printBooksPrimaryIndex()
    {
        cout << "--------------\n";
        cout << "Books Primary Index:\n";
        for (auto record : booksPrimaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }

    void printAuthorsSecondaryIndex()
    {
        cout << "--------------\n";
        cout << "Authors Secondary Index:\n";
        for (auto record : authorsSecondaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }

    void printBooksSecondaryIndex()
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
        fstream invertedFile(booksSecondaryIndexLinkedListFile, ios::in | ios::binary);
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

    // Main method to start the program
    void start()
    {
        cout << "\tWelcome to the Library Catalog System!\n";
        checkFilesExist();
        // Load index files into memory
        // loadAuthorPrimaryIndex();
        loadBookPrimaryIndex();
        loadBookSecondaryIndex();
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
                    "10. Exit\n"
                    "11. Print Authors File\n"
                    "12. Print Books File\n"
                    "13. Print Authors Primary Index\n"
                    "14. Print Books Primary Index\n"
                    "15. Print Authors Secondary Index\n"
                    "16. Print Books Secondary Index\n"
                    "17. Print Books Secondary Index Inverted list\n";
            cout << "Please Enter Choice (1-10) ==> ";
            string choice;
            getline(cin, choice);
            if (choice == "1")
            {
                cout << "\tAdding New Author\n";
                addAuthor();
            }
            else if (choice == "2")
            {
                cout << "\tAdding New Book\n";
                addBook();
            }
            else if (choice == "4")
            {
                cout << "\tUpdating Book Title\n";
                updateBookTitle();
            }
            else if (choice == "5")
            {
                deleteBook();
            }
            else if (choice == "7")
            {
                cout << "\tPrinting Author Using ID\n";
                printAuthor();
            }
            else if (choice == "8")
            {
                cout << "\tPrinting Book Using ISBN\n";
                printBook();
            }
            else if (choice == "9")
            {
                writeQuery();
            }
            else if (choice == "10")
            {
                terminate();
                break;
            }
            else if (choice == "11")
            {
                printAuthorsFile();
            }
            else if (choice == "12")
            {
                printBooksFile();
            }
            else if (choice == "13")
            {
                printAuthorsPrimaryIndex();
            }
            else if (choice == "14")
            {
                printBooksPrimaryIndex();
            }
            else if (choice == "15")
            {
                printAuthorsSecondaryIndex();
            }
            else if (choice == "16")
            {
                printBooksSecondaryIndex();
            }
            else if (choice == "17")
            {
                printBooksInvertedList();
            }
            else
            {
                cerr << "\n\tINVALID CHOICE!!\n";
            }
        }
    }

    // Terminate the program correctly with saved changes
    void terminate()
    {
        // Save the primary index files to the disk with the updated indices
        // saveAuthorPrimaryIndex();
        saveBookPrimaryIndex();
        saveBookSecondaryIndex();
        cout << "\tThank you for using our Library System!\n";
    }
};

// Start the program
int main()
{
    Library lib;
    lib.start();
    return 0;
}