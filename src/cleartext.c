#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/task.h"

int main() {
    const char *keywords1[] = {"urgent", "home", "work"};
    const char *keywords2[] = {"urgent", "office"};

    Task task1 = create_task("Complete project", keywords1, 3);
    Task task2 = create_task("Send report", keywords2, 2);

    print_task(&task1);
    print_task(&task2);

    add_keyword(&task1, "important");

    printf("\nUpdated task1:\n");
    print_task(&task1);

    free_task(&task1);
    free_task(&task2);

    return 0;
}
