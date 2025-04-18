#include "treasure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    
    char *action = argv[1];

    if(argc < 2)
    {
        printf("Argumente gresite!\n");
        printf("Incercati: \n");
        printf("./treasure_manager --add <hunt_id>       -> Adauga o comoara\n");
        printf(" ./treasure_manager --list <hunt_id>      -> Afiseaza detaliile despre toate comorile dintr-un hunt\n");
        printf(" ./treasure_manager --view <hunt_id> <id> -> Afiseaza detaliile unei comori\n");
        printf(" ./treasure_manager --remove_treasure <hunt_id> <id> -> Sterge o comoara\n");
        printf(" ./treasure_manager --remove_hunt <hunt_id> -> Sterge un hunt\n");
        exit(-1);
    } 
    if(strcmp(action, "--add") == 0)
    {
        if(argc != 3)
        {
            printf("Argumente gresite! Incercati: ./treasure_manager --add <hunt_id>\n");
            exit(-2);
        }
        add_treasure(argv[2]);
    }
    else if(strcmp(action, "--list") == 0)
    {
        if(argc != 3)
        {
            printf("Argumente gresite! Incercati: ./treasure_manager --list <hunt_id>\n");
            exit(-3);
        }
        list_treasures(argv[2]);
    }
    else if(strcmp(action,"--view") == 0)
    {
        if( argc != 4)
        {
            printf("Argumente gresite! Incercati : ./treasure_manager --view <hunt_id> <treasure_id>\n");
            exit(-4);
        }
        view_treasure(argv[2],atoi(argv[3]));
    }
    else if(strcmp(action, "--remove_treasure") == 0)
    {
        if( argc != 4)
        {
            printf("Argumente gresite! Incercati: ./treasure_manager --remove_treasure <hunt_id> <treasure_id>\n");
            exit(-5);
        }
        remove_treasure(argv[2],atoi(argv[3]));
    }
    else if(strcmp(action, "--remove_hunt") == 0)
    {
        if( argc != 3)
        {
            printf("Argumente gresite!\n Incercati: ./treasure_manager --remove_hunt <hunt_id>\n");
            exit(-6);
        }
        remove_hunt(argv[2]);
    }
    else
    {
        printf("Argumente gresite!\n");
        printf("Incercati: \n");
        printf("./treasure_manager --add <hunt_id>       -> Adauga o comoara\n");
        printf(" ./treasure_manager --list <hunt_id>      -> Afiseaza detaliile despre toate comorile dintr-un hunt\n");
        printf(" ./treasure_manager --view <hunt_id> <id> -> Afiseaza detaliile unei comori\n");
        printf(" ./treasure_manager --remove_treasure <hunt_id> <id> -> Sterge o comoara\n");
        printf(" ./treasure_manager --remove_hunt <hunt_id> -> Sterge un hunt\n");
        exit(-1);
    }

    return 0;
}
