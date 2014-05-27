#include <utlist2.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define P_VOID 0
#define P_INT 1
#define UNDEFINIERT 0
#define DEFINIERT 1


struct variableIdentifikation
{
       char *name;
       int groesse;
};
typedef struct variableIdentifikation variableIdentifikation;

struct variableDefinition
{
       int typ;
       struct variableIdentifikation *identifikation;
       struct variableDefinition *prev,*next;
};
typedef struct variableDefinition variableDefinition;

struct variablen
{
	struct variableDefinition *varDef;
        struct variablen *prev,*next;
};
typedef struct variablen variablen;

struct funktionen
{
        char *funktionsname;
        int rueckgabetyp;
        struct variablen *lokal;
        struct funktionen *prev,*next;
	int definiert;
};
typedef struct funktionen funktionen;

struct parameter
{
        char *name;
        int type;
        struct parameter *prev,*next;
};
typedef struct parameter parameter;

char *typErkennung(int i);
void gebeFunktionenAus();
void gebeGlobaleVariablenAus();
int namecmp(variablen *a, variablen *b);
int fcmp(funktionen *a, funktionen *b);
variableIdentifikation *erzeugeVariableIdentifikation(char *name, int groesse);
variableDefinition *erzeugeVariableDefinition(variableDefinition *schonda, int typ, variableIdentifikation *variableIdentifikation);
void integriereVariableGlobal (variableDefinition *variable);
void integriereFunktion (int type, char *name, variablen *parameter, variablen *lokal, int definiert);
variablen *integriereVariable (variableDefinition *variable, variablen *liste);
variablen *global;
funktionen *fglobal;
