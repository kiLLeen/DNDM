CREATED 4-24-2014

Assignment: Project 2 - Lottery Scheduler
Class: CMPS111 - Spring 2014
Professor: N. Whitehead

Melanie Dickinson - mldickin
Neil Killeen - nkilleen
Dave Lazell - dlazell
Desmond Vehar - dvehar

For this project we are using Minix version 3.2.1
The package gcc44-4.4.6 must be installed to make this project.
To install it type "pkgin install gcc44-4.4.6nb4"

Installation:
Unzip the proj2.tgz file into a fresh directory on the
Minix installation. CD to the created directory. Then:

To install the modified kernel:
type "./install"
when the process completes, type "reboot"
then select the modified kernel (option 2)

To build test files:
type "./makedeps"
type "make"

Tests scripts for the static scheduler:
"./test1" - This script runs two CPU bound tasks each with 20
            tickets. Both should finish with similar times.
"./test2" - This script runs one CPU task with 20 tickets,
            and another with 40 tickets. They should finish
			with the correct ratio of 3:4.
"./test3" - This script runs one CPU task with 25 tickets,
            one with 50 tickets, and another with 100 tickets.
			They should finish near the correct ratio of
			7:10:12

Tests for the both schedulers:
"./test6" - This script runs three IO and three CPU tasks,
            each with 20 tickets. When run with the dynamic
			scheduler, you can see that the printouts from
			the IO and CPU programs are sufficiently
			interleaved to finish close to each other. Due
			to the dynamic scheduler also handling system
			processes, this script must be run twice after
			a fresh minix boot. The correct results will
			be obtained from the second run.
"./test7" - As test6, but does not run concurrently. Thus we
			can tell if test6 correctly runs all tasks quicker.
"./test4" - This script runs one IO task with 1 ticket, and
            three CPU tasks with 100 tickets. Make sure the
			"test.txt" file does not exist before running
			the script. Run the script, and keep typeing
			"ls". If you see the file has been created, then
			you know that the process has run at least once
			and has not been starved.
