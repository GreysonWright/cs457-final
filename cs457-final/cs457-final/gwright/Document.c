//
//  Document.c
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdlib.h>
#include "Document.h"

struct DOCUMENT {
	int id;
	int count;
};

Document *newDocument(int id, int count) {
	Document *newDoc = malloc(sizeof *newDoc);
	newDoc->id = id;
	newDoc->count = count;
	return newDoc;
}

int getCountDocument(Document *document) {
	return document->count;
}

int getIDDocument(Document *document) {
	return document->id;
}

void setCountDocument(Document *document, int value) {
	document->count = value;
}

void incrementCountDocument(Document *document) {
	document->count++;
}

void displayDocument(FILE *file, void *document) {
	fprintf(file, "id: %d, count: %d", getIDDocument(document), getCountDocument(document));
}
