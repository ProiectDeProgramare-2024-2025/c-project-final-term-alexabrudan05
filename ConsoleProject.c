#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define White   "\033[0m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"
#define MAX_STORAGE 1024.0

typedef struct {
    char name[50];
    float size_gb;
} Game;

void header() {
    printf("\e[1;1H\e[2J\n");
    printf("1 - " RED "Pirate" White " a game from web" BOLD " (not available on the official store)\n" White);
    printf("2 - " BLUE "Display" White " installed games\n");
    printf("3 - " GREEN "Uninstall\n" White);
    printf("4 - " CYAN "Install" White " from Game Store\n");
    printf("5 - View installed games " GREEN "sorted\n" White);
    printf("0 - Exit\n");
}

float get_total_storage(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return 0;

    float total = 0;
    Game game;
    char line[100];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^|]|%f", game.name, &game.size_gb) == 2) {
            total += game.size_gb;
        }
    }
    fclose(file);
    return total;
}

int is_game_installed(const char* game_name, const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) return 0;

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char name[50];
        sscanf(line, "%[^|]", name);
        if (strcmp(name, game_name) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void add_game(const char* path) {
    Game game;
    printf("Enter game name: ");
    getchar();
    scanf("%[^\n]", game.name);
    printf("Enter game size (in GB): ");
    scanf("%f", &game.size_gb);

    if (is_game_installed(game.name, path)) {
        printf("Game '%s' is already installed.\n", game.name);
        return;
    }

    float used = get_total_storage(path);
    if (used + game.size_gb > MAX_STORAGE) {
        printf("Cannot install. Not enough storage (%.2f GB used / %.2f GB max).\n", used, MAX_STORAGE);
        return;
    }

    FILE* file = fopen(path, "a");
    if (!file) {
        printf("Cannot open file.\n");
        return;
    }
    fprintf(file, "%s|%.2f\n", game.name, game.size_gb);
    fclose(file);
    printf("Game '%s' installed successfully.\n", game.name);
}

void add() {
    char a;
    printf("\e[1;1H\e[2J\n");
    add_game("file.txt");
    printf("\nPress Enter to go Back\n");
    getchar();
    scanf("%c", &a);
}

void display_games(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Could not open file.\n");
        return;
    }
    char line[100];
    float total = 0;
    printf("Installed Games:\n");
    while (fgets(line, sizeof(line), file)) {
        Game game;
        if (sscanf(line, "%[^|]|%f", game.name, &game.size_gb) == 2) {
            printf("- %s (%.2f GB)\n", game.name, game.size_gb);
            total += game.size_gb;
        }
    }
    fclose(file);
    printf("\nStorage Used: %.2f GB / %.2f GB\n", total, MAX_STORAGE);
}

void display() {
    char a;
    printf("\e[1;1H\e[2J\n");
    display_games("file.txt");
    getchar();
    scanf("%c", &a);
}

void uninstall_game(const char* path) {
    Game games[100];
    int count = 0;
    char line[100];
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Could not open file.\n");
        return;
    }
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^|]|%f", games[count].name, &games[count].size_gb) == 2) {
            count++;
        }
    }
    fclose(file);
    if (count == 0) {
        printf("No games installed.\n");
        return;
    }
    printf("\e[1;1H\e[2J\n");
    printf("Installed Games:\n");
    for (int i = 0; i < count; i++) {
        printf("%d - %s (%.2f GB)\n", i + 1, games[i].name, games[i].size_gb);
    }
    int choice;
    printf("Enter the number of the game to uninstall (0 to cancel): ");
    scanf("%d", &choice);
    getchar();
    if (choice <= 0 || choice > count) {
        printf("Invalid selection.\n");
        return;
    }
    FILE* temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("Could not open temp file.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        if (i != choice - 1) {
            fprintf(temp, "%s|%.2f\n", games[i].name, games[i].size_gb);
        }
    }
    fclose(temp);
    remove(path);
    rename("temp.txt", path);
    printf("Game '%s' has been uninstalled.\n", games[choice - 1].name);
    printf("\nPress Enter to return...\n");
    getchar();
}

void undoing() {
    uninstall_game("file.txt");
}

int compare_by_size_desc(const void* a, const void* b) {
    Game* gameA = (Game*)a;
    Game* gameB = (Game*)b;
    return (gameB->size_gb > gameA->size_gb) - (gameB->size_gb < gameA->size_gb);
}

int compare_by_name(const void* a, const void* b) {
    Game* gameA = (Game*)a;
    Game* gameB = (Game*)b;
    return strcmp(gameA->name, gameB->name);
}

void sort_and_display(const char* path) {
    Game games[100];
    int count = 0;
    char line[100];
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Could not open file.\n");
        return;
    }
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^|]|%f", games[count].name, &games[count].size_gb) == 2) {
            count++;
        }
    }
    fclose(file);
    if (count == 0) {
        printf("No games installed.\n");
        return;
    }
    int option;
    printf("\e[1;1H\e[2J\n");
    printf("View installed games by:\n");
    printf("1 - Storage size (descending)\n");
    printf("2 - Game name (A-Z)\n");
    printf("Enter your choice: ");
    scanf("%d", &option);
    getchar();

    if (option == 1) {
        qsort(games, count, sizeof(Game), compare_by_size_desc);
    } else if (option == 2) {
        qsort(games, count, sizeof(Game), compare_by_name);
    } else {
        printf("Invalid selection.\n");
        return;
    }
    float total = 0;
    printf("\e[1;1H\e[2J\n");
    printf("\nSorted Games:\n");
    for (int i = 0; i < count; i++) {
        printf("- %s (%.2f GB)\n", games[i].name, games[i].size_gb);
        total += games[i].size_gb;
    }
    printf("\nStorage Used: %.2f GB / %.2f GB\n", total, MAX_STORAGE);
    printf("\nPress Enter to return...\n");
    getchar();
}

void install_from_store(const char* store_path, const char* install_path) {
    FILE* store = fopen(store_path, "r");
    if (!store) {
        printf("Could not open store file.\n");
        return;
    }
    Game games[100];
    int count = 0;
    char line[100];
    printf("\e[1;1H\e[2J\n");
    printf("Available games in the store:\n");
    while (fgets(line, sizeof(line), store)) {
        if (sscanf(line, "%[^|]|%f", games[count].name, &games[count].size_gb) == 2) {
            printf("%d - %s (%.2f GB)\n", count + 1, games[count].name, games[count].size_gb);
            count++;
        }
    }
    fclose(store);
    if (count == 0) {
        printf("No games available in the store.\n");
        return;
    }
    int choice;
    printf("Enter the number of the game to install (0 to cancel): ");
    scanf("%d", &choice);
    getchar();
    if (choice <= 0 || choice > count) {
        printf("Invalid selection.\n");
        return;
    }
    Game selected = games[choice - 1];
    if (is_game_installed(selected.name, install_path)) {
        printf("Game '%s' is already installed.\n", selected.name);
    } else {
        float used = get_total_storage(install_path);
        if (used + selected.size_gb > MAX_STORAGE) {
            printf("Cannot install. Not enough storage (%.2f GB used / %.2f GB max).\n", used, MAX_STORAGE);
            printf("\nPress Enter to return...\n");
            getchar();
            return;
        }
        FILE* install = fopen(install_path, "a");
        if (!install) {
            printf("Could not open install file.\n");
            return;
        }
        fprintf(install, "%s|%.2f\n", selected.name, selected.size_gb);
        fclose(install);
        printf("Game '%s' installed successfully.\n", selected.name);
    }
    printf("\nPress Enter to return...\n");
    getchar();
}

void menu(int op) {
    switch (op) {
        case 1: add(); break;
        case 2: display(); break;
        case 3: undoing(); break;
        case 4: install_from_store("GameStore.txt", "file.txt"); break;
        case 5: sort_and_display("file.txt"); break;
        default: printf("EXIT\n");
    }
}

int main() {
    int option;
    do {
        header();
        printf("Enter option: ");
        scanf("%d", &option);
        menu(option);
    } while (option >= 0 && option <= 5);
    return 0;
}
