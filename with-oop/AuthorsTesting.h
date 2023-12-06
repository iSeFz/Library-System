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
        cout << "----------------------------\n";
        fstream authors(LibraryUtilities::authorsFile, ios::in | ios::binary);

        authors.seekg(0, ios::end);
        int fileEndOffset = authors.tellg();
        authors.seekg(0, ios::beg);

        short header;
        authors.read((char *)&header, sizeof(header));
        cout << "Avail list header: " << header << "\n";
        while (authors)
        {
            int recordOffset = authors.tellg();
            if (recordOffset == fileEndOffset or recordOffset == -1)
                break;
            cout << "\n-------- "
                 << "Record Offset: " << recordOffset << " --------\n";
            char firstChar;
            authors.read((char *)&firstChar, sizeof(char));
            // if the record is deleted
            if (firstChar == '*')
            {
                short previousRecord, recordSize;
                authors.ignore(1);                                    // ignore the delimiter
                authors.read((char *)&previousRecord, sizeof(short)); // read the previous record
                authors.ignore(1);                                    // ignore the delimiter
                authors.read((char *)&recordSize, sizeof(short));     // read the record size
                authors.seekg(recordOffset + recordSize, ios::beg);   // jump to the next record
                cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
                cout << "Redcord deleted\n";
                cout << "Previous deleted record: " << previousRecord << "\n";
                cout << "Record Size: " << recordSize << "\n";
                cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
                continue;
            }
            else
            {
                authors.seekg(-1, ios::cur);
            }
            cout << "############################\n";
            short recordSize;
            authors.read((char *)&recordSize, sizeof(short));
            if (recordSize == -1)
                break;
            cout << "Record Size: " << recordSize << "\n";
            Author author;
            authors.getline(author.authorID, 15, '|');
            authors.getline(author.authorName, 30, '|');
            authors.getline(author.address, 30, '|');
            if (authors.peek() == '|')
            {
                authors.seekg(1, ios::cur);
            }
            cout << "\tAuthor ID #" << author.authorID << " - ";
            cout << "Author Name: " << author.authorName << " - ";
            cout << "Author Address: " << author.address << "\n";
            cout << "############################\n";
            authors.seekg(recordOffset + recordSize, ios::beg);   // jump to the next record
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

    void printAuthorsSecondaryIndex(map<string, short> &authorsSecondaryIndex)
    {
        cout << "--------------\n";
        cout << "Authors Secondary Index:\n";
        for (auto record : authorsSecondaryIndex)
        {
            cout << record.first << " " << record.second << "\n";
        }
    }

    void printAuthorsInvertedList()
    {
        fstream invertedFile(LibraryUtilities::authorsSecondaryIndexLinkedListFile, ios::in | ios::binary);
        invertedFile.seekg(0, ios::end);
        int endOffset = invertedFile.tellg();
        invertedFile.seekg(0, ios::beg);

        while (invertedFile)
        {
            if (invertedFile.tellg() == endOffset)
                break;
            long long authorId;
            invertedFile.read((char *)&authorId, sizeof(long long));
            short nextRecordPointer;
            invertedFile.read((char *)&nextRecordPointer, sizeof(short));
            cout << authorId << " " << nextRecordPointer << "\n";
        }
    }

};