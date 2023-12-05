#ifndef LIBRARY_UTILITIES_H
#define LIBRARY_UTILITIES_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
using namespace std;

class LibraryUtilities
{
public:
    // Authors data file
    static const string authorsFile;
    static const string authorsPrimaryIndexFile;
    static const string authorsSecondaryIndexFile;
    static const string authorsSecondaryIndexLinkedListFile;

    static const string booksFile;
    static const string booksPrimaryIndexFile;
    static const string booksSecondaryIndexFile;
    static const string booksSecondaryIndexLinkedListFile;

    static const char lengthDelimiter;

    static void checkFilesExist()
    {
        fstream authors(authorsFile, ios::in | ios::binary);
        // check if the file exists
        if (!authors)
        {
            // create the file
            authors.open(authorsFile, ios::out | ios::binary);
            // write the header
            short header = -1;
            authors.write((char *)&header, sizeof(header));
            authors.close();
        }

        fstream books(booksFile, ios::in | ios::binary);
        // check if the file exists
        if (!books)
        {
            // create the file
            books.open(booksFile, ios::out | ios::binary);
            // write the header
            short header = -1;
            books.write((char *)&header, sizeof(header));
            books.close();
        }

        fstream authorsPrimIdx(authorsPrimaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!authorsPrimIdx)
        {
            // create the file
            authorsPrimIdx.open(authorsPrimaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            authorsPrimIdx.write((char *)&flag, sizeof(char));
            authorsPrimIdx.close();
        }

        fstream authorsSeondaryIndexFileFstream(authorsSecondaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!authorsSeondaryIndexFileFstream)
        {
            // create the file
            authorsSeondaryIndexFileFstream.open(authorsSecondaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            authorsSeondaryIndexFileFstream.write((char *)&flag, sizeof(char));
            authorsSeondaryIndexFileFstream.close();
        }

        fstream booksPrimIdx(booksPrimaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!booksPrimIdx)
        {
            // create the file
            booksPrimIdx.open(booksPrimaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            booksPrimIdx.write((char *)&flag, sizeof(char));
            booksPrimIdx.close();
        }

        fstream booksSecondaryIndexFileFstream(booksSecondaryIndexFile, ios::in | ios::binary);
        // check if the file exists
        if (!booksSecondaryIndexFileFstream)
        {
            // create the file
            booksSecondaryIndexFileFstream.open(booksSecondaryIndexFile, ios::out | ios::binary);
            // write the header
            char flag = '1';
            booksSecondaryIndexFileFstream.write((char *)&flag, sizeof(char));
            booksSecondaryIndexFileFstream.close();
        }

        fstream booksInvertedList(booksSecondaryIndexLinkedListFile, ios::in | ios::binary);
        // check if the file exists
        if (!booksInvertedList)
        {
            // create the file
            booksInvertedList.open(booksSecondaryIndexLinkedListFile, ios::out | ios::binary);
            booksInvertedList.close();
        }
    }

    // Update the status flag of the books primary index file
    static void markBooksPrimaryIndexFlag(char value)
    {
        // Open the file in multiple modes
        fstream booksPrimaryFile(booksPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        booksPrimaryFile.seekp(0, ios::beg);
        booksPrimaryFile.write((char *)&value, sizeof(char));
        booksPrimaryFile.close();
    }

    static void markBooksSecondaryIndexFlag(char value)
    {
        // Open the file in multiple modes
        fstream booksSecondaryIndexFstream(booksSecondaryIndexFile, ios::in | ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        booksSecondaryIndexFstream.seekp(0, ios::beg);
        booksSecondaryIndexFstream.write((char *)&value, sizeof(char));
        booksSecondaryIndexFstream.close();
    }

    static void markBooksSecondaryIndexFlag(short value)
    {
        fstream booksSecondaryFile(booksSecondaryIndexFile, ios::out | ios::binary);
        booksSecondaryFile.write((char *)&value, sizeof(value));
        booksSecondaryFile.close();
    }

    // Update the status flag of the authors primary index file
    static void markAuthorsPrimaryIndexFlag(char value)
    {
        // Open the file in multiple modes
        fstream authorsPrimaryFile(authorsPrimaryIndexFile, ios::in | ios::out | ios::binary);

        // Write the status flag at the beginning of the file
        authorsPrimaryFile.seekp(0, ios::beg);
        authorsPrimaryFile.write((char *)&value, sizeof(char));
        authorsPrimaryFile.close();
    }

    static void markAuthorsSecondaryIndexFlag(char value)
    {
        fstream authorsSecondaryFile(authorsSecondaryIndexFile, ios::out | ios::binary);
        authorsSecondaryFile.write((char *)&value, sizeof(value));
        authorsSecondaryFile.close();
    }

    // Convert array of characters into long long data type
    static long long convertCharArrToLongLong(char arr[])
    {
        long long num = 0;
        // Loop over the length of the array or until the length delimiter
        for (int i = 0; arr[i] != '\0' && arr[i] != lengthDelimiter; i++)
        {
            num *= 10;
            num += (arr[i] - '0');
        }
        return num;
    }
};

const string LibraryUtilities::authorsFile = "authors.txt"; // Authors data file

const string LibraryUtilities::authorsPrimaryIndexFile = "authorsPrimaryIndex.txt"; // Authors primary index file

const string LibraryUtilities::authorsSecondaryIndexFile = "authorsSecondaryIndex.txt"; // Authors secondary index file

const string LibraryUtilities::authorsSecondaryIndexLinkedListFile = "authorsSecondaryIndexLinkedList.txt"; // Authors secondary index linked list file

const string LibraryUtilities::booksFile = "books.txt"; // Books data file

const string LibraryUtilities::booksPrimaryIndexFile = "booksPrimaryIndex.txt"; // Books primary index file

const string LibraryUtilities::booksSecondaryIndexFile = "booksSecondaryIndex.txt"; // Books secondary index file

const string LibraryUtilities::booksSecondaryIndexLinkedListFile = "booksSecondaryIndexLinkedList.txt"; // Books secondary index linked list file

const char LibraryUtilities::lengthDelimiter = '|'; // Delimiter to separate length indicator from record data

#endif // LIBRARY_UTILITIES_H