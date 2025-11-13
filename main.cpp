#include <iostream>
#include <sqlite3.h>
#include <string>
#include <cstring>
extern "C" {
#include "auth.h"
}

using namespace std;

// Execute SQL command (no return)
bool executeSQL(sqlite3 *db, const string &sql) {
    char *errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Check if user exists
bool userExists(sqlite3 *db, const string &username) {
    string query = "SELECT COUNT(*) FROM users WHERE username='" + username + "';";
    sqlite3_stmt *stmt;
    int count = 0;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW)
            count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count > 0;
}

void signup(sqlite3 *db) {
    string username, password;
    cout << "Enter new username: ";
    cin >> username;

    if (userExists(db, username)) {
        cout << "Username already exists!\n";
        return;
    }

    cout << "Enter password: ";
    cin >> password;

    char hash[65];
    hash_password(password.c_str(), hash);

    string sql = "INSERT INTO users (username, password_hash) VALUES ('" + username + "', '" + string(hash) + "');";

    if (executeSQL(db, sql))
        cout << "Signup successful!\n";
}

void login(sqlite3 *db) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    char hash[65];
    hash_password(password.c_str(), hash);

    string query = "SELECT * FROM users WHERE username='" + username + "' AND password_hash='" + string(hash) + "';";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW)
            cout << "✅ Login successful! Welcome, " << username << "!\n";
        else
            cout << "❌ Invalid username or password.\n";
    }
    sqlite3_finalize(stmt);
}

int main() {
    sqlite3 *db;
    if (sqlite3_open("users.db", &db)) {
        cerr << "Can't open database.\n";
        return 1;
    }

    executeSQL(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password_hash TEXT);");

    int choice;
    do {
        cout << "\n===== User Authentication System =====\n";
        cout << "1. Sign Up\n2. Login\n3. Exit\nChoose: ";
        cin >> choice;
        switch (choice) {
            case 1: signup(db); break;
            case 2: login(db); break;
            case 3: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 3);

    sqlite3_close(db);
    return 0;
}
