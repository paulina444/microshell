#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define true 1
#define false 0
#define RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define PURPLE "\033[0;35m"
#define PURPLE2 "\033[1;35m"
#define BLUE "\033[0;34m"
#define BLUE2 "\033[1;36m"
#define BLUE3 "\033[1;34m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

int run();
void help();
int cd(const char*);
int touch(const char*);
int cp(const char*, const char*);
int rm(const char*);
int mv(const char*, const char*);
int head(const char*);
void errors();
void CTRL_C();

pid_t child_pid = -1; 

int main(int argc, char *argv[])
{
    signal(SIGINT, CTRL_C); 
    run(argc, argv);
    return 0;
}

int run(int argc, char *argv[])
{       
    char cwd[128]; 
    char last_cwd[128]; 
    getcwd(last_cwd, sizeof(last_cwd));
    size_t buffer_size=32;   
    char* raw_buffer= (char *) malloc(buffer_size*sizeof(char));
    char* buffer;
    char* buffer_copy = (char *) malloc(buffer_size*sizeof(char));

    while (1)
    {   
        errno=0;
        getcwd(cwd, sizeof(cwd));
        
        char* username=getenv("USER");

        printf(GREEN);
        printf("(MICROSHELL)%s@", username);
        printf(RESET);
        
        printf(BLUE2);
        printf("[%s] $ ", cwd);
        printf(RESET);
        getline(&raw_buffer, &buffer_size, stdin);
        buffer = raw_buffer;

        size_t index = strspn(buffer, " \n");
        if (index != 0)
        {
            if(index == strlen(buffer))
            {
                continue;
            }
            else 
            {
                buffer = raw_buffer + index;
            }
        }
        
        strtok(buffer, "\n"); 
        strcpy(buffer_copy,buffer); 

        //exit
        if (strcmp(buffer,"exit")==0) break;

        //help
        if (strcmp(buffer,"help")==0) 
        {
            help(buffer);
            continue;
        }

        //cd

        if (strcmp(buffer,"cd")==0)
        { 
            if(cd(getenv("HOME")))
                {
                    errno = ENOENT;
                    perror("\033[1;31m cd error"); 
                    continue;  
                }
            strcpy(last_cwd, cwd);
            continue;
        }
        
        if(strcmp(strtok(buffer, " "), "cd") == 0)
        {
            char* path = strchr(buffer_copy, ' ');
            ++path; 

            if(strcmp(path, "~")==0)
            {
                if(cd(getenv("HOME")))
                {
                    errno = ENOENT;
                    perror("\033[1;31m cd error"); 
                    continue;  
                }
                strcpy(last_cwd, cwd); 
                continue;
            }

            if(strcmp(path, "-")==0)
            {
                cd(last_cwd);
                strcpy(last_cwd, cwd);
                continue;
            }

            if(cd(path))
            {
                errno = ENOENT;
                perror("\033[1;31mcd error");
                continue;   
            }
            strcpy(last_cwd, cwd);
            continue;
        }
        else
        {
            strcpy(buffer, buffer_copy);
        }

        //touch 

        if (strcmp(buffer,"touch")==0) 
        {
            errors();
            continue;
        }

        if(strcmp(strtok(buffer, " "), "touch") == 0)  
        {
            char* path = strchr(buffer_copy, ' ');
            ++path; 
            touch(path);
           
            continue;
        }
        else
        {
            strcpy(buffer, buffer_copy); 
        }
        
        //cp

        if (strcmp(buffer,"cp")==0) 
        {
            errors();
            continue;
        }

        if(strcmp(strtok(buffer, " "), "cp") == 0) 
        {
            int ile = 0;
            for (int i = 0; i < strlen(buffer_copy); i++)
            {
                if (buffer_copy[i] == ' ')ile++;
            }
            if(ile != 2)
            {
                errno = EINVAL;
                perror("\033[1;31mnie odpowiednia ilosc argumentow");
                continue;
            }

            char* path = strchr(buffer_copy, ' ');
            ++path; 

            char* src = (char *) malloc(buffer_size*sizeof(char)); 
            strcpy(src,path); 
            strtok(src," "); 

            char* dest = (char *) malloc(buffer_size*sizeof(char));
            strcpy(dest, path);
            char *dest2 = strchr(dest, ' ');
            dest2++;

            cp(src, dest2);
            free(src);
            free(dest);
            continue;
        }
        else
        {
            strcpy(buffer, buffer_copy);
        }

        //rm

        if (strcmp(buffer,"rm")==0) 
        {
            errors();  
            continue;
        }

        if(strcmp(strtok(buffer, " "), "rm") == 0)  
        {
            char* path = strchr(buffer_copy, ' ');
            ++path; 
            if(rm(path))
            {
                errno = ENOENT;
                perror("\033[1;31mrm error");   
            }
           
            continue;
        }
        else
        {
            strcpy(buffer, buffer_copy); 
        }

        //mv

        if (strcmp(buffer,"mv")==0) 
        {
            errors(); 
            continue;
        }

        if(strcmp(strtok(buffer, " "), "mv") == 0) 
        {
             int ile = 0;
            for (int i = 0; i < strlen(buffer_copy); i++)
            {
                if (buffer_copy[i] == ' ')ile++;
            }
            if(ile != 2)
            {
                errno = EINVAL;
                perror("\033[1;31mnie odpowiednia ilosc argumentow");
                continue;
            }


            char* path = strchr(buffer_copy, ' ');
            ++path; 

            char* src = (char *) malloc(buffer_size*sizeof(char)); 
            strcpy(src,path); 
            strtok(src," "); 

            char* dest = (char *) malloc(buffer_size*sizeof(char));
            strcpy(dest, path); 
            char *dest2 = strchr(dest, ' ');
            dest2++;
            printf(PURPLE);
            printf("przenoszenie {%s} => {%s}\n", src,dest2);
            printf(RESET);

            mv(src, dest2);
            free(src);
            free(dest);
            continue;
        }
        else
        {
            strcpy(buffer, buffer_copy);
        }

        //head

        if (strcmp(buffer,"head")==0) 
        {
            errors();
            continue;
        }

        if(strcmp(strtok(buffer, " "), "head") == 0)  
        {
            char* path = strchr(buffer_copy, ' ');
            ++path; 
            head(path);   
            continue;
        }
        else
        {
            strcpy(buffer, buffer_copy); 
        }

        
       //forki 
        {
            int status=0;
            if((child_pid = fork()) == 0)
            {
                char *argument_list[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

                int i=0;
                for(char *word = strtok(buffer, " "); word != NULL ; i++)
                {
                    argument_list[i] = word;
                    word = strtok (NULL, " ");
                }

                exit(execvp(argument_list[0], argument_list));
            }
            else
            {
                waitpid(child_pid, &status, 0); 
                child_pid = -1;
                if (status==65280)
                {   
                    printf(RED);
                    errno=EBADRQC;
                    char msg[256]= "\033[1;31m komenda `";
                    strcat(msg, buffer);
                    strcat(msg, "` nie znaleziona\n ");       
                    perror(msg);    
                    printf("status %d\n", status);
                    printf(RESET);       
                }
                else if(status == 2)
                {
                    errno = EINTR;
                    perror("ERROR:");
                }
            }
        }
        
    }
    free(buffer_copy);
    free(raw_buffer);
    return 0;
}

int cd(const char* path) 
{   
    return chdir(path);
}

void help()
{

    printf(BLUE2"\t\t-------------------MICROSHELL-------------------\t\t\n"RESET);
    printf(BLUE"Autor programu:\n"RESET);
    printf(BLUE3"Paulina Kaczmarek\n"RESET);
    printf(PURPLE2);
    printf("Funkcje programu:\n");
    printf(RESET);
    printf(PURPLE2"\texit "RESET);
    printf(PURPLE"- powoduje zakonczenie programu\n"RESET);
    printf(PURPLE2"\tcd"RESET);
    printf(PURPLE"{katalog_docelowy}- przemieszczanie sie po katalogach\n"RESET);
    printf(PURPLE"\t\t . - biezacy katalog\n");
    printf("\t\t .. - nadrzedna lokalizacja\n");
    printf("\t\t - - przeniesie nas do poprzedniego katalogu\n");
    printf("\t\t ~ - przeniesie nas do katalogu domowego\n"RESET);
    printf(PURPLE2"\tcp "RESET);
    printf(PURPLE"{plik_zrodlowy} {miejsce_docelowe} - kopiuje plik zrodlowy do miejsca docelowego. Plik docelowy jest tworzony, gdy nie istnieje\n"RESET);
    printf(PURPLE2"\ttouch "RESET);
    printf(PURPLE"{plik} - tworzy nowy plik\n"RESET);
    printf(PURPLE2"\trm "RESET);
    printf(PURPLE"{plik} - usuwa plik\n"RESET);
    printf(PURPLE2"\tmv "RESET);
    printf(PURPLE"{plik_zrodlowy} {katalog docelowy} - przenosi plik zrodlowy do katalogu docelowego\n"RESET); 
    printf(PURPLE2"\thead {plik} "RESET);
    printf(PURPLE"- wypisuje pierwsze 10 linijek tekstu\n"RESET); 
    printf(PURPLE2"\tobsluga ctrl+c "RESET);
    printf(PURPLE"- przerwanie programu\n"RESET);
}

int touch(const char* path) 
{
    int plik = open(path, O_RDONLY);
    if(plik != -1)
    {
        errno = EEXIST;
        perror("\033[1;31mten plik juz istnieje\n");
        close(plik);
        return -1;
    }
    else 
    {
        if(creat(path, 0666) == -1)
        {
            errno = EACCES; 
            perror("\033[1;31mbrak uprawnien do stworzenia pliku\n"); 
            return -1;
        }
        return 0;
    }
}

int cp(const char* src, const char* dest)
{
    int plik1, plik2;
    plik1=open(src, O_RDONLY);
    plik2=open(dest, O_RDWR);

    if(plik1==-1)
    {
        errno = ENOENT;
        perror("\033[1;31m plik 1 nie istnieje");  
        if(plik2 != -1) close(plik2);
        return -1;
    }

    if(plik2 == -1) 
    { 
        if(touch(dest) == -1) 
        {
            return -1;
        }
        plik2=open(dest, O_RDWR); 
    }

    int buffer_size = 128;
    char* buffer = (char *) malloc(buffer_size*sizeof(char));

    while(read(plik1, buffer, buffer_size) > 0)
    {
        write(plik2, buffer, buffer_size);
    }

    free(buffer);
    close(plik1);
    close(plik2);

    return 0;
}

int head(const char* file)
{
    int linia=0;
    FILE* plik=fopen(file, "r");
    if(plik==NULL)
    {
        errno = ENOENT;
        perror("\033[1;31m plik nie istnieje");  
        return -1;
    }
    int buffer_size = 128;
    char* buffer = (char *) malloc(buffer_size*sizeof(char));

    while(fgets(buffer, sizeof(buffer),plik)!=NULL)
    {
        if(linia==10) break;
        printf("%s", buffer);
        linia++;
    }
    free(buffer);
    fclose(plik);
    return 0;
}

int rm(const char* path)
{
    if(remove(path)!=0)
    {
        errno = ENOENT;
        perror("\033[1;31m nie mozna usunac pliku");  
    }
    return 0;
}

int mv(const char* src, const char* dest)
{
    cp(src, dest);
    rm(src);
    return 0;
}

void errors()
{
    errno = EINVAL;
    perror("\033[1;31mnie zapomniales jakiegos argumentu?");  
    
}

void CTRL_C()
{
    printf(RED"\nWYKRYTO PRZERWANIE\n"RESET);
    if(child_pid != -1)
    {
        printf(RED);
        printf("ZABIJAM PROCES %d \n",child_pid);
        printf(RESET);
        kill(child_pid, SIGINT);
        child_pid = -1;
    }
    else
    {
        exit(2);
    }
}



 





