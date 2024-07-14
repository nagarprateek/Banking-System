#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

using namespace std;

class Account {
protected:
    string accountNumber;
    double balance;

public:
    Account(const string &accNumber, double initialBalance)
        : accountNumber(accNumber), balance(initialBalance) {}

    virtual void deposit(double amount) {
        balance += amount;
        cout << "Deposited: " << amount << ". New balance: " << balance << endl;
    }

    virtual bool withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            cout << "Withdrawn: " << amount << ". New balance: " << balance << endl;
            return true;
        } else {
            cout << "Insufficient balance!" << endl;
            return false;
        }
    }

    virtual void checkBalance() const {
        cout << "Account Number: " << accountNumber << ", Balance: " << balance << endl;
    }

    virtual void save(ofstream &out) const {
        out << accountNumber << " " << balance << " ";
    }

    virtual ~Account() = default;
};

class SavingsAccount : public Account {
private:
    double interestRate;

public:
    SavingsAccount(const string &accNumber, double initialBalance, double rate)
        : Account(accNumber, initialBalance), interestRate(rate) {}

    void applyInterest() {
        balance += balance * interestRate;
        cout << "Interest applied. New balance: " << balance << endl;
    }

    void save(ofstream &out) const override {
        out << "S " << accountNumber << " " << balance << " " << interestRate << " ";
    }
};

class CheckingAccount : public Account {
public:
    CheckingAccount(const string &accNumber, double initialBalance)
        : Account(accNumber, initialBalance) {}

    void save(ofstream &out) const override {
        out << "C " << accountNumber << " " << balance << " ";
    }
};

class Bank {
private:
    unordered_map<string, Account*> accounts;

public:
    ~Bank() {
        for (auto &entry : accounts) {
            delete entry.second;
        }
    }

    void openSavingsAccount(const string &accNumber, double initialBalance, double rate) {
        accounts[accNumber] = new SavingsAccount(accNumber, initialBalance, rate);
        cout << "Savings account opened. Account Number: " << accNumber << endl;
    }

    void openCheckingAccount(const string &accNumber, double initialBalance) {
        accounts[accNumber] = new CheckingAccount(accNumber, initialBalance);
        cout << "Checking account opened. Account Number: " << accNumber << endl;
    }

    void deleteAccount(const string &accNumber) {
        if (accounts.find(accNumber) != accounts.end()) {
            delete accounts[accNumber];
            accounts.erase(accNumber);
            cout << "Account " << accNumber << " deleted." << endl;
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void deposit(const string &accNumber, double amount) {
        if (accounts.find(accNumber) != accounts.end()) {
            accounts[accNumber]->deposit(amount);
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void withdraw(const string &accNumber, double amount) {
        if (accounts.find(accNumber) != accounts.end()) {
            accounts[accNumber]->withdraw(amount);
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void transfer(const string &fromAcc, const string &toAcc, double amount) {
        if (accounts.find(fromAcc) != accounts.end() && accounts.find(toAcc) != accounts.end()) {
            if (accounts[fromAcc]->withdraw(amount)) {
                accounts[toAcc]->deposit(amount);
                cout << "Transferred " << amount << " from " << fromAcc << " to " << toAcc << endl;
            }
        } else {
            cout << "One or both accounts not found!" << endl;
        }
    }

    void checkBalance(const string &accNumber) const {
        if (accounts.find(accNumber) != accounts.end()) {
            accounts.at(accNumber)->checkBalance();
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void applyInterest(const string &accNumber) {
        if (accounts.find(accNumber) != accounts.end()) {
            SavingsAccount *sa = dynamic_cast<SavingsAccount*>(accounts[accNumber]);
            if (sa) {
                sa->applyInterest();
            } else {
                cout << "Account is not a savings account!" << endl;
            }
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void saveAccounts(const string &filename) const {
        ofstream out(filename);
        if (out.is_open()) {
            for (const auto &entry : accounts) {
                entry.second->save(out);
            }
            out.close();
        }
    }

    void loadAccounts(const string &filename) {
        ifstream in(filename);
        if (in.is_open()) {
            char type;
            string accNumber;
            double balance, rate;
            while (in >> type) {
                if (type == 'S') {
                    in >> accNumber >> balance >> rate;
                    accounts[accNumber] = new SavingsAccount(accNumber, balance, rate);
                } else if (type == 'C') {
                    in >> accNumber >> balance;
                    accounts[accNumber] = new CheckingAccount(accNumber, balance);
                }
            }
            in.close();
        }
    }
};

int main() {
    Bank bank;
    bank.loadAccounts("accounts.txt");

    int choice;
    string accNumber, fromAcc, toAcc;
    double initialBalance, rate, amount;

    do {
        cout << "\nBanking System Menu:\n";
        cout << "1. Open Savings Account\n";
        cout << "2. Open Checking Account\n";
        cout << "3. Delete Account\n";
        cout << "4. Deposit\n";
        cout << "5. Withdraw\n";
        cout << "6. Transfer Money\n";
        cout << "7. Check Balance\n";
        cout << "8. Apply Interest (Savings Account)\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Account Number: ";
                cin >> accNumber;
                cout << "Enter Initial Balance: ";
                cin >> initialBalance;
                cout << "Enter Interest Rate: ";
                cin >> rate;
                bank.openSavingsAccount(accNumber, initialBalance, rate);
                break;

            case 2:
                cout << "Enter Account Number: ";
                cin >> accNumber;
                cout << "Enter Initial Balance: ";
                cin >> initialBalance;
                bank.openCheckingAccount(accNumber, initialBalance);
                break;

            case 3:
                cout << "Enter Account Number: ";
                cin >> accNumber;
                bank.deleteAccount(accNumber);
                break;

            case 4:
                cout << "Enter Account Number: ";
                cin >> accNumber;
                cout << "Enter Amount to Deposit: ";
                cin >> amount;
                bank.deposit(accNumber, amount);
                break;

            case 5:
                cout << "Enter Account Number: ";
                cin >> accNumber;
                cout << "Enter Amount to Withdraw: ";
                cin >> amount;
                bank.withdraw(accNumber, amount);
                break;

            case 6:
                cout << "Enter Source Account Number: ";
                cin >> fromAcc;
                cout << "Enter Destination Account Number: ";
                cin >> toAcc;
                cout << "Enter Amount to Transfer: ";
                cin >> amount;
                bank.transfer(fromAcc, toAcc, amount);
                break;

            case 7:
                cout << "Enter Account Number: ";
                cin >> accNumber;
                bank.checkBalance(accNumber);
                break;

            case 8:
                cout << "Enter Savings Account Number: ";
                cin >> accNumber;
                bank.applyInterest(accNumber);
                break;

            case 9:
                cout << "Exiting... Thank you for using our banking system!" << endl;
                bank.saveAccounts("accounts.txt");
                break;

            default:
                cout << "Invalid choice! Please try again." << endl;
                break;
        }
    } while (choice != 9);

    return 0;
}
