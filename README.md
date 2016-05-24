primes.c
========

Usage:
./primesieve *outputfile* *number*

Finds all prime numbers less than a given number using the Sieve of Eratosthenes and outputs them to a file.
When compiling, you must run the compiler with the -lm flag to ensure that the math library is linked, or run "make primesieve" if you are using a GNU/Linux system.
For example, to compile with clang, run
    clang -o primesieve primesieve.c -lm

Source:
https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
