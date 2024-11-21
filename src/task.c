// task.c
#include "../include/task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Task create_task(const char *name, const char **keywords, size_t num_keywords) {
    Task task;
    task.name = strdup(name);
    task.completed = 0;

    task.num_keywords = num_keywords;
    task.keywords = (char **)malloc(sizeof(char *) * num_keywords);

    for (size_t i = 0; i < num_keywords; ++i) {
        task.keywords[i] = strdup(keywords[i]);
    }

    return task;
}

void add_keyword(Task *task, const char *keyword) {
    task->keywords = (char **)realloc(task->keywords, sizeof(char *) * (task->num_keywords + 1));
    task->keywords[task->num_keywords] = strdup(keyword);
    task->num_keywords++;
}

void print_task(const Task *task) {
    printf("Task: %s\n", task->name);
    printf("Completed: %s\n", task->completed ? "Yes" : "No");

    printf("Keywords: ");
    for (size_t i = 0; i < task->num_keywords; ++i) {
        printf("%s", task->keywords[i]);
        if (i < task->num_keywords - 1) {
            printf(", ");
        }
    }
    printf("\n");
}

void free_task(Task *task) {
    free(task->name);
    for (size_t i = 0; i < task->num_keywords; ++i) {
        free(task->keywords[i]);
    }
    free(task->keywords);
}
