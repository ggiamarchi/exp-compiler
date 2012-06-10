
%{
#include <string.h>
#include <stdio.h>
#include "uniteslexicales.h"
#include "erreur.h"
#ifndef MAX_TAILLE_IDENTIF
#define MAX_TAILLE_IDENTIF (200)
#endif
char lexeme[MAX_TAILLE_IDENTIF];
int valeur;
int nblignes = 1;
extern void erreur(int type_err, char * s);    /* dans erreur.c */
%}

%%

\/\/.*\n     nblignes++; /* pour gerer des comentaires de ce type */
si           return SI;
alors        return ALORS;
fonction     return FONCTION;
ecrire       return ECRIRE;
lire         return LIRE;
tantque      return TANTQUE;
sinon        return SINON;
renvoyer     return RENVOYER;
appel        return APPEL;
entier       return ENTIER;
tableau      return TABLEAU;
programme    return PROGRAMME;
finprog      return FINPROG;
"+"          return PLUS;
"-"          return MOINS;
"*"          return FOIS;
"/"          return DIV;
"="          return EGAL;
"=="         return EGALEGAL;
"!="         return DIFF;
"<="         return INFEGAL;
">="         return SUPEGAL;
"<"          return INF;
">"          return SUP;
"!"          return NON;
"&&"         return ET;
"||"         return OU;
";"          return POINTVIRGULE;
","          return VIRGULE;
"("          return PAREN_OUVR;
")"          return PAREN_FERM;
"{"          return ACCOL_OUVR;
"}"          return ACCOL_FERM;
"["          return CROCH_OUVR;
"]"          return CROCH_FERM;

[a-zA-Z][a-zA-Z0-9_]* {
  strncpy(lexeme,yytext,200);
  if (strcmp(lexeme,yytext) != 0)
    printf("ATTENTION : ligne %d : identificateur tronqué car trop long.\n",nblignes);
  return IDENTIF;
}

[0-9]+       { valeur = atoi(yytext); return NOMBRE; }
[\ \t]       ;
[\n]         nblignes++;

.            erreur(ERR_LEX, yytext);


%%
