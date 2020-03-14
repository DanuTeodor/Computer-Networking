/* cliTCPIt.c - Exemplu de client TCP
   Trimite un nume la server; primeste de la server "Hello nume".

   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#define PORT 4532
#define size 100

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[]) {
    srand(time(NULL));

    int sd;			// descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare
    int msg;		// mesajul trimis
    int secunde = 0, incercari = 0;
    char msgrasp[100];
    /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* portul de conectare */
    server.sin_port = htons(PORT);


  /* ne conectam la server */
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1) {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }

      while(incercari < 3) {

          read(sd,&secunde,8);

          printf("incearcarea %d\n",incercari);

          if(secunde == -1) {
              printf("Nu mai sunt locuri la cantina");
              fflush(stdout);
              close(sd);
              return 0;
          }

          time_t inceput;
          time(&inceput);

          int trecute = 0;
          while(trecute < secunde) {
              time_t actual;
              time(&actual);
              trecute = (int)(actual-inceput);
          }
          msg=rand()%5+1;
            printf("clientul cere felul %d\n",msg);fflush(stdout);

            char trimitem[2] = {0};
            trimitem[0]=msg+'0';
            trimitem[1]='\0';
              write(sd,trimitem,100);
              bzero(msgrasp,100);
              read(sd,msgrasp,size);
              printf("%s\n",msgrasp);fflush(stdout);

              if(strcmp(msgrasp, "Masa e servita") == 0){
                  printf("Satul!");
                  fflush(stdout);
                  close(sd);
                  return 0;
              }
              else if(strcmp(msgrasp, "Indisponibil") == 0){
                  incercari++;
                  printf("incercarea %d\n",incercari); fflush(stdout);
              }
              else {
                  printf("Schimb cantina! Aici mor de foame!\n");
                  fflush(stdout);
                  close(sd);
                  return 0;
              }
      }
  /* inchidem conexiunea, am terminat */
  close (sd);
}
