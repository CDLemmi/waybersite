#include "game.h"


#include <stdio.h>


#include "db.h"



cJSON* get_match_list(int user_id, Database db) {
	int ids[256];
	int id_count = 0;
	db_get_match_ids(db, ids, &id_count);
	
	cJSON* list = cJSON_CreateArray();

	for(int i = 0; i < id_count; i++) {
		char time[128];
		char team1[128];
		char team2[128];
		
		db_get_match(db, ids[i], time, team1, team2);
		
		cJSON* match = cJSON_CreateObject();
		cJSON_AddNumberToObject(match, "id", ids[i]);
		cJSON_AddStringToObject(match, "time", time);
		cJSON_AddStringToObject(match, "team1", team1);
		cJSON_AddStringToObject(match, "team2", team2);

		int pred1 = -1, pred2 = -1;
		db_get_bet(db, ids[i], user_id, &pred1, &pred2); 
		cJSON_AddNumberToObject(match, "prediction1", pred1);
		cJSON_AddNumberToObject(match, "prediction2", pred2);

		int score1 = -1, score2 = -1;
		db_get_match_score(db, ids[i], &score1, &score2);
		cJSON_AddNumberToObject(match, "score1", score1);
		cJSON_AddNumberToObject(match, "score2", score2);

		cJSON_AddItemToArray(list, match);
	}

	return list;


}



int parse_config(Database db) {
	if(db_create_config_tables(db) != DB_DONE) {
		printf("[ERROR] failed creating game tables\n");
		exit(1);
	}

	FILE* file = fopen("config/matches.json", "r");

	
	fseek(file, 0, SEEK_END);

	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);  /* same as rewind(f); */

	char *content = malloc(fsize + 1);
	fread(content, fsize, 1, file);
	fclose(file);

	content[fsize] = 0;


	cJSON* json = cJSON_Parse(content);
	cJSON* match_json;
	cJSON* matches_json = cJSON_GetObjectItem(json, "matches");
	cJSON_ArrayForEach(match_json, matches_json) {
		char* group = cJSON_GetObjectItem(match_json, "group")->valuestring;
		char* time = cJSON_GetObjectItem(match_json, "time")->valuestring;
		char* team1 = cJSON_GetObjectItem(match_json, "team1")->valuestring;
		char* team2 = cJSON_GetObjectItem(match_json, "team2")->valuestring;
		db_create_match(db, group, time, team1, team2);
	}

	free(content);
	return 0;
}



int init_game(Database db) {

	parse_config(db);

	db_create_game_tables(db);





}




