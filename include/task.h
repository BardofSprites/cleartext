// task.h
#ifndef TASK_H
#define TASK_H

#include <stddef.h>

// Task structure definition
typedef struct {
    char *name;          // Name of the task
    int completed;       // 0 for incomplete, 1 for completed
    char **keywords;     // Array of keywords (strings)
    size_t num_keywords; // Number of keywords in the array
} Task;

// Function declarations
Task create_task(const char *name, const char **keywords, size_t num_keywords);
void add_keyword(Task *task, const char *keyword);
void print_task(const Task *task);
void free_task(Task *task);

#endif // TASK_H
