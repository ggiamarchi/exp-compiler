#include <stdio.h>
#include "opcodes.h"
#include "erreur.h"
#include "mem-machine.h"   /* definition de MAX_MEM ---> plus grande adresse memoire disponible */


/* renvoie une erreur d'execution en cas de debordement de pile*/
void testSp(int sp){
  if (sp > MAX_MEM) 
    erreur(ERR_EXEC, "debordement de la pile d'execution");
}

/* renvoie une erreur d'execution si co prend une valeur errone */
void testCo(int co, int tailleCode){
  if (co >= tailleCode || co < 0) 
    erreur(ERR_EXEC, "erreur du compteur ordinal");
}


void lancerMachine(int * mem, int tailleCode, int tailleEspGlob){
  int sp, bel, beg, co, i, v;

  /**************** init. de la machine *****************/

  co = 0;
  bel = -1;
  beg = tailleCode + 1;       /* une case reste libre pour stocker le code de retour de la fct principale */
  sp = beg + tailleEspGlob;

  testSp(sp);


  /******** Affichage des informations memoire **********/

  printf("  initialisation de la machine ok.\n");
  printf("  memoire totale disponible = %i bytes\n", MAX_MEM + 1);
  printf("    memoire occupe par le code = %i bytes\n", tailleCode);
  printf("    memoire reserve = %i bytes\n", tailleEspGlob + 1);
  printf("    memoire libre disponible = %i bytes\n\n", MAX_MEM - tailleCode - tailleEspGlob);
  printf("execution du programme...\n\n");



  /************** simulation du processeur **************/

  while (1){
    switch(mem[co++]){
    case EMPC :
      mem[sp++] = mem[co++];
      testSp(sp);
      testCo(co, tailleCode);
      break;
    case EMPL :
      mem[sp++] = mem[mem[co++]+bel];
      testSp(sp);
      testCo(co, tailleCode);
      break;
    case DEPL :
      mem[mem[co++]+bel] = mem[--sp];
      testCo(co, tailleCode);
      break;
    case EMPG :
      mem[sp++] = mem[mem[co++]+beg];;
      testSp(sp);
      testCo(co, tailleCode);
      break;
    case DEPG :
      mem[mem[co++]+beg] = mem[--sp];
      testCo(co, tailleCode);
      break;
    case EMPT :
      i = mem[--sp];
      mem[sp++] = mem[mem[co++]+beg+i];
      testSp(sp);
      testCo(co, tailleCode);
      break;
    case DEPT :
      v = mem[--sp];
      i = mem[--sp];
      mem[mem[co++]+beg+i] = v;
      testCo(co, tailleCode);
      break;
    case ADD :
      i = mem[--sp];
      mem[sp-1] = mem[sp-1] + i;
      break;
    case SOUS :
      i = mem[--sp];
      mem[sp-1] = mem[sp-1] - i;
      break;
    case MULT :
      i = mem[--sp];
      mem[sp-1] = mem[sp-1] * i;
      break;
    case DIVI :
      i = mem[--sp];
      mem[sp-1] = mem[sp-1] / i;
      break;
    case EQU :
      i = mem[--sp];
      if (i == mem[sp-1])
	mem[sp-1] = 1;
      else
	mem[sp-1] = 0;
      break;
    case INFE :
      i = mem[--sp];
      if (mem[sp-1]<i)
	mem[sp-1] = 1;
      else
	mem[sp-1] = 0;
      break;
    case INFEG :
      i = mem[--sp];
      if (mem[sp-1]<=i)
	mem[sp-1] = 1;
      else
	mem[sp-1] = 0;
      break;
    case NEG :
      i = mem[--sp];
      if (i)
	mem[sp++] = 0;
      else
	mem[sp++] = 1;
      testSp(sp);
      break;
    case LIR :
      scanf("%i", & mem[sp++]);
      testSp(sp);
      break;
    case ECRIV :
      printf("%i\n", mem[--sp]);
      break;
    case SAUT :
      co = mem[co];
      testCo(co, tailleCode);
      break;
    case SIVRAI :
      if (mem[--sp])
	co = mem[co];
      else
	co++;
      testCo(co, tailleCode);
      break;
    case SIFAUX :
      if (mem[--sp])
	co++;
      else
	co = mem[co];
      testCo(co, tailleCode);
      break;
    case APPELF :
      mem[sp++] = co+1;
      testSp(sp);
      co = mem[co];
      testCo(co, tailleCode);
      break;
    case RETOUR :
      co = mem[--sp];
      testCo(co, tailleCode);
      break;
    case ENTREE :
      mem[sp++] = bel;
      testSp(sp);
      bel = sp;
      break;
    case SORTIE :
      sp = bel;
      bel = mem[--sp];
      break;
    case PILE :
      sp += mem[co++];
      testSp(sp);
      break;
    case STOP :
      printf("\nprogramme termine avec le code de retour %i\n\n  Arret de la machine.\n\n", mem[beg - 1]);
      return;     
    default:
      erreur(ERR_EXEC, "instruction inconnue");
    }
    testCo(co, tailleCode);
  }
}
