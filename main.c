#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h> // Include for basename
#include "encode.h"
#include "decode.h"
#include <limits.h>

int main(int argc, char *argv[])
{
    static char bin[64], bout[64];
    setvbuf(stdin, bin, _IOFBF, 64);
    setvbuf(stdout, bout, _IOFBF, 64);

    char *exec_name = basename(argv[0]); // Get the executable name
    char *stage = getenv("STAGE");

    if (strcmp(exec_name, "encode") == 0) {
        int maxBits = 12;
        int pruning = 0;
        
        //parsing
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
                maxBits = atoi(argv[++i]);
            } else if (strcmp(argv[i], "-p") == 0) {
                pruning = 1; 
            } else {
                fprintf(stderr, "encode: invalid option '%s'\n", argv[i]);
                exit(1);
            }
        }

        //check maxBits
        if(maxBits == CHAR_BIT || maxBits > 2*CHAR_BIT+4)
          maxBits = CHAR_BIT+4;

        //execute encode
        if(stage && strcmp(stage,"1") == 0)
          encode(0,9);
        else if(stage && strcmp(stage,"2") == 0)
          encode(pruning, maxBits); 
        else
          encode(pruning, maxBits);
    
    } else if (strcmp(exec_name, "decode") == 0) {
        if (argc > 1) {
            fprintf(stderr, "decode: invalid option '%s'\n", argv[1]);
            exit(1);
        }
        decode();
    } else {
        fprintf(stderr, "Usage: %s [-m MAXBITS] [-p] < input > output\n", argv[0]);
        fprintf(stderr, "       %s < input > output\n", argv[0]);
        exit(1);
    }
    
    exit(0);
}