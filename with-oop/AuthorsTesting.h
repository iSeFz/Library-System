#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsTesting
{
public:
    // For testing
    void printAuthorsFile()
    {
        cout << "--------------\n";
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);
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

    void printAuthorsPrimaryIndex(map<long long, short> &authorsPrimaryIndex)
    {
        cout << "--------------\n";
        cout << "Authors Primary Index:\n";
        for (auto record : authorsPrimaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }

    void printAuthorsSecondaryIndex(map<long long, short> &authorsSecondaryIndex)
    {
        cout << "--------------\n";
        cout << "Authors Secondary Index:\n";
        for (auto record : authorsSecondaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }
};