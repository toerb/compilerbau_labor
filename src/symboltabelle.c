#include "symboltabelle.h"

void gebeGlobaleVariablenAus() {
	variablen *elt;
	printf("global:\n");
	DL_FOREACH(global,elt) {
		if (elt->varDef->identifikation->groesse > 0)
			printf("\t%s %s[%d]\n",typErkennung(elt->varDef->typ),elt->varDef->identifikation->name,elt->varDef->identifikation->groesse);
		else
			printf("\t%s %s\n",typErkennung(elt->varDef->typ),elt->varDef->identifikation->name);
	}
}

char *typErkennung(int i) {
	switch(i) {
		case 0: return "void";
		case 1: return "int";
		default: return sprintf("da ist wohl was schiefgelaufen @ %s",__FUNCTION__);
	}
}

void gebeFunktionenAus() {
	funktionen *felt;
	DL_FOREACH(fglobal,felt) {
		variablen *elt;
		printf("%s %s:\n",typErkennung(felt->rueckgabetyp),felt->funktionsname);
		DL_FOREACH(felt->lokal,elt) {
			if (elt->varDef->identifikation->groesse > 0)
				printf("\t%s %s[%d]\n",typErkennung(elt->varDef->typ),elt->varDef->identifikation->name,elt->varDef->identifikation->groesse);
			else
				printf("\t%s %s\n",typErkennung(elt->varDef->typ),elt->varDef->identifikation->name);
		}
	}
}


integriereFunktion (int typ, char *name, variablen *parameter, variablen *lokal, int definiert) {
	funktionen *newEl = (funktionen *) malloc(sizeof(funktionen));
	DL_CONCAT(lokal, parameter);
	newEl->rueckgabetyp = typ;
	newEl->funktionsname = name;
	newEl->lokal = lokal;
	newEl->next = NULL;
	newEl->definiert = definiert;
	if (!fglobal) {
		DL_APPEND(fglobal,newEl);
		return;
	}
	funktionen *elt;
	DL_SEARCH(fglobal, elt, newEl, fcmp);
	if (elt) {
		variablen *velt;
		int anz=0, anz2=0;
		DL_COUNT(elt->lokal,velt,anz);
		DL_COUNT(parameter,velt,anz2);
		if (anz == anz2 && elt->definiert == UNDEFINIERT && newEl->definiert == DEFINIERT) {
			variablen *var1=elt->lokal;
			variablen *var2=parameter;
			for (int i=0; i<anz; i++) {
				if (var1->varDef->typ == var2->varDef->typ && var1->varDef->identifikation->groesse == var2->varDef->identifikation->groesse && !strcmp(var1->varDef->identifikation->name,var2->varDef->identifikation->name)) {
					var1=var1->next;
					var2=var2->next;
				} else {
					yyerror("Funktion schon deklariert mit anderem Prototypen.");
					return;
				}
			}
			DL_REPLACE_ELEM(fglobal,elt,newEl);
		} else {
			yyerror("Funktion schon definiert.");
		}	
	}
	else {
		DL_APPEND(fglobal,newEl);
	}
}

void integriereVariableGlobal (variableDefinition *variable) {
	global = integriereVariable(variable, global);
}

variablen *integriereVariable (variableDefinition *variable, variablen *liste) {
	if (!variable) {
		return liste;
	}
	variableDefinition *elt;
	DL_FOREACH(variable,elt) {
		variablen *newEl = (variablen *) malloc(sizeof(variablen));
		newEl->varDef = elt;
		newEl->next = NULL;
		if (!liste) {
			DL_APPEND(liste,newEl);
		} else {
			variablen *elt;
			DL_SEARCH(liste, elt, newEl, namecmp);
			if (elt) {
			}
			else {
				DL_APPEND(liste,newEl);
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
	return strcmp(a->varDef->identifikation->name,b->varDef->identifikation->name);
}

int fcmp(funktionen *a, funktionen *b) {
	return strcmp(a->funktionsname,b->funktionsname);
}
