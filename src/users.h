#ifndef USERS_H
#define USERS_H

#include "db.h"



typedef struct {
	int id;
	char name[128];
	char hash[HASH_SIZE];
	int admin;
} User;





void hash_password(char* hash, char* password);


int user_get(User* user, int id, Database db);

int user_get_from_name(User* user, char* name, Database db);

int user_save(User user, Database db);

int user_create(char* name, char* password, int* out_id, Database db);

int user_set_pw(User* user, char* password);


int change_password(char* name, char* pw_old, char* pw_new, Database db);


char* handle_user_login(char* name, char* password, Database db);






#endif
