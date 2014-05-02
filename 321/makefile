# CREATED 4-24-2014

# Project 2 - Lottery Scheduler
# CMPS111 - Spring 2014 - N. Whitehead
# ------------------------------------
# Melanie Dickinson - mldickin
# Neil Killeen - nkilleen
# Dave Lazell - dlazell
# Desmond Vehar - dvehar

# GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++0x
GCC        = g++ -Ofast -Wall -Wextra -std=gnu++0x

ASG        = proj2
MKFILE     = makefile
SOURCES1   = cpu.c
SOURCES2   = io.c
HEADERS    = 
OBJECTS1   = ${SOURCES1:.c=.o} read_tsc_64.o read_tsc.o
OBJECTS2   = ${SOURCES2:.c=.o} read_tsc_64.o read_tsc.o
EXECBIN1   = cpu
EXECBIN2   = io
SRCFILES   = ${HEADERS} ${SOURCES1} ${SOURCES2} ${MKFILE}
OTHERS     = copynew config.h main.c schedproc.h schedule.c install makedeps

all : makedeps ${EXECBIN1} ${EXECBIN2}

${EXECBIN1} : ${OBJECTS1}
	${GCC} -o ${EXECBIN1} ${OBJECTS1}

${EXECBIN2} : ${OBJECTS2}
	${GCC} -o ${EXECBIN2} ${OBJECTS2}

%.o : %.c
	${GCC} -c $<

clean :
	- rm ${OBJECTS1}
	- rm ${OBJECTS2}

spotless : clean
	- rm ${EXECBIN1}
	- rm ${EXECBIN2}
	- rm test.txt
	- rm ${ASG}.tar ${ASG}.tgz

zip:
	tar -c -f ${ASG}.tar README.txt design.pdf ${SRCFILES} ${OTHERS}
	gzip -c ${ASG}.tar > ${ASG}.tgz
	rm ${ASG}.tar
