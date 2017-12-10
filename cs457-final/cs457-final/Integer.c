//
//  Integer.c
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Integer.h"

struct INTEGER {
	int isNull;
	int value;
};

char *stripWhiteSpaceInteger(char *);

Integer *newInteger(int value) {
	Integer *newInt = malloc(sizeof *newInt);
	newInt->isNull = 0;
	newInt->value = value;
	return newInt;
}

Integer *nullInteger() {
	Integer *newInt = newInteger(0);
	newInt->isNull = 1;
	return newInt;
}

int getInteger(Integer *integer) {
	return integer->value;
}

void setInteger(Integer *integer, int value) {
	integer->value = value;
	integer->isNull = 0;
}

Integer *parseInteger(char *source, char *key) {
	char *token = malloc(strlen(source));
	strcpy(token, source);
	
	char *pkey = strtok(token, ":><");
	char *pval = strtok(0, " ");
	
	while (pkey) {
		pkey = stripWhiteSpaceInteger(pkey);
		pval = stripWhiteSpaceInteger(pval);
		if (strcmp(pkey, key) == 0) {
			return newInteger(atoi(pval));
		}
		pkey = strtok(0, ":><");
		pval = strtok(0, " ");
	}
	
	return nullInteger();
}

char *stripWhiteSpaceInteger(char *token) {
	long tokenLen = strlen(token);
	char *newToken = malloc(tokenLen);
	int count = 0;
	for (int i = 0; i < tokenLen; i++) {
		if (!isspace(token[i])) {
			newToken[count++] = token[i];
		}
	}
	return newToken;
}

int compareInteger(void *right, void *left) {
	return getInteger(right) - getInteger(left);
}

void displayInteger(FILE *file, void *integer) {
	Integer *i = integer;
	if (i->isNull) {
		fprintf(file, "nil");
	} else {
		fprintf(file, "%d", getInteger(i));
	}
}
