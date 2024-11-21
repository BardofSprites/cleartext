#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.ha>
#include <errno.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 512
#define MAX_CONTENT_LENGTH 4096
#define NOTES_DIRECTORY "./notes/"

// Note structure to capture richer metadata
typedef struct {
    char title[MAX_FILENAME_LENGTH];
    time_t created_at;
    char* keywords[5];  // Support up to 5 keywords
    int keyword_count;
} Note;

void ensure_directory_exists(const char* path);
char* generate_note_path(time_t timestamp);
void create_note(const char* title);
void list_notes();
void view_note(const char* relative_path);
void search_notes(const char* query);
void delete_note(const char* filename);
void edit_note(const char* filename);

int main(int argc, char* argv[]) {
    // Ensure notes directory exists
    ensure_directory_exists(NOTES_DIRECTORY);

    // Basic CLI argument parsing
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s create \"Note Title\"\n", argv[0]);
        printf("  %s list\n", argv[0]);
        printf("  %s search <query>\n", argv[0]);
        printf("  %s edit <filename>\n", argv[0]);
        printf("  %s delete <filename>\n", argv[0]);
        return 1;
    }

    // Command routing
    if (strcmp(argv[1], "create") == 0 && argc == 3) {
        create_note(argv[2]);
    } else if (strcmp(argv[1], "list") == 0) {
        list_notes();
    } else if (strcmp(argv[1], "search") == 0 && argc == 3) {
        search_notes(argv[2]);
    } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
        delete_note(argv[2]);
    } else if (strcmp(argv[1], "edit") == 0 && argc == 3) {
        edit_note(argv[2]);
    } else {
        printf("Invalid command or arguments.\n");
        return 1;
    }

    return 0;
}

void ensure_directory_exists(const char* path) {
    char tmp[MAX_PATH_LENGTH];
    char* p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(tmp, S_IRWXU);
}

char* generate_note_path(time_t timestamp) {
    static char path[MAX_PATH_LENGTH];
    struct tm* tm_info = localtime(&timestamp);

    // Create YYYY/MM directory structure
    snprintf(path, sizeof(path), "%s%04d/%02d/%04d-%02d-%02d.txt",
             NOTES_DIRECTORY,
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday);

    // Ensure directory exists
    char dir_path[MAX_PATH_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s%04d/%02d/",
             NOTES_DIRECTORY,
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1);
    ensure_directory_exists(dir_path);

    return path;
}

void create_note(const char* title) {
    time_t now = time(NULL);
    char* note_path = generate_note_path(now);

    // Open file for writing
    FILE* file = fopen(note_path, "w");
    if (!file) {
        perror("Error creating note");
        return;
    }

    // Get current time details
    struct tm* tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %a %H:%M", tm_info);

    // Write note template
    fprintf(file, "= %s\n", title);
    fprintf(file, "@created: [%s]\n", timestamp);
    fprintf(file, "@keyword: []\n\n");

    fclose(file);

    // Open in default editor
    char command[MAX_PATH_LENGTH + 20];
    snprintf(command, sizeof(command), "${EDITOR} %s", note_path);
    system(command);

    printf("Note created at: %s\n", note_path);
}

void list_notes() {
    char notes_dir[MAX_PATH_LENGTH];
    snprintf(notes_dir, sizeof(notes_dir), "%s", NOTES_DIRECTORY);

    DIR* dir;
    struct dirent* entry;

    // Walk through years
    DIR* year_dir;
    struct dirent* year_entry;

    dir = opendir(notes_dir);
    if (!dir) {
        perror("Could not open notes directory");
        return;
    }

    printf("Available Notes:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {

            char year_path[MAX_PATH_LENGTH];
            snprintf(year_path, sizeof(year_path), "%s%s", notes_dir, entry->d_name);

            year_dir = opendir(year_path);
            if (year_dir) {
                while ((year_entry = readdir(year_dir)) != NULL) {
                    if (year_entry->d_type == DT_DIR &&
                        strcmp(year_entry->d_name, ".") != 0 &&
                        strcmp(year_entry->d_name, "..") != 0) {

                        char month_path[MAX_PATH_LENGTH];
                        snprintf(month_path, sizeof(month_path), "%s/%s", year_path, year_entry->d_name);

                        DIR* month_dir = opendir(month_path);
                        struct dirent* file_entry;

                        if (month_dir) {
                            while ((file_entry = readdir(month_dir)) != NULL) {
                                if (strstr(file_entry->d_name, ".txt")) {
                                    printf("  %s/%s/%s\n", entry->d_name, year_entry->d_name, file_entry->d_name);
                                }
                            }
                            closedir(month_dir);
                        }
                    }
                }
                closedir(year_dir);
            }
        }
    }
    closedir(dir);
}

void edit_note(const char* filename) {
    char command[MAX_FILENAME_LENGTH + 20];
    char full_path[MAX_FILENAME_LENGTH];

    snprintf(full_path, sizeof(full_path), "%s%s", NOTES_DIRECTORY, filename);
    snprintf(command, sizeof(command), "${EDITOR:-nano} %s", full_path);

    system(command);
}

void delete_note(const char* filename) {
    char full_path[MAX_FILENAME_LENGTH];
    snprintf(full_path, sizeof(full_path), "%s%s", NOTES_DIRECTORY, filename);

    if (remove(full_path) != 0) {
        perror("Error deleting note");
    } else {
        printf("Note deleted successfully.\n");
    }
}

void search_notes(const char* query) {
    char notes_dir[MAX_PATH_LENGTH];
    snprintf(notes_dir, sizeof(notes_dir), "%s", NOTES_DIRECTORY);

    DIR* dir, *year_dir, *month_dir;
    struct dirent* entry, *year_entry, *month_entry;

    dir = opendir(notes_dir);
    if (!dir) {
        perror("Could not open notes directory");
        return;
    }

    printf("Search Results for '%s':\n", query);

    // Walk through years, months, and files
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {

            char year_path[MAX_PATH_LENGTH];
            snprintf(year_path, sizeof(year_path), "%s%s", notes_dir, entry->d_name);

            year_dir = opendir(year_path);
            if (year_dir) {
                while ((year_entry = readdir(year_dir)) != NULL) {
                    if (year_entry->d_type == DT_DIR &&
                        strcmp(year_entry->d_name, ".") != 0 &&
                        strcmp(year_entry->d_name, "..") != 0) {

                        char month_path[MAX_PATH_LENGTH];
                        snprintf(month_path, sizeof(month_path), "%s/%s", year_path, year_entry->d_name);

                        month_dir = opendir(month_path);
                        struct dirent* file_entry;

                        if (month_dir) {
                            while ((file_entry = readdir(month_dir)) != NULL) {
                                if (strstr(file_entry->d_name, ".txt")) {
                                    char full_path[MAX_PATH_LENGTH];
                                    snprintf(full_path, sizeof(full_path), "%s/%s", month_path, file_entry->d_name);

                                    FILE* file = fopen(full_path, "r");
                                    if (!file) continue;

                                    char buffer[MAX_CONTENT_LENGTH];
                                    while (fgets(buffer, sizeof(buffer), file)) {
                                        if (strcasestr(buffer, query)) {
                                            printf("  Match in %s/%s/%s: %s",
                                                   entry->d_name,
                                                   year_entry->d_name,
                                                   file_entry->d_name,
                                                   buffer);
                                            break;
                                        }
                                    }

                                    fclose(file);
                                }
                            }
                            closedir(month_dir);
                        }
                    }
                }
                closedir(year_dir);
            }
        }
    }
    closedir(dir);
}
