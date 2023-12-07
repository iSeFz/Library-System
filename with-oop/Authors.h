#ifndef AUTHORS_H
#define AUTHORS_H

#include "AuthorsAdding.h"
#include "AuthorsConfiguration.h"
#include "AuthorsDeleting.h"
#include "AuthorsTesting.h"
#include "AuthorsUpdating.h"

class Authors
{
private:
    map<long long, short> authorsPrimaryIndex; // (Author ID, byte offset) Authors primary index
    map<string, short> authorsSecondaryIndex;  // (Author Name, First record in linked list) Authors secondary index
    AuthorsAdding authorsAdding = AuthorsAdding();
    AuthorsConfiguration authorsConfiguration = AuthorsConfiguration();
    AuthorsUpdating authorsUpdating = AuthorsUpdating();
    AuthorsDeleting authorsDeleting = AuthorsDeleting();
    AuthorsTesting authorsTesting = AuthorsTesting();

public:
    // Getters for indices
    map<long long, short> &getPrimaryIndex() { return authorsPrimaryIndex; }
    map<string, short> &getSecondaryIndex() { return authorsSecondaryIndex; }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveAuthorPrimaryIndex() { authorsConfiguration.saveAuthorPrimaryIndex(authorsPrimaryIndex); }

    // Load authors primary index file into memory
    void loadAuthorPrimaryIndex() { authorsConfiguration.loadAuthorPrimaryIndex(authorsPrimaryIndex); }

    // Load authors secondary index file into memory
    void loadAuthorsSecondaryIndex() { authorsConfiguration.loadAuthorsSecondaryIndex(authorsSecondaryIndex); }

    // Retrieve data from the map & write it back to the physical file on disk
    void saveAuthorsSecondaryIndex() { authorsConfiguration.saveAuthorsSecondaryIndex(authorsSecondaryIndex); }

    // Add a new author helper function
    void addAuthor() { authorsAdding.addAuthor(authorsPrimaryIndex, authorsSecondaryIndex); }

    // Update author name using author ID
    void updateAuthorName() { authorsUpdating.updateAuthorName(authorsSecondaryIndex); }

    // Delete an author using author ID
    void deleteAuthor() { authorsDeleting.deleteAuthor(authorsPrimaryIndex, authorsSecondaryIndex); }

    // Print author using author ID
    void printAuthor() { authorsConfiguration.printAuthor(authorsPrimaryIndex); }

    // For testing
    void printAuthorsFile() { authorsTesting.printAuthorsFile(); }

    void printAuthorsPrimaryIndex() { authorsTesting.printAuthorsPrimaryIndex(authorsPrimaryIndex); }

    void printAuthorsSecondaryIndex() { authorsTesting.printAuthorsSecondaryIndex(authorsSecondaryIndex); }

    void printAuthorsInvertedList() { authorsTesting.printAuthorsInvertedList(); }
};

#endif