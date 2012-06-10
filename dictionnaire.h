
#ifndef __DICTIONNAIRE_H_
#define __DICTIONNAIRE_H_

#ifndef MAX_TAILLE_IDENTIF
#define MAX_TAILLE_IDENTIF (200)
#endif

#define C_VAR_LOCALE       (401)
#define C_VAR_GLOBALE      (402)
#define C_ARG              (403)
#define C_FONCTION         (404)
#define T_ENTIER           (405)
#define T_TABLEAU          (406)

typedef struct identificateur{
  char nom[MAX_TAILLE_IDENTIF];
  int classe;
  int type;
  int adresse;
  int complement;  /* pour une fonction le nombre d'argumments et pour un tableau le nombre d'elements */
} identificateur;

#endif
