#include "users.h"

#include "db.h"

#include <sodium.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


#define HASH_SIZE 128


char* create_session_id() {
	uint32_t v = randombytes_random();
	char* id = calloc(9, 1);
	for(int i = 0; i < 8; i++) {
		char c;
		int b = v & 0b1111;
		v = v >> 4;
		if(b >= 0 && b <= 9) {
			c = '0' + b;
		} else if(b >= 10 && b <= 15) {
			c = 'a' + b - 10;
		}
		id[i] = c;
	}
	return id;
}


//0 = success
//1 = wrong
int change_password(char* name, char* pw_old, char* pw_new, Database db) {
	int user_id = 0;
	User user;
	if(user_get_from_name(&user, name, db)) {
		return 1;
	}
	if(crypto_pwhash_str_verify(user.hash, pw_old, strlen(pw_old))) return 1;
	
	char hash[HASH_SIZE];
	hash_password(hash, pw_new);

	memcpy(user.hash, hash, HASH_SIZE);
	user_save(user, db);

	return 0;
}


void hash_password(char* hash, char* password) {
	if (crypto_pwhash_str(
            hash,
            password, strlen(password),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Out of memory\n");
        exit(2);
    } 
}



int user_save(User user, Database db) {
	if(db_set_user(db, user.id, user.name, user.hash, user.admin) == DB_DONE) {
		return 0;
	}
	return 1;
}


int user_create(char* name, char* password, int* out_id,  Database db) {
	char hash[HASH_SIZE];
	hash_password(hash, password);

	if(db_create_user(db, name, hash, 0, out_id) != DB_DONE) {
		return 1;
	}
	return 0;
}



DB_RESULT db_get_user_from_name(Database db, char* in_name, int* out_id, char* out_hash, int* out_admin);

DB_RESULT db_get_user(Database db, int in_id, char* out_name, char* out_hash, int* out_admin);






int user_get_from_name(User* user, char* name, Database db) {
	if(db_get_user_from_name(db, name, &(user->id), (user->hash), &user->admin) != DB_DONE) return 1;
	return 0;
}

int user_get(User* user, int id, Database db) {
	if(db_get_user(db, id, (user->name), (user->hash), &user->admin) != DB_DONE) return 1;
	user->id = id;
	return 0;
}




char* handle_user_login(char* name, char* password, Database db) {
	User user;
	if(user_get_from_name(&user, name, db)) return NULL;
	
	if(crypto_pwhash_str_verify(user.hash, password, strlen(password))) return NULL;

	char* session_id = create_session_id();

	db_create_session(db, session_id, user.id);

	return session_id;
	
}















