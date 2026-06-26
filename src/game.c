#define _GNU_SOURCE
#include "game.h"


#include <stdio.h>
#include <string.h>
#include <time.h>


#include "db.h"



cJSON* get_match_list(int user_id, int from_id, int to_id, Database db) {
	int ids[256];
	int id_count = 0;
	db_get_match_ids(db, ids, &id_count);
	
	if(from_id < 1) from_id = 1; //Makes sure the from_id is not smaller than allowed
	if(id_count < to_id) to_id = id_count; //Makes sure the to_id is not greater than the max possible ids

	cJSON* list = cJSON_CreateArray();

	for(int i = from_id - 1; i < to_id; i++) {
		char time[128];
		char team1[128];
		char team2[128];
		
		db_get_match(db, ids[i], time, team1, team2);
		
		cJSON* match = cJSON_CreateObject();
		cJSON_AddNumberToObject(match, "id", ids[i]);
		cJSON_AddStringToObject(match, "time", time);
		cJSON_AddStringToObject(match, "team1", team1);
		cJSON_AddStringToObject(match, "team2", team2);

		int pred1 = 0, pred2 = 0, predWinner = 0;
		db_get_bet(db, ids[i], user_id, &pred1, &pred2, &predWinner); 
		cJSON_AddNumberToObject(match, "prediction1", pred1);
		cJSON_AddNumberToObject(match, "prediction2", pred2);
		cJSON_AddNumberToObject(match, "predicted_winner", predWinner);

		int score1 = -1, score2 = -1, winner = -1;
		db_get_match_score(db, ids[i], &score1, &score2, &winner);
		cJSON_AddNumberToObject(match, "score1", score1);
		cJSON_AddNumberToObject(match, "score2", score2);
		cJSON_AddNumberToObject(match, "winner", winner);

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



int match_hasnt_started(int match_id, Database db) {
	
	char iso[128];
	db_get_time(db, match_id, iso);


    struct tm t = {0};
    strptime(iso, "%Y-%m-%d %H:%M:%S", &t);
    t.tm_isdst = -1; // let mktime figure out DST
    int time_match = mktime(&t);
	

	return ((int)time(NULL) < time_match);

}

void update_points(Database db)
{
	//Get all match ids before updating users
	int ids[256];
	int id_count = 0;
	db_get_match_ids(db, ids, &id_count);

	int users[128];
	int user_count = 0;
	db_get_user_list(db, users, &user_count);

	//Update the scores for every user
	for(int i = 0; i < user_count; i++)
	{
		int user_id = users[i];
		int points = 0;

		for(int j = 0; j < id_count; j++)
		{
			int p1 = 0, p2 = 0;
			int s1 = -1, s2 = -1;	
				
			db_get_match_score(db, ids[j], &s1, &s2, NULL);

			if(s1 == -1 || s2 == -1) continue; //Skip matches that aren't finished

			db_get_bet(db, ids[j], user_id, &p1, &p2, NULL);		
			points += calc_score(p1, p2, s1, s2);

		}
			
		db_set_points(db, user_id, points);
	}
}

int calc_score(int pred1, int pred2, int score1, int score2)
{
	int points = 0;

	if((pred1 > pred2 && score1 > score2) //2 points for guessing the right winner
        || (pred1 < pred2 && score1 < score2)
        || (pred1 == pred2 && score1 == score2)) points = 2;
    
    if((pred1 - pred2 == score1 - score2) && score1 != score2) points = 3; //3 points for the correct goal diff (except when tie)

    if(pred1 == score1 && pred2 == score2) points = 4; //4 points for the correct match score

	printf("%d, %d, %d, %d, %d\n", pred1, pred2, score1, score2, points);

    return points;
}