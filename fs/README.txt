CREATED 5-27-14

Assignment: Project 4 - Extending a filesystem
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
Extract the proj4.tgz file into a fresh directory on the
Minix installation. CD to the created directory. Then:

To install the modified kernel:
type "./install"
Due to the need to recompile the libraries, this
can take a long time.
When the process completes, type "reboot"
then select the modified kernel (option 2)

To build command line utilities and test files, make sure you
  are on the minix filesystem, not on a mounted directory, then:
type "gmake"
