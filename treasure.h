#ifndef TREASURE_H
#define TREASURE_H

#define LEN 1000

typedef struct{
    int id;
    char username[LEN];
    float GPS_latitude,GPS_longitude;
    char clue[LEN];
    int value;
}Treasure_t;

void add_treasure(char* hunt_id);
void list_treasures(char* hunt_id);
void view_treasure(char* hunt_id,int treasure_id);
void remove_treasure(char* hunt_id,int treasure_id);
void remove_hunt(char* hunt_id);

int is_hunt_directory(char* hunt_id);
void log_action(char* hunt_id, char *action);
void create_symlink(char* hunt_id);
Treasure_t *make_treasure();
void print_treasure(Treasure_t *treasure);
void printf_treasure_info(char* filepath);

#endif
