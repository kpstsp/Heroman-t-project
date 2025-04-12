#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "game.h"

// Database initialization and cleanup
int db_init(const char* filename, sqlite3** db);
void db_close(sqlite3* db);

// Player operations
int db_save_player(sqlite3* db, const PlayerStats* player);
int db_load_player(sqlite3* db, PlayerStats* player);

// Task operations
int db_create_task(sqlite3* db, const Task* task);
int db_update_task(sqlite3* db, const Task* task);
int db_delete_task(sqlite3* db, int task_id);
int db_get_all_tasks(sqlite3* db, Task** tasks, int* count);
int db_get_task_by_id(sqlite3* db, int task_id, Task* task);

// Database schema creation
int db_create_schema(sqlite3* db);

#endif // DATABASE_H 