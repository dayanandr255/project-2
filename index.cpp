#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// BankAccount class encapsulating customer data and core operations
class BankAccount {
private:
    int accountNumber;
    string accountHolderName;
    double balance;

public:
    // Constructors
    BankAccount() : accountNumber(0), accountHolderName(""), balance(0.0) {}
    BankAccount(int accNum, string name, double initialBalance) 
        : accountNumber(accNum), accountHolderName(name), balance(initialBalance) {}

    // Getters
    int getAccountNumber() const { return accountNumber; }
    string getAccountHolderName() const { return accountHolderName; }
    double getBalance() const { return balance; }

    // Core Banking Operations
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "\nSuccessfully deposited ₹" << fixed << setprecision(2) << amount;
            cout << "\nNew Balance: ₹" << balance << endl;
        } else {
            cout << "\nInvalid deposit amount!" << endl;
        }
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "\nInvalid withdrawal amount!" << endl;
            return false;
        }
        if (amount > balance) {
            cout << "\nInsufficient funds! Current Balance: ₹" << fixed << setprecision(2) << balance << endl;
            return false;
        }
        balance -= amount;
        cout << "\nSuccessfully withdrew ₹" << fixed << setprecision(2) << amount;
        cout << "\nRemaining Balance: ₹" << balance << endl;
        return true;
    }

    void displayAccountDetails() const {
        cout << "\n-----------------------------------------\n";
        cout << " Account Details                         \n";
        cout << "-----------------------------------------\n";
        cout << " Account Number : " << accountNumber << "\n";
        cout << " Name           : " << accountHolderName << "\n";
        cout << " Balance        : ₹" << fixed << setprecision(2) << balance << "\n";
        cout << "-----------------------------------------\n";
    }

    // Serialization helper functions for file storage
    string toFileString() const {
        return to_string(accountNumber) + "|" + accountHolderName + "|" + to_string(balance);
    }

    static BankAccount fromFileString(const string& line) {
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);

        if (pos1 != string::npos && pos2 != string::npos) {
            int accNum = stoi(line.substr(0, pos1));
            string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
            double bal = stod(line.substr(pos2 + 1));
            return BankAccount(accNum, name, bal);
        }
        return BankAccount();
    }
};

// Manager class handling persistent storage and menu workflow
class BankManager {
private:
    const string fileName = "bank_accounts.txt";

    vector<BankAccount> loadAllAccounts() {
        vector<BankAccount> accounts;
        ifstream inFile(fileName);
        if (!inFile) return accounts;

        string line;
        while (getline(inFile, line)) {
            if (!line.empty()) {
                accounts.push_back(BankAccount::fromFileString(line));
            }
        }
        inFile.close();
        return accounts;
    }

    void saveAllAccounts(const vector<BankAccount>& accounts) {
        ofstream outFile(fileName, ios::trunc);
        for (const auto& acc : accounts) {
            outFile << acc.toFileString() << "\n";
        }
        outFile.close();
    }

    void clearBuffer() {
        cin.clear();
        cin.ignore(10000, '\n');
    }

public:
    void createAccount() {
        int accNum;
        string name;
        double initialDeposit;

        cout << "\n--- Create New Account ---\n";
        cout << "Enter Account Number: ";
        while (!(cin >> accNum)) {
            cout << "Invalid input. Enter a numeric Account Number: ";
            clearBuffer();
        }
        clearBuffer();

        // Check for duplicate account number
        vector<BankAccount> accounts = loadAllAccounts();
        for (const auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                cout << "\nAccount Number already exists! Operation canceled.\n";
                return;
            }
        }

        cout << "Enter Account Holder Name: ";
        getline(cin, name);

        cout << "Enter Initial Deposit Amount: ";
        while (!(cin >> initialDeposit) || initialDeposit < 0) {
            cout << "Invalid input. Enter a positive amount: ";
            clearBuffer();
        }

        BankAccount newAcc(accNum, name, initialDeposit);
        accounts.push_back(newAcc);
        saveAllAccounts(accounts);

        cout << "\nAccount created successfully!\n";
    }

    void handleDeposit() {
        int accNum;
        cout << "\nEnter Account Number: ";
        cin >> accNum;

        vector<BankAccount> accounts = loadAllAccounts();
        bool found = false;

        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                found = true;
                double amount;
                cout << "Enter amount to deposit: ";
                cin >> amount;
                acc.deposit(amount);
                break;
            }
        }

        if (found) {
            saveAllAccounts(accounts);
        } else {
            cout << "\nAccount Number not found!\n";
        }
    }

    void handleWithdrawal() {
        int accNum;
        cout << "\nEnter Account Number: ";
        cin >> accNum;

        vector<BankAccount> accounts = loadAllAccounts();
        bool found = false;

        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                found = true;
                double amount;
                cout << "Enter amount to withdraw: ";
                cin >> amount;
                acc.withdraw(amount);
                break;
            }
        }

        if (found) {
            saveAllAccounts(accounts);
        } else {
            cout << "\nAccount Number not found!\n";
        }
    }

    void checkBalance() {
        int accNum;
        cout << "\nEnter Account Number: ";
        cin >> accNum;

        vector<BankAccount> accounts = loadAllAccounts();
        for (const auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                acc.displayAccountDetails();
                return;
            }
        }
        cout << "\nAccount Number not found!\n";
    }
};

int main() {
    BankManager manager;
    int choice;

    do {
        cout << "\n=========================================\n";
        cout << "        BANK MANAGEMENT SYSTEM           \n";
        cout << "=========================================\n";
        cout << "1. Open New Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Check Balance / Account Info\n";
        cout << "5. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter your choice (1-5): ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.createAccount();
                break;
            case 2:
                manager.handleDeposit();
                break;
            case 3:
                manager.handleWithdrawal();
                break;
            case 4:
                manager.checkBalance();
                break;
            case 5:
                cout << "\nThank you for using Bank Management System!\n";
                break;
            default:
                cout << "\nInvalid choice! Please enter a option between 1 and 5.\n";
                cin.clear();
                cin.ignore(10000, '\n');
        }
    } while (choice != 5);

    return 0;
}