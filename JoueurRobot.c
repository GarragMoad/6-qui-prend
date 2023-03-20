#include <stdio.h>
#include<sys/fcntl.h>
#include <unistd.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


struct Carte // Structure qui définit une cartes
{
  int Numero;
  int TetesBoeuf;
  int rangee;
  int joueur;
};
struct Manche // structure qui définit les rangées d'une manche
{
  struct Carte Rangee1[6];
  struct Carte Rangee2[6];
  struct Carte Rangee3[6];
  struct Carte Rangee4[6];
};

int NbJoueur;
int NbTotal;
struct Carte cartesTotal[104]; 
struct Carte cartes[10]; 
struct Manche ComencerManche;
char* Msg;

void RecevCarte( char * tube)
{
    printf(" je reçois les cartes par %s \n",tube);
    int fd=open(tube, O_RDONLY);
    read(fd,&cartesTotal,sizeof(cartesTotal));
}

void CarteJoueur(int N)
{
    if(N==1)
    {
        for (int i = 0; i < 10; i++)
        {
            cartes[i]=cartesTotal[i];
            cartes[i].joueur=1;
        }
        
    }
    else
    {
        int j=(N-1)*10;
        for (int i= j; i < j+10; i++)
        {
            cartes[i-j]=cartesTotal[i];
            cartes[i-j].joueur=N;
        }
        
    }
}
void shuffle(struct Carte arr[], int size){
    srand(time(0));
    for (int i = 0; i < size; i++) {
        int j = rand() % size;
        struct Carte t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}
void ReceveManche(char * tube)
{
    printf("je reçois les rangées %s \n",tube);
    int fd=open(tube, O_RDONLY);
    read(fd,&ComencerManche,sizeof(ComencerManche));
}

void afficheCartes()
{
    for (int i = 0; i < 10; i++)
    {
        if(cartes[i].Numero!=0)
        printf(" Carte N: %d avec %d tête de boeuf \n",cartes[i].Numero,cartes[i].TetesBoeuf);
    }
    printf("\n");
    
}

void AfficheRangee()
{
   for(int i=0;i<6;i++)
   {
    if(ComencerManche.Rangee1[i].Numero!=0)
      printf(" %d %d // ",ComencerManche.Rangee1[i].Numero,ComencerManche.Rangee1[i].TetesBoeuf);
   }
   printf("\n");
   for(int i=0;i<6;i++)
   {
    if(ComencerManche.Rangee2[i].Numero!=0)
      printf(" %d %d // ",ComencerManche.Rangee2[i].Numero,ComencerManche.Rangee2[i].TetesBoeuf);
   }
   printf("\n");
   for(int i=0;i<6;i++)
   {
    if(ComencerManche.Rangee3[i].Numero!=0)
      printf(" %d %d // ",ComencerManche.Rangee3[i].Numero,ComencerManche.Rangee3[i].TetesBoeuf);
   }
   printf("\n");
   for(int i=0;i<6;i++)
   {
    if(ComencerManche.Rangee4[i].Numero!=0)
      printf(" %d %d // ",ComencerManche.Rangee4[i].Numero,ComencerManche.Rangee4[i].TetesBoeuf);
   }
   printf("\n");
}

int SuppCarte( struct Carte* tab,int NbCarte)
 {
   for (int i = 0; i < 10; i++)
   {
    if(tab[i].Numero==NbCarte)
    {
       tab[i].Numero=0;
       tab[i].TetesBoeuf=0;
    }
   }
 }

  void ChoisirCarte(char * tube)
{
    struct Carte c;
    int searchedValue;
    do
    {
     searchedValue = rand() % 10; 
     c=cartes[searchedValue];
    } while (c.Numero==0);
    SuppCarte(cartes,c.Numero);
    //printf("\n Carte Choisie %d \n",c.Numero);
    printf("je vais envoyer la carte par %s \n",tube);
    int fd=open(tube, O_WRONLY);
    write(fd,&c,sizeof(c));
    
}

int main(int argc, char const *argv[])
{
    NbJoueur=atoi(argv[1]);
    NbTotal=atoi(argv[2]);

    char Lire [6];
    char Ecrire[6];

    strcpy(Lire,"Fifo");
    strcpy(Ecrire,"Fifo");

    int a=NbJoueur+NbTotal;
    char charEcrire= a+'0';
    char charLecture=NbJoueur+'0';

    strncat(Lire, &charLecture, 1);
    strncat(Ecrire, &charEcrire, 1);

    printf("%s\n",Lire);

     RecevCarte(Lire);
     printf("***** La partie va commencer ******* \n");
     CarteJoueur(NbJoueur);
     
    int manche=0;
    while(manche<2)
    {
      afficheCartes();
      for (int i = 0; i < 10; i++)
      {
         ReceveManche(Lire);
         AfficheRangee();
        ChoisirCarte(Ecrire);
      }
      manche++;
      shuffle(cartesTotal,104);
      CarteJoueur(NbJoueur);

    }
      ReceveManche(Lire);
      AfficheRangee();

    return 0;
}
