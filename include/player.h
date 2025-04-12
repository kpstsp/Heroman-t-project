#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

void player_init(PlayerStats* player);
void player_add_experience(PlayerStats* player, int amount);
void player_add_gold(PlayerStats* player, int amount);
void player_damage(PlayerStats* player, int amount);
void player_heal(PlayerStats* player, int amount);

#endif // PLAYER_H 