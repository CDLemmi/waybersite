#include "db.h"

#include <stdio.h> 
#include <stdlib.h>
#include <sodium.h>
#include <string.h>
#include <time.h>


#define HASH_SIZE crypto_pwhash_STRBYTES



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
		"hash BLOB NOT NULL"
		");";
	
	char* err_msg = NULL;

	rc = sqlite3_exec(db.db, sql_create_table_users, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "sql error: %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}

	
	char* s2 = 
		"CREATE TABLE IF NOT EXISTS sessions ("
		"session_id TEXT PRIMARY KEY,"
		"user_id INTEGER,"
		"expires_at INTEGER"
		");";
	
	err_msg = NULL;

	rc = sqlite3_exec(db.db, s2, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "sql error: %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}


	return db;

}


void db_create_session(Database db, char* session_id, int user_id) {
	int current_time = time(NULL);
	int expires_at = current_time + 604800;

	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "INSERT INTO sessions (session_id, user_id, expires_at)"
			"VALUES (?, ?, ?);", -1, &s, NULL);
	sqlite3_bind_text(s, 1, session_id, -1, SQLITE_STATIC);
	sqlite3_bind_int(s, 2, user_id);
	sqlite3_bind_int(s, 3, expires_at);
	if(sqlite3_step(s) != SQLITE_DONE) {
		fprintf(stderr, "cant open db: %s\n", sqlite3_errmsg(db.db));
		exit(1);
	}
	sqlite3_finalize(s);
};


int db_verify_user_session(Database db, char* session_id) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT user_id, expires_at FROM sessions WHERE session_id = ?;", -1, &s, NULL);
	sqlite3_bind_text(s, 1, session_id, -1, SQLITE_STATIC);
	if(sqlite3_step(s) != SQLITE_ROW) {
		return -1;
	}
	int expires_at = sqlite3_column_int(s, 1);
	if(time(NULL) >= expires_at) {
		return -1;
	}
	return sqlite3_column_int(s, 0);
}



char* db_get_user_hash_and_id(Database db, char* name, int* user_id) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT id, hash FROM users WHERE name = ?", -1, &s, NULL);
	sqlite3_bind_text(s, 1, name, -1, SQLITE_STATIC);
	if(sqlite3_step(s) != SQLITE_ROW) {
		*user_id = -1;
		return NULL;
	}
	*user_id = sqlite3_column_int(s, 0);
	char* output = sqlite3_column_blob(s, 1);
	char* hash = malloc(HASH_SIZE);
	memcpy(hash, output, HASH_SIZE);
	sqlite3_finalize(s);
	return hash;
}



//0 = success
//1 = username already taken
//2 = hash failed
int db_set_user(Database db, int id, char* name, char* hash) {
	
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "INSERT INTO users (id, name, hash) VALUES (?, ?, ?)"
		"ON CONFLICT(id) DO UPDATE SET name=excluded.name, hash=excluded.hash;", -1, &s, NULL); 
	sqlite3_bind_int(s, 1, id);
	sqlite3_bind_text(s, 2, name, -1, SQLITE_STATIC);
	sqlite3_bind_blob(s, 3, hash, HASH_SIZE, SQLITE_STATIC);
	if(sqlite3_step(s) != SQLITE_DONE) return 1;
	sqlite3_finalize(s);

	return 0;
}







