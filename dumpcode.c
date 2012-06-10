#include <stdio.h>
#include "opcodes.h"

static struct {
    int opCode;
    int avecOperande;
    char *mnemonique;
} codes[] = {
    { EMPC,    1,  "EMPC    " },
    { EMPL,    1,  "EMPL    " },
    { DEPL,    1,  "DEPL    " },
    { EMPG,    1,  "EMPG    " },
    { DEPG,    1,  "DEPG    " },
    { EMPT,    1,  "EMPT    " },
    { DEPT,    1,  "DEPT    " },
    { ADD,     0,  "ADD     " },
    { SOUS,    0,  "SOUS    " },
    { MULT,    0,  "MULT    " },
    { DIVI,    0,  "DIVI    " },
    { MOD,     0,  "MOD     " },
    { EQU,     0,  "EQU     " },
    { INFE,    0,  "INFE    " },
    { INFEG,   0,  "INFEG   " },
    { NEG,     0,  "NEG     " },
    { LIR,     0,  "LIR     " },
    { ECRIV,   0,  "ECRIV   " },
    { SAUT,    1,  "SAUT    " },
    { SIVRAI,  1,  "SIVRAI  " },
    { SIFAUX,  1,  "SIFAUX  " },
    { APPELF,  1,  "APPELF  " },
    { RETOUR,  0,  "RETOUR  " },
    { ENTREE,  0,  "ENTREE  " },
    { SORTIE,  0,  "SORTIE  " },
    { PILE,    1,  "PILE    " },
    { STOP,    0,  "STOP    " },
    { 0,       0,  "INCONNU " } };

#define NCODES (sizeof codes / sizeof codes[0] - 1) 

/* dumpCode(mem, debut, fin) affiche le code qui occupe 
   les mémoires mem[debut], mem[debut + 1] ... mem[fin - 1]
   Le texte affiché est également mis dans un fichier nommé 
   dumpCode.log */

void dumpCode(int *mem, int debut, int fin) {
    int i, j, k;
    char tmp[40];
    FILE *fichier = fopen("dumpCode.log", "w");
    
    i = debut;
    while (i < fin) {
        k = mem[i];
        for (j = 0; j < NCODES; j++)
            if (codes[j].opCode == k)
                break;
                
        if (codes[j].avecOperande) {
            sprintf(tmp, "%4d  %s %d\n", i, codes[j].mnemonique, mem[i + 1]);
            i += 2;
        }
        else { 
            sprintf(tmp, "%4d  %s\n", i, codes[j].mnemonique);
            i += 1;
        }
        printf(tmp);
        if (fichier != NULL) { 
            fprintf(fichier, tmp);
        }
    }
    if (fichier != NULL)
        fclose(fichier);   
}

