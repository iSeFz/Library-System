#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsDeleting
{
public:
    // Delete an author using author ID
    void deleteAuthor(map<long long, short> &authorsPrimaryIndex, map<string, short> &authorsSecondaryIndex)
    {
        long long authorId;
        cout << "Enter author ID: ";
        cin >> authorId;
        cin.ignore();
        auto authorInMap = authorsPrimaryIndex.lower_bound(authorId);
        if (authorInMap != authorsPrimaryIndex.end() && authorInMap->first == authorId)
        {
            short recordStartoffset = authorInMap->second - 2;
            deleteFromAuthorsSecondaryIndexFile(authorsSecondaryIndex, authorId, getAuthorNameAt(recordStartoffset));
            deleteFromAuthorsPrimaryIndexFile(authorsPrimaryIndex, authorId);
            deleteFromAuthorsDataFile(recordStartoffset);
        }
        else
        {
            cout << "\tAuthor does not exist\n";
        }
    }

    char *getAuthorNameAt(short startOffset)
    {
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);
        Author tmpAuthor;
        authors.seekg(startOffset, ios::beg);
        authors.ignore(2); // ignore the record size
        authors.getline(tmpAuthor.authorID, 15, '|');
        authors.getline(tmpAuthor.authorName, 30, '|');
        authors.getline(tmpAuthor.address, 30, '|');
        authors.close();
        return tmpAuthor.authorName;
    }

    void deleteFromAuthorsDataFile(int byteOffset) {}

    void deleteFromAuthorsPrimaryIndexFile(map<long long, short> &authorsPrimaryIndex, long long authorId) {}

    void deleteFromAuthorsSecondaryIndexFile(map<string, short> &authorsSecondaryIndex, long long authorId, char authorName[30]) {}

    void deleteFromAuthorsSecondaryIndexLinkedListFile(int firstPosition, char authorId[30]) {}
};