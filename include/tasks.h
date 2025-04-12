#ifndef TASKS_H
#define TASKS_H

#include "game.h"

void task_init(Task* task, const char* title, const char* description, int difficulty, int type);
void task_complete(Task* task);
void task_reset(Task* task);
int task_get_reward(const Task* task);

#endif // TASKS_H 