#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "LibraryUtilities.h"
#include "Book.h"
#include "Author.h"

using namespace std;

class Queries {
private:
    map<long long, short> authorPrimaryIndex;
    map<string, short> authorSecondaryIndex;
    map<long long, short> bookPrimaryIndex;
    map<long long, short> bookSecondaryIndex;
    string query, table;
    map<string, bool> columns;
    map<string, string> conditions;
    bool validColumns, validTable, validConditions;
public:
    void initiate(){
        validColumns = true;
        validTable = true;
        validConditions = true;
        query = "";
        table = "";
        conditions.clear();
        columns.clear();
    }
    void setIndexes(Authors &author, Books &book) {
        authorPrimaryIndex = author.getPrimaryIndex();
        authorSecondaryIndex = author.getSecondaryIndex();
        bookPrimaryIndex = book.getPrimaryIndex();
        bookSecondaryIndex = book.getSecondaryIndex();
    }
    static void removeSpaces(string &s) {
        while (s.back() == ' ')
            s.pop_back();
        int j = 0;
        while (s[j] == ' ')
            j++;
        s = s.substr(j);
    }

    void readQuery(){
        cout << "Enter the query: ";
        getline(cin, query);
    }

    void parseQuery(){
        parseTable();
        parseColumns();
        parseConditions();
    }
    void parseTable() {
        int fromPointer = (int) query.find("from"), wherePointer = (int) query.find("where");
        table = query.substr(fromPointer + 5, wherePointer - fromPointer - 6);
    }
    void parseColumns() {
        int fromPointer = (int) query.find("from");
        string selectedColumns = query.substr(7, fromPointer - 8);
        string column;
        for (int i = 0; i < selectedColumns.size(); ++i) {
            if (selectedColumns[i] == ',') {
                removeSpaces(column);
                columns[column] = true;
                column.clear();
            } else column += selectedColumns[i];
            if (i == selectedColumns.size() - 1) {
                removeSpaces(column);
                columns[column] = true;
            }
        }
    }
    void parseConditions() {
        int wherePointer = (int) query.find("where");
        string totalConditions = query.substr(wherePointer + 5);
        string condition;
        for (int i = 0; i < totalConditions.size(); ++i) {
            if (totalConditions[i] == ',' || totalConditions[i] == ';') {
                int equalityPointer = (int) condition.find('=');
                string lhs = condition.substr(0, equalityPointer);
                string rhs = condition.substr(equalityPointer + 1);
                removeSpaces(rhs);
                removeSpaces(lhs);
                if (rhs.back() == '\'')rhs.pop_back();
                if (rhs.front() == '\'')rhs = rhs.substr(1);
                conditions[lhs] = rhs;
                condition.clear();
            } else
                condition += totalConditions[i];
            if (i == totalConditions.size() - 1 && totalConditions[i] != ';') {
                int equalityPointer = (int) condition.find('=');
                string lhs = condition.substr(0, equalityPointer);
                string rhs = condition.substr(equalityPointer + 1);
                removeSpaces(rhs);
                removeSpaces(lhs);
                if (rhs.back() == '\'')rhs.pop_back();
                if (rhs.front() == '\'')rhs = rhs.substr(1);
                conditions[lhs] = rhs;
                condition.clear();
            }
        }
    }

    void validateQuery(){
        if(!validateTable()){
            validTable = false;
            cerr << "Invalid Query: Table " + table + " does not exist.\n";
            return;
        }
        if(!validateColumns()){
            validColumns = false;
            cerr << "Invalid Query: Columns DOES NOT exist\n";
            return;
        }
        if(!validateConditions()){
            validConditions = false;
            cerr << "Invalid Query: Error in Where conditions\n";
            return;
        }
    }
    bool validateTable() {
        return table == "books" || table == "authors";
    }
    bool validateColumns() {
        for (const auto &c: columns) {
            string column = c.first;
            if (column == "all" || column == "author id")continue;
            if (table == "books" && (column == "isbn" || column == "book title"))continue;
            if (table == "authors" && (column == "author name" || column == "address"))continue;
            return false;
        }
        if (columns.count("all") && columns.size() != 1)return false;
        return true;
    }
    bool validateConditions(){
        if (query.find("where") != -1) {
            for (const auto &condition: conditions) {
                string column = condition.first;
                if (column == "all" || column == "author id")continue;
                if (table == "books" && (column == "isbn" || column == "book title"))continue;
                if (table == "authors" && (column == "author name" || column == "address"))continue;
                return false;
            }
            return true;
        }
        else return true;
    }

    void runQuery(){
        if (table == "authors") {
            vector<Author> authors;
            vector<string> authorsID;
            if (conditions.count("author id") && conditions.count("author name")) {
                string name = conditions["author name"];
                string id = conditions["author id"];
                if (authorSecondaryIndex.find(name) != authorSecondaryIndex.end()) {
                    short nextRecordPointer = authorSecondaryIndex[name];
                    long long authorID;
                    fstream invertedFile(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::binary);
                    while (nextRecordPointer != -1) {
                        invertedFile.seekg(nextRecordPointer * 10, ios::beg);
                        invertedFile.read((char *) &authorID, sizeof(long long));
                        invertedFile.read((char *) &nextRecordPointer, sizeof(short));
                        if (id == to_string(authorID))
                            authorsID.push_back(to_string(authorID));
                    }
                    invertedFile.close();
                }
            } else if (conditions.count("author id"))
                authorsID.push_back(conditions["author id"]);
            else if (conditions.count("author name")) {
                string name = conditions["author name"];
                if (authorSecondaryIndex.find(name) != authorSecondaryIndex.end()) {
                    short nextRecordPointer = authorSecondaryIndex[name];
                    long long authorID;
                    fstream invertedFile(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::binary);
                    while (nextRecordPointer != -1) {
                        invertedFile.seekg(nextRecordPointer * 10, ios::beg);
                        invertedFile.read((char *) &authorID, sizeof(long long));
                        invertedFile.read((char *) &nextRecordPointer, sizeof(short));
                        authorsID.push_back(to_string(authorID));
                    }
                    invertedFile.close();
                }
            } else {
                // NO WHERE CONDITIONS
                for (auto authorID: authorPrimaryIndex)
                    authorsID.push_back(to_string(authorID.first));
            }
            // remove duplicates
            sort(authorsID.begin(), authorsID.end());
            authorsID.erase(unique(authorsID.begin(), authorsID.end()), authorsID.end());
            for (int i = 0; i < authorsID.size(); ++i) {
                long long id = stoll(authorsID[i]);
                if (authorPrimaryIndex.find(id) != authorPrimaryIndex.end()) {
                    short offset = authorPrimaryIndex[id];
                    fstream authorsFile(LibraryUtilities::authorsFile, ios::in | ios::binary);
                    authorsFile.seekg(offset, ios::beg);
                    Author author{};
                    authorsFile.getline(author.authorID, 15, '|');
                    authorsFile.getline(author.authorName, 30, '|');
                    authorsFile.getline(author.address, 30, '|');
                    authors.push_back(author);
                }
            }

            if(columns.count("all")){
                columns["author id"] = true;
                columns["author name"] = true;
                columns["address"] = true;
            }
            if (columns.count("author id"))
                cout << "Author ID" << setw(11) << ' ';
            if (columns.count("author name"))
                cout << "Author Name" << setw(24) << ' ';
            if (columns.count("address"))
                cout << "Author Address";
            cout << '\n';
            for (auto &author: authors) {
                if (columns.count("author id"))
                    cout << author.authorID << setw(20-(int)strlen(author.authorID)) << ' ';
                if (columns.count("author name"))
                    cout << author.authorName << setw(35-(int)strlen(author.authorName)) << ' ';
                if (columns.count("address"))
                    cout << author.address;
                cout << '\n';
            }
        } else if (table == "books") {
            vector<Book> books;
            vector<string> booksISBN;
            if (conditions.count("isbn") && conditions.count("author id")) {
                string isbn = conditions["isbn"];
                string id = conditions["author id"];
                if (bookSecondaryIndex.find(stoll(id)) != bookSecondaryIndex.end()) {
                    short nextRecordPointer = bookSecondaryIndex[stoll(id)];
                    long long bookISBN;
                    fstream invertedFile(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::binary);
                    while (nextRecordPointer != -1) {
                        invertedFile.seekg(nextRecordPointer * 10, ios::beg);
                        invertedFile.read((char *) &bookISBN, sizeof(long long));
                        invertedFile.read((char *) &nextRecordPointer, sizeof(short));
                        if (isbn == to_string(bookISBN))
                            booksISBN.push_back(to_string(bookISBN));
                    }
                    invertedFile.close();
                }
            } else if (conditions.count("isbn"))
                booksISBN.push_back(conditions["isbn"]);
            else if (conditions.count("author id")) {
                string id = conditions["author id"];
                if (bookSecondaryIndex.find(stoll(id)) != bookSecondaryIndex.end()) {
                    short nextRecordPointer = bookSecondaryIndex[stoll(id)];
                    long long bookISBN;
                    fstream invertedFile(LibraryUtilities::booksSecondaryIndexLinkedListFile, ios::in | ios::binary);
                    while (nextRecordPointer != -1) {
                        invertedFile.seekg(nextRecordPointer * 10, ios::beg);
                        invertedFile.read((char *) &bookISBN, sizeof(long long));
                        invertedFile.read((char *) &nextRecordPointer, sizeof(short));
                        booksISBN.push_back(to_string(bookISBN));
                    }
                    invertedFile.close();
                }
            } else {
                // NO WHERE CONDITIONS
                for (auto bookISBN: bookPrimaryIndex)
                    booksISBN.push_back(to_string(bookISBN.first));
            }
            // remove duplicates
            sort(booksISBN.begin(), booksISBN.end());
            booksISBN.erase(unique(booksISBN.begin(), booksISBN.end()), booksISBN.end());

            for (int i = 0; i < booksISBN.size(); ++i) {
                long long id = stoll(booksISBN[i]);
                if (bookPrimaryIndex.find(id) != bookPrimaryIndex.end()) {
                    short offset = bookPrimaryIndex[id];
                    fstream booksFile(LibraryUtilities::booksFile, ios::in | ios::binary);
                    booksFile.seekg(offset, ios::beg);
                    Book book{};
                    booksFile.getline(book.ISBN, 15, '|');
                    booksFile.getline(book.bookTitle, 30, '|');
                    booksFile.getline(book.authorID, 30, '|');
                    books.push_back(book);
                }
            }
            if(columns.count("all")){
                columns["isbn"] = true;
                columns["book title"] = true;
                columns["author id"] = true;
            }
            if (columns.count("isbn"))
                cout << "Book ISBN" << setw(11) << ' ';
            if (columns.count("book title"))
                cout << "Book Title" << setw(24) << ' ';
            if (columns.count("author id"))
                cout << "Author ID";
            cout << '\n';

            for (auto &book: books) {
                if (columns.count("isbn"))
                    cout << book.ISBN << setw(20-(int)strlen(book.ISBN)) << ' ';
                if (columns.count("book title"))
                    cout << book.bookTitle << setw(35-(int)strlen(book.bookTitle)) << ' ';
                if (columns.count("author id"))
                    cout << book.authorID;
                cout << '\n';
            }
        }
    }
    // Handle the select query
    void writeQuery() {
        // Select all from Authors
        // Select all from Authors where Author ID=’xxx’;
        // Select all from Authors where Author Name='xxx';

        // Select Author Name, Author ID, Address from Authors where Author Name = 'Belal', Author ID = '2021';
        // select all from authors where author id=1;
        // select all from books where author id=’xxx’;
        // select author name from authors where author id=2;
        // select author id from authors where author name='Belal';
        initiate();
        readQuery();
        parseQuery();
        validateQuery();
        if(!validTable||!validColumns||!validConditions)return;
        runQuery();
    }

};