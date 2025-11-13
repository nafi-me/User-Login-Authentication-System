#include <iostream>
#include <sqlite3.h>
#include <string>
#include <cstring>
#include <regex>
extern "C" {
#include "auth.h"
}

using namespace std;

// Function to validate username and password
bool validateInput(const string &input, const string &type) {
    if (input.empty()) {
        cout << "❌ " << type << " cannot be empty.\n";
        return false;
    }
    if (input.length() < 3) {
        cout << "❌ " << type << " must be at least 3 characters long.\n";
        return false;
    }
    if (!regex_match(input, regex("^[a-zA-Z0-9_]+$"))) {
        cout << "❌ " << type << " can only contain letters, numbers, and underscores.\n";
        return false;
    }
    return true;
}

// --- Create users table ---
bool setupDatabase(sqlite3 *db) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE,"
        "password_hash TEXT);";

    char *errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// --- Signup function ---
void signup(sqlite3 *db) {
    string username;
    char password[100], confirm[100], hash[65];

    cout << "\nEnter new username: ";
    cin >> username;
    if (!validateInput(username, "Username")) return;

    cout << "Enter password: ";
    get_hidden_input(password, 100);

    cout << "Confirm password: ";
    get_hidden_input(confirm, 100);

    if (strcmp(password, confirm) != 0) {
        cout << "❌ Passwords do not match.\n";
        return;
    }

    if (!validateInput(password, "Password")) return;

    hash_password(password, hash);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (username, password_hash) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, hash, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE)
            cout << "✅ Signup successful!\n";
        else
            cout << "❌ Username already exists.\n";
    }
    sqlite3_finalize(stmt);
}

// --- Login function ---
void login(sqlite3 *db) {
    string username;
    char password[100], hash[65];

    cout << "\nEnter username: ";
    cin >> username;

    cout << "Enter password: ";
    get_hidden_input(password, 100);

    hash_password(password, hash);

    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM users WHERE username=? AND password_hash=?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, hash, -1, SQLITE_STATIC);
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
        cerr << "❌ Can't open database.\n";
        return 1;
    }

    setupDatabase(db);

    int choice;
    do {
        cout << "\n===== 🔐 User Authentication System =====\n";
        cout << "1. Sign Up\n2. Login\n3. Exit\nChoose: ";
        cin >> choice;

        switch (choice) {
            case 1: signup(db); break;
            case 2: login(db); break;
            case 3: cout << "👋 Goodbye!\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 3);

    sqlite3_close(db);
    return 0;
}
