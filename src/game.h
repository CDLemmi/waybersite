#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "db.h"


int init_game(Database db);


cJSON* get_match_list(int user_id, Database db);










#endif
