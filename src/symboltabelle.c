#include "symboltabelle.h"

void fehler(char* msg, YYLTYPE yylloc){
	FATAL_COMPILER_ERROR(INVALID_SEMANTICS, 0, "(%d.%d-%d.%d): %s\n", yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column, msg);
	anzFehler++;

}

void gebeGlobaleVariablenAus() {
	variablen *elt;
	DL_FOREACH(global,elt) {
		if (elt->varDef->identifikation->groesse > 0)
			printf("global:\t%s %s[%d]\n",typErkennung(elt->varDef->typ),elt->varDef->identifikation->name,elt->varDef->identifikation->groesse);
		else
			printf("global:\t%s %s\n",typErkennung(elt->varDef->typ),elt->varDef->identifikation->name);
	}
}

char *typErkennung(int i) {
	switch(i) {
		case 0: return "void";
		case 1: return "int";
		default: return "";
	}
}

void gebeFunktionenAus() {
	funktionen *felt;
	DL_FOREACH(fglobal,felt) {
		variablen *elt;
		DL_FOREACH(felt->lokal,elt) {
			if (elt->varDef->identifikation->groesse > 0)
				printf("%s:\t%s %s[%d]\n",felt->funktionsname,typErkennung(elt->varDef->typ),elt->varDef->identifikation->name,elt->varDef->identifikation->groesse);
			else
				printf("%s:\t%s %s\n",felt->funktionsname,typErkennung(elt->varDef->typ),elt->varDef->identifikation->name);
		}
	}
}

funktionen *makeFunc(int typ, char *name) {
	funktionen *newEl = (funktionen *) malloc(sizeof(funktionen)); 
	newEl->rueckgabetyp = typ; 
	newEl->funktionsname = name; 
	return newEl;
}

funktionen *integriereFunktion (funktionen *newEl, variablen *parameter, variablen *lokal, int definiert, YYLTYPE y) {
	variablen *telt;
	DL_FOREACH(parameter, telt){
		variablen *telt2;
		DL_SEARCH(lokal, telt2, telt, namecmp);
		if (telt2) {
			fehler("Variable mehrfach definiert.",y);
		}
	}
	if (suchVarGlobal(newEl->funktionsname)) {
		fehler("Funktion kann nicht deklariert werden, da es schon eine Variable gleichen namens gibt.", y);
	} else {
		DL_CONCAT(lokal,parameter);
		newEl->lokal = lokal;
		newEl->parameter = parameter;
		newEl->next = NULL;
		newEl->definiert = definiert;
		if (!fglobal) {
			DL_APPEND(fglobal,newEl);
			return newEl;
		}
		funktionen *elt;
		DL_SEARCH(fglobal, elt, newEl, fcmp);
		if (elt) {
			variablen *velt;
			int anz=0, anz2=0;
			DL_COUNT(elt->parameter,velt,anz);
			DL_COUNT(parameter,velt,anz2);
			if (anz == anz2 && elt->definiert == UNDEFINIERT && newEl->definiert == DEFINIERT) {
				variablen *var1=elt->parameter;
				variablen *var2=parameter;
				for (int i=0; i<anz; i++) {
					if (var1->varDef->typ == var2->varDef->typ && var1->varDef->identifikation->groesse == var2->varDef->identifikation->groesse && !strcmp(var1->varDef->identifikation->name,var2->varDef->identifikation->name)) {
						var1=var1->next;
						var2=var2->next;
					} else {
						fehler("Funktion schon deklariert mit anderem Prototypen.", y);
						return NULL;
					}
				}
//				DL_REPLACE_ELEM(fglobal,elt,newEl);
				DL_DELETE(fglobal,elt);
				DL_APPEND(fglobal,newEl);
			} else {
				fehler("Funktion schon definiert.", y);
			}
		}
		else {
			DL_APPEND(fglobal,newEl);
		}
	}
	return newEl;
}

void integriereVariableGlobal (variableDefinition *variable, YYLTYPE y) {
	global = integriereVariable(variable, global, y);
}

variablen *integriereVariable (variableDefinition *variable, variablen *liste, YYLTYPE y) {
	if (!variable) {
		return liste;
	}
	if (variable->typ == P_VOID && variable->identifikation->groesse != 0) {
		fehler("Void kann kein array sein.", y);
	} else {
		variableDefinition *elt;
		DL_FOREACH(variable,elt) {
			variablen *newEl = (variablen *) malloc(sizeof(variablen));
			newEl->varDef = elt;
			newEl->next = NULL;
			if (!liste) {
				DL_APPEND(liste,newEl);
				aktuelleListe=liste;
			} else {
				variablen *elt;
				DL_SEARCH(liste, elt, newEl, namecmp);
				if (elt) {
					fehler("Variable schon deklariert.", y);
				}
				else {
					DL_APPEND(liste,newEl);
				}
			}
		}
	}
	return liste;
}

variableIdentifikation *erzeugeVariableIdentifikation(char *name, int groesse) {
	variableIdentifikation *variable = (variableIdentifikation *) malloc(sizeof(variableIdentifikation));
	variable->groesse = groesse;
	variable->name = name;
	return variable;
}

variableDefinition *erzeugeVariableDefinition(variableDefinition *schonda, int typ, variableIdentifikation *variableIdentifikation) {
	variableDefinition *variable = (variableDefinition *) malloc(sizeof(variableDefinition));
	variable->typ = typ;
	variable->identifikation = variableIdentifikation;
	DL_APPEND(schonda,variable);
	return schonda;
}

int namecmp(variablen *a, variablen *b) {
	if (a && b) {
		if (a->varDef && b->varDef) {
			if (a->varDef->identifikation && b->varDef->identifikation) {
				return strcmp(a->varDef->identifikation->name,b->varDef->identifikation->name);
			}
		}
	}
	return false;
}

int fcmp(funktionen *a, funktionen *b) {
	return strcmp(a->funktionsname,b->funktionsname);
}

int suchVarGlobal(char *name) {
	variableIdentifikation *ident = (variableIdentifikation *) malloc(sizeof(variableIdentifikation));
	ident->name = name;
	variableDefinition *def = (variableDefinition *) malloc(sizeof(variableDefinition));
	def->identifikation = ident;
	variablen *gesucht = (variablen *) malloc(sizeof(variablen));
	gesucht->varDef = def;
	variablen *elt;
	DL_SEARCH(global, elt, gesucht, namecmp);
	if (elt) {
		return 1;
	}
	else {
		return 0;
	}
}

variableDefinition *suchFunktionTyp(char *name, variablen *parameter, YYLTYPE y) {
	funktionen *newEl = (funktionen *) malloc(sizeof(funktionen));
	newEl->funktionsname = name;
	funktionen *elt;
	DL_SEARCH(fglobal, elt, newEl, fcmp);
	if (elt) {
		if (elt->definiert == DEFINIERT) {
			if (parameter == NULL && elt->parameter == NULL) {
				variableDefinition *var = (variableDefinition *) malloc(sizeof(variableDefinition));
				var->typ = elt->rueckgabetyp;
				return var;
			}
			variablen *velt;
			int anz=0, anz2=0;
			DL_COUNT(elt->parameter,velt,anz);
			DL_COUNT(parameter,velt,anz2);
			if (anz2 == anz) {
				variablen *var1=elt->parameter;
				variablen *var2=parameter;
				for (int i=0; i<anz; i++) {

					if (var1->varDef->typ == var2->varDef->typ && var1->varDef->identifikation->groesse == var2->varDef->identifikation->groesse) {
						var1=var1->next;
						var2=var2->next;
					} else {
						fehler("Funktion deklariert mit anderen Parametern.", y);
					}
				}
				variableDefinition *var = (variableDefinition *) malloc(sizeof(variableDefinition));
				var->typ = elt->rueckgabetyp;
				return var;
			} else {
				fehler("Funktion deklariert mit anderen Parametern.", y);
			}	
		} else {
			fehler("Funktion noch nicht definiert.", y);
		}
	} else {
		fehler("Funktion existiert nicht mit gegebenem Namen.", y);
	}
	return NULL;
}

variablen *addToVarList(variableDefinition *expr, variablen *liste, YYLTYPE y) {
	return integriereVariable(expr, liste, y);
}

variableDefinition *newInt(int typ) {
	variableDefinition *def = (variableDefinition *) malloc(sizeof(variableDefinition));
	def->typ = P_INT;
	return def;
}

variableDefinition *getVar(char *name, YYLTYPE y) {
	variablen *elt;
	if (scope != NULL) {
		DL_FOREACH(aktuelleListe,elt) {
			if (!strcmp(elt->varDef->identifikation->name,name)) {
				return elt->varDef;
			}
		}
	}
	DL_FOREACH(global,elt) {
		if (!strcmp(elt->varDef->identifikation->name,name)) {
			return elt->varDef;
		}
	}
	fehler("Variable nicht gefunden.",y);
	return NULL;
}
