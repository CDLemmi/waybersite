#include "db.h"

#include <stdio.h> 
#include <stdlib.h>


Database init_database() {
	Database db;
	int rc = sqlite3_open("database.db", &(db.db));
	if(rc != SQLITE_OK) {
		fprintf(stderr, "cant open db: %s\n", sqlite3_errmsg((db.db)));
		exit(1);
	}

	char* sql_create_table_users = 
		"CREATE TABLE IF NOT EXISTS users ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name TEXT NOT NULL UNIQUE,"
		"hash TEXT NOT NULL"
		");";
	
	char* err_msg = NULL;

	rc = sqlite3_exec(db.db, sql_create_table_users, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "sql error: %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}

	return db;
}











