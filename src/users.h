#ifndef USERS_H
#define USERS_H

#include "db.h"

void hash_password(char* hash, char* password);




char* handle_user_login(char* name, char* password, Database db);



void create_user(int id, char* name, char* password, Database db);












#endif
