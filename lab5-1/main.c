#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>

int callback(void *data, int argc, char *argv[], char *azColName[]) {
    (void)data;
    printf("Row:\n");
    for (int i = 0; i < argc; ++i) {
        printf("  %s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

int main(void) {
    int rc;
    const char *sql;
    sqlite3 *db = NULL;

    if ((rc = sqlite3_open(":memory:", &db)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sql = "CREATE TABLE student (id INTEGER NOT NULL, name VARCHAR(255) NOT NULL, PRIMARY KEY (id AUTOINCREMENT));";

    if ((rc = sqlite3_exec(db, sql, NULL, NULL, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sql = "INSERT INTO student (name) VALUES ('Alice');"
          "INSERT INTO student (name) VALUES ('Bob');"
          "INSERT INTO student (name) VALUES ('Carol');"
          "INSERT INTO student (name) VALUES ('Dave');";

    if ((rc = sqlite3_exec(db, sql, NULL, NULL, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sql = "DELETE FROM student WHERE name = 'Bob';";

    if ((rc = sqlite3_exec(db, sql, NULL, NULL, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sql = "SELECT * FROM student;";

    if ((rc = sqlite3_exec(db, sql, callback, NULL, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sqlite3_close(db);
    db = NULL;
    
    return 0;

cleanup:
    if (db) {
        sqlite3_close(db);
    }
    return 1;
}
