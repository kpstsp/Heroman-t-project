#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"
#include "game.h"
#include "tasks.h"

int db_init(const char* filename, sqlite3** db) {
    if (sqlite3_open(filename, db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return 1;
    }

    // Create tasks table if it doesn't exist
    const char* create_table_sql = 
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "difficulty INTEGER NOT NULL,"
        "type INTEGER NOT NULL,"
        "completed INTEGER DEFAULT 0,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    char* err_msg = NULL;
    if (sqlite3_exec(*db, create_table_sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(*db);
        return 1;
    }

    return 0;
}

void db_close(sqlite3* db) {
    if (db) {
        sqlite3_close(db);
    }
}

int db_create_schema(sqlite3* db) {
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS player ("
        "health INTEGER,"
        "experience INTEGER,"
        "level INTEGER,"
        "gold INTEGER,"
        "strength INTEGER,"
        "intelligence INTEGER,"
        "constitution INTEGER,"
        "perception INTEGER"
        ");"
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "difficulty INTEGER,"
        "type INTEGER,"
        "completed INTEGER,"
        "streak INTEGER,"
        "last_completed INTEGER"
        ");";

    char* err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }
    
    return 0;
}

int db_save_player(sqlite3* db, const PlayerStats* player) {
    const char* sql = "INSERT OR REPLACE INTO player (health, experience, level, gold, "
                     "strength, intelligence, constitution, perception) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_int(stmt, 1, player->health);
    sqlite3_bind_int(stmt, 2, player->experience);
    sqlite3_bind_int(stmt, 3, player->level);
    sqlite3_bind_int(stmt, 4, player->gold);
    sqlite3_bind_int(stmt, 5, player->strength);
    sqlite3_bind_int(stmt, 6, player->intelligence);
    sqlite3_bind_int(stmt, 7, player->constitution);
    sqlite3_bind_int(stmt, 8, player->perception);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

int db_load_player(sqlite3* db, PlayerStats* player) {
    const char* sql = "SELECT health, experience, level, gold, strength, "
                     "intelligence, constitution, perception FROM player LIMIT 1;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 1;
    }

    player->health = sqlite3_column_int(stmt, 0);
    player->experience = sqlite3_column_int(stmt, 1);
    player->level = sqlite3_column_int(stmt, 2);
    player->gold = sqlite3_column_int(stmt, 3);
    player->strength = sqlite3_column_int(stmt, 4);
    player->intelligence = sqlite3_column_int(stmt, 5);
    player->constitution = sqlite3_column_int(stmt, 6);
    player->perception = sqlite3_column_int(stmt, 7);

    sqlite3_finalize(stmt);
    return 0;
}

int db_create_task(sqlite3* db, const Task* task) {
    const char* sql = "INSERT INTO tasks (title, description, difficulty, type) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_text(stmt, 1, task->title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, task->description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, task->difficulty);
    sqlite3_bind_int(stmt, 4, task->type);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

int db_update_task(sqlite3* db, const Task* task) {
    const char* sql = "UPDATE tasks SET title = ?, description = ?, difficulty = ?, "
                     "type = ?, completed = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_text(stmt, 1, task->title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, task->description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, task->difficulty);
    sqlite3_bind_int(stmt, 4, task->type);
    sqlite3_bind_int(stmt, 5, task->completed);
    sqlite3_bind_int(stmt, 6, task->id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

int db_delete_task(sqlite3* db, int task_id) {
    const char* sql = "DELETE FROM tasks WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_int(stmt, 1, task_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

int db_get_all_tasks(sqlite3* db, Task** tasks, int* count) {
    const char* sql = "SELECT id, title, description, difficulty, type, completed FROM tasks;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Count tasks
    int task_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        task_count++;
    }
    sqlite3_reset(stmt);

    // Allocate memory for tasks
    *tasks = malloc(task_count * sizeof(Task));
    if (!*tasks) {
        fprintf(stderr, "Failed to allocate memory for tasks\n");
        sqlite3_finalize(stmt);
        return 1;
    }

    // Read tasks
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Task* task = &(*tasks)[i];
        task->id = sqlite3_column_int(stmt, 0);
        strncpy(task->title, (const char*)sqlite3_column_text(stmt, 1), sizeof(task->title) - 1);
        strncpy(task->description, (const char*)sqlite3_column_text(stmt, 2), sizeof(task->description) - 1);
        task->difficulty = sqlite3_column_int(stmt, 3);
        task->type = sqlite3_column_int(stmt, 4);
        task->completed = sqlite3_column_int(stmt, 5);
        i++;
    }

    *count = task_count;
    sqlite3_finalize(stmt);
    return 0;
} 