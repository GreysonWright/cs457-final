//
//  DocumentStore.c
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include <stdlib.h>
#include "Document.h"
#include "DocumentStore.h"

struct DOCUMENTSTORE {
	DArray *store;
};

DocumentStore *newDocumentStore(void (*display)(FILE *file, void *value)) {
	DocumentStore *documentStore = malloc(sizeof *documentStore);
	documentStore->store = newDArray(display);
	return documentStore;
}

Document *insertDocumentStore(DocumentStore *documentStore, int docID) {
	int docIndex = getIndexDocumentStore(documentStore, docID);
	Document *document = 0;
	if (docIndex == -1) {
		document = newDocument(docID, 1);
		insertDArray(documentStore->store, document);
	} else {
		document = getDocumentStore(documentStore, docIndex);
		incrementCountDocument(document);
		setDArray(documentStore->store, docIndex, document);
	}
	
	return document;
}

int getIndexDocumentStore(DocumentStore *documentStore, int docID) {
	for (int i = 0; i < sizeDArray(documentStore->store); i++) {
		Document *currentDoc = getDocumentStore(documentStore, i);
		if (getIDDocument(currentDoc) == docID) {
			return i;
		}
	}
	
	return -1;
}

Document *getDocumentStore(DocumentStore *documentStore, int index) {
	return getDArray(documentStore->store, index);
}

int sizeDocumentStore(DocumentStore *documentStore) {
	return sizeDArray(documentStore->store);
}

void displayDocumentStore(FILE *file, DocumentStore *documentStore) {
	displayDArray(file, documentStore->store);
}
