#include "db.h"

#include <stdio.h> 
#include <stdlib.h>
#include <sodium.h>
#include <string.h>
#include <time.h>





//****** game tables ******


DB_RESULT db_get_group(Database db, char* g, char* teams) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT team FROM groups WHERE `group` = ?;", -1, &s, NULL);
	sqlite3_bind_text(s, 1, g, -1, SQLITE_STATIC);
	char* team = teams;
	while(1) {
		int result = sqlite3_step(s);
		if(result != SQLITE_ROW) break;
		char* out = sqlite3_column_text(s, 0);
		strcpy(team, out);
		team += strlen(team) + 1;
	}
	return DB_DONE;
}


DB_RESULT db_create_config_tables(Database db) {
	
	char* err_msg = NULL;
	int rc;

	char* s1 = "DROP TABLE IF EXISTS matches;";
	rc = sqlite3_exec(db.db, s1, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "[ERROR] sql (q2): %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}


	char* s2 = 
		"CREATE TABLE matches ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"`group` TEXT NOT NULL,"
		"`datetime` TEXT NOT NULL,"
		"team1 TEXT NOT NULL,"
		"team2 TEXT NOT NULL"
		");";
	rc = sqlite3_exec(db.db, s2, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "[ERROR] sql (q3): %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}
	
	char* s4 = "DROP TABLE IF EXISTS groups;";
	rc = sqlite3_exec(db.db, s4, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "[ERROR] sql (q2): %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}

	char* s3 = 
		"CREATE TABLE groups ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"`group` TEXT NOT NULL,"
		"team TEXT NOT NULL UNIQUE"
		");";
	rc = sqlite3_exec(db.db, s3, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "[ERROR] sql (q4): %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}

	return DB_DONE;
}

DB_RESULT db_create_match(Database db, char* group, char* datetime, char* team1, char* team2) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "INSERT INTO matches (`group`, `datetime`, team1, team2) VALUES (?, ?, ?, ?);", -1, &s, NULL);
	sqlite3_bind_text(s, 1, group, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 2, datetime, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 3, team1, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 4, team2, -1, SQLITE_STATIC);
	if(sqlite3_step(s) != SQLITE_DONE) return DB_ERROR;
	sqlite3_finalize(s);
	return DB_DONE;
}

DB_RESULT db_create_team(Database db, char* group, char* team) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "INSERT INTO groups (`group`, team) VALUES (?, ?);", -1, &s, NULL);
	sqlite3_bind_text(s, 1, group, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 2, team, -1, SQLITE_STATIC);
	if(sqlite3_step(s) != SQLITE_DONE) return DB_ERROR;
	sqlite3_finalize(s);
	return DB_DONE;
}

DB_RESULT db_create_game_tables(Database db) {
	char* s1 = 
		"CREATE TABLE IF NOT EXISTS bets ("
		"user_id INTEGER,"
		"match_id INTEGER,"
		"prediction1 INTEGER,"
		"prediction2 INTEGER"
		");";
	
	char* err_msg = NULL;

	int rc = sqlite3_exec(db.db, s1, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "[ERROR] sql (q5): %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}
	
	char* s2 = 
		"CREATE TABLE IF NOT EXISTS played_matches ("
		"match_id INTEGER PRIMARY KEY,"
		"score1 INTEGER,"
		"score2 INTEGER"
		");";
	

	rc = sqlite3_exec(db.db, s2, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "[ERROR] sql (q6): %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}

	char* s3 = 
		"CREATE TABLE IF NOT EXISTS group_bets ("
		"user_id INTEGER,"
		"team TEXT NOT NULL,"
		"prediction INTEGER"
		");";
	

	rc = sqlite3_exec(db.db, s3, NULL, NULL, &err_msg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "[ERROR] sql (q7): %s\n", err_msg);
		sqlite3_free(err_msg);
		exit(1);
	}
	return DB_DONE;
}

DB_RESULT db_get_match_ids(Database db, int* ids, int* id_count) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT id FROM matches;", -1, &s, NULL);
	int i;
	for(i = 0; 1; i++) {
		int result = sqlite3_step(s);
		if(result == SQLITE_ROW) {
			ids[i] = sqlite3_column_int(s, 0);
		} else if(result == SQLITE_DONE) {
			*id_count = i;
			return DB_DONE;
		} else {
			return DB_ERROR;
		}
	}
	return DB_ERROR;
}

DB_RESULT db_get_match(Database db, int id, char* out_time, char* out_team1, char* out_team2) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT `datetime`, team1, team2 FROM matches WHERE id = ?;", -1, &s, NULL);
	sqlite3_bind_int(s, 1, id);
	if(sqlite3_step(s) == SQLITE_ROW) {
		char* time = sqlite3_column_text(s, 0);
		char* team1= sqlite3_column_text(s, 1);
		char* team2 = sqlite3_column_text(s, 2);
		strcpy(out_time, time);
		strcpy(out_team1, team1);
		strcpy(out_team2, team2);
		return DB_DONE;
	}
	return DB_ERROR;
}

DB_RESULT db_get_bet(Database db, int match_id, int user_id, int* pred1, int* pred2) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT prediction1, prediction2 FROM bets WHERE match_id = ? AND user_id = ?;", -1, &s, NULL);
	sqlite3_bind_int(s, 1, match_id);
	sqlite3_bind_int(s, 2, user_id);
	if(sqlite3_step(s) == SQLITE_ROW) {
		*pred1 = sqlite3_column_int(s, 0);
		*pred2 = sqlite3_column_int(s, 1);
		printf("%d\n", *pred1);
		printf("%d\n", *pred2);
		return DB_DONE;
	}
	return DB_NO_RESULT;
}

DB_RESULT db_get_match_score(Database db, int match_id, int* score1, int* score2) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT score1, score2 FROM played_matches WHERE match_id = ?;", -1, &s, NULL);
	sqlite3_bind_int(s, 1, match_id);
	if(sqlite3_step(s) == SQLITE_ROW) {
		*score1 = sqlite3_column_int(s, 0);
		*score2 = sqlite3_column_int(s, 1);
		return DB_DONE;
	}
	return DB_NO_RESULT;
}

DB_RESULT db_get_time(Database db, int match_id, char* out_time) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT `datetime` FROM matches WHERE match_id = ?;", -1, &s, NULL);
	sqlite3_bind_int(s, 1, match_id);
	if(sqlite3_step(s) != SQLITE_ROW) return DB_ERROR;
	char* time = sqlite3_column_text(s, 0);
	strcpy(out_time, time);
	sqlite3_finalize(s);

}

DB_RESULT db_place_bet(Database db, int match_id, int user_id, int pred1, int pred2) {
	sqlite3_stmt* s1;
	sqlite3_prepare_v2(db.db, "SELECT * FROM bets WHERE match_id = ? AND user_id = ?;", -1, &s1, NULL);
	sqlite3_bind_int(s1, 1, match_id);
	sqlite3_bind_int(s1, 2, user_id);

	sqlite3_stmt* s2;
	if(sqlite3_step(s1) == SQLITE_ROW) {
		sqlite3_prepare_v2(db.db, "UPDATE bets SET prediction1 = ?, prediction2 = ? WHERE match_id = ? AND user_id = ?;", -1, &s2, NULL);
	} else {
		sqlite3_prepare_v2(db.db, "INSERT INTO bets (prediction1, prediction2, match_id, user_id) VALUES (?, ?, ?, ?);", -1, &s2, NULL);
	}
	sqlite3_bind_int(s2, 1, pred1);
	sqlite3_bind_int(s2, 2, pred2);
	sqlite3_bind_int(s2, 3, match_id);
	sqlite3_bind_int(s2, 4, user_id);
	if(sqlite3_step(s2) != SQLITE_DONE) return DB_ERROR;
	sqlite3_finalize(s1);
	sqlite3_finalize(s2);
	return DB_DONE;

}

DB_RESULT db_get_group_bet(Database db, int user_id, char* team, int* pred) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "SELECT prediction FROM group_bets WHERE team = ? AND user_id = ?;", -1, &s, NULL);
	sqlite3_bind_text(s, 1, team, -1, SQLITE_STATIC);
	sqlite3_bind_int(s, 2, user_id);
	if(sqlite3_step(s) == SQLITE_ROW) {
		*pred = sqlite3_column_int(s, 0);
		return DB_DONE;
	}
	return DB_NO_RESULT;
}

DB_RESULT db_place_group_bet(Database db, int user_id, char* team, int pred) {
	sqlite3_stmt* s1;
	sqlite3_prepare_v2(db.db, "SELECT * FROM group_bets WHERE team = ? AND user_id = ?;", -1, &s1, NULL);
	sqlite3_bind_text(s1, 1, team, -1, NULL);
	sqlite3_bind_int(s1, 2, user_id);

	sqlite3_stmt* s2;
	if(sqlite3_step(s1) == SQLITE_ROW) {
		sqlite3_prepare_v2(db.db, "UPDATE group_bets SET prediction = ? WHERE team = ? AND user_id = ?;", -1, &s2, NULL);
	} else {
		sqlite3_prepare_v2(db.db, "INSERT INTO group_bets (prediction, team, user_id) VALUES (?, ?, ?);", -1, &s2, NULL);
	}
	sqlite3_finalize(s1);
	sqlite3_bind_int(s2, 1, pred);
	sqlite3_bind_text(s2, 2, team, -1, SQLITE_STATIC);
	sqlite3_bind_int(s2, 3, user_id);
	if(sqlite3_step(s2) != SQLITE_DONE) return DB_ERROR;
	sqlite3_finalize(s2);
	return DB_DONE;
}


//****** user tables ******

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


DB_RESULT db_delete_user(Database db, int id) {
	sqlite3_stmt* s;
	sqlite3_prepare_v2(db.db, "DELETE FROM users WHERE id = ?;", -1, &s, NULL);
	sqlite3_bind_int(s, 1, id);
	int result = sqlite3_step(s);
	if(result != SQLITE_DONE) {
		return DB_ERROR;
	}	
	sqlite3_finalize(s);
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
		fprintf(stderr, "[ERROR] (q1): %s\n", sqlite3_errmsg(db.db));
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











