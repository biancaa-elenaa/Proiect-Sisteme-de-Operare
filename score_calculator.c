#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // O_CREAT, O_RDONLY, ETC.
#include <unistd.h> // read(), write(), close()...
#include <sys/stat.h> // for directory
#include <sys/types.h> // --^
#include <time.h>
#include <dirent.h>
#include <ctype.h>

#define LEN 1000

typedef struct{
    int id;
    char username[LEN];
    float GPS_latitude,GPS_longitude;
    char clue[LEN];
    int value;
}Treasure_t;

typedef struct{
    char username[LEN];
    int total_score;
}UserScore_t;

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("Eroare! Incercati: %s <hunt_id>\n",argv[0]);
        return 1;
    }

    char path[250];
    snprintf(path,sizeof(path),"%s/treasures.bin",argv[1]);
    int f = open(path, O_RDONLY);
    if(f < 0)
    {
        perror("Eroare la deschiderea fisierului treasures.bin!");
        return 1;
    }

    UserScore_t users[100];
    int count=0;

    Treasure_t treasure;
    while(read(f,&treasure,sizeof(Treasure_t)) == sizeof(Treasure_t))
    {
        int found=0;
        for(int i=0;i<count;i++)
        {
            if(strcmp(users[i].username,treasure.username) == 0)
            {
                users[i].total_score += treasure.value;
                found = 1;
                break;
            }
        }

        if(!found)
        {
            strcpy(users[count].username,treasure.username);
            users[count].total_score = treasure.value;
            count++;
        }
    }

    close(f);

    for(int i=0;i<count;i++)
    {
        printf("User %s: score = %d\n",users[i].username,users[i].total_score);
    }
    return 0;
}