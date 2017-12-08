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
#include "DataBase.h"
#include "Record.h"
#include "darray.h"
#include "scanner.h"

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
	line = readLine(queryFile);
	while (!feof(dataFile)) {
		if (strstr(line, "query")) {
			
		} else if (strstr(line, "count")) {

		} else if (strtok(line, "sort")) {

		}

		line = readLine(dataFile);
	}
	fclose(queryFile);

	FILE *outFile = fopen("/Users/gwright/Git/cs457-final/cs457-final/cs457-final/gwright.txt", "w+");
	fclose(outFile);
	
//	char *searchVal = "";
//	char *resultFields = "";
//	DataBase *dataBase = newDataBase(displayRecord);
//	insertDataBase(dataBase, "DocID:1 test:1 a:0");
//	insertDataBase(dataBase, "DocID:2 test:2");
//	insertDataBase(dataBase, "DocID:1 test:2 a:1");
//	insertDataBase(dataBase, "DocID:0 test:2");
//	insertDataBase(dataBase, "DocID:3 test:2");
//	insertDataBase(dataBase, "DocID:2 test:2");
//	DArray *results = queryDataBase(dataBase, "sysid=0");
//	displayDArray(stdout, results);
//	printf("\n");
	return 0;
}
