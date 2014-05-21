# CREATED 5-13-2014

# Test files follow naming convention test#.c
# A similarly named executable is generated for each such file

CC	= cc
ASG	= proj3

TESTSRC = ${wildcard test*.c}
TESTOBJ = ${TESTSRC:.c=.o}

HEADERS = slug.h
CSOURCE	= slug.c
OBJECTS = slug.o
EXECBIN	= ${TESTSRC:.c=}
SRCFILES= ${HEADERS} ${CSOURCE} ${TESTSRC}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS} ${TESTOBJ}
	${CC} -o $@ ${OBJECTS} $@.o

%.o : %.c
	${CC} -c -Wall -Wextra $< 

clean :
	- rm ${OBJECTS}
	- rm ${TESTOBJ}

spotless : clean
	- rm ${EXECBIN}

pack:
	tar -cvzf ${ASG}.tgz Makefile README.txt design.pdf ${SRCFILES}
