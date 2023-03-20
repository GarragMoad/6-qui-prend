#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

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




struct Carte cartes[104];     // les cartes d'une partie
struct Manche ComencerManche; // les rangées d'une manche
struct Carte triCartes[4];    // Pour trier les 4 dernières cartes dans les rangées
int indiceRangee[4];          // pour se positionner dans les rangées
int NbJoueurTotal;
int TeteParJoueur[10];
int Somme; //Pour additionner les têtes de boeuf 


void RemplirCartes() // fonction qui Remplit des cartes
{
  for (int i = 0; i < 104; i++)
  {
    cartes[i].Numero = i + 1;

    if ((i + 1) == 55) // Numéro 55 est le seul numéro qui est multiple de 11 et se termine par 5
    {
      cartes[i].TetesBoeuf = 7;
    }

    else if ((i + 1) % 10 == 0) // Numéros qui se termine par 0
    {
      cartes[i].TetesBoeuf = 3;
    }

    else if ((i + 1) % 5 == 0 && (i + 1) % 2 != 0) // Numéros qui se termine par 5
    {
      cartes[i].TetesBoeuf = 2;
    }

    else if ((i + 1) % 11 == 0) // Numéros qui se termine par 5
    {
      cartes[i].TetesBoeuf = 5;
    }
    else // Cartes normaux
    {
      cartes[i].TetesBoeuf = 1;
    }
  }
  for (int i = 0; i < 104; i++)
  {
    /* On choisit un entier au pif entre i et 104; */
    int randval = (rand() % (104 - i)) + i;
    /* On permute Carte[i] et Carte[randval] */
    struct Carte tmp = cartes[i];
    cartes[i] = cartes[randval];
    cartes[randval] = tmp;
  }
}

void CreerTube(int N)
{
   
  for (int i = 1; i <= 2*N; i++)
  {
    char Fifo[6];
    strcpy(Fifo,"Fifo");
    char Ajouter=i+'0';
    strncat(Fifo, &Ajouter, 1);
    mkfifo(Fifo,0644);
  }
  
}

void DistribuerCartes( int Nb) // fonction qui distribue 10 cartes au joueur
{
  for (int i = 0; i < Nb; i++)
  {
    char Fifo[6];
    strcpy(Fifo,"Fifo");
    int num=i+1;
    char charEcrire= num+'0';
    strncat(Fifo, &charEcrire, 1);
    //printf("j'envoie les cartes au joueur %d par %s \n",num,Fifo);
    int fd=open(Fifo, O_WRONLY); 
    write(fd,&cartes,sizeof(cartes));
  }
}
void RemplirRangee(int NB)
{
  ComencerManche.Rangee1[0] = cartes[NB*10];
  ComencerManche.Rangee2[0] = cartes[(NB*10)+1];
  ComencerManche.Rangee3[0] = cartes[(NB*10)+2];
  ComencerManche.Rangee4[0] = cartes[(NB*10)+3];

}

void EnvoyerManche(int NB)
{
  
  for (int i = 0; i < NB; i++)
  {
    char Fifo[6];
    strcpy(Fifo,"Fifo");
    int num=i+1;
    char charEcrire= num+'0';
    strncat(Fifo, &charEcrire, 1);
    //printf("j'envoie les rangées au joueur %d par %s \n",num,Fifo);
    int fd=open(Fifo, O_WRONLY); 
    write(fd,&ComencerManche,sizeof(ComencerManche));
  }
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
}

void RecupererCarte(struct Carte *tab,int Nb)
{
 
  char tube[6];
  for (int i = 1; i <= Nb; i++)
  {
    struct Carte c;
    strcpy(tube,"Fifo");
    int a=i+Nb;
    char charEcrire= a+'0';
    strncat(tube, &charEcrire, 1);
    //printf("je récupère la carte du joueur %d par %s \n",i,tube);
    int fd2 = open(tube, O_RDONLY); 
    read(fd2, &c, sizeof(c));
    tab[i-1]=c;
  }
  printf("J'ai récupere : \n");
  for (int i = 0; i < Nb; i++)
  {
    printf("Carte N %d par le joueur %d: \n",tab[i].Numero,tab[i].joueur);
  }
  
  
}

void TrierCroissant(struct Carte *tab, int Taille)
{
  struct Carte c;
  for (int j = 1; j <= Taille; j++) // Trier les cartes choisies par les joueur en ordre croissant
    for (int i = 0; i < Taille - 1; i++)
      if (tab[i].Numero > tab[i + 1].Numero)
      {
        c = tab[i];
        tab[i] = tab[i + 1];
        tab[i + 1] = c;
      }
}
void RegrouperRangee()
{

  for (int i = 0; i < 6; i++)
  {
    if (ComencerManche.Rangee1[i].Numero != 0)
    {
      indiceRangee[0] = i;
      ComencerManche.Rangee1[i].rangee = 1;
    }

    if (ComencerManche.Rangee2[i].Numero != 0)
    {
      indiceRangee[1] = i;
      ComencerManche.Rangee2[i].rangee = 2;
    }
    if (ComencerManche.Rangee3[i].Numero != 0)
    {
      indiceRangee[2] = i;
      ComencerManche.Rangee3[i].rangee = 3;
    }
    if (ComencerManche.Rangee4[i].Numero != 0)
    {
      indiceRangee[3] = i;
      ComencerManche.Rangee4[i].rangee = 4;
    }
  }

  triCartes[0] = ComencerManche.Rangee1[indiceRangee[0]];
  triCartes[1] = ComencerManche.Rangee2[indiceRangee[1]];
  triCartes[2] = ComencerManche.Rangee3[indiceRangee[2]];
  triCartes[3] = ComencerManche.Rangee4[indiceRangee[3]];

  TrierCroissant(triCartes, 4);
}

int informationCarte(struct Carte c)
{
  int infos[3];
  infos[0] = c.rangee;
  infos[1] = indiceRangee[(c.rangee - 1)];
  infos[2] = c.Numero;

  return infos[1];
}
int TetedeBoeuf(struct Carte *tab,int somme)
{

  for (int i = 0; i < 6; i++)
  {
    if(tab[i].TetesBoeuf>0)
        somme=somme+ tab[i].TetesBoeuf;
  }
  return somme;
  
}

void CarteInferieur(struct Carte c ,int rangee)
{
  
  switch (rangee)
  {
    case 1:
        TeteParJoueur[c.joueur-1]=TetedeBoeuf(ComencerManche.Rangee1,TeteParJoueur[c.joueur-1]);
        ComencerManche.Rangee1[0]=c;
        c.rangee=1;
        for (int i = 1; i < 6; i++)
        {
          ComencerManche.Rangee1[i].Numero=0;
          ComencerManche.Rangee1[i].TetesBoeuf=0;
          ComencerManche.Rangee1[i].rangee=1;
        }
        
      break;
       case 2:
        TeteParJoueur[c.joueur-1]=TetedeBoeuf(ComencerManche.Rangee2,TeteParJoueur[c.joueur-1]);
        ComencerManche.Rangee2[0]=c;
        c.rangee=2;
        for (int i = 1; i < 6; i++)
        {
          ComencerManche.Rangee2[i].Numero=0;
          ComencerManche.Rangee2[i].TetesBoeuf=0;
          ComencerManche.Rangee2[i].rangee=2;
        }
        
      break;
       case 3:
        TeteParJoueur[c.joueur-1]=TetedeBoeuf(ComencerManche.Rangee3,TeteParJoueur[c.joueur-1]);
        ComencerManche.Rangee3[0]=c;
        c.rangee=3;
        for (int i = 1; i < 6; i++)
        {
          ComencerManche.Rangee3[i].Numero=0;
          ComencerManche.Rangee3[i].TetesBoeuf=0;
          ComencerManche.Rangee3[i].rangee=3;
        }
        
      break;
       case 4:
        TeteParJoueur[c.joueur-1]=TetedeBoeuf(ComencerManche.Rangee4,TeteParJoueur[c.joueur-1]);
        ComencerManche.Rangee4[0]=c;
        c.rangee=4;
        for (int i = 1; i < 6; i++)
        {
          ComencerManche.Rangee4[i].Numero=0;
          ComencerManche.Rangee4[i].TetesBoeuf=0;
          ComencerManche.Rangee4[i].rangee=4;
        }
        
      break;
    
    default:
    break;
  }
}

void PlacerCarte(struct Carte *tab, int Taille)
{
  int infos;
  TrierCroissant(tab, Taille);
  for (int i = 0; i < Taille; i++)
  {
    RegrouperRangee();

    if(tab[i].Numero<triCartes[0].Numero) // le cas au la carte est plus petite que la petite carte de les 4 rangées
    {
      //printf("on va placer la carte N %d dans la rangée N %d en position 0 \n", tab[i].Numero,triCartes[0].rangee);
      CarteInferieur(tab[i],triCartes[0].rangee);
    }
    

    if(tab[i].Numero>triCartes[3].Numero)
    {
          infos = informationCarte(triCartes[3])+1;
          //printf("on va placer la carte N %d dans la rangée N %d en position %d \n", tab[i].Numero,triCartes[3].rangee,infos);
           switch (triCartes[3].rangee)
         {
              case 1 :
                         if(infos==5)
                        {
                          CarteInferieur(tab[i],1);
                        }
                        else
                        {
                          ComencerManche.Rangee1[infos]=tab[i];
                          tab[i].rangee=1;
                        }
                break;
              case 2 :
                         if(infos==5)
                        {
                          CarteInferieur(tab[i],2);
                        }
                        else
                        {
                          ComencerManche.Rangee2[infos]=tab[i];
                          tab[i].rangee=2;
                        }
                break;
              case 3 :
                         if(infos==5)
                        {
                          CarteInferieur(tab[i],3);
                        }
                        else
                        {
                          ComencerManche.Rangee3[infos]=tab[i];
                          tab[i].rangee=3;
                        }
                break;
              case 4 :
                         if(infos==5)
                        {
                          CarteInferieur(tab[i],4);
                        }
                        else
                        {
                          ComencerManche.Rangee1[infos]=tab[i];
                          tab[i].rangee=1;
                        }
                break;
              
              default:
                break;
         }
          
    }
    for (int j = 0; j < 4; j++)
    {
      if (tab[i].Numero > triCartes[j].Numero && tab[i].Numero < triCartes[j + 1].Numero)
      {
         infos = informationCarte(triCartes[j])+1;
         //printf("on va placer la carte N %d dans la rangée N %d en position %d \n", tab[i].Numero,triCartes[j].rangee,infos);
         switch (triCartes[j].rangee)
         {
              case 1 :
                        if(infos==5)
                        {
                          CarteInferieur(tab[i],triCartes[j].rangee);
                        }
                        else
                        {
                          ComencerManche.Rangee1[infos]=tab[i];
                          tab[i].rangee=1;
                        }
                        
                break;
              case 2 :
                        if(infos==5)
                        {
                          CarteInferieur(tab[i],2);
                        }
                        else
                        {
                          ComencerManche.Rangee2[infos]=tab[i];
                          tab[i].rangee=2;
                        }
                break;
              case 3 :
                       if(infos==5)
                        {
                          CarteInferieur(tab[i],3);
                        }
                        else
                        {
                          ComencerManche.Rangee3[infos]=tab[i];
                          tab[i].rangee=3;
                        }
                break;
              case 4 :
                        if(infos==5)
                        {
                          CarteInferieur(tab[i],4);
                        }
                        else
                        {
                          ComencerManche.Rangee4[infos]=tab[i];
                          tab[i].rangee=4;
                        }
                break;
              
              default:
                break;
         }
      }
    }
  }
}


int main(int argc, char const *argv[])
{

    NbJoueurTotal=atoi(argv[1]);
    struct Carte CarteJoueurs[NbJoueurTotal];
    CreerTube(NbJoueurTotal);
    

    RemplirCartes();
    DistribuerCartes(NbJoueurTotal);

    RemplirRangee(NbJoueurTotal);
  
while (1)
{
    EnvoyerManche(NbJoueurTotal);

    RecupererCarte(CarteJoueurs,NbJoueurTotal);
    PlacerCarte(CarteJoueurs,NbJoueurTotal);

    AfficheRangee();
    

    for (int i = 0; i < NbJoueurTotal; i++)
    {
      printf(" \nle Joueur %d a %d tete de boeuf \n",(i+1),TeteParJoueur[i]);
    }
    
}

   
    

  return 0;
}
