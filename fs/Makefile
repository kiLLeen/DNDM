# CREATED 5-27-2014

# Test files follow naming convention test#.c
# A similarly named executable is generated for each such file

CC	= gcc
ASG	= proj4

CSOURCE  = metacat.c metatag.c
TESTSRC  = ${wildcard test*.c}
ALLSRC   = ${CSOURCE} ${TESTSRC}

ALLOBJ   = ${ALLSRC:.c=.o}
ALLBIN   = ${ALLSRC:.c=}

OTHERS   = copynew install unistd.h vfsproto.h vfstable.c vfsread.c vfswrite.c callnr.h sysread.c syswrite.c

all : ${ALLBIN}

${ALLBIN} : ${ALLOBJ}
	${CC} -o $@ $@.o

%.o : %.c
	${CC} -c -Wall -Wextra $<

clean :
	- rm ${ALLOBJ}

spotless : clean
	- rm ${ALLBIN}

pack:
	tar -cvzf ${ASG}.tgz Makefile README.txt design.pdf ${ALLSRC} ${OTHERS}
