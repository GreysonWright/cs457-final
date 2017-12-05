//
//  Integer.c
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdlib.h>
#include "Integer.h"

struct INTEGER {
	int isNull;
	int value;
};

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

