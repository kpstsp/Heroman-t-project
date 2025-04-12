#include "player.h"
#include <stdio.h>

void player_init(PlayerStats* player) {
    if (!player) return;
    
    player->health = 50;
    player->experience = 0;
    player->level = 1;
    player->gold = 0;
    player->strength = 1;
    player->intelligence = 1;
    player->constitution = 1;
    player->perception = 1;
}

void player_add_experience(PlayerStats* player, int amount) {
    if (!player) return;
    
    player->experience += amount;
    // TODO: Implement level up logic
}

void player_add_gold(PlayerStats* player, int amount) {
    if (!player) return;
    
    player->gold += amount;
}

void player_damage(PlayerStats* player, int amount) {
    if (!player) return;
    
    player->health -= amount;
    if (player->health < 0) {
        player->health = 0;
    }
}

void player_heal(PlayerStats* player, int amount) {
    if (!player) return;
    
    player->health += amount;
    // TODO: Add max health limit
} 