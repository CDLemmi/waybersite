#include "users.h"

#include "db.h"

#include <sodium.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


#define HASH_SIZE crypto_pwhash_STRBYTES


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




void create_user(int id, char* name, char* password, Database db) {
	char hash[HASH_SIZE];
	hash_password(hash, password);

	db_set_user(db, id, name, hash);

}





char* handle_user_login(char* name, char* password, Database db) {
	int user_id = 0;
	char* db_hash = db_get_user_hash_and_id(db, name, &user_id);
	if(db_hash == NULL) return NULL;
	
	if(crypto_pwhash_str_verify(db_hash, password, strlen(password))) return NULL;

	char* session_id = create_session_id();

	db_create_session(db, session_id, user_id);

	return session_id;
	
}















