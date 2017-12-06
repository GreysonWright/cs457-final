//
//  Document.h
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#ifndef Document_h
#define Document_h

#include <stdio.h>

typedef struct DOCUMENT Document;

Document *newDocument(int, int);

int getCountDocument(Document *);
int getIDDocument(Document *);
void setCountDocument(Document *, int);
void incrementCountDocument(Document *);
void displayDocument(FILE *, void *);

#endif /* Document_h */
