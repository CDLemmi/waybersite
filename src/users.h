#ifndef USERS_H
#define USERS_H

#include "db.h"

void hash_password(char* hash, char* password);




char* handle_user_login(char* name, char* password, Database db);



void create_user(int id, char* name, char* password, Database db);


int change_password(char* name, char* pw_old, char* pw_new, Database db);


int verify_session(char* session, Database db);






#endif
