/**
 * primes.c
 *
 * Copyright (c) 2016 Daniel Szoke
 * License information contained in the LICENSE.txt file
 * 
 * Calculates all prime numbers up to a given number using the Sieve of
 * Eratosthenes, and outputs them to a text file.
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

typedef unsigned long long large;
typedef unsigned long smlarge;
typedef unsigned char byte;

large stoull(char*);
large powull(large, unsigned int);
void siv(FILE*, large);
large sivfirstseg(FILE*, smlarge*, smlarge*, large);
large sivseg(FILE*, smlarge*, smlarge*, large, large, large);


int main(int argc, char* argv[])
{
    // variables
    FILE* outfile;
    large maxnum;

    // check proper usage
    if (argc != 3)
    {
        printf("Usage: primes <outputfile> <number>\n");
        return 1;
    }

    // ensure a proper number was entered
    maxnum = stoull(argv[2]);
    if (maxnum < 1)
    {
        printf("You must enter a positive integer.\n");
        return 2;
    }

    // check if file exists
    if (fopen(argv[1], "r") != NULL)
    {
        printf("Warning! The file, %s, already exists.\n", argv[1]);\
        printf("Overwrite [Y/n]? ");
        char answer = fgetc(stdin);
        if (answer != 'y' && answer != 'Y')
        {
            return -1;
        }
    }

    // open file for writing
    outfile = fopen(argv[1], "w");

    // sieve primes
    printf("Please wait...\n");
    siv(outfile, maxnum);
    printf("Done\n");

    return 0;
}

/**
 * Convert a string of numbers to an unsigned long long, or return 0 in case of
 * error.
 */

large stoull(char* string)
{
    large num = 0;
    size_t digits = strlen(string);

    if (digits >= 19)
    {
        // warn if entiring large numbers
        printf("Warning: Entering a number greater than (2^64 - 2^32) will\n");
        printf("cause unpredicted program behavior!!\n");
    }

    for (int i = 0; i < digits; i++)
    {
        // convert char to correct number
        int digit;
        digit = (int) string[i] - 48;

        // verify that the value is a digit
        if (digit < 0 || digit > 9)
        {
            return 0;
        }

        // add digit into the number
        num += digit * powull(10, digits - i - 1);
    }

    return num;
}

/**
 * Raises an integer base to a positive or zero integer exponent
 */

large powull(large base, unsigned int exp)
{
    large result = 1;
    for (int i = 0; i < exp; i++)
    {
        result *= base;
    }
    return result;
}

/**
 * Find prime numbers until maxnum. Seperated from main
 * to enhance readability.
 */
void siv(FILE* outfile, large maxnum)
{
    large numof_primes;

    fprintf(outfile, "Prime numbers less than %llu\n", maxnum);
    fprintf(outfile, "====================================\n\n");

    if (maxnum < 5)
    {
        // handle special cases
        if (maxnum > 2)
        {
            fprintf(outfile, "2\n");
            numof_primes = 1;
            if (maxnum > 3)
            {
                fprintf(outfile, "3\n");
                numof_primes = 2;
            }
        }
        else
        {
            numof_primes = 0;
        }
    }
    else
    {
        // variables
        large segsize;
        smlarge* xprimes;
        smlarge* next;
        large numof_xprimes;
        large* primes;

        segsize = ceill(sqrtl(maxnum));

        // allocate memory
        xprimes = malloc(sizeof(smlarge) * segsize);
        next = malloc(sizeof(smlarge) * segsize);
        primes = malloc(sizeof(large) * segsize);

        // sieve first segment
        numof_xprimes = sivfirstseg(outfile, xprimes, next, segsize);
        numof_primes = numof_xprimes;

        // sieve all other sements
        for (large i = segsize; i < maxnum; i += segsize)
        {
            segsize = (i + segsize > maxnum) ? (maxnum - i) : (segsize);
            numof_primes += sivseg(outfile, xprimes, next, segsize,
                    numof_xprimes, i);
        }
    }

    fprintf(outfile, "\n====================================\n");
    fprintf(outfile, "Primes found: %llu", numof_primes);
}

/**
 * Traditional sieve. Used to sieve first segment only. Stores primes found in
 * primes, and prints them to outfile. Returns number of primes found
 */
large sivfirstseg(FILE* outfile, smlarge* primes, smlarge* next, large segsize)
{
    byte* segment;
    segment = malloc(sizeof(byte) * segsize);

    // we know 0 and 1 are not prime
    segment[0] = 0;
    segment[1] = 0;
    //printf("%llu\n", segsize);

    // assume all other numbers are prime
    for (large i = 2; i < segsize; i++)
    {
        segment[i] = 1;
    }

    // time to sieve
    large i = 2;
    large found = 0; // how many primes have been found
    byte done = 0;
    while (!(done))
    {
        // add number to primes list
        primes[found] = i;

        // cross out numbers and update next array
        large j = i * i;
        while (j < segsize)
        {
            segment[j] = 0;
            j += i;
        }
        next[found] = j - segsize;

        // see if done, otherwise find next prime
        done = 1;
        for (large j = i + 1; j < segsize; j++)
        {
            if (segment[j] == 1)
            {
                i = j;
                done = 0;
                break;
            }
        }
        found++;
    }

    for (large i = 0; i < found; i++)
    {
        fprintf(outfile, "%lu\n", primes[i]);
    }

    return found;
}

/**
 * Sieves all segments except for first segment. Prints them to outfile.
 */

large sivseg(FILE* outfile, smlarge* xprimes, smlarge* next, large segsize,
        large numof_xprimes, large startnum)
{
    byte* segment;
    large endnum;
    large numof_primes = 0;
    segment = malloc(sizeof(byte) * segsize);

    // initialize all of segment to 1
    for (large i = 0; i < segsize; i++)
    {
        segment[i] = 1;
    }

    // sieve the segment
    for (large i = 0; i < numof_xprimes; i++)
    {
        large j;
        for (j = next[i]; j < segsize; j += xprimes[i])
        {
            segment[j] = 0;
        }
        next[i] = j - segsize;
    }

    // print the primes
    for (large i = 0; i < segsize; i++)
    {
        if (segment[i])
        {
            fprintf(outfile, "%llu\n", i + startnum);
            numof_primes++;
        }
    }

    return numof_primes;
}
