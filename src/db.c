#include "db.h"

#include <stdio.h> 
#include <stdlib.h>
#include <sodium.h>
#include <string.h>
#include <time.h>





DB_RESULT db_create_user(Database db, char* name, char* hash, int admin, int* out_id) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "INSERT INTO users (name, hash, admin) VALUES (?, ?, ?) RETURNING id;", -1, &s, NULL);
	sqlite3_bind_text(s, 1, name, -1, SQLITE_STATIC);
	sqlite3_bind_blob(s, 2, hash, HASH_SIZE, SQLITE_STATIC);
	sqlite3_bind_int(s, 3, admin);
	int result = sqlite3_step(s);
	if(result != SQLITE_ROW) {
		return DB_ERROR;
	}
	if(out_id != NULL) *out_id = sqlite3_column_int(s, 0);
	sqlite3_finalize(s);
	return DB_DONE;
}

DB_RESULT db_set_user(Database db, int id, char* name, char* hash, int admin) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "UPDATE users SET name = ?, hash = ?, admin = ? WHERE id = ?;", -1, &s, NULL);
	sqlite3_bind_text(s, 1, name, -1, SQLITE_STATIC);
	sqlite3_bind_blob(s, 2, hash, HASH_SIZE, SQLITE_STATIC);
	sqlite3_bind_int(s, 3, admin);
	sqlite3_bind_int(s, 4, id);
	int result = sqlite3_step(s);
	if(result != SQLITE_DONE) {
		return DB_ERROR;
	}
	sqlite3_finalize(s);
	return DB_DONE;
}

DB_RESULT db_get_user_from_name(Database db, char* in_name, int* out_id, char* out_hash, int* out_admin) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT id, hash, admin FROM users WHERE name = ?;", -1, &s, NULL);
	sqlite3_bind_text(s, 1, in_name, -1, SQLITE_STATIC);
	int result = sqlite3_step(s);
	if(result == SQLITE_DONE) {
		return DB_NO_RESULT;
	} else if(result != SQLITE_ROW) {
		return DB_ERROR;
	}
	*out_id = sqlite3_column_int(s, 0);
	*out_admin = sqlite3_column_int(s, 2);
	char* hash = sqlite3_column_text(s, 1);
	memcpy(out_hash, hash, HASH_SIZE);
	sqlite3_finalize(s);
	return DB_DONE;
}

DB_RESULT db_get_user(Database db, int in_id, char* out_name, char* out_hash, int* out_admin) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT name, hash, admin FROM users WHERE id = ?;", -1, &s, NULL);
	sqlite3_bind_int(s, 1, in_id);
	int result = sqlite3_step(s);
	if(result == SQLITE_DONE) {
		return DB_NO_RESULT;
	} else if(result != SQLITE_ROW) {
		return DB_ERROR;
	}
	*out_admin = sqlite3_column_int(s, 2);
	char* hash = sqlite3_column_text(s, 1);
	memcpy(out_hash, hash, HASH_SIZE);
	char* name = sqlite3_column_text(s, 0);
	strcpy(out_name, name);
	sqlite3_finalize(s);
	return DB_DONE;
}


DB_RESULT db_get_user_list(Database db, int* id_list, int* id_count) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT id FROM users;", -1, &s, NULL);
	int i;
	for(i = 0; i < 256; i++) {
		int result = sqlite3_step(s);
		if(result == SQLITE_DONE) break;
		if(result != SQLITE_ROW) return DB_ERROR;
		id_list[i] = sqlite3_column_int(s, 0);
	}
	*id_count = i;
	return DB_DONE;
}


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
		"hash BLOB NOT NULL,"
		"admin INTEGER"
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











