#include "treasure.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // O_CREAT, O_RDONLY, ETC.
#include <unistd.h> // read(), write(), close()...
#include <sys/stat.h> // for directory
#include <sys/types.h> // --^
#include <time.h>
#include <dirent.h>


int is_hunt_directory(char *hunt_id)
{
    struct stat st;
    if(stat(hunt_id, &st) == -1) // verifica daca directorul exista deja
    {
        if(mkdir(hunt_id,0777) == -1) 
        {
            perror("Eroare la crearea directorului\n");
            return -1;
        }
    }
    return 0;
}

void log_action(char *hunt_id, char *action)
{
    char log_filepath[LEN];
    snprintf(log_filepath,sizeof(log_filepath),"%s/logged_hunt", hunt_id);

    int log_f=open(log_filepath, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(log_f == -1)
    {
        perror("Eroare la deschiderea fisierului de logg\n");
        return;
    }

    time_t current_time=time(NULL);
    char time_str[LEN];
    strftime(time_str, sizeof(time_str),  "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    dprintf(log_f, "[%s] %s\n", time_str,action);

    close(log_f);
}

void create_symlink(char* hunt_id)
{
    char target_path[LEN];
    snprintf(target_path, sizeof(target_path), "%s/logged_hunt", hunt_id);

    char link_name[LEN];
    snprintf(link_name, sizeof(link_name), "logged_hunt-%s", hunt_id);

    struct stat st;
    // verificam daca exista deja un symlink
    if(lstst(link_name, &st) == 0)
    {
        return; //link ul exista deja
    }
    //cream symlink daca nu exista
    if (symlink(target_path, link_name) == -1)
    {
        perror("Eroare la crearea linkului simbolic");
    }
    else
    {
        printf("Linkul simbolic a fost creat: %s -> %s\n", link_name, target_path);
    }
}

Treasure_t *make_treasure()
{
    Treasure_t *treasure = NULL;
    treasure = (Treasure_t *)malloc(sizeof(Treasure_t));

    if(treasure == NULL)
    {
        perror("Eroare la crearea comorii\n");
        exit(-1);
    }

    printf("Introduceti datele comorii\n");
    printf("ID Comoara: ");
    scanf("%d", &treasure->id);

    printf("Username: ");
    scanf("%s", treasure->username);

    printf("Coordonate GPS:\n");
    printf("Latitudine: ");
    scanf("%f", &treasure->GPS_latitude);
    printf("Longitudine: ");
    scanf("%f",&treasure->GPS_longitude);

    while(getchar() != '\n');
    printf("Indicatie: ");
    fgets(treasure->clue,sizeof(treasure->clue),stdin);

    printf("Valoare: ");
    scanf("%d", &treasure->value);

    while(getchar() != '\n');
    return treasure;
}

void add_treasure(char* hunt_id)
{
    if(is_hunt_directory(hunt_id) != 0 )
    {
        printf("Eroare! Nu se poate crea sau gasi acest director\n");
        return;
    }

    Treasure_t *new_treasure = make_treasure();

    char filepath[LEN];
    snprintf(filepath,sizeof(filepath),"%s/treasures.b",hunt_id);

    int f = open(filepath, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(f == -1)
    {
        perror("Eroare la deschiderea fisierului treasures.b");
        free(new_treasure);
        return;
    }

    if(write(f,new_treasure,sizeof(Treasure_t)) == -1)
    {
        perror("Eroare la scrierea comorii in fisier\n");
        close(f);
        free(new_treasure);
        return;
    }

    close(f);

    free(new_treasure);

    log_action(hunt_id,"S-a adaugat o comoara noua");
    create_symlink(hunt_id);
}

void printf_treasure_info(char* filepath)
{
    int f = open(filepath,O_RDONLY);
    if(f == -1)
    {
        perror("Eroare la deschiderea fisierului de comori\n");
        return;
    }

    struct stat file_stat;
    if(stat(filepath, &file_stat) == -1)
    {
        perror("Eroare informatii");
        close(f);
        return;
    }

    printf("Nume Fisier: %s\n",filepath);
    printf("Dimensiune Fisier: %ld bytes\n", file_stat.st_size);

    //afisam data ultimei modificari
    char time_str[LEN];
    struct tm *time;
    time = localtime(&file_stat.st_mtime);
    strftime(time_str,sizeof(time_str), "%Y-%m-%d %H:%M:%S", time);
    printf("Ultima modificare: %s\n", time_str);

    Treasure_t treasure;
    while(read(f,&treasure,sizeof(Treasure_t)) == sizeof(Treasure_t))
    {
        printf("ID Comoara: %d\n", treasure.id);
        printf("Nume Utilizator: %s\n",treasure.username);
        printf("Coordonate GPS: Latitudine -> %.6f, Longitudine -> %.6f\n", treasure.GPS_latitude, treasure.GPS_longitude);
        printf("Indicatie: %s\n", treasure.clue);
        printf("Valoare %d\n", treasure.value);
    }

    close(f);
}

void list_treasures(char* hunt_id)
{
    DIR *director = opendir(hunt_id);
    if(director==NULL)
    {
        perror("Eroare la deschiderea directorului\n");
        return;
    }

    struct dirent *dir_file;
    while((dir_file = readdir(director)) != NULL)
    {
        if(strstr(dir_file->d_name,".b") != NULL)
        {
            char filepath[LEN];
            snprintf(filepath,sizeof(filepath), "%s/%s", hunt_id, dir_file->d_name);

            printf_treasure_info(filepath);
        }
    }

    close(director);

}
