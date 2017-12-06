//
//  Integer.h
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#ifndef Integer_h
#define Integer_h

#include <stdio.h>

typedef struct INTEGER Integer;

Integer *newInteger(int);
Integer *nullInteger(void);
int getInteger(Integer *);
void setInteger(Integer *, int);
Integer *parseInteger(char *, char *);

#endif /* Integer_h */
