COMPILEC  = cc

CSOURCE	= test.c slug.c
EXECBIN	= tester
OBJECTS	= test.o slug.o

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILEC} -o $@ ${OBJECTS}
	
%.o : %.c
	${COMPILEC} -c -Wall -Wextra $< 

clean :
	- rm ${OBJECTS}

spotless : clean
	- rm ${EXECBIN}

#pack:
#	tar -cvzf shell.tgz Makefile shell.c lex.c README.txt design.pdf design_desmond.pdf
