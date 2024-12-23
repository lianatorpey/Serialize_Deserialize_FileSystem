#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
*   RUN THE SERIALIZE FUNCTIONALITY ON COMMAND TERMINAL: bin/transplant -s -p rsrc/testdir > outfile
*   VIEW THE SERIALIZED OUTPUT: od -t x1 outfile
*   RUN THE DESERIALIZE FUNCTIONALITY: bin/transplant -d -p test_out
*   RUN THE PIPING FROM STDOUT SERIALIZED TO STDIN DESERIALIZE: bin/transplant -s -p rsrc/testdir | bin/transplant -d -p test_out
*/

int main(int argc, char **argv) {
    int ret; // Return value after function return (part of provided code, not certain of use yet)

    /**
     * If no flags are provided then DISPLAY the usage and return with an EXIT_FAILURE return code.
     * If the -h flag is provided then DISPLAY the usage and return with an EXIT_SUCCESS return code.
     * If the -s flag is provided then perform serialization to stdout and exit with respective EXIT_SUCCESS and EXIT_FAILURE
     * If the -d flag is provided the perform deserialization from stdin and exit with respective EXIT_SUCCESS and EXIT_FAILURE
    */

    if(validargs(argc, argv) == -1) { // Validargs function returned an error in argument validation
        fflush(stdout); /// All output produced - clear output streams at end of execution
        fprintf(stderr, "ERROR: Invalid argument combination passed on command line - failed to validate. \n");
        USAGE(*argv, EXIT_FAILURE); // Validargs function always returns, USAGE macro called in main function
        return EXIT_FAILURE;
    }
    if(global_options & 0x1) { // Global options is a bit representation of flags set in validargs funcion
        // Validargs set the least significant bit to 1 (the -h flag was passed) then program call USAGE macro as success
        fflush(stdout); // Clear output stream at end of execution
        USAGE(*argv, EXIT_SUCCESS); // this means that the -h flag  was present
        return EXIT_SUCCESS;
    }

    // Serialize and Deserialize flags

    // name_buf was set in the validate arguments with the directory name if the -p flag was passed (the current dir)
    // if name_buf was not set, then the default working directory is the current directory
    // below initializes path_buf to contain the path to the directory whether that be the one provided or the default
    char *ptr = name_buf; // Pointer declared to traverse name_buff - ptr must be declared and initialized before use
    int length = 0; // counter - to get the length of name_buf
    while (*ptr != '\0') { // Does not include the null terminator
        length++; // Increment the length for each character
        ptr++; // Traverse and move to the next character
    }
    // Below attempts to initlize path_buf to a specified base path
    if (length == 0) { // If name_buf is empty, use the current working directory
        if (path_init(".") == -1) { // if there is an error making the current working directory the path
            fflush(stdout);
            fprintf(stderr, "ERROR: Failed to set path_buf to current working directory. \n");
            return EXIT_FAILURE;
        }
    } else {
        if (path_init(name_buf) == -1) { // if there is an error making the name_buf path to the directory
            fflush(stdout);
            fprintf(stderr, "ERROR: Failed to set path_buf to directory path passed with -p flag buffered on name_buf. \n");
            return EXIT_FAILURE;
        }
    }
    // name_buf was initialized to the directory path passed with the -p flag in the validate arguments if it was present
    // above checks the length of name_buf with pointer arithmetic, if empty current working directory is set to path_buf
    // else the passed in component is made as path for path_buf

    // Check for -s flag (serialization)
    if(global_options & 0x2) { // 0x2 represents the serialization flag - i think it's the same as 1<<1
        // Call serialize function - check the return statement of the function call

        if(serialize()== -1) {
            fflush(stdout);  // Clear output stream at the end of execution
            fprintf(stderr, "ERROR: Serialize() failed. \n");
            return EXIT_FAILURE; // Print usage message and exit with failure code if serialization fails
        }
    }

    // Check for -d flag (deserialization)
    if(global_options & 0x4) { // 0x4 represents the deserialization flag - i think it's the same as 1<<2
        // Call deserialize function - check the return statement of the function call

        if(deserialize()== -1) {
            fflush(stdout);  // Clear output stream at the end of execution
            fprintf(stderr, "ERROR: Deserialize() failed. \n");
            return EXIT_FAILURE;  // Print usage message and exit with failure code if deserialization fails
        }
    }
    fflush(stdout); // serialization or deserialization was successfull and make sure to clear output stream
    return EXIT_SUCCESS;

    /**
     * bin/transplant -s -p rsrc/testdir | bin/transplant -d -p test_out
     * the above commad works correctly to serialize the data in the rsrc test file into the test_out directory
     * bin/transplant -s -p rsrc/testdir: ensures the serialization correctly writes to stdout
     * |: takes the stdout from serialization and passed it in as the input to the next command stdin
     * bin/transplant -d -p test_out: proves deserialization correctly reads from stdin
    */

}
/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
