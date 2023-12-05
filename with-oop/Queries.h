#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "LibraryUtilities.h"
#include "Book.h"
#include "Author.h"
using namespace std;

class Queries
{
public:
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
};