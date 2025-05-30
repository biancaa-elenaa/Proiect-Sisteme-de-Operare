#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <dirent.h>
#include "treasure.h"
#include "treasure.c"

#define CMD_FILE "command_file.txt"

volatile sig_atomic_t got_command = 0;
volatile sig_atomic_t stop_signal = 0;

void handle_sigusr1(int sig)
{
    got_command = 1;
}

void list_hunts()
{
    DIR *directory = opendir(".");
    if(directory  == NULL)
    {
        perror("Eroare la deschiderea directorului principal\n");
        return;
    }

    struct dirent *files;
    while((files = readdir(directory)) != NULL)
    {
        if(files->d_type == DT_DIR && strcmp(files->d_name, ".") != 0 && strcmp(files->d_name, "..") != 0)
        {
            char treasure_file_path[300];
            snprintf(treasure_file_path,sizeof(treasure_file_path), "%s/treasures.bin", files->d_name);

            if(access(treasure_file_path, F_OK) == 0)
            {
                //treasure.bin exista deci e un hunt valid
                int f = open(treasure_file_path, O_RDONLY);
                if(f == -1)
                {
                    perror("eroare la deschiderea fisierului treasures.bin");
                    continue ;
                }

                int count =0 ;
                Treasure_t treasure;
                while(read(f,&treasure,sizeof(Treasure_t)) == sizeof(Treasure_t))
                {
                    count ++;
                }

                close(f);
                printf("Hunt: %s | Comori: %d\n", files->d_name,count);
                //fflush(stdout);
            }
        }
    }
    closedir(directory);
}

void process_comand()
{
    char filepath[100];
    snprintf(filepath,sizeof(CMD_FILE),CMD_FILE);

    int op;
    if((op = open(filepath, O_RDONLY)) == -1)
    {
        perror("Eroare deschidere fisier\n");
        exit(-1);
    }

    char command[100];
    int reead = read(op,command,sizeof(command)-1);
    if(reead > 0)
    {
        command[reead] = '\0';
    } else{
        perror("Eroare la citirea comenzii din fisier");
        return;
    }
    close(op);

    command[strcspn(command, "\r\n")] = 0;


    if(strcmp(command,"start_monitor") == 0)
    {
        truncate(CMD_FILE,0);
    }
    if(strstr(command, "list_hunts") != 0)
    {      
        list_hunts();      
    }
    else if(strstr(command, "list_treasures") != 0)
    {
        //printf("Comanda '%s'\n",command);
        char *dup = strdup(command);
        char *token = strtok(dup, " "); //list_treasures
        char *hunt_id = strtok(NULL, " ");
        hunt_id[strcspn(hunt_id, "\r\n")] = '\0';
        //printf("hunt_id transmis '%s'\n",hunt_id);
        if(hunt_id)
        {
            list_treasures(hunt_id);
        }
        else
        {
            printf("[Monitor] Eroare hunt_id!\n");

        }
        free(dup);
    }
    else if(strstr(command, "view_treasures") != 0)
    {
        char *dup = strdup(command);
        char *token = strtok(dup," "); //view_treasures
        char *hunt_id = strtok(NULL," "); 
        char *id = strtok(NULL," ");

        if(hunt_id && id)
        {
            int treasure_id = atoi(id);
            view_treasure(hunt_id,treasure_id);
        }
        else
        {
            printf("[Monitor] Eroare trebuie introdus atat hunt_id cat si treasure_id");
        }

        free(dup);


    }
    else if(strstr(command, "stop_monitor") != 0)
    {
        usleep(3000000);
        exit(0);
    }
    else if(strcmp(command, "exit") == 0)
    {
        printf("[Monitor] Monitorul a fost oprit prin comanda 'exit'!\n");
        exit(0);
    }
    else
    {
        printf("[Monitor] Comanda necunoscuta! %s\n", command);
        
    }

    truncate(CMD_FILE,0);
}

int main()
{
    setvbuf(stdout, NULL, _IONBF,0);

    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if(sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("Eroare sigaction\n");
        exit(-1);
    }
    
    while(1)
    {
        pause(); //asteapta dupa semnal
        if ( got_command )
        {
            process_comand();
            got_command=0;
        }
        usleep(1000000);
    }
}