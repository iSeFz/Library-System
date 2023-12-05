#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Author.h"

class AuthorsUpdating
{
public:
    // Update author name using author ID
    void updateAuthorName(map<long long, short> &authorsSecondaryIndex) {}

    void updateAuthorNameInDataFile(char newAuthorName[3], int byteOffset) {}

    void updateAuthorNameInSecondaryIndex(char oldAuthorName[30], char newAuthorName[30], map<long long, short> &authorsSecondaryIndex) {}
};