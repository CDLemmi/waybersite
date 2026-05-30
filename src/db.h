#ifndef DB_H
#define DB_H


#include <sqlite3.h>



typedef struct {
	sqlite3* db;
} Database;



Database init_database();
	
int db_set_user(Database db, int id, char* name, char* pw);


char* db_get_user_hash_and_id(Database db, char* name, int* user_id);

char* db_get_hash(Database db, int id);


void db_create_session(Database db, char* session_id, int user_id);

int db_set_username(Database db, int id, char* name);

char* db_get_username(Database db, int id);

int db_verify_user_session(Database db, char* session_id);

#endif
