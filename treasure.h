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

#endif
