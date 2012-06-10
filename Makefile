#
# Makefile
#


# compilateur et editeur de liens

CC      = gcc
LD      = gcc
CFLAGS  =  -c -g
LDFLAGS = -g -lfl 
LEX     = lex


# fichiers

GRAMMAIRE = grammaire.h
UNITESLEX = uniteslexicales.h
DICO      = dictionnaire.h
OPCODES   = opcodes.h
ERREUR    = erreur.h
MEM       = mem-machine.h

LEX_LEXIC = analyselex.lex
SRC_COMP  = compilateur.c
SRC_ERR   = erreur.c
SRC_DUMP  = dumpcode.c
SRC_MACH  = machine.c
EXEC      = compilateur

SRC_LEXIC = analyselex.c
OBJ_LEXIC = analyselex.o
OBJ_COMP  = compilateur.o
OBJ_ERR   = erreur.o
OBJ_DUMP  = dumpcode.o
OBJ_MACH  = machine.o

# (re)compiler le projet 

all : $(EXEC)
	@echo
	@echo "*** compilation ok ! ***"
	@echo "*** 'make help' pour la liste des options ***"
	@echo


# creation du fichier objet pour le traitement des erreurs

$(OBJ_ERR) : $(SRC_ERR)
	@$(CC) $(CFLAGS) $(SRC_ERR)
	@echo "--- compilation de du ficher d'erreurs ------> $(OBJ_ERR) ...............ok"


# creation du fichier objet pour l'analyseur synthaxique semantique et compilateur

$(OBJ_COMP) : $(SRC_COMP) $(GRAMMAIRE) $(UNITESLEX) $(MEM) $(SRC_ERR) $(DICO) $(SRC_DUMP)
	@$(CC) $(CFLAGS) $(SRC_COMP)
	@echo "--- compilation du compilateur --------------> $(OBJ_COMP) ..........ok"


# creation du fichier source C pour l'anlyseur lexical

$(SRC_LEXIC) : $(LEX_LEXIC) $(UNITESLEX)
	@$(LEX) -o$(SRC_LEXIC) $(LEX_LEXIC)
	@echo "--- creation de l'analyseur lexical ---------> $(SRC_LEXIC) ...........ok"


# creation du fichier objet pour l'analyseur lexical

$(OBJ_LEXIC) : $(SRC_LEXIC) $(UNITESLEX)
	@$(CC) $(CFLAGS) $(SRC_LEXIC)
	@echo "--- compilation de l'analyseur lexical ------> $(OBJ_LEXIC) ...........ok"


# creation du fichier objet pour dumpcode

$(OBJ_DUMP) : $(SRC_DUMP) $(OPCODES)
	@$(CC) $(CFLAGS) $(SRC_DUMP)
	@echo "--- compilation du fichier de dumpcodes -----> $(OBJ_DUMP) .............ok"


# creation du fichier objet pour la machine virtuelle

$(OBJ_MACH) : $(SRC_MACH) $(OPCODES) $(ERREUR) $(MEM)
	@$(CC) $(CFLAGS) $(SRC_MACH) $(SRC_ERR)
	@echo "--- compilation du fichier de la machine ----> $(OBJ_MACH) ..............ok"


# creation de l'executable

$(EXEC) : $(OBJ_COMP) $(OBJ_LEXIC) $(OBJ_ERR) $(OBJ_DUMP) $(OBJ_MACH)
	@$(LD) $(OBJ_COMP) $(OBJ_LEXIC) $(OBJ_ERR) $(OBJ_DUMP) $(OBJ_MACH) -o $(EXEC) $(LDFLAGS)
	@echo "--- creation de l'executable ----------------> $(EXEC) ............ok"



# suppression des fichiers objets et des fichiers temporaires

clean : 
	@rm -f *.o $(SRC_LEXIC) *~ \#*
	@echo '*** suppresion des fichiers objets et temporaires ok ! ***'



# nettoyage des sources

distclean : 
	@rm -f *.o $(SRC_LEXIC) $(EXEC) *~ \#* *.log
	@echo '*** nettoyage des sources ok ! ***'



# menu

help :
	@echo
	@echo '  options du make : '
	@echo
	@echo '     all ............. (re)compiler le projet'
	@echo '     clean ........... supprimer des fichiers objets et temporaires.'
	@echo '     distclean ....... nettoyer les sources'
	@echo '     help ............ afficher ce menu'
	@echo

