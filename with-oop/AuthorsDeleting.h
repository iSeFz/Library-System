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
    void deleteAuthor(map<long long, short> &authorsPrimaryIndex, map<char[30], short> &authorsSecondaryIndex) {}

    void deleteFromAuthorsDataFile(int byteOffset) {}

    void deleteFromAuthorsPrimaryIndexFile(map<long long, short> &authorsPrimaryIndex, char authorId[30]) {}

    void deleteFromAuthorsSecondaryIndexFile(map<char[30], short> &authorsSecondaryIndex, char authorName[30]) {}

    void deleteFromAuthorsSecondaryIndexLinkedListFile(int firstPosition, char authorId[30]) {}
};