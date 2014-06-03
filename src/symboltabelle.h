#include <utlist2.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <diag.h>
#define P_VOID 0
#define P_INT 1
#define UNDEFINIERT 0
#define DEFINIERT 1
#define BEGONNEN 2

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
struct YYLTYPE
{
	int first_line;
	int first_column;
	int last_line;
	int last_column;
};
typedef struct YYLTYPE YYLTYPE;
# define yyltype YYLTYPE
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

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
        struct variablen *lokal,*parameter;
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

void fehler(char* msg, YYLTYPE y);
char *typErkennung(int i);
void gebeFunktionenAus();
void gebeGlobaleVariablenAus();
int namecmp(variablen *a, variablen *b);
int fcmp(funktionen *a, funktionen *b);
variableIdentifikation *erzeugeVariableIdentifikation(char *name, int groesse);
variableDefinition *erzeugeVariableDefinition(variableDefinition *schonda, int typ, variableIdentifikation *variableIdentifikation);
void integriereVariableGlobal (variableDefinition *variable, YYLTYPE y);
funktionen *integriereFunktion (funktionen *newEl, variablen *parameter, variablen *lokal, int definiert, YYLTYPE y);
funktionen *makeFunc(int typ, char *name);
variablen *integriereVariable (variableDefinition *variable, variablen *liste, YYLTYPE y);
variablen *global, *aktuelleListe;
funktionen *fglobal;
int anzFehler;
char *scope;
int yylex(void);
void yyerror(const char *msg);
int suchVarGlobal(char *name);
variableDefinition *suchFunktionTyp(char *name, variablen *parameter, YYLTYPE y);
variablen *addToVarList(variableDefinition *expr, variablen *liste, YYLTYPE y);
variableDefinition *newInt(int typ), *getVar(char *name, YYLTYPE y); 

