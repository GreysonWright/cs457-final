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

int main(int argc, const char * argv[]) {
	char *searchVal = "";
	char *resultFields = "";
	DataBase *dataBase = newDataBase(displayRecord);
	insertDataBase(dataBase, "DocID:1 test:1");
	insertDataBase(dataBase, "DocID:1 test:2");
	insertDataBase(dataBase, "DocID:2 test:2");
	DArray *results = queryDataBase(dataBase, "test:2");
	displayDArray(stdout, results);
//	displayDataBase(stdout, dataBase);
//	DArray *results = query(store, searchVal);
//	writeResults(results, resultFields);
	printf("\n");
	return 0;
}
