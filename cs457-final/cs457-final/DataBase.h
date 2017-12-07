//
//  DataBase.h
//  cs457-final
//
//  Created by Greyson Wright on 12/6/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#ifndef DataBase_h
#define DataBase_h

#include <stdio.h>
#include "darray.h"

typedef struct DATABASE DataBase;

DataBase *newDataBase(void (*)(FILE *, void *));
void insertDataBase(DataBase *, char *);
DArray *queryDataBase(DataBase *, char *);
int countDataBase(DataBase *, char *);
DArray *sortDataBase(DataBase *, char *);
void displayDataBase(FILE *, DataBase *);

#endif /* DataBase_h */
