#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsPrinting
{
public:
    // Print author using author ID
    void printAuthor(map<long long, short> &authorsPrimaryIndex)
    {
        int authorID;
        cout << "Enter author ID: ";
        cin >> authorID;
        auto id = authorsPrimaryIndex.lower_bound(authorID);
        if (id != authorsPrimaryIndex.end() && id->first == authorID)
        {
            int offset = id->second;
            fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);
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
};