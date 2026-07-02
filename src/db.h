#ifndef DB_H
#define DB_H


#include <sqlite3.h>



#define HASH_SIZE 128




typedef struct {
	sqlite3* db;
} Database;

typedef enum {DB_DONE = 0, DB_ERROR = 1, DB_NO_RESULT = 2} DB_RESULT;

Database init_database();



//game tables

DB_RESULT db_create_config_tables(Database db);

DB_RESULT db_create_match(Database db, char* group, char* datetime, char* team1, char* team2);

DB_RESULT db_create_game_tables(Database db);

DB_RESULT db_get_match_ids(Database db, int* ids, int* id_count);

DB_RESULT db_get_match(Database db, int id, char* time, char* team1, char* team2);

DB_RESULT db_get_bet(Database db, int match_id, int user_id, int* pred1, int* pred2, int* predWinner);

DB_RESULT db_get_match_score(Database db, int match_id, int* score1, int* score2, int* winner);

DB_RESULT db_get_time(Database db, int match_id, char* iso);

DB_RESULT db_place_bet(Database db, int match_id, int user_id, int pred1, int pred2, int predWinner);

DB_RESULT db_get_group(Database db, char* group, char* teams);

DB_RESULT db_get_group_bet(Database db, int user_id, char* team, int* pred);

DB_RESULT db_place_group_bet(Database db, int user_id, char* team, int pred);

DB_RESULT db_create_team(Database db, char* group, char* team);

DB_RESULT db_set_match_score(Database db, int id, int score1, int score2, int winner);

DB_RESULT db_get_points(Database db, int* user_ids, int* points, int* count);

DB_RESULT db_get_points_user(Database db, int id, int* out_points);

DB_RESULT db_create_group_result(Database db, char* group, char* team, int position);

DB_RESULT db_get_teams(Database db, char** out_teams);

DB_RESULT db_get_group_result(Database db, char* in_team, int* out_position);

DB_RESULT db_get_group_placements(Database db, char* in_group, char** out_teams);

DB_RESULT db_get_group_defaults(Database db, char* in_group, char** out_teams);

DB_RESULT db_get_group_id(Database db, char* in_team, char** out_group);

// user management

DB_RESULT db_create_user(Database db, char* name, char* pw, int admin, int* out_id);

DB_RESULT db_set_user(Database db, int id, char* name, char* pw, int admin);

DB_RESULT db_get_user_from_name(Database db, char* in_name, int* out_id, char* out_hash, int* out_admin);

DB_RESULT db_get_user(Database db, int in_id, char* out_name, char* out_hash, int* out_admin);

DB_RESULT db_get_user_list(Database db, int* id_list, int* id_count);
		
DB_RESULT db_delete_user(Database db, int id);

DB_RESULT db_set_points(Database db, int user_id, int points);

void db_create_session(Database db, char* session_id, int user_id);

int db_set_username(Database db, int id, char* name);

char* db_get_username(Database db, int id);

int db_user_is_admin(Database db, int id);

int db_verify_user_session(Database db, char* session_id);

#endif
