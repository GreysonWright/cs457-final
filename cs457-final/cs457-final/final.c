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

int main(int argc, const char * argv[]) {
	char *searchVal = "";
	char *resultFields = "";
	DataBase *dataBase = newDataBase(displayRecord);
//	DArray *results = query(store, searchVal);
//	writeResults(results, resultFields);
	printf("\n");
	return 0;
}
