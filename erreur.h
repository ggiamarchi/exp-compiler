
#ifndef __ERREUR_H_
#define __ERREUR_H_

#define ERR_NO       (1000)
#define ERR_LEX      (1010)
#define ERR_SYNT     (1020)
#define ERR_SEM      (1030)
#define ERR_OUT_MEM  (1040)
#define ERR_EXEC     (1050)


void erreur(int type_err, char * s);

#endif

