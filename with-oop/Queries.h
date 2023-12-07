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
public:
    void setIndexes(Authors &author, Books &book) {
        authorPrimaryIndex = author.getPrimaryIndex();
        authorSecondaryIndex = author.getSecondaryIndex();
        bookPrimaryIndex = book.getPrimaryIndex();
        bookSecondaryIndex = book.getSecondaryIndex();
    }

    void readQuery(string &query) {
        cout << "Enter the query: ";
        getline(cin, query);
//        for (char &ch: query)
//            if (isupper(ch))
//                ch = (char) tolower(ch);
    }

    void removeSpaces(string &s) {
        while (s.back() == ' ')
            s.pop_back();
        int j = 0;
        while (s[j] == ' ')
            j++;
        s = s.substr(j);
    }

    string parseTable(string &query) {
        int fromPointer = (int) query.find("from"), wherePointer = (int) query.find("where");
        string table = query.substr(fromPointer + 5, wherePointer - fromPointer - 6);
        return table;
    }

    bool validTable(string &table) {
        return table == "books" || table == "authors";
    }

    void parseColumns(string &query, map<string, bool> &columns) {
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

    bool validColumn(const string &column, string &table) {
        if (column == "all" || column == "author id")return true;
        if (table == "books" && (column == "isbn" || column == "book title"))return true;
        if (table == "authors" && (column == "author name" || column == "address"))return true;
        return false;
    }

    void parseConditions(string &query, map<string, string> &conditions) {
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

    // Handle the select query
    void writeQuery() {
        string query;
        readQuery(query);
        // parsing table
        string table = parseTable(query);
        if (!validTable(table)) {
            cerr << "Invalid Query: Table " + table + " does not exist.\n";
            return;
        }
        // parsing columns
        map<string, bool> columns;
        parseColumns(query, columns);
        for (const auto &column: columns) {
            if (!validColumn(column.first, table)) {
                cerr << "Invalid Query: Column " + column.first + " DOES NOT exists\n";
                return;
            }
        }
        if (columns.count("all") && columns.size() != 1) {
            cerr << "Invalid Query\n";
            return;
        }
        // parse conditions
        map<string, string> conditions;
        if (query.find("where") != -1) {
            parseConditions(query, conditions);
            for (const auto &condition: conditions) {
                if (!validColumn(condition.first, table)) {
                    cerr << "Invalid Query: Column " + condition.first + " DOES NOT exists\n";
                    return;
                }
            }
        }

        // Select all from Authors
        // Select all from Authors where Author ID=’xxx’;
        // Select all from Authors where Author Name='xxx';

        // Select Author Name, Author ID, Address from Authors where Author Name = 'Belal', Author ID = '2021';

        // select all from authors where author id=1;
        // select all from books where author id=’xxx’;
        // select author name from authors where author id=2;
        // select author id from authors where author name='Belal';
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
            }
            else if (conditions.count("author id"))
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
            }
            // remove duplicates
            authorsID.resize(distance(authorsID.begin(), unique(authorsID.begin(), authorsID.end())));
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

            if (columns.size() == 1 && columns.count("all")) {
                cout << "Author ID                    Author Name                    Author Address\n";
                for (auto &author: authors)
                    cout << author.authorID << "   " << author.authorName << "       " << author.address << endl;
                return;
            }

            if (columns.count("author id"))
                cout << "Author ID             ";
            if (columns.count("author name"))
                cout << "Author Name           ";
            if (columns.count("address"))
                cout << "Author Address        ";

            for (auto &author: authors) {
                if (columns.count("author id"))
                    cout << author.authorID << "      ";
                if (columns.count("author name"))
                    cout << author.authorName << "           ";
                if (columns.count("address"))
                    cout << author.address << endl;
            }
        } else if (table == "books") {
            vector<Book> books;
        }
    }

};