//
//  darray.h
//  Assign_3
//
//  Created by Greyson Wright on 3/19/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#ifndef darray_h
#define darray_h

#include <stdio.h>

typedef struct DArray DArray; //forward declaration of the DArray struct

DArray *newDArray(void (*)(FILE *, void *));
void insertDArray(DArray *, void *);
void *removeDArray(DArray *);
void *getDArray(DArray *, int);
void setDArray(DArray *, int, void *);
int sizeDArray(DArray *);
void displayDArray(FILE *, DArray *);
#endif /* darray_h */
