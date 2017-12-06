//
//  DocumentStore.h
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#ifndef DocumentStore_h
#define DocumentStore_h

#include <stdio.h>
#include "darray.h"
#include "Integer.h"

typedef struct DOCUMENTSTORE DocumentStore;

DocumentStore *newDocumentStore(void (*)(FILE *, void *));
void insertDocument(DocumentStore *, int);
int getIndexDocumentStore(DocumentStore *, int);
Document *getDocumentStore(DocumentStore *, int);

#endif /* DocumentStore_h */
