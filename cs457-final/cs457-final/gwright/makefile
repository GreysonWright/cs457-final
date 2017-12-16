OBJS = final.o Integer.o scanner.o darray.o Record.o Document.o DocumentStore.o DataBase.o
OPTS = -Wall -g -std=c99 -Wextra

final : $(OBJS)
		gcc $(OPTS) $(OBJS) -o final -lm

final.o : final.c
		gcc $(OPTS) -c final.c

Integer.o : Integer.c Integer.h
		gcc $(OPTS) -c Integer.c

darray.o : darray.c darray.h
		gcc $(OPTS) -c darray.c

scanner.o : scanner.c scanner.h
		gcc $(OPTS) -c scanner.c

Record.o : Record.c Record.h
		gcc $(OPTS) -c Record.c		

Document.o : Document.c Document.h
		gcc $(OPTS) -c Document.c

DocumentStore.o : DocumentStore.c DocumentStore.h
		gcc $(OPTS) -c DocumentStore.c

DataBase.o : DataBase.c DataBase.h
		gcc $(OPTS) -c DataBase.c

clean :
		rm -f $(OBJS) final

test :	./final
		./final
		
