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
#include <ctype.h>


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
    snprintf(log_filepath,sizeof(log_filepath),"%s/logged_hunt.log", hunt_id);

    int log_f=open(log_filepath, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(log_f == -1)
    {
        perror("Eroare la deschiderea fisierului de logg\n");
        return;
    }

    time_t current_time=time(NULL);
    char time_str[LEN];
    strftime(time_str, sizeof(time_str),  "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    //dprintf(log_f, "[%s] %s\n", time_str,action);
    char log_message[LEN];
    int bytes_written = snprintf(log_message,sizeof(log_message),"[%s] %s\n", time_str,action);

    if(bytes_written > 0)
    {
        ssize_t rez = write(log_f,log_message,bytes_written);

        if(rez == -1)
        {
            perror("Eroare la scrierea in fisier\n");
        }
        else if(rez < bytes_written)
        {
            fprintf(stderr,"Nu au fost scrisi toti octetii cititi\n");
        }
    }

    close(log_f);
}

void create_symlink(char* hunt_id)
{
    char target_path[LEN];
    snprintf(target_path, sizeof(target_path), "%s/logged_hunt.log", hunt_id);

    char link_name[LEN];
    snprintf(link_name, sizeof(link_name), "logged_hunt-%s.log", hunt_id);

    struct stat st;
    // verificam daca exista deja un symlink
    if(lstat(link_name, &st) == 0)
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

void clear_stdin()
{
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF);
}

Treasure_t *make_treasure()
{
    Treasure_t *treasure = NULL;
    treasure = (Treasure_t *)malloc(sizeof(Treasure_t));

    if(treasure == NULL)
    {
        perror("Eroare la alocarea memoriei pentru comoara\n");
        exit(-1);
    }

    printf("Introduceti datele comorii\n");

    do{
        printf("ID Comoara (numar intreg pozitiv): ");
        if(scanf("%d", &treasure->id) != 1 || treasure->id <= 0)
        {
            printf("ID invalid! Incercati un numar natural pozitiv\n");
            clear_stdin();
        } else break;
    }while(1);

    do{
        int valid=1;
        printf("Username (fara spatii): ");
        clear_stdin();

        if(fgets(treasure->username, sizeof(treasure->username), stdin) == NULL)
        {
            perror("Eroare la citirea username-ului!\n");
            return NULL;
        }
        treasure->username[strcspn(treasure->username,"\n")] = '\0';

        for(int i=0;i<strlen(treasure->username);i++)
        {
            if(isspace(treasure->username[i]))
            {
                valid=0;
                break;
            }
        }
        if(strlen(treasure->username) == 0 || !valid)
        {
            printf("Username invalid!Nu folositi spatiu!\n");
        }else break;
    }while(1);
    

    printf("Coordonate GPS:\n");

    do{
        printf("Latitudine: ");
        if(scanf("%f", &treasure->GPS_latitude) != 1 )
        {
            printf("Latitudine invalida!\n");
            clear_stdin();
        }else break;
    }while(1);

    do{
        printf("Longitudine: ");
        if( scanf("%f",&treasure->GPS_longitude) != 1 )
        {
            printf("Longitudine invalida!\n");
            clear_stdin();
        }else break;
    }while(1);

    clear_stdin();

    do{
        printf("Indicatie: ");
        if(fgets(treasure->clue,sizeof(treasure->clue),stdin) == NULL)
        {
            perror("Eroare la citirea indicatiei!\n");
            return NULL;
        }

        treasure->clue[strcspn(treasure->clue, "\n")] = '\0'; //eliminam newline-ul

        int only_spaces=1;
        for(int i=0;i<strlen(treasure->clue);i++)
        {
            if(!isspace(treasure->clue[i]))
            {
                only_spaces = 0;
                break;
            }
        }

        if(strlen(treasure->clue) == 0 || only_spaces)
        {
            printf("Indicatie invalida! Indicatia nu poate fi goala!\n");
        }else break;
    }while(1);

    do{
        printf("Valoare (numar pozitiv): ");
        if(scanf("%d", &treasure->value) != 1 || treasure->value < 0)
        {
            printf("Valoare invalida!\n");
            clear_stdin();
        }else break;
    }while(1);

    clear_stdin();

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
    snprintf(filepath,sizeof(filepath),"%s/treasures.bin",hunt_id);

    int f = open(filepath, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(f == -1)
    {
        perror("Eroare la deschiderea fisierului treasures.bin");
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

    char action[256];
    snprintf(action,sizeof(action), "S-a adaugat comoara cu ID-ul %d!", new_treasure->id);
    log_action(hunt_id,action);
    create_symlink(hunt_id);
    free(new_treasure);
}

void print_treasure(Treasure_t *treasure)
{
    printf("ID Comoara: %d\n", treasure->id);
    printf("Nume Utilizator: %s\n",treasure->username);
    printf("Coordonate GPS: Latitudine -> %.6f, Longitudine -> %.6f\n", treasure->GPS_latitude, treasure->GPS_longitude);
    printf("Indicatie: %s\n", treasure->clue);
    printf("Valoare: %d\n", treasure->value);
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
        print_treasure(&treasure);
    }

    close(f);
}

void list_treasures(char* hunt_id)
{
    //printf("Director: '%s'\n",hunt_id);
    DIR *director = opendir(hunt_id);
    if(director==NULL)
    {
        perror("Eroare la deschiderea directorului!\n");
        return;
    }

    struct dirent *dir_file;
    while((dir_file = readdir(director)) != NULL)
    {
        if(strstr(dir_file->d_name,".bin") != NULL)
        {
            char filepath[LEN];
            snprintf(filepath,sizeof(filepath), "%s/%s", hunt_id, dir_file->d_name);

            printf_treasure_info(filepath);
            fflush(stdout);
        }
    }

    closedir(director);

    log_action(hunt_id,"S-au listat informatiile despre comori!");
    create_symlink(hunt_id);

}

void view_treasure(char* hunt_id,int treasure_id)
{
    DIR *director = opendir(hunt_id);
    if(director == NULL)
    {
        printf("Eroare! Hunt-ul %s nu exista!\n",hunt_id);
        return;
    }
    closedir(director); //l-am deschis doar sa ne asiguram ca exista, deci il inchidem

    char filepath[LEN];
    snprintf(filepath,sizeof(filepath),"%s/treasures.bin",hunt_id);

    int f = open(filepath,O_RDONLY);
    if(f == -1)
    {
        perror("Eroare la deschiderea fisierului treasures.bin!\n");
        return;
    }

    Treasure_t treasure;
    int flag=0; // 1 cand gasim comoara pe care o vrem

    while(read(f,&treasure,sizeof(Treasure_t)) == sizeof(Treasure_t))
    {
        if(treasure.id == treasure_id)
        {
            print_treasure(&treasure);
            flag=1;
            break;
        }
    }

    close(f);

    if(flag == 0)
    {
        printf("Nu exista comoara cu ID-ul %d in hunt-ul %s!\n",treasure_id,hunt_id);
        char action[256];
        snprintf(action,sizeof(action), "S-a incercat vizualizarea comorii cu ID-ul %d, dar aceasta nu exista!", treasure_id);
        log_action(hunt_id,action);
        create_symlink(hunt_id);

    }
    else{
        char action[256];
        snprintf(action,sizeof(action), "S-a vizualizat comoara cu ID-ul %d!", treasure_id);
        log_action(hunt_id,action);
        create_symlink(hunt_id);
    }
} 

void remove_treasure(char *hunt_id, int treasure_id)
{
    char answer[10];
    printf("Sigur doriti sa stergeti comoara cu ID-ul %d? yes/no\n",treasure_id);
    if( scanf("%s", answer) != 1)
    {
        perror("Eroare la citirea raspunsului!\n");
        return;
    }

    if(strcmp(answer,"no") == 0)
    {
        printf("Comoara nu va fi stearsa!\n");
        return;
    }
    else if(strcmp(answer,"yes") == 0)
    {
        char filepath[LEN];
        snprintf(filepath,sizeof(filepath), "%s/treasures.bin",hunt_id);

        int f = open(filepath,O_RDONLY);
        if(f == -1)
        {
            perror("Eroare la deschiderea fisierului!\n");
            return;
        }

        char temp_filepath[LEN];
        snprintf(temp_filepath, sizeof(temp_filepath), "%s/temp.bin", hunt_id);

        int temp_f = open(temp_filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if(temp_f == -1)
        {
            perror("Eroare la deschiderea fisierului temporar!\n");
            close(temp_f);
            return;
        }

        Treasure_t treasure;
        int flag=0; //1 inseamna ca am gasit si am sters comoara

        while(read(f,&treasure,sizeof(Treasure_t)) == sizeof(Treasure_t))
        {
            if(treasure.id == treasure_id)
            {
                flag=1;
                continue; // nu adaugam comoara in fisierul temporar
            }
            if(write(temp_f,&treasure,sizeof(Treasure_t)) != sizeof(Treasure_t))
            {
                perror("Eroare la scrierea in fisier!\n");
                return;

            }
        }

        close(f);
        close(temp_f);

        if(flag == 1)
        {
            if(remove(filepath) == -1)
            {
                perror("Eroare la stergerea fisierului .bin\n");
                return;
            } 
            if(rename(temp_filepath,filepath) == -1)
            {
                perror("Eroare la redenumirea fisierului temporar\n");
                return;
            }

            //am sters comoara 
            printf("Comoara cu ID ul %d a fost stearsa!\n", treasure_id);
            char action[256];
            snprintf(action,sizeof(action), "S-a sters comoara cu ID-ul %d!", treasure_id);
            log_action(hunt_id,action);
            create_symlink(hunt_id);
        }
        else
        {
            //stergem fisierul temporar pentru ca este in plus

            if(remove(temp_filepath) == -1)
            {
                perror("Eroare la stergerea fisierului temporar!\n");
                return;
            }
            printf("Nu exista comoara cu ID-ul %d pentru a fi stearsa!\n", treasure_id);
            char action[256];
            snprintf(action,sizeof(action), "S-a incercat stergerea comoarii cu ID-ul %d, dar aceasta nu exista!", treasure_id);
            log_action(hunt_id,action);
            create_symlink(hunt_id);
        }
    }
    else
    {
        printf("Raspuns gresit! Incercati yes/no!\n");
    }


}

void remove_hunt(char *hunt_id)
{
    char answer[10];
    printf("Sigur doriti sa stergeti acest hunt? yes/no\n");
    if( scanf("%s", answer) != 1)
    {
        perror("Eroare la citirea raspunsului!\n");
        return;
    }

    if(strcmp(answer,"no") == 0)
    {
        printf("Hunt-ul nu va fi sters!\n");
        return ;
    }
    else if(strcmp(answer,"yes") == 0)
    {
        DIR *director = opendir(hunt_id);
        if(director == NULL)
        {
            perror("Eroare la deschiderea directorului\n");
            return;
        }

        struct dirent *files;
        char filepath[LEN];

        while( (files = readdir(director)) != NULL )
        {
            if(strcmp(files->d_name, ".") == 0 || strcmp(files->d_name, "..") == 0)
                continue;

            snprintf(filepath,sizeof(filepath), "%s/%s", hunt_id, files->d_name);

            if(remove(filepath) == -1)
            {
                perror("Eroare la stergerea fisierului din hunt!\n");
                closedir(director);
                return;
            }
        }

        closedir(director);

        if(rmdir(hunt_id) == -1)
        {
            perror("Eroare la stergerea directorului hunt!\n");
            return;
        }

        char symlink_filepath[LEN];
        snprintf(symlink_filepath,sizeof(symlink_filepath), "logged_hunt-%s.log",hunt_id);

        if(unlink(symlink_filepath) == -1)
        {
            perror("Eroare la stergerea link-ului simbolic!\n");
            return ;
        }

        printf("Hunt-ul %s a fost sters complet!\n", hunt_id);
    }
    else
    {
        printf("Raspuns gresit! Incercati yes/no!\n");
    }
}