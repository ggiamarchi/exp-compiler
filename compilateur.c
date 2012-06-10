#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uniteslexicales.h"
#include "grammaire.h"
#include "dictionnaire.h"
#include "erreur.h"
#include "opcodes.h"
#include "mem-machine.h"   /* definition de MAX_MEM ---> plus grande adresse memoire disponible */

#define FAUX          (0)
#define VRAI          (1)
#define MAX_IDENTIF   (1000)  

extern FILE * yyin;                                                           /* dans analyselex.c (analyselex.lex) */
extern char lexeme[];                                                         /* dans analyselex.c (analyselex.lex) */
extern int valeur;                                                            /* dans analyselex.c (analyselex.lex) */
extern int nblignes;                                                          /* dans analyselex.c (analyselex.lex) */
extern int yylex();                                                           /* dans analyselex.c (analyselex.lex) */
extern void erreur(int type_err, char * s);                                   /* dans erreur.c                      */
extern void dumpCode(int *mem, int debut, int fin);                           /* dans dumpcode.c                    */
extern void lancerMachine(int * mem, int tailleCode, int tailleEspGlob);      /* dans machine.c                     */

int uc;
int base = 0;
int sommet = 0;
int contexteLocal = FAUX;
int adrGlobCrt = 0;
int adrLocCrt = 0;
int imem = 0;                       /* indice de la premier case case memoire libre */
int nbArgs = 0;                     /* nombre d'argument de la fonction en cours d'analyse */
int retour = FAUX;                  /* prend la valeur VRAI si la fontion en cours d'analyse renvoie une expression, FAUX sinon */
int fctPrincipaleEnCours = FAUX;    /* indique si le nom de la fonction en cours d'analyse s'appelle 'principale' */
int fctPrincipaleExiste = FAUX;
char choix[200];

identificateur * dico[MAX_IDENTIF];

int mem[MAX_MEM+1];


/******************* fonctions necessaires a la creation du dico et a l'analyse semantique *******************/
void afficherDico(void){
  int i;
  printf("\n\t    ************** dictionnaire ***************\n");
  printf("\ncontexte inferieur :\n\tid\t\tclasse\ttype\tadresse\tcomplement\n");
  printf("\t---------------------------------------------------\n");
  for( i=0 ; i<base ; i++)
    printf("\t%s\t\t%d\t%d\t%d\t%d\n", dico[i]->nom, dico[i]->classe, dico[i]->type, dico[i]->adresse, dico[i]->complement);
  printf("\t---------------------------------------------------\n");
  printf("\ncontexte superieur :\n\tid\t\tclasse\ttype\tadresse\tcomplement\n");
  printf("\t---------------------------------------------------\n");
  for( i=base ; i<sommet ; i++)
    printf("\t%s\t\t%d\t%d\t%d\t%d\n", dico[i]->nom, dico[i]->classe, dico[i]->type, dico[i]->adresse, dico[i]->complement);
  printf("\t---------------------------------------------------\n");
  printf("\n\t    *********** fin du dictionnaire ***********\n");
}

int rechercher(char *id, int haut, int bas){
  if (haut == bas)
    return -1;
  haut--;
  while (haut >= bas){
    if (strcmp(id,dico[haut]->nom) == 0)
      return haut;
    haut--;
  }
  return -1;
}

int ajouter(char *id){
  if (sommet > MAX_IDENTIF){
    fprintf(stderr, "erreur : debordement du dictionnaire des identificateurs.\n");
    exit(1);
  }
  dico[sommet] = malloc(sizeof(identificateur));
  if (dico[sommet] == NULL){
    fprintf(stderr, "erreur d'allocation memoire.\n");
    exit(1);
  }
  strcpy(dico[sommet]->nom, id);
  dico[sommet]->classe = 0;
  dico[sommet]->type = 0;
  dico[sommet]->adresse = 0;
  dico[sommet]->complement = 0;
  return sommet++;
}


/**************************************************************************************************************/

void ajouterCode(int instr){
  if (imem > MAX_MEM)
    erreur(ERR_OUT_MEM, "compilation");
  mem[imem++] = instr;
}

/**************************************************************************************************************/

void programme(void) {
  int i, id;

  if (uc != PROGRAMME)
    erreur(ERR_SYNT, "'programme'");

  uc = yylex();

  if (uc != IDENTIF)
    erreur(ERR_SYNT, "identificateur");

  uc = yylex();

  ajouterCode(PILE);
  ajouterCode(1);
  ajouterCode(APPELF);
  i = imem++;
  ajouterCode(STOP);

  while (uc == ENTIER)
    declarationVariable();
  while (uc == TABLEAU)
    declarationTableau();
  while (uc == FONCTION)
    declarationFonction();

  if (!fctPrincipaleExiste)
    erreur(ERR_SEM, "fonction 'principale' absente");

  id = rechercher("principale", sommet, base);

  if (dico[id]->type != T_ENTIER)
    erreur(ERR_SEM, "la fonction 'principale' doit renvoyer un code de sortie");

  mem[i] = dico[id]->adresse;

  if (uc != FINPROG){
    erreur(ERR_SYNT, "declaration ou 'finprog'");
  }

  uc = yylex();

  /**** destruction du dico global  ****/
  for( i = sommet-1 ; i >= base ; i--)
    free(dico[i]);
  /*************************************/
}


void declarationVariable(void){
  int id;
  if (uc != ENTIER)
    erreur(ERR_SYNT, "'entier'");

  uc = yylex();

  if (uc != IDENTIF)
    erreur(ERR_SYNT, "identificateur");

  id = rechercher(lexeme, sommet, base);

  if (id >= 0)
    erreur(ERR_SEM, "identificateur deja declare");

  id = ajouter(lexeme);
  dico[id]->classe = contexteLocal ? C_VAR_LOCALE : C_VAR_GLOBALE;

  if (contexteLocal){
    dico[id]->classe = C_VAR_LOCALE;
    dico[id]->adresse = adrLocCrt++;
  }
  else{
    dico[id]->classe = C_VAR_GLOBALE;
    dico[id]->adresse = adrGlobCrt++;
  }

  dico[id]->type = T_ENTIER;
  uc = yylex();

  if (uc != POINTVIRGULE)
    erreur(ERR_SYNT, ";");

  uc = yylex();
}


void declarationTableau(void){
  int id;

  if (uc != TABLEAU )
    erreur(ERR_SYNT, "'tableau'");

  uc = yylex();
  if (uc != IDENTIF )
    erreur(ERR_SYNT, "identificateur");

  id = rechercher(lexeme, sommet, base);

  if (id >= 0)
    erreur(ERR_SEM, "identificateur deja declare");

  id = ajouter(lexeme);
  dico[id]->type = T_TABLEAU;
  uc = yylex();

  if (uc != CROCH_OUVR )
     erreur(ERR_SYNT, "'['");

  uc = yylex();

  if (uc != NOMBRE )
     erreur(ERR_SYNT, "nombre entier");

  if (valeur == 0)
    erreur(ERR_SEM, "un tableau doit avoir au minimum une case");

  dico[id]->complement = valeur;

  if (contexteLocal)
    dico[id]->classe = C_VAR_LOCALE;
  else{
    dico[id]->classe = C_VAR_GLOBALE;
    dico[id]->adresse = adrGlobCrt;
    adrGlobCrt += valeur;
  }

  uc = yylex();

  if (uc != CROCH_FERM )
     erreur(ERR_SYNT, "']'");

  uc = yylex();

  if (uc != POINTVIRGULE )
     erreur(ERR_SYNT, "';'");

  uc = yylex();
}


void declarationFonction(void){
  int id, id2, i, nbVarLoc = 0;

  nbArgs = 0;

  if (uc != FONCTION)
     erreur(ERR_SYNT, "'fonction'");

  uc = yylex();

  if (uc != IDENTIF)
     erreur(ERR_SYNT, "identificateur");

  if (strcmp(lexeme, "principale") == 0){
    fctPrincipaleEnCours = VRAI;
    fctPrincipaleExiste = VRAI;
  }

  id = rechercher(lexeme, sommet, base);

  if (id >= 0)
    erreur(ERR_SEM, "identificateur deja declare");

  id = ajouter(lexeme);
  dico[id]->classe = C_FONCTION;
  dico[id]->adresse = imem;
  ajouterCode(ENTREE);
  uc = yylex();

  if (uc != PAREN_OUVR)
     erreur(ERR_SYNT, "'('");

  uc = yylex();

  /******** creation du dico local *********/
  contexteLocal = VRAI;
  base = sommet;
  /*****************************************/

  while (uc == IDENTIF){
    id2 = rechercher(lexeme, sommet, base);

    if (id2 >= 0)
      erreur(ERR_SEM, "identificateur deja declare");

    id2 = ajouter(lexeme);
    dico[id2]->classe = C_ARG;
    dico[id2]->type = T_ENTIER;
    dico[id]->complement++;
    uc = yylex();

    if (uc == PAREN_FERM)
      break;
    if (uc != VIRGULE)
      erreur(ERR_SYNT, "','");
    uc = yylex();
  }
  
  if (uc != PAREN_FERM)
    erreur(ERR_SYNT, "')'");

  uc = yylex();
  nbArgs = dico[id]->complement;

  while (uc == ENTIER){
    declarationVariable();
    nbVarLoc++;
  }

  if (nbVarLoc > 0){
    ajouterCode(PILE);
    ajouterCode(nbVarLoc);
  }

  /* attribution des adresses des arguments (relatives a BEL) */
  for( i = 0 ; i < dico[id]->complement ; i++ )
    dico[base+i]->adresse = i - dico[id]->complement - 2;

  instructionBloc();
  dico[id]->type = retour ? T_ENTIER : 0;

  /**** destruction du dico local  ***/
  for( i = sommet-1 ; i >= base ; i--)
    free(dico[i]);
  contexteLocal = FAUX;
  sommet = base;
  base = 0;
  adrLocCrt = 0;
  /***********************************/

  if (dico[id]->type == T_ENTIER){
    ajouterCode(DEPL);
    ajouterCode(- dico[id]->complement - 3);
  }
  if (mem[imem-1] != RETOUR){
    ajouterCode(SORTIE);
    ajouterCode(RETOUR);
  }

  retour = FAUX;
  fctPrincipaleEnCours = FAUX;
}


void instruction(void){
  switch(uc){
  case IDENTIF:      instructionAffect();   break;
  case ACCOL_OUVR:   instructionBloc();     break;
  case SI:           instructionSi();       break;
  case TANTQUE:      instructionTantque();  break;
  case APPEL:        instructionAppel();    break;
  case RENVOYER:     instructionRenvoyer(); break;
  case ECRIRE:       instructionEcrire();   break;
  case POINTVIRGULE: instructionVide();     break;
  default:  erreur(ERR_SYNT, "instruction");
  }
}


void instructionAffect(void){
  int id;
  if (uc != IDENTIF)
     erreur(ERR_SYNT, "identificateur");

  uc = yylex();

  id = rechercher(lexeme, sommet, 0);

  if (id<0)
    erreur(ERR_SEM, "identificateur non declare.");

  if (uc == CROCH_OUVR){
    uc = yylex();
    expression();
    if (uc != CROCH_FERM)
       erreur(ERR_SYNT, "']'");
    if (dico[id]->type != T_TABLEAU)
      erreur(ERR_SEM, "membre gauche d'affectation, type incompatible");
    uc = yylex();
  }
  else
    if (dico[id]->type != T_ENTIER)
      erreur(ERR_SEM, "membre gauche d'affectation, type incompatible");

  if (uc != EGAL)
     erreur(ERR_SYNT, "'='");

  uc = yylex();
  expression();

  if (uc != POINTVIRGULE)
     erreur(ERR_SYNT, "';'");

  if (dico[id]->type == T_ENTIER){
    if (dico[id]->classe == C_VAR_GLOBALE)
      ajouterCode(DEPG);
    else
      ajouterCode(DEPL);
    ajouterCode(dico[id]->adresse);
  }
  else{
    ajouterCode(DEPT);
    ajouterCode(dico[id]->adresse);
  }

  uc = yylex();
}


void instructionBloc(void){
  if (uc != ACCOL_OUVR){
     erreur(ERR_SYNT, "'{'");
  }
  uc = yylex();
  while (uc != ACCOL_FERM)
    instruction();
  uc = yylex();
}


void instructionSi(void){
  int tmp1, tmp2;

  if (uc != SI)
     erreur(ERR_SYNT, "'si'");

  uc = yylex();
  expression();

  if (uc != ALORS)
     erreur(ERR_SYNT, "'alors'");

  uc = yylex();
  ajouterCode(SIFAUX);
  tmp1 = imem++;
  instruction();
  ajouterCode(SAUT);
  tmp2 = imem++;
  mem[tmp1] = imem;

  if (uc == SINON){
    uc = yylex();
    instruction();
  }

  mem[tmp2] = imem;
}


void instructionTantque(void){
  int tmp1, tmp2;

  if (uc != TANTQUE)
     erreur(ERR_SYNT, "'tantque'");

  uc = yylex();
  tmp1 = imem;
  expression();
  ajouterCode(SIFAUX);
  tmp2 = imem++;
  instruction();
  ajouterCode(SAUT);
  ajouterCode(tmp1);
  mem[tmp2] = imem;  
}


void instructionAppel(void){
  int id;

  if (uc != APPEL)
     erreur(ERR_SYNT, "'appel'");

  uc = yylex();

  if (uc != IDENTIF)
     erreur(ERR_SYNT, "identificateur");
  
  id = rechercher(lexeme, sommet, 0);

  if (id < 0)
    erreur(ERR_SEM, "identificateur non declare");

  if (dico[id]->classe != C_FONCTION)
    erreur(ERR_SEM, "appel doit etre suivi d'un identificateur de fonction");
  
  uc = yylex();

  if (uc != PAREN_OUVR)
     erreur(ERR_SYNT, "'('");

  uc = yylex();
  finAppelFonction(id);

  if (uc != PAREN_FERM)
     erreur(ERR_SYNT, "')'");

  uc = yylex();

  if (uc != POINTVIRGULE)
     erreur(ERR_SYNT, "';'");

  uc = yylex();
  ajouterCode(APPELF);
  ajouterCode(dico[id]->adresse);

  if (dico[id]->complement > 0){
    ajouterCode(PILE);
    ajouterCode(-dico[id]->complement);
  }
}


void finAppelFonction(int id){
  int nbArgs = 0;

  if (uc != PAREN_FERM){
    expression();
    nbArgs++;
    while (uc == VIRGULE){
      uc = yylex();
      expression();
      nbArgs++;
    }
  }

  if (nbArgs > dico[id]->complement)
    erreur(ERR_SEM, "trop d'arguments dans la fonction");

  if (nbArgs < dico[id]->complement)
    erreur(ERR_SEM, "trop peu d'arguments dans la fonction");
}


void instructionRenvoyer(void){
  if (uc != RENVOYER)
     erreur(ERR_SYNT, "'renvoyer'");

  uc = yylex();
  expression();

  if (fctPrincipaleEnCours){
    ajouterCode(DEPG);
    ajouterCode(-1);
  }
  else{
    ajouterCode(DEPL);
    ajouterCode(-nbArgs-3);
  }

  if (uc != POINTVIRGULE)
     erreur(ERR_SYNT, "';'");

  ajouterCode(SORTIE);
  ajouterCode(RETOUR);

  retour = VRAI;
  uc = yylex();
}


void instructionEcrire(void){
  if (uc != ECRIRE)
     erreur(ERR_SYNT, "'ecrire'");

  uc = yylex();

  if (uc != PAREN_OUVR)
     erreur(ERR_SYNT, "'('");

  uc = yylex();
  expression();

  if (uc != PAREN_FERM)
     erreur(ERR_SYNT, "')'");

  uc = yylex();

  if (uc != POINTVIRGULE)
     erreur(ERR_SYNT, "';'");

  uc = yylex();
  ajouterCode(ECRIV);
}


void instructionVide(void){
  if (uc != POINTVIRGULE)
     erreur(ERR_SYNT, "';'");
  uc = yylex();
}


void expression(void)
{
  int memAtt[100],imemAtt=0,multOp=0;
  
  conjonction();
  
  if(uc==OU){
    ajouterCode(SIVRAI);
    memAtt[imemAtt++]= imem++;
    multOp=1;
  }
  
  while(uc==OU){
    uc=yylex();
    conjonction();
    ajouterCode(SIVRAI);
    memAtt[imemAtt++]= imem++;
  }
  
  if(multOp){
    ajouterCode(EMPC);
    ajouterCode(0);
    ajouterCode(SAUT);
    mem[imem] = imem + 3;
    imem++;
    for(imemAtt--;imemAtt>=0;imemAtt--)
      mem[memAtt[imemAtt]] = imem; 
    ajouterCode(EMPC);
    ajouterCode(1);
  }
}


void conjonction(void)
{
  int memAtt[100],imemAtt=0,multoOp=0;
  
  comparaison();
  
  if(uc==ET){
    ajouterCode(SIFAUX);
    memAtt[imemAtt++]= imem++;
    multoOp=1;
  }
  
  while(uc==ET){
    uc=yylex();
    comparaison();
    ajouterCode(SIFAUX);
    memAtt[imemAtt++]= imem++;
  }
  
  if(multoOp){
    ajouterCode(EMPC);
    ajouterCode(1);
    ajouterCode(SAUT);
    mem[imem] = imem + 3;
    imem++;
    for(imemAtt--;imemAtt>=0;imemAtt--)
      mem[memAtt[imemAtt]] = imem; 
    ajouterCode(EMPC);
    ajouterCode(0);
  }
}


void comparaison(void){
  int comp;
 
  expArith();
  comp = uc;

  switch(comp){
  case EGALEGAL:
    uc = yylex();
    expArith();
    ajouterCode(EQU);
    break;
  case DIFF:
    uc = yylex();
    expArith();
    ajouterCode(EQU);
    ajouterCode(NEG);
    break;
  case INF:
    uc = yylex();
    expArith();
    ajouterCode(INFE);
    break;
  case INFEGAL:
    uc = yylex();
    expArith();
    ajouterCode(INFEG);
    break;
  case SUP:
    uc = yylex();
    expArith();
    ajouterCode(INFEG);
    ajouterCode(NEG);
    break;
  case SUPEGAL:
    uc = yylex();
    expArith();
    ajouterCode(INFE);
    ajouterCode(NEG);
    break;
  }
}


void expArith(void){
  int signe;
  terme();
  while (uc == PLUS || uc == MOINS){
    signe = uc;
    uc = yylex();
    terme();
    if (signe == PLUS)
      ajouterCode(ADD);
    else
      ajouterCode(SOUS);
  }
}


void terme(void){
  int signe;
  facteur();
  while (uc == FOIS || uc == DIV){
    signe = uc;
    uc = yylex();
    facteur();
    if (signe == FOIS)
      ajouterCode(MULT);
    else
      ajouterCode(DIVI);
  }
}


void facteur(void){
  int id = 0;
  switch (uc){

  case PAREN_OUVR:{
    uc = yylex();
    expression();
    if (uc != PAREN_FERM){
       erreur(ERR_SYNT, "')'");
    }
    uc = yylex();
    break;
  }

  case NOMBRE:{
    uc = yylex();
    ajouterCode(EMPC);
    ajouterCode(valeur);
    break;
  }

  case IDENTIF:{
    id = rechercher(lexeme, sommet, 0);

    if (id < 0)
      erreur(ERR_SEM, "identificateur non declare");

    uc = yylex();

    if (uc == CROCH_OUVR){
      uc = yylex();
      expression();
      if (uc != CROCH_FERM){
	 erreur(ERR_SYNT, "']'");
      }
      uc = yylex();
      if (dico[id]->type != T_TABLEAU)
	erreur(ERR_SEM, "type incompatible");
      ajouterCode(EMPT);
      ajouterCode(dico[id]->adresse);
    }
    else
      if (uc == PAREN_OUVR){
	uc = yylex();
	if (dico[id]->classe != C_FONCTION)
	  erreur(ERR_SEM, "type incompatible");
	ajouterCode(PILE);
	ajouterCode(1);
	finAppelFonction(id);
	ajouterCode(APPELF);
	ajouterCode(dico[id]->adresse);
	if (dico[id]->complement > 0){
	  ajouterCode(PILE);
	  ajouterCode(-dico[id]->complement);
	}
	if (uc != PAREN_FERM){
	   erreur(ERR_SYNT, "')'");
	}
	uc = yylex();
      }
      else{
	if (dico[id]->type != T_ENTIER)
	  erreur(ERR_SEM, "type incompatible");
	if (dico[id]->classe == C_VAR_GLOBALE)
	  ajouterCode(EMPG);
	else
	  ajouterCode(EMPL);
	ajouterCode(dico[id]->adresse);
      }
    break;
  }
    
  case LIRE:{
    uc = yylex();
    if (uc != PAREN_OUVR){
       erreur(ERR_SYNT, "'('");
    }
    uc = yylex();
    if (uc != PAREN_FERM){
       erreur(ERR_SYNT, "')'");
    }
    uc = yylex();
    ajouterCode(LIR);
    break;
  }
    
  default :    
    erreur(ERR_SYNT, "expression");
  }
}


int main(int argc, char **argv) {
  
  if (argc != 2){
    fprintf(stderr, "\n  usage : %s <fichier source>\n\n", argv[0]);
    return EXIT_FAILURE;
  }

  yyin = fopen(argv[1], "r");

  if (yyin == NULL) {
    fprintf(stderr, "impossible ouvrir '%s'\n", argv[1]);
    return EXIT_FAILURE;
  }

  uc = yylex();
  programme();

  if (uc != 0)
    erreur(ERR_SYNT, "fin de fichier");

  strcpy(choix, "");

  while (choix[0] != 'n' && choix[0] != 'N'){
    printf("\n voulez-vous afficher le code machine (o/n)...");
    fflush(stdout);
    scanf("%s", choix);
    if (choix[0] == 'o' || choix[0] == 'O'){
      printf("\n");
      dumpCode(mem, 0, imem);
      break;
    }
  }
  
  strcpy(choix, "");

  while (choix[0] != 'n' && choix[0] != 'N'){
    printf("\n voulez-vous lancer la machine virtuelle (o/n)...");
    fflush(stdout);
    scanf("%s", choix);
    if (choix[0] == 'o' || choix[0] == 'O'){
      printf("\n");
      lancerMachine(mem, imem, adrGlobCrt);
    }
  }

  printf("\n fin.\n\n");
  
  return EXIT_SUCCESS;
}
