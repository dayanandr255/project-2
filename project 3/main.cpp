#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

// Class representing a Book entity
class Book {
private:
    int id;
    string title;
    string author;
    bool isIssued;
    int issuedToMemberId;

public:
    Book() : id(0), title(""), author(""), isIssued(false), issuedToMemberId(0) {}
    Book(int id, string title, string author, bool isIssued = false, int memberId = 0)
        : id(id), title(title), author(author), isIssued(isIssued), issuedToMemberId(memberId) {}

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    bool getIsIssued() const { return isIssued; }
    int getIssuedToMemberId() const { return issuedToMemberId; }

    void issueBook(int memberId) {
        isIssued = true;
        issuedToMemberId = memberId;
    }

    void returnBook() {
        isIssued = false;
        issuedToMemberId = 0;
    }

    string toFileString() const {
        return to_string(id) + "|" + title + "|" + author + "|" + (isIssued ? "1" : "0") + "|" + to_string(issuedToMemberId);
    }

    static Book fromFileString(const string& line) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);

        if (p1 != string::npos && p2 != string::npos && p3 != string::npos && p4 != string::npos) {
            int id = stoi(line.substr(0, p1));
            string title = line.substr(p1 + 1, p2 - p1 - 1);
            string author = line.substr(p2 + 1, p3 - p2 - 1);
            bool isIssued = (line.substr(p3 + 1, p4 - p3 - 1) == "1");
            int memberId = stoi(line.substr(p4 + 1));
            return Book(id, title, author, isIssued, memberId);
        }
        return Book();
    }
};

// Class representing a Library Member
class Member {
private:
    int id;
    string name;

public:
    Member() : id(0), name("") {}
    Member(int id, string name) : id(id), name(name) {}

    int getId() const { return id; }
    string getName() const { return name; }

    string toFileString() const {
        return to_string(id) + "|" + name;
    }

    static Member fromFileString(const string& line) {
        size_t p = line.find('|');
        if (p != string::npos) {
            int id = stoi(line.substr(0, p));
            string name = line.substr(p + 1);
            return Member(id, name);
        }
        return Member();
    }
};

// Class to manage Library Operations
class LibraryManager {
private:
    const string booksFile = "books.txt";
    const string membersFile = "members.txt";

    vector<Book> loadBooks() {
        vector<Book> books;
        ifstream in(booksFile);
        string line;
        while (getline(in, line)) {
            if (!line.empty()) books.push_back(Book::fromFileString(line));
        }
        return books;
    }

    void saveBooks(const vector<Book>& books) {
        ofstream out(booksFile, ios::trunc);
        for (const auto& b : books) out << b.toFileString() << "\n";
    }

    vector<Member> loadMembers() {
        vector<Member> members;
        ifstream in(membersFile);
        string line;
        while (getline(in, line)) {
            if (!line.empty()) members.push_back(Member::fromFileString(line));
        }
        return members;
    }

    void saveMembers(const vector<Member>& members) {
        ofstream out(membersFile, ios::trunc);
        for (const auto& m : members) out << m.toFileString() << "\n";
    }

    void clearBuffer() {
        cin.clear();
        cin.ignore(10000, '\n');
    }

    string toLower(string str) const {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

public:
    void addBook() {
        int id;
        string title, author;

        cout << "\n--- Add New Book ---\n";
        cout << "Enter Book ID: ";
        while (!(cin >> id)) {
            cout << "Invalid input. Enter numeric ID: ";
            clearBuffer();
        }
        clearBuffer();

        vector<Book> books = loadBooks();
        for (const auto& b : books) {
            if (b.getId() == id) {
                cout << "Book ID already exists!\n";
                return;
            }
        }

        cout << "Enter Title: ";
        getline(cin, title);
        cout << "Enter Author: ";
        getline(cin, author);

        books.push_back(Book(id, title, author));
        saveBooks(books);
        cout << "Book added successfully!\n";
    }

    void addMember() {
        int id;
        string name;

        cout << "\n--- Register New Member ---\n";
        cout << "Enter Member ID: ";
        while (!(cin >> id)) {
            cout << "Invalid input. Enter numeric ID: ";
            clearBuffer();
        }
        clearBuffer();

        vector<Member> members = loadMembers();
        for (const auto& m : members) {
            if (m.getId() == id) {
                cout << "Member ID already exists!\n";
                return;
            }
        }

        cout << "Enter Member Name: ";
        getline(cin, name);

        members.push_back(Member(id, name));
        saveMembers(members);
        cout << "Member registered successfully!\n";
    }

    void issueBook() {
        int bookId, memberId;
        cout << "\n--- Issue Book ---\n";
        cout << "Enter Book ID: ";
        cin >> bookId;

        vector<Book> books = loadBooks();
        int bookIdx = -1;
        for (size_t i = 0; i < books.size(); ++i) {
            if (books[i].getId() == bookId) {
                bookIdx = i;
                break;
            }
        }

        if (bookIdx == -1) {
            cout << "Book not found!\n";
            return;
        }

        if (books[bookIdx].getIsIssued()) {
            cout << "Book is already issued to Member ID: " << books[bookIdx].getIssuedToMemberId() << "\n";
            return;
        }

        cout << "Enter Member ID: ";
        cin >> memberId;

        vector<Member> members = loadMembers();
        bool memberExists = false;
        for (const auto& m : members) {
            if (m.getId() == memberId) {
                memberExists = true;
                break;
            }
        }

        if (!memberExists) {
            cout << "Member ID not found! Please register the member first.\n";
            return;
        }

        books[bookIdx].issueBook(memberId);
        saveBooks(books);
        cout << "Book successfully issued to Member ID: " << memberId << "\n";
    }

    void returnBook() {
        int bookId;
        cout << "\n--- Return Book ---\n";
        cout << "Enter Book ID: ";
        cin >> bookId;

        vector<Book> books = loadBooks();
        bool found = false;

        for (auto& b : books) {
            if (b.getId() == bookId) {
                found = true;
                if (!b.getIsIssued()) {
                    cout << "Book was not issued.\n";
                    return;
                }
                b.returnBook();
                cout << "Book successfully returned!\n";
                break;
            }
        }

        if (found) {
            saveBooks(books);
        } else {
            cout << "Book not found!\n";
        }
    }

    void searchBooks() {
        clearBuffer();
        string query;
        cout << "\nEnter Title or Author to search: ";
        getline(cin, query);

        vector<Book> books = loadBooks();
        string lowerQuery = toLower(query);
        bool found = false;

        cout << "\n----------------------------------------------------------------------\n";
        cout << left << setw(8) << "ID" << setw(30) << "Title" << setw(20) << "Author" << "Status\n";
        cout << "----------------------------------------------------------------------\n";

        for (const auto& b : books) {
            if (toLower(b.getTitle()).find(lowerQuery) != string::npos ||
                toLower(b.getAuthor()).find(lowerQuery) != string::npos) {
                found = true;
                cout << left << setw(8) << b.getId()
                     << setw(30) << b.getTitle()
                     << setw(20) << b.getAuthor()
                     << (b.getIsIssued() ? "Issued (Member ID: " + to_string(b.getIssuedToMemberId()) + ")" : "Available") << "\n";
            }
        }
        cout << "----------------------------------------------------------------------\n";

        if (!found) cout << "No matching books found.\n";
    }

    void displayAllBooks() {
        vector<Book> books = loadBooks();
        if (books.empty()) {
            cout << "\nNo books registered in the library.\n";
            return;
        }

        cout << "\n----------------------------------------------------------------------\n";
        cout << left << setw(8) << "ID" << setw(30) << "Title" << setw(20) << "Author" << "Status\n";
        cout << "----------------------------------------------------------------------\n";

        for (const auto& b : books) {
            cout << left << setw(8) << b.getId()
                 << setw(30) << b.getTitle()
                 << setw(20) << b.getAuthor()
                 << (b.getIsIssued() ? "Issued (Member ID: " + to_string(b.getIssuedToMemberId()) + ")" : "Available") << "\n";
        }
        cout << "----------------------------------------------------------------------\n";
    }
};

int main() {
    LibraryManager manager;
    int choice;

    do {
        cout << "\n=========================================\n";
        cout << "       LIBRARY MANAGEMENT SYSTEM         \n";
        cout << "=========================================\n";
        cout << "1. Add New Book\n";
        cout << "2. Register Member\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Search Book (by Title/Author)\n";
        cout << "6. Display All Books\n";
        cout << "7. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter your choice (1-7): ";
        cin >> choice;

        switch (choice) {
            case 1: manager.addBook(); break;
            case 2: manager.addMember(); break;
            case 3: manager.issueBook(); break;
            case 4: manager.returnBook(); break;
            case 5: manager.searchBooks(); break;
            case 6: manager.displayAllBooks(); break;
            case 7: cout << "\nExiting Library Management System. Goodbye!\n"; break;
            default:
                cout << "\nInvalid choice! Please select between 1 and 7.\n";
                cin.clear();
                cin.ignore(10000, '\n');
        }
    } while (choice != 7);

    return 0;
}