/* servTCPIt.c - Exemplu de server TCP iterativ
   Asteapta un nume de la clienti; intoarce clientului sirul
   "Hello nume".

   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#define PORT 4531
#define size 100
#define N 1
#define T 10

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int CNT;

void hndl_child() {
	while(waitpid(-1, NULL, WNOHANG) > 0)
	    CNT--;
}

int main () {
    fclose(fopen("feluri.txt", "w"));
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    int msg;		//mesajul primit de la client
    char msgrasp[100];        //mesaj de raspuns pentru client
    int sd;			//descriptorul de socket
    time_t inceput;
    time(&inceput);

  /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM , 0)) == -1) {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

  /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1) {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 5) == -1) {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }

  /* servim in mod iterativ clientii... */
    while (1) {
        int client;
        int length = sizeof (from);


        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        client = accept (sd, (struct sockaddr *) &from, &length);

        /* eroare la acceptarea conexiunii de la un client */
        if (client < 0) {
            perror ("[server]Eroare la accept().\n");
            continue;
        }
        else CNT++;

        int pid = fork();

        if(pid < 0) {
            perror("eroare fork");
            continue;
        }
        if(pid == 0) {// copilul satisface clientul
            if(CNT > N){
                int full=-1;
                write(client, &full, size);
                close(client);
                exit(0);
            }

                /* s-a realizat conexiunea, se astepta mesajul */
                printf("avem un client\n"); fflush(stdout);

                int incercari = 0;
                while(incercari < 3) {
                    printf("incercarea %d\n",incercari);fflush(stdout);
                    time_t actual;
                    time(&actual);

                    int timp = T - ((int)round(actual-inceput) % T);

                    printf("trimitem timpul %d\n",timp); fflush(stdout);
                    write(client,&timp,size);

                    /* citirea mesajului */
                    printf("asteptam felul\n");fflush(stdout);
                    if (read(client, &msg, 100) <= 0) {
                        perror("[server]Eroare la read() de la client.\n");
                        close(client);    /* inchidem conexiunea cu clientul */
                        continue;        /* continuam sa ascultam */
                    }
                    printf("am primit felul %d\n",msg);fflush(stdout);
                    FILE *f = fopen("feluri.txt", "a");
                    fprintf(f,"%d\n",msg); fflush(f);
                    fclose(f);

                    f = fopen("feluri.txt", "r");
                    fseek(f,0,SEEK_SET);

                    int depuse=0, fel, dorit, dorit_freq;

                    while(depuse < CNT) {
                        int freq[6] = {0};
                        dorit = -1; dorit_freq = -1;

                        while(fscanf(f,"%d",&fel) < 0) {
                            freq[fel]++;
                            if(dorit_freq < freq[fel]) {
                                dorit_freq = freq[fel];
                                dorit = fel;
                            }
                            depuse++;
                        }

                        printf("clientul cu felul %d a numarat %d feluri in fisier\n",fel,depuse); fflush(stdout);
                    }

                    fclose(fopen("feluri.txt", "w"));

                    if(msg == dorit) {
                        printf("perfect\n");fflush(stdout);
                        write(client,"Masa e servita",size);
                        close(client);
                        exit(0);
                    }
                    else {
                        printf("ghinion\n");fflush(stdout);
                        incercari++;
                        write(client,"Indisponibil",size);
                    }
                }
                write(client,"rip",size);
                close(client);
                exit(0);
        }
        else {
            signal(SIGCHLD,hndl_child);
        }
    }				/* while */
}				/* main */
