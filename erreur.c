#include <stdio.h>
#include <stdlib.h>
#include "erreur.h"

extern int nblignes;

void erreur(int type_err, char * s)
{
  switch (type_err){
  case ERR_LEX:
    fprintf(stderr, "ligne %d : charactere '%s' illegal.\n", nblignes, s);
    exit(type_err);
  case ERR_SYNT:
    fprintf(stderr, "erreur : ligne %d : %s attendu.\n", nblignes, s);
    exit(type_err);
  case ERR_SEM:
    fprintf(stderr, "erreur : ligne %d : %s.\n", nblignes, s);
    exit(type_err);
  case ERR_OUT_MEM:
    fprintf(stderr, "%s : debordement de memoire.\n", s);
    exit(type_err);
  case ERR_EXEC:
    fprintf(stderr, "erreur d'execution : %s.\narret de la machine.\n", s);
    exit(type_err);
  }
}

