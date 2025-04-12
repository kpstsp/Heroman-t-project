#include "tasks.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void task_init(Task* task, const char* title, const char* description, int difficulty, int type) {
    if (!task) return;
    
    strncpy(task->title, title, 255);
    task->title[255] = '\0';
    
    if (description) {
        strncpy(task->description, description, 511);
        task->description[511] = '\0';
    } else {
        task->description[0] = '\0';
    }
    
    task->difficulty = difficulty;
    task->type = type;
    task->completed = 0;
    task->streak = 0;
    task->last_completed = 0;
}

void task_complete(Task* task) {
    if (!task) return;
    
    time_t now = time(NULL);
    if (task->last_completed > 0) {
        // Check if this is part of a streak (completed within 24 hours)
        if (now - task->last_completed <= 86400) {
            task->streak++;
        } else {
            task->streak = 1;
        }
    } else {
        task->streak = 1;
    }
    
    task->completed = 1;
    task->last_completed = now;
}

void task_reset(Task* task) {
    if (!task) return;
    
    task->completed = 0;
}

int task_get_reward(const Task* task) {
    if (!task) return 0;
    
    // Base reward is difficulty * 10
    int base_reward = task->difficulty * 10;
    
    // Add streak bonus (10% per streak)
    int streak_bonus = (base_reward * task->streak) / 10;
    
    return base_reward + streak_bonus;
} 