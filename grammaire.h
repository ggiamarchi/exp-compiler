
#ifndef __GRAMMAIRE_H_
#define __GRAMMAIRE_H_

void programme(void);
void declarationVariable(void);
void declarationTableau(void);
void declarationFonction(void);
void instruction(void);
void instructionAffect(void);
void instructionBloc(void);
void instructionSi(void);
void instructionTantque(void);
void instructionAppel(void);
void finAppelFonction(int);
void instructionRenvoyer(void);
void instructionEcrire(void);
void instructionLire(void);
void instructionVide(void);
void expression(void);
void conjonction(void);
void comparaison(void);
void expArith(void);
void terme(void);
void facteur(void);

#endif


/*

programme -> PROGRAMME IDENTIF
             { declarationVariable }
             { declarationTableau } 
             { declarationFonction }
	     FINPROG

declarationVariable -> ENTIER IDENTIF POINTVIRGULE

declarationTableau -> TABLEAU IDENTIF CROCH_OUVR NOMBRE CROCH_FERM POINTVIRGULE

declarationFonction -> FONCTION IDENTIF PAREN_OUVR [ IDENTIF { VIRGULE IDENTIF } ] PAREN_FERM 
                       { declarationVariable }
                       instructionBloc  
    
instruction -> instructionAffect 
             | instructionBloc
             | instructionSi
             | instructionTantque
             | instructionAppel
             | instructionRenvoyer
             | instructionEcrire
             | instructionVide
                    
instructionAffect -> IDENTIF [ CROCH_OUVR expression CROCH_FERM ] EGAL expression POINTVIRGULE

instructionBloc -> ACCOL_OUVR { instruction } ACCOL_FERM
    
instructionSi -> SI expression ALORS instruction [ SINON instruction ]
    
instructionTantque -> TANTQUE expression FAIRE instruction

instructionAppel -> APPEL IDENTIF PAREN_OUVR finAppelFonction PAREN_FERM POINTVIRGULE

finAppelFonction -> [ expression { VIRGULE expression } ]
    
instructionRenvoyer -> RENVOYER expression POINTVIRGULE 

instructionEcrire -> ECRIRE PAREN_OUVR expression PAREN_FERM POINTVIRGULE

instructionVide -> POINTVIRGULE
    
expression -> conjonction { OU conjonction }
    
conjonction -> comparaison { ET comparaison }
    
comparaison -> expArith [ (EGALEGAL | DIFF | INF | INFEGAL | SUP | SUPEGAL ) expArith ]
    
expArith -> terme { ( PLUS | MOINS ) terme }
    
terme -> facteur { ( FOIS | DIV ) facteur }
    
facteur -> PAREN_OUVR expression PAREN_FERM
         | NOMBRE
         | IDENTIF [ CROCH_OUVR expression CROCH_FERM | PAREN_OUVR finAppelFonction PAREN_FERM ]
         | LIRE PAREN_OUVR PAREN_FERM

*/
