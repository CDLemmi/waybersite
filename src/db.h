#ifndef DB_H
#define DB_H


#include <sqlite3.h>



typedef struct {
	sqlite3* db;
} Database;



Database init_database();
	

#endif
