//
//  DocumentStore.c
//  cs457-final
//
//  Created by Greyson Wright on 12/5/17.
//  Copyright © 2017 Greyson Wright. All rights reserved.
//

#include "Document.h"
#include "DocumentStore.h"

struct DOCUMENTSTORE {
	DArray *store;
};

void insertDocumentStore(DocumentStore *documentStore, int docID) {
	int docIndex = getIndexDocumentStore(documentStore, docID);
	if (docIndex != -1) {
		Document *newDoc = newDocument(docID, 1);
		insertDArray(documentStore->store, newDoc);
	} else {
		Document *document = getDocumentStore(documentStore, docIndex);
		incrementCountDocument(document);
		setDArray(documentStore->store, docIndex, document);
	}
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
