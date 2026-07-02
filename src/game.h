#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "db.h"


int init_game(Database db);

cJSON* get_match_list(int user_id, int from_id, int to_id, Database db);

int match_hasnt_started(int match_id, Database db);

cJSON* get_groups(int user_id, Database db);

void update_points(Database db);

int calc_points_match(int match_id, int pred1, int pred2, int score1, int score2, int predWin, int finalWin);

int get_default_pos(Database db, char* team);

#endif
