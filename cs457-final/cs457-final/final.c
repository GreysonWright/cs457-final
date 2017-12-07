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
#include "Integer.h"

int main(int argc, const char * argv[]) {
	char *searchVal = "";
	char *resultFields = "";
	DataBase *dataBase = newDataBase(displayRecord);
	insertDataBase(dataBase, "DocID:1 test:1 a:0");
	insertDataBase(dataBase, "DocID:2 test:2");
	insertDataBase(dataBase, "DocID:1 test:2 a:1");
	insertDataBase(dataBase, "DocID:0 test:2");
	insertDataBase(dataBase, "DocID:3 test:2");
	insertDataBase(dataBase, "DocID:2 test:2");
	DArray *results = sortDataBase(dataBase, "DocID");
	displayDArray(stdout, results);
	printf("\n");
	return 0;
}
