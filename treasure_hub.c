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

pid_t monitor_pid = -1;
int monitor_exiting = 0;

void sigchld_handler(int sig)
{
    int status;
    waitpid(monitor_pid, &status, 0);
    monitor_pid = -1;
    monitor_exiting = 0;

    printf("Monitorul a terminat cu statusul : %d\n", WEXITSTATUS(status));

}

void send_command(const char *cmd)
{
    if(monitor_pid <= 0 || monitor_exiting)
    {
        printf("Monitorul nu ruleaza sau a fost oprit\n");
        return;
    }

    int fd = open(CMD_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd == -1)
    {
        printf("Eroare la deschiderea fisierului\n");
        exit(-1);
    }
    int written = write(fd,cmd,strlen(cmd));
    if(written == -1)
    {
        printf("Eroare la scrierea in fisier\n");
        close(fd);
        return;
    }
    close(fd);

    if(kill(monitor_pid, SIGUSR1) == -1)
    {
        printf("Eroare la trimiterea semnalului SIGUSR1 catre monitor\n");
    }
}

void start_monitor()
{
    if(monitor_pid > 0)
    {
        printf("Monitorul ruleaza deja.\n");
        return;
    }

    monitor_pid = fork();
    if(monitor_pid < 0)
    {
        perror("eroare pid");
        exit(-1);
    }
    if(monitor_pid == 0)
    {
        execl("./monitor", "monitor", NULL);
        perror("exec"); // daca execl a esuat
        exit(1);
    }

    printf("Monitorul a inceput cu PID %d\n", monitor_pid);

}

void stop_monitor()
{
    if(monitor_pid <= 0)
    {
        printf("Monitorul nu ruleaza.\n");
        return ;
    }

    send_command("stop_monitor");
    monitor_exiting = 1;

    while(monitor_pid > 0 )
    {
        sleep(1);
    }
    printf("Monitorul a fost oprit!\n");

}

int main()
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

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
            send_command(command);    
        }
        else if(strcmp(command, "list_treasures") == 0)
        {
            char hunt_id[30];
            printf("Introduceti hunt-ul din care doriti listarea comorilor: ");
            scanf("%s", hunt_id);
            hunt_id[strcspn(hunt_id, "\n")] =0;

            char full_command[200];
            sprintf(full_command, "%s %s","list_treasures ", hunt_id);
            send_command(full_command);
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
            send_command(full_command);
        }
        else if(strcmp(command, "stop_monitor") == 0)
        {
            stop_monitor();
        }
        else if(strcmp(command, "exit") == 0)
        {
            if(monitor_pid > 0 && !monitor_exiting)
            {
                printf("Eroare: Monitorul inca ruleaza.\n");
                printf("Utilizati stop_monitor inainte de aceasta comanda.\n");
            }else{
                printf("Programul se inchide ...\n");
                break;
            }
        }
        else
        {
            printf("Comanda gresita!\n");
            printf("Incercati: \n");
            printf("start_monitor -> Porneste Monitorul\n");
            printf("list_hunts   -> Afiseaza comorile dintr-un hunt si numarul total de comori in fiecare\n");
            printf("list_treasures  -> Afiseaza detaliile despre toate comorile dintr-un hunt\n");
            printf("view_treasures  -> Afiseaza detaliile unei comori\n");
            printf("stop_monitor -> Opreste monitorul\n");
            printf("exit  -> Daca monitorul inca e pornit, afisaza eroare, altfel termina programul\n");
            
            
        }
    }

    return 0;
}