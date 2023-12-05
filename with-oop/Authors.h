#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include "LibraryUtilities.h"
#include "Author.h"
#include "AuthorsAdding.h"
#include "AuthorsConfiguration.h"
#include "AuthorsDeleting.h"
#include "AuthorsPrinting.h"
#include "AuthorsTesting.h"
#include "AuthorsUpdating.h"

using namespace std;

class Authors
{
private:
    map<long long, short> authorsPrimaryIndex;   // (Author ID, byte offset) Authors primary index
    map<long long, short> authorsSecondaryIndex; // (Author Name, First record in linked list) Authors secondary index
    AuthorsAdding authorsAdding = AuthorsAdding();
    AuthorsConfiguration authorsConfiguration = AuthorsConfiguration();
    AuthorsUpdating authorsUpdating = AuthorsUpdating();
    AuthorsDeleting authorsDeleting = AuthorsDeleting();
    AuthorsPrinting authorsPrinting = AuthorsPrinting();
    AuthorsTesting authorsTesting = AuthorsTesting();

public:
    // Retrieve data from the map & write it back to the physical file on disk
    void saveAuthorPrimaryIndex()
    {
        authorsConfiguration.saveAuthorPrimaryIndex(authorsPrimaryIndex);
    }

    // Load authors primary index file into memory
    void loadAuthorPrimaryIndex()
    {
        authorsConfiguration.loadAuthorPrimaryIndex(authorsPrimaryIndex);
    }

    // Add a new author helper function
    void addAuthor()
    {
        authorsAdding.addAuthor(authorsPrimaryIndex, authorsSecondaryIndex);
    }

    // Update author name using author ID
    void updateAuthorName()
    {
        authorsUpdating.updateAuthorName(authorsSecondaryIndex);
    }

    // Delete an author using author ID
    void deleteAuthor()
    {
        authorsDeleting.deleteAuthor(authorsPrimaryIndex, authorsSecondaryIndex);
    }

    // Print author using author ID
    void printAuthor()
    {
        authorsPrinting.printAuthor(authorsPrimaryIndex);
    }

    // For testing
    void printAuthorsFile()
    {
        authorsTesting.printAuthorsFile();
    }

    void printAuthorsPrimaryIndex()
    {
        authorsTesting.printAuthorsPrimaryIndex(authorsPrimaryIndex);
    }

    void printAuthorsSecondaryIndex()
    {
        authorsTesting.printAuthorsSecondaryIndex(authorsSecondaryIndex);
    }
};