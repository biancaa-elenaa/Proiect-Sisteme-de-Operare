#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <dirent.h>
#include "treasure.h"

#define CMD_FILE "command_file.txt"


int pfd[2]; // pfd[0]-citire pfd[1]-scriere

pid_t monitor_pid = -1;
int monitor_exiting = 0;

void sigchld_handler(int sig)
{
    int status;
    waitpid(monitor_pid, &status, 0);
    monitor_pid = -1;
    monitor_exiting = 0;

    //printf("[Hub] Monitorul a terminat cu statusul : %d\n", WEXITSTATUS(status));

}

int send_command(const char *cmd)
{
    if(monitor_pid <= 0 || monitor_exiting)
    {
        printf("[Hub] Monitorul nu ruleaza sau a fost oprit!\n");
        printf("[Hub] Introduceti comanda start_monitor\n");
        return 0; //nu s-a trimis comanda
    }

    int fd = open(CMD_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd == -1)
    {
        printf("[Hub] Eroare la deschiderea fisierului\n");
        exit(-1);
    }
    int written = write(fd,cmd,strlen(cmd));
    if(written == -1)
    {
        printf("[Hub] Eroare la scrierea in fisier\n");
        close(fd);
        return 0;
    }
    close(fd);

    if(kill(monitor_pid, SIGUSR1) == -1)
    {
        printf("[Hub] Eroare la trimiterea semnalului SIGUSR1 catre monitor\n");
    }
    return 1; // am trimis comanda cu succes
}

void start_monitor()
{
    if(monitor_pid > 0)
    {
        printf("[Hub] Monitorul ruleaza deja.\n");
        return;
    }

    if(pipe(pfd) == -1)
    {
        perror("Eroare creare pipe");
        exit(0);
    }

    monitor_pid = fork();
    if(monitor_pid < 0)
    {
        perror("eroare fork");
        exit(-1);
    }
    if(monitor_pid == 0)
    {
        close(pfd[0]); // inchidem capatul de citire in copil
        dup2(pfd[1],STDOUT_FILENO); //redirectam stdout catre pipe
        close(pfd[1]); //inchidem duplicatul

        execl("./monitor", "monitor", NULL);
        perror("exec"); // daca execl a esuat
        exit(1);
    }

    close(pfd[1]); //inchidem capatul de scriere in parinte
    printf("[Hub] Monitorul a inceput cu PID %d\n", monitor_pid);

}

void read_monitor_output()
{
    
    char buffer[1024];
    int flags = fcntl(pfd[0],F_GETFL,0);
    fcntl(pfd[0],F_SETFL,flags | O_NONBLOCK); // setam pipe-ul in non-blocking
    int total_waits=0;

    printf("[Monitor Output]: \n");

    while (1)
    {
        int readd = read(pfd[0], buffer, sizeof(buffer)-1);
        if (readd > 0)
        {
            buffer[readd] = '\0';
            printf("%s", buffer);
            fflush(stdout); 
            total_waits = 0; // reset dacă am citit ceva
        }
        else
        {
            usleep(10000); // așteaptă puțin
            total_waits++;
            if (total_waits > 10) break; // dacă 1 sec nu vine nimic, ieșim
        }
    }
   
}

void stop_monitor()
{
    if(monitor_pid <= 0)
    {
        printf("[Hub] Monitorul nu ruleaza.\n");
        return ;
    }

    send_command("stop_monitor");
    read_monitor_output();
    monitor_exiting = 1;

    while(monitor_pid > 0 )
    {
        sleep(1);
    }
    printf("[Hub] Monitorul a fost oprit!\n");

}

void calculate_score()
{
    DIR *directory = opendir(".");
    struct dirent *dir;

    if(directory == NULL)
    {
        perror("[Hub] Eroare deschidere director curent!\n");
        return;
    }

    while((dir = readdir(directory)) != NULL)
    {
        if(dir->d_type == DT_DIR && strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,".."))
        {
            char filepath[300];
            snprintf(filepath,sizeof(filepath),"%s/treasures.bin",dir->d_name);

            if(access(filepath,F_OK) == 0) //daca am reusit sa ajungem la treasure.bin inseamna ca ne aflam intr-un hunt
            {
                int pfd1[2];
                if(pipe(pfd1) == -1)
                {
                    perror("Eroare la pipe");
                    exit(-1);
                }
                pid_t pid1;
                pid1 = fork();
                if(pid1 == -1)
                {
                    perror("Eroare la fork");
                    exit(-2);
                }

                if(pid1 == 0)
                {
                    //copil
                    close(pfd1[0]);
                    dup2(pfd1[1],STDOUT_FILENO); // redirectam iesirea standard
                    close(pfd1[1]);

                    execl("./score_calculator","score_calculator",dir->d_name,NULL);
                    perror("Eroare la execl score_calculator");
                    exit(-3);
                } else {
                    // procesul parinte
                    close(pfd1[1]);
                    char buffer[10000];
                    int n;
                    
                    printf("Scoruri pentru Hunt-ul : %s\n",dir->d_name);
                    while((n=read(pfd1[0],buffer,sizeof(buffer)-1)) > 0)
                    {
                        buffer[n] = '\0';
                        printf("%s", buffer);
                    }
                    
                    fflush(stdout);
                    close(pfd1[0]);
                    waitpid(pid1,NULL,0);
                    
                }
            }
        }
    }
    closedir(directory);
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    setvbuf(stdout,NULL,_IONBF,0);
    if(sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("Eroare sigaction\n");
        exit(-1);
    }

    char command[100];

    while(1)
    {
        printf("treasure_hub> ");
        scanf("%s", command);
        command[strcspn(command,"\r\n")] = 0;

        if(strcmp(command, "start_monitor") == 0)
        {
            start_monitor();
        }
        else if(strcmp(command, "list_hunts") == 0)
        {
            if(send_command(command))
            {
                usleep(100000);
                read_monitor_output();   
            }
        }
        else if(strcmp(command, "list_treasures") == 0)
        {
            char hunt_id[30];
            printf("[Hub] Introduceti hunt-ul din care doriti listarea comorilor: ");
            scanf("%s", hunt_id);
            hunt_id[strcspn(hunt_id, "\n")] =0;
            printf("hunt primit '%s': \n",hunt_id);
            char full_command[200];
            sprintf(full_command, "%s %s\n","list_treasures", hunt_id);
            if(send_command(full_command))
            {
                read_monitor_output();
            }
            
        }
        else if(strcmp(command, "view_treasures") == 0)
        {
            char hunt_id[30];
            int treasure_id;
            printf("Introduceti hunt-ul din care doriti vizualizarea comorii: ");
            scanf("%s",hunt_id);
            hunt_id[strcspn(hunt_id, "\n")] =0;
            printf("Introduceti ID-ul comorii: ");
            scanf("%d", &treasure_id);


            char full_command[200];
            sprintf(full_command, "%s %s %d","view_treasures ",hunt_id,treasure_id);
            if( send_command(full_command))
            {
                read_monitor_output();
            }
        }
        else if(strcmp(command, "stop_monitor") == 0)
        {
            stop_monitor();
        }
        else if(strcmp(command, "exit") == 0)
        {
            if(monitor_pid > 0 && !monitor_exiting)
            {
                printf("[Hub] Eroare: Monitorul inca ruleaza.\n");
                printf("[Hub] Utilizati stop_monitor inainte de aceasta comanda.\n");
            }else{
                printf("[Hub] Programul se inchide ...\n");
                break;
            }
        } else if(strcmp(command,"calculate_score") == 0)
        {
            
            if(monitor_pid > 0 && !monitor_exiting)
            {
                printf("[Hub] Eroare! Monitorul este inca pornit!\n");
                printf("[Hub] Opriti monitorul (stop_monitor) inainte de a calcula scorurile!\n");
                continue;
            }
            else{
                calculate_score();

            }
            
            
        }
        else
        {
            printf("[Hub] Comanda gresita!\n");
            printf("[Hub] Incercati: \n");
            printf("[Hub] start_monitor -> Porneste Monitorul\n");
            printf("[Hub] list_hunts   -> Afiseaza comorile dintr-un hunt si numarul total de comori in fiecare\n");
            printf("[Hub] list_treasures  -> Afiseaza detaliile despre toate comorile dintr-un hunt\n");
            printf("[Hub] view_treasures  -> Afiseaza detaliile unei comori\n");
            printf("[Hub] stop_monitor -> Opreste monitorul\n");
            printf("[Hub] exit  -> Daca monitorul inca e pornit, afisaza eroare, altfel termina programul\n");
            printf("[Hub] calculate_score -> Afiseaza scorul fiecarui utilizator din fiecare Hunt\n");
            
        }
    }

    return 0;
}