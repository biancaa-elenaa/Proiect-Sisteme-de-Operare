#include "treasure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        perror("Argumente gresite!\n");
        printf("Usage: \n");
        printf("  --add <hunt_id>       -> Add a new treasure\n");
        printf("  --list <hunt_id>      -> List all treasures in the specified hunt\n");
        printf("  --view <hunt_id> <id> -> View details of a specific treasure\n");
        printf("  --remove_treasure <hunt_id> <id> -> Remove a specific treasure\n");
        printf("  --remove_hunt <hunt_id> -> Remove an entire hunt\n");
        exit(-1);
    }

    char *action = argv[1];
     
    if(strcmp(action, "--add") == 0)
    {
        if(argc != 3)
        {
            printf("Usage: --add <hunt_id>\n");
            exit(-1);
        }
        add_treasure(argv[2]);
    }
    else if(strcmp(action, "--list") == 0)
    {
        if(argc != 3)
        {
            printf("Usage: --list <hunt_id>\n");
            exit(-1);
        }
        list_treasures(argv[2]);
    }
    return 0;
}
