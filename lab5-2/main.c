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
    sqlite3_stmt *stmt = NULL;

    if ((rc = sqlite3_open(":memory:", &db)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sql = "CREATE TABLE student ("
          "    id INTEGER NOT NULL," 
          "    name VARCHAR(255) NOT NULL,"
          "    gender CHAR(1) NOT NULL,"
          "    age INTEGER NOT NULL,"
          "    department INTEGER NOT NULL,"
          "    class INTEGER NOT NULL,"
          "    PRIMARY KEY (id AUTOINCREMENT)"
          ");"
          "CREATE TABLE course ("
          "    id INTEGER NOT NULL,"
          "    name VARCHAR(255) NOT NULL,"
          "    hours INTEGER NOT NULL,"
          "    credits INTEGER NOT NULL,"
          "    PRIMARY KEY (id AUTOINCREMENT)"
          ");"
          "CREATE TABLE student_course ("
          "    student_id INTEGER NOT NULL,"
          "    course_id INTEGER NOT NULL,"
          "    score INTEGER NOT NULL,"
          "    PRIMARY KEY (student_id, course_id),"
          "    FOREIGN KEY (student_id) REFERENCES student(id),"
          "    FOREIGN KEY (course_id) REFERENCES course(id)"
          ");"
          "CREATE VIEW course_2_view AS "
          "    SELECT student.id, student.name, student_course.score "
          "    FROM student_course "
          "        JOIN student ON student_course.student_id = student.id "
          "        JOIN course ON student_course.course_id = course.id "
          "    WHERE course.id = 2;";

    if ((rc = sqlite3_exec(db, sql, NULL, NULL, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sql = "INSERT INTO student (name, gender, age, department, class) VALUES ('Alice', 'F', 18, 1, 101);"
          "INSERT INTO student (name, gender, age, department, class) VALUES ('Bob', 'M', 19, 2, 201);"
          "INSERT INTO student (name, gender, age, department, class) VALUES ('Carol', 'F', 20, 3, 301);"
          "INSERT INTO course (name, hours, credits) VALUES ('Math', 64, 3);"
          "INSERT INTO course (name, hours, credits) VALUES ('English', 48, 2);"
          "INSERT INTO student_course (student_id, course_id, score) VALUES (1, 1, 90);"
          "INSERT INTO student_course (student_id, course_id, score) VALUES (1, 2, 91);"
          "INSERT INTO student_course (student_id, course_id, score) VALUES (2, 1, 92);"
          "INSERT INTO student_course (student_id, course_id, score) VALUES (2, 2, 93);"
          "INSERT INTO student_course (student_id, course_id, score) VALUES (3, 1, 94);"
          "INSERT INTO student_course (student_id, course_id, score) VALUES (3, 2, 95);";
    
    if ((rc = sqlite3_exec(db, sql, NULL, NULL, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    sql = "SELECT student.id, student.name, student_course.score "
          "FROM student_course "
          "    JOIN student ON student_course.student_id = student.id "
          "    JOIN course ON student_course.course_id = course.id "
          "WHERE course.id = 1;";

    if ((rc = sqlite3_exec(db, sql, callback, NULL, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }

    putchar('\n');

    sql = "SELECT * FROM course_2_view;";

    if ((rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)) != SQLITE_OK) {
        fprintf(stderr, "sqlite3_prepare_v2: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        int score = sqlite3_column_int(stmt, 2);
        printf("Row:\n");
        printf("  id: %d\n", id);
        printf("  name: %s\n", name);
        printf("  score: %d\n", score);
    }
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "sqlite3_step: %s\n", sqlite3_errmsg(db));
        goto cleanup;
    }
    sqlite3_finalize(stmt);
    stmt = NULL;

    sqlite3_close(db);
    db = NULL;
    
    return 0;

cleanup:
    if (stmt) {
        sqlite3_finalize(stmt);
    }
    if (db) {
        sqlite3_close(db);
    }
    return 1;
}
