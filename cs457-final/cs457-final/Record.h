//
//  Record.h
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#ifndef Record_h
#define Record_h

#include <stdio.h>

typedef struct RECORD Record;

Record *newRecord(char *);
int compareRecord(void *, void *);
void displayRecord(FILE *, void *);
char *getRecord(Record *);

#endif /* Record_h */
