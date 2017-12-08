//
//  main.c
//  cs457-final
//
//  Created by Greyson Wright on 12/2/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "DataBase.h"
#include "Record.h"
#include "darray.h"
#include "scanner.h"

char *stripWhiteSpace(char *);
int parseVersion(char *);

int main(int argc, const char * argv[]) {
	DataBase *dataBase = newDataBase(displayRecord);
	FILE *dataFile = fopen("/Users/gwright/Git/cs457-final/cs457-final/cs457-final/data.txt", "r");
	char *line = readLine(dataFile);
	while (!feof(dataFile)) {
		insertDataBase(dataBase, line);
		line = readLine(dataFile);
	}
	fclose(dataFile);
	line = 0;
	
	FILE *queryFile = fopen("/Users/gwright/Git/cs457-final/cs457-final/cs457-final/queries.txt", "r");
	FILE *outFile = fopen("/Users/gwright/Git/cs457-final/cs457-final/cs457-final/gwright.txt", "w+");
	line = readLine(queryFile);
	line = stripWhiteSpace(line);
	while (!feof(dataFile)) {
		char *collectionCommand = strtok(line, "[");
		if (strstr(collectionCommand, "final.")) {
			char *query = strtok(0, "]");
			if (strstr(collectionCommand, "count")) {
				(void)strtok(0, "[");
				char *versionString = strtok(0, "]");
				int version = parseVersion(versionString);
				int count = countDataBase(dataBase, query, version);
				fprintf(outFile, "count_%s:%d\n", query, count);
			} else {
				(void)strtok(0, "[");
				char *fields = strtok(0, "]");
				(void)strtok(0, "[");
				char *versionString = strtok(0, "]");
				int version = parseVersion(versionString);
				DArray *results = newDArray(0);
				if (strstr(collectionCommand, "query")) {
					results = queryDataBase(dataBase, query, version);
				} else if (strtok(collectionCommand, "sort")) {
					results = sortDataBase(dataBase, query, version);
				}
				displaySelectDataBase(outFile, results, fields);
			}
		}
		
		line = readLine(dataFile);
	}
	fclose(outFile);
	fclose(queryFile);
	return 0;
}

char *stripWhiteSpace(char *token) {
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

int parseVersion(char *versionString) {
	if (versionString == 0) {
		return 1;
	}
	if (strcmp(versionString, ")")) {
		return 0;
	}
	return atoi(versionString);
}
