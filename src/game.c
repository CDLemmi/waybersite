#define _GNU_SOURCE
#include "game.h"


#include <stdio.h>
#include <string.h>
#include <time.h>


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


cJSON* get_groups(int user_id, Database db) {
	cJSON* json = cJSON_CreateArray();
	
	char group[2];
	for(int i = 0; i < 12; i++) {
		group[0] = 'A' + i;

		char teams[800] = {0};
		db_get_group(db, group, teams);

		cJSON* group_json = cJSON_CreateObject();
		cJSON_AddStringToObject(group_json, "group", group);
		
		cJSON* teams_json = cJSON_CreateArray();
		char* team = teams;
		for(int j = 0; j < 4; j++) {
			cJSON* team_json = cJSON_CreateObject();
			cJSON_AddStringToObject(team_json, "team", team);
			int pred = -1; 
			db_get_group_bet(db, user_id, team, &pred);
			cJSON_AddNumberToObject(team_json, "prediction", pred);
			cJSON_AddItemToArray(teams_json, team_json);
			team = team + strlen(team) + 1;
		}
		cJSON_AddItemToObject(group_json, "teams", teams_json);
		cJSON_AddItemToArray(json, group_json);
	}
	
	return json;
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

	char* content = malloc(fsize + 1);
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
	cJSON_Delete(json);

	file = fopen("config/groups.json", "r");

	
	fseek(file, 0, SEEK_END);

	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);  /* same as rewind(f); */

	content = (char*)malloc(fsize + 1);
	fread(content, fsize, 1, file);
	fclose(file);

	content[fsize] = 0;


	json = cJSON_Parse(content);
	cJSON* group_json;
	cJSON* groups_json = cJSON_GetObjectItem(json, "groups");
	cJSON_ArrayForEach(group_json, groups_json) {
		char* group = cJSON_GetObjectItem(group_json, "name")->valuestring;
		cJSON* teams_json = cJSON_GetObjectItem(group_json, "teams");
		cJSON* team_json;
		cJSON_ArrayForEach(team_json, teams_json) {
			db_create_team(db, group, team_json->valuestring);
		}
	}

	free(content);
	cJSON_Delete(json);


	return 0;
}



int init_game(Database db) {

	parse_config(db);

	db_create_game_tables(db);
}



int is_bet_valid(int match_id, Database db) {
	
	char iso[128];
	db_get_time(db, match_id, iso);


    struct tm t = {0};
    strptime(iso, "%Y-%m-%dT%H:%M:%S", &t);
    t.tm_isdst = -1; // let mktime figure out DST
    int time_match = mktime(&t);
	

	return ((int)time(NULL) < time_match);

}

