#include "treasure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("Argumente gresite!\n");
        printf("Incercati: \n");
        printf("  --add <hunt_id>       -> Adauga o comoara\n");
        printf("  --list <hunt_id>      -> Afiseaza detaliile despre toate comorile dintr-un hunt\n");
        printf("  --view <hunt_id> <id> -> Afiseaza detaliile unei comori\n");
        printf("  --remove_treasure <hunt_id> <id> -> Sterge o comoara\n");
        printf("  --remove_hunt <hunt_id> -> Sterge un hunt\n");
        exit(-1);
    }

    char *action = argv[1];
     
    if(strcmp(action, "--add") == 0)
    {
        if(argc != 3)
        {
            printf("Argumente gresite! Incercati --add <hunt_id>\n");
            exit(-2);
        }
        add_treasure(argv[2]);
    }
    else if(strcmp(action, "--list") == 0)
    {
        if(argc != 3)
        {
            printf("Argumente gresite! Incercati: --list <hunt_id>\n");
            exit(-3);
        }
        list_treasures(argv[2]);
    }
    else if(strcmp(action,"--view") == 0)
    {
        if( argc != 4)
        {
            printf("Argumente gresite! Incercati : --view <hunt_id> <treasure_id>\n");
            exit(-4);
        }
        view_treasure(argv[2],atoi(argv[3]));
    }
    else if(strcmp(action, "--remove_treasure") == 0)
    {
        if( argc != 4)
        {
            printf("Argumente gresite! Incercati: --remove_treasure <hunt_id> <treasure_id>\n");
            exit(-5);
        }
        remove_treasure(argv[2],atoi(argv[3]));
    }
    else if(strcmp(action, "--remove_hunt") == 0)
    {
        if( argc != 3)
        {
            printf("Argumente gresite!\n Incercati: --remove_hunt <hunt_id>\n");
            exit(-6);
        }
        remove_hunt(argv[2]);
    }
    return 0;
}
