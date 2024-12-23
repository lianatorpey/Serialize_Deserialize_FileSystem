#include "global.h"
#include "debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

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
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 *
 * Variables to hold the pathname of the current file or directory
 * as well as other data have been pre-declared for you in global.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/*
 * @brief  Initialize path_buf to a specified base path.
 * @details  This function copies its null-terminated argument string into
 * path_buf, including its terminating null byte.
 * The function fails if the argument string, including the terminating
 * null byte, is longer than the size of path_buf.  The path_length variable
 * is set to the length of the string in path_buf, not including the terminating
 * null byte.
 *
 * @param  Pathname to be copied into path_buf.
 * @return 0 on success, -1 in case of error
 */
int path_init(char *name) {
    // Pointers for the current position in path_buf and the end of the buffer
    char *path_end = path_buf + PATH_MAX - 1;
    char *dest = path_buf; // pointer to path_buf

    // Copy the string to path_buf, including the null terminator until hit the end of name or path_buf
    // Try and increase optimality by checking for overflow while copying (don't calculate length of name upfront)
    while (*name != '\0' && dest < path_end) {
        *dest++ = *name++;
    }

    // Check if copied the entire name (including the null terminator)
    if (*name != '\0') { // Checkes if entire string has been copied - if source string exceeds buffer capacity = error
        // The source string was too long for the buffer
        fprintf(stderr, "ERROR: path_init() failed to initialize path_buf - exceeded buffer capacity. \n");
        return -1;
    }
    *dest = '\0'; // Add the null terminator

    // Calculate the length of the path in path_buf (excluding the null terminator)
    path_length = dest - path_buf; // Update path_length
    return 0;
}

/*
 * @brief  Append an additional component to the end of the pathname in path_buf.
 * @details  This function assumes that path_buf has been initialized to a valid
 * string.  It appends to the existing string the path separator character '/',
 * followed by the string given as argument, including its terminating null byte.
 * The length of the new string, including the terminating null byte, must be
 * no more than the size of path_buf.  The variable path_length is updated to
 * remain consistent with the length of the string in path_buf.
 *
 * @param  The string to be appended to the path in path_buf.  The string must
 * not contain any occurrences of the path separator character '/'.
 * @return 0 in case of success, -1 otherwise.
 */
int path_push(char *name) {
    // Pointers for the current position in path_buf and the end of the buffer
    char *path_end = path_buf + PATH_MAX - 1;

    // Calculate the length of the new component
    // and check for the parameter specification
    //The string must not contain any occurrences of the path separator character '/'.
    int name_length = 0;
    char *name_ptr = name;
    while (*name_ptr != '\0') {
        if (*name_ptr == '/') {
            fprintf(stderr, "ERROR: The component contains the separator character '/' \n");
            return -1;
        }
        name_length++;
        name_ptr++;
    }

    // Ensure there's enough space to add '/' and the new component
    if (path_length + 1 + name_length >= PATH_MAX) {
        fprintf(stderr, "ERROR: path_push failed - not enough space to append '/' and the new component. \n");
        return -1; // Not enough space
    }

    // Move to the end of the current path
    char *dest = path_buf + path_length;

    // Check if the current path already ends with a '/'
    if (path_length > 0 && *(dest - 1) != '/') { // avoid reduntantly adding another '/'
        // Append the '/' character if it does not already exist
        if (dest < path_end) {
            *dest++ = '/';
        } else {
            fprintf(stderr, "ERROR: path_push failed - not enough space to append '/' after the new component. \n");
            return -1; // No space for '/'
        }
    }

    // Append the new component
    while (*name != '\0' && dest < path_end) {
        *dest++ = *name++;
    }
    // Check if we have copied the entire component (including the null terminator)
    if (*name != '\0') {
        fprintf(stderr, "ERROR: path_push failed - the entire component was not appended including the null terminator. \n");
        return -1; // The component was too long
    }
    *dest = '\0'; // Add the null terminator since includes terminating null

    path_length = dest - path_buf; // Update path_length
    return 0;
}

/*
 * @brief  Remove the last component from the end of the pathname.
 * @details  This function assumes that path_buf contains a non-empty string.
 * It removes the suffix of this string that starts at the last occurrence
 * of the path separator character '/'.  If there is no such occurrence,
 * then the entire string is removed, leaving an empty string in path_buf.
 * The variable path_length is updated to remain consistent with the length
 * of the string in path_buf.  The function fails if path_buf is originally
 * empty, so that there is no path component to be removed.
 *
 * @return 0 in case of success, -1 otherwise.
 */
int path_pop() { // Traverse using pointer arithmetic backwards from the end, searching for the last '/'
    // Ensure path_buf is not empty
    if (path_length == 0) {
        fprintf(stderr, "ERROR: path_pop failed because path_buf is empty and there is no component to remove. \n");
        return -1;
    }

    // Pointer to the end of the current path
    char *end = path_buf + path_length - 1;

    // Search backwards for the last '/'
    char *slash = end;
    while (slash >= path_buf && *slash != '/') {
        slash--;
    }

    // Update path_length based on the position of the last '/'
    if (slash >= path_buf) {
        path_length = slash - path_buf;
    } else {
        // No '/' found, so clear the whole path
        path_length = 0;
    }

    // Null-terminate the string at the new end
    *(path_buf + path_length) = '\0';

    return 0;
}

int len_string(char *str) { // implementing the string length function for name
    // can't use the string library
    int counter = 0;
    while (*(str + counter) != '\0') {
        counter++;
    }
    return counter;
}

/*
 * @brief Deserialize directory contents into an existing directory.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory.  It reads (from the standard input) a sequence of DIRECTORY_ENTRY
 * records bracketed by a START_OF_DIRECTORY and END_OF_DIRECTORY record at the
 * same depth and it recreates the entries, leaving the deserialized files and
 * directories within the directory named by path_buf.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * each of the records processed.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including depth fields in the records read that do not match the
 * expected value, the records to be processed to not being with START_OF_DIRECTORY
 * or end with END_OF_DIRECTORY, or an I/O error occurs either while reading
 * the records from the standard input or in creating deserialized files and
 * directories.
 */
unsigned char debug_getchar() {
    /**
    * a wrapper function for getchar() as directed by professor stark for debugging errors in reading bytes of serialized data in deserialize function
    * functions reads a byte from the standard input and prints the byte as a hex value for debugging purposes.
    */
    int byte = getchar();
    /*
    if (byte != EOF) { // Just for debugging purposes
        fprintf(stderr, "byte read: %02x\n",  byte);
    } else {
        fprintf(stderr, "ERROR: EOF was read for the byte \n");
    }
    */
    return byte;
}
int check_while_condition(int depth) {
    /**
    * a function to check the while condition when recursively navigating when conducting deserialization
    * get the type of the record
    * check that the depth is correct
    * also make sure that megic bytes are consistent
    */
    // Check the magic bytes
    unsigned char magic1 = debug_getchar();
    unsigned char magic2 = debug_getchar();
    unsigned char magic3 = debug_getchar();
    if (magic1 == EOF || magic2 == EOF || magic3 == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF character when attempting to read the magic bytes in checking the while condition to parse type. \n");
        return -1; // Check for EOF
    }
    if (magic1 != 0x0C || magic2 != 0x0D || magic3 != 0xED) {
        fprintf(stderr, "ERROR: Magic bytes mismatched when checking the while condition to parse type; bytes read: %02x%02x%02x\n", magic1, magic2, magic3);
        return -1; // Magic bytes mismatch
    }

    // Check the record type
    char type = debug_getchar(); // record type returned after deserialization
    if (type == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF character read when attempting to parse type in checking of while condition. \n");
        return -1;
    }

    int depth_parsed = 0; // depth should be at 0
    for (int i = 0; i < 4; i++) {
        int byte = debug_getchar();
        if (byte == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF character read when attempting to parse depth in checking of while condition. \n");
            return -1;
        }
        depth_parsed = (depth_parsed << 8) | byte;
    }
    if (depth_parsed != depth) {
        fprintf(stderr, "ERROR: The depth parsed does not match the expected depth in checking of while condition. \n");
        return -1;
    }

    return type;
}
int deserialize_directory(int depth) {
    // Process records

    int type;

    while ((type = check_while_condition(depth)) != -1) {
        // type 0: START_OF_TRANSMISSION (Checked before enter this fnct in deserialize())
        if (type == 2) { // START_OF_DIRECTORY
            // check matching the size (uint64_t)
            int size = 0; // size should be 16
            for (int i = 0; i < 8; i++) {
                int byte = debug_getchar();
                if (byte == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF character when checking the size of the START OF DIRECTORY record. \n");
                    return -1;
                }
                size = (size << 8) | byte;
            }
            if (size != 16) {
                fprintf(stderr, "ERROR: Size does not equal 16 of the START OF DIRECTORY record. \n");
                return -1;
            }
            continue;
        }

        if (type == 3) { // END_OF DIRECTORY
            // check matching the size (uint64_t)
            int size = 0; // size should be 16
            for (int i = 0; i < 8; i++) {
                int byte = getchar();
                if (byte == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF character when checking the size of the END OF DIRECTORY record. \n");
                    return -1;
                }
                size = (size << 8) | byte;
            }
            if (size != 16) {
                fprintf(stderr, "ERROR: Size does not equal 16 of the END OF DIRECTORY record. \n");
                return -1;
            }
            if (path_pop() == -1) {
                fprintf(stderr, "ERROR: Failed to pop the directory off path_buf at read of END OF DIRECTORY record. \n");
                return -1;
            }
            if (depth == 1) return 0; // return to deserialize function and read the end of directory (recursive case to end it)
            // The above check makes sure start and end of directory come in nested pairs like parentheses
            // Recursive nature of depth popping and pushing components checks that there is a corresponding match
            // Does not allow mismatches because explicitly checks for each type and handles the corresponding depth changes
            return deserialize_directory(depth - 1);  // reached the end of a directory so go up one level

        } else if (type == 4) { // DIRECTORY_ENTRY
            // Read the size of the entry (should correspond to the name length + file metadata size)
            int total_size = 0;
            for (int i = 0; i < 8; i++) {
                int byte = debug_getchar();
                if (byte == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF character when reading the size of DIRECTORY ENTRY record. \n");
                    return -1;
                }
                total_size = (total_size << 8) | byte;
            }

            int name_length = total_size - 16 - 12; // total size - header size - metadata size

            uint32_t mode;
            for (int i = 0; i < 4; i++) {
                // read the first 4 bytes for st_mode
                // can't use stat() since directory path does not exist and state of filesystem unknown
                // need correct results if a file or directory supposed to be
                int byte = debug_getchar();
                if (byte == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF character when reading the mode of the DIRECTORY ENTRY record. \n");
                    return -1;
                }
                mode = (mode << 8) | byte;
            }
            // skip the next 8 bytes though to maintain pointer to read the correct bytes
            for (int i = 0; i < 8; i++) { // skip the next 12 bytes for metadata of directory entry line
                if (debug_getchar() == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF character when skipping bytes of DIRECTORY ENTRY. \n");
                return -1; // just skipping read not storing
                }
            }

            char *name_ptr = name_buf; // read the name of the component
            while (name_ptr < name_buf + len_string(name_buf)) {
                *name_ptr++ = '\0'; // clearing what was previously stored in name_buf
            }
            name_ptr = name_buf;
            for (int i = 0; i < name_length; i++) {
                int byte = debug_getchar();
                if (byte == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF character when reading component name from DIRECTORY ENTRY into name_buf buffer.\n");
                return -1; // unexpected end of file response
            }
                *name_ptr++ = (char)byte;
            }
            *name_ptr = '\0';

            if (path_push(name_buf) == -1) {
                fprintf(stderr, "ERROR: failed to push new component DIRECTORY ENTRY onto path_buf\n");
                return -1; // add the name of the new component
            }

            if (S_ISDIR(mode)) { // DIRECTORY
                // try and open the directory
                struct dirent *de;
                DIR *dir = opendir(path_buf);
                if (dir) { // directory exists now check if the -c clobber flag is present or if it is the top level directory
                    if (global_options & (1 << 3)) {
                        return deserialize_directory(depth + 1); // directory already exists and c flag present; increment the depth and go level down
                    } else { // directory exists but the -c clobber flag was not passed as well
                        fprintf(stderr, "ERROR: The DIRECTORY ENTRY was an already existing directory, but the clobber flag was not passed so cannot recreate.\n");
                        return -1;
                    }
                    if (closedir(dir) == -1) {
                        fprintf(stderr, "ERROR: Directory failed to close. \n");
                        return -1;
                    }
                } else if (ENOENT == errno) { // directory does not exist must create the directory
                    if (mkdir(path_buf, 0777) != 0) {
                        // This sets the permission settings for the directory
                        // 7: owner's permissions (rwx) which has the read (4), write (2), and execute (1) permissions
                        fprintf(stderr, "ERROR: Failed to make new directory at path_buf. \n");
                        return -1;
                    }
                    struct stat stat_buf;
                    if (stat(path_buf, &stat_buf) == -1) {
                        fprintf(stderr, "ERROR: Failed to extract metadata of directory. \n");
                        return -1;
                    }
                    // Check if the permissions were set correctly
                    // chmod(path_buf, stat_buf.st_mode & 0777);
                    if (chmod(path_buf, stat_buf.st_mode & 0777) != 0) {
                        fprintf(stderr, "ERROR: Permissions of directory not set correctly. \n");
                        return -1;
                    }
                    return deserialize_directory(depth + 1);
                } else {
                    fprintf(stderr, "ERROR: Unexpected erorr.\n");
                    return -1;
                }
            } else if (S_ISREG(mode)) { // FILE
                if (deserialize_file(depth) != 0) return -1; // don't increment depth explore same level recursively
                /*
                struct stat stat_buf;
                if (stat(path_buf, &stat_buf) == -1) {
                    fprintf(stderr, "ERROR: Failed to extract metadata of file written. \n");
                    return -1;
                }
                // chmod(path_buf, stat_buf.st_mode & 0777);
                */
                if (chmod(path_buf, mode & 0777) != 0) {
                    fprintf(stderr, "ERROR: Permissions of file written not correct. \n");
                    return -1;
                }
                if (path_pop() == -1) {
                    fprintf(stderr, "ERROR: Failed to pop component off path_buf after writing file. \n");
                    return -1;
                }
            }
            else { // entry not a file or a directory (probably never reach)
                fprintf(stderr, "ERROR: Unexpected error - not a file or a directory. \n");
                return -1;
            }

        } else {  // not a type 0-5 (probably should never reach)
            fprintf(stderr, "ERROR: Unexpected record type in deserialize directory recursion. \n");
            return -1; // Invalid type
        }
        // type 1: END_OF_TRANSMISSION (Checked after leave this fnct in deserialize())
    }

    return -1;
}

/*
 * @brief Deserialize the contents of a single file.
 * @details  This function assumes that path_buf contains the name of a file
 * to be deserialized.  The file must not already exist, unless the ``clobber''
 * bit is set in the global_options variable.  It reads (from the standard input)
 * a single FILE_DATA record containing the file content and it recreates the file
 * from the content.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * the FILE_DATA record.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including a depth field in the FILE_DATA record that does not match
 * the expected value, the record read is not a FILE_DATA record, the file to
 * be created already exists, or an I/O error occurs either while reading
 * the FILE_DATA record from the standard input or while re-creating the
 * deserialized file.
 */
int deserialize_file(int depth) {
    // Check if the file already exists
    struct stat stat_buf;
    if(stat(path_buf, &stat_buf) == 0) {
        // The file already exists
        if (!(global_options & (1 << 3))) {
            fprintf(stderr, "ERROR: File already exists but clobber flag not passed so cannot overwrite file. \n");
            return -1;
        }
    }

    // at this point the file either does not exist already, or it does and has the permission to overwrite from the clobber flag
    // STEP 1: check  the magic bytes
    unsigned char magic1 = debug_getchar();
    unsigned char magic2 = debug_getchar();
    unsigned char magic3 = debug_getchar();
    if (magic1 == EOF || magic2 == EOF || magic3 == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF character read in deserialize file. \n");
        return -1; // Check for EOF
    }
    if (magic1 != 0x0C || magic2 != 0x0D || magic3 != 0xED) {
        fprintf(stderr, "ERROR: Magic bytes mismatched in deserialize file. \n");
        return -1; // Magic bytes mismatch
    }

    // STEP 2: check that the file type is FILE_DATA 5
    unsigned char type = debug_getchar();
    if (type != 5 || type == -1 || type == EOF) {
        fprintf(stderr, "ERROR: Type is not 5 FILE DATA as expected in deserialize file. \n");
        return -1;
    }

    // STEP 3: make sure the depth matches (uint32_t)
    int depth_parsed = 0;
    for (int i = 0; i < 4; i++) {
        int byte = debug_getchar();
        if (byte == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF character when reading depth of file. \n");
            return -1;
        }
        depth_parsed = (depth_parsed << 8) | byte;
    }
    if (depth_parsed != depth) {
        fprintf(stderr, "ERROR: Depth parsed is not as expected. \n");
        return -1;
    }

    // STEP 4: parse out the file size
    // check matching the size (uint64_t)
    uint64_t file_size = 0; // size should be 16
    for (int i = 0; i < 8; i++) {
        int byte = debug_getchar();
        if (byte == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF character when reading file size. \n");
            return -1;
        }
        file_size = (file_size << 8) | byte;
    }
    file_size = file_size - 16; // remove the constant size of the header from the file size

    FILE *file = fopen(path_buf, "wb"); // truncates file and clears the contents
    if (!file || file == NULL) {
        fprintf(stderr, "ERROR: Not a file or file is null in deserialize file. \n");
        return -1;
    }
    // Write file contents byte-by-byte
    for (unsigned int i = 0; i < file_size; i++) {
        int c = debug_getchar();
        if (c == EOF) {
            fclose(file);
            fprintf(stderr, "ERROR: Unexpected EOF character when attempting to write file contents byte by byte in deserialize file. \n");
            return -1; // EOF or error during read
        }
        if (fputc(c, file) == EOF) {
            fclose(file);
            fprintf(stderr, "ERROR: Reached the EOF when writing file.  \n");
            return -1; // Error during write
        }
    }

    if (fclose(file) == -1) {
        fprintf(stderr, "ERROR: File failed to close in deserialize file. \n");
        return -1;
    }
    return 0; // Success
}

/*
 * @brief  Serialize the contents of a directory as a sequence of records written
 * to the standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory to be serialized.  It serializes the contents of that directory as a
 * sequence of records that begins with a START_OF_DIRECTORY record, ends with an
 * END_OF_DIRECTORY record, and with the intervening records all of type DIRECTORY_ENTRY.
 *
 * @param depth  The value of the depth field that is expected to occur in the
 * START_OF_DIRECTORY, DIRECTORY_ENTRY, and END_OF_DIRECTORY records processed.
 * Note that this depth pertains only to the "top-level" records in the sequence:
 * DIRECTORY_ENTRY records may be recursively followed by similar sequence of
 * records describing sub-directories at a greater depth.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open files, failure to traverse directories, and I/O errors
 * that occur while reading file content and writing to standard output.
 */
int serialize_directory(int depth) {
    // Write the START OF DIRECTORY
    // **************WRITE THE DIRECTORY START*****************
    // WRITE THE MAGIC BYTES
    if (fputc(0x0C, stdout) == EOF) {
        fprintf(stderr, "ERROR: Reached unexpected EOF when writing magic byte 1 when writing START OF DIRECTORY.\n");
        return -1; // Magic byte 1
    }
    if (fputc(0x0D, stdout) == EOF) {
        fprintf(stderr, "ERROR: Reached unexpected EOF when writing magic byte 2 when writing START OF DIRECTORY.\n");
        return -1; // Magic byte 2
    }
    if (fputc(0xED, stdout) == EOF) {
        fprintf(stderr, "ERROR: Reached unexpected EOF when writing magic byte 3 when writing START OF DIRECTORY. \n");
        return -1; // Magic byte 3
    }

    // WRITE THE TYPE
    unsigned char start_directory_type = 2; // START_OF_DIRECTORY = 2
    if (fputc(start_directory_type, stdout) == EOF) {
        fprintf(stderr, "ERROR: Reached unexpected EOF when writing type of START OF DIRECTORY. \n");
        return -1;
    }

    // WRITE THE DEPTH: 4 bytes
    uint32_t node_depth = (uint32_t)depth;
    for (int i = 3; i >= 0; i--) {
        if (fputc((node_depth >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Reached unexpected EOF when writing depth of START OF DIRECTORY. \n");
            return -1;
        }
    }

    // WRITE THE SIZE: 8 bytes
    uint64_t header_size = 16;
    for (int i = 7; i >=0; i--) {
        if (fputc((header_size >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Reached unexpected EOF when writing the size of START OF DIRECTORY. \n");
            return -1;
        }
    }

    // Open the directory
    struct dirent *de;
    DIR *dir = opendir(path_buf);
    if (!dir || dir == NULL) { // If the directory is null
        fprintf(stderr, "ERROR: Failed to open directory because not a directory or directory is null. \n");
        return -1; // Failed to open directory
    }

    //*************LOGIC FOR SERIALIZING************************
    struct stat stat_buf; // Hold the metadata and follow other implementation outline notes as provided by assignment
    while ((de = readdir(dir)) != NULL) { // Iterate over directory entries
        // Skip "." and ".."
        if (*(de->d_name) == '.' && (*(de->d_name + 1) == '\0' || (*(de->d_name + 1) == '.' && *(de->d_name + 2) == '\0'))) {
            continue;
        }

        // Push directory entry name into path_buf using pointer arithmetic
        if (path_push(de->d_name) == -1) {
            if (closedir(dir) == -1) {
                fprintf(stderr, "ERROR: Directory failed to close. \n");
                return -1;
            }
            fprintf(stderr, "ERROR: Failed to push component onto path_buf. \n");
            return -1; // Failed to append to path_buf
        }

        if (stat(path_buf, &stat_buf) == -1) {
            if (path_pop() == -1) {
                fprintf(stderr, "ERROR: Failed to pop commponent off path_buf. \n");
                return -1; // Failed to restore path_buf
            }// Restore path_buf
            if (closedir(dir) == -1) {
                fprintf(stderr, "ERROR: Directory failed to close. \n");
                return -1;
            }
            fprintf(stderr, "ERROR: Failed to retrieve metadata of component. \n");
            return -1; // Failed to retrieve metadata
        }

        // %%%%%%%%%%%RECURISVELY SERIALIZE FILE OR DIRECTORY%%%%%%%%%%%%%%%%%
        if (S_ISDIR(stat_buf.st_mode)) {
            // ^^^^^^^^^^^^^^WRITE RECORDS DIRECTORY ENTRY^^^^^^^^^^^^^
            // WRITE THE MAGIC BYTES - always the same for every entry
            if (fputc(0x0C, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 1 when writing DIRECTORY ENTRY - directory. \n");
                return -1; // Magic byte 1
            }
            if (fputc(0x0D, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 2 when writing DIRECTORY ENTRY - directory. \n");
                return -1; // Magic byte 2
            }
            if (fputc(0xED, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 3 when writing DIRECTORY ENTRY - directory. \n");
                return -1; // Magic byte 3
            }

            // WRITE THE TYPE
            unsigned char directory_entry_type = 4; // DIRECTORY_ENTRY = 4
            if (fputc(directory_entry_type, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing DIRECTORY ENTRY type 4 \n");
                return -1;
            }

            // WRITE THE DEPTH: 4 bytes
            for (int i = 3; i >= 0; i--) {
                if (fputc(((uint32_t) depth >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing depth. \n");
                    return -1;
                }
            }

            int name_length = len_string(de->d_name);
            uint64_t entry_size = name_length + 16 + 12; // REMEMBER the header is given as a constant size of 16 and the metadata collected above is given as a constant size of 12
            // WRITE THE SIZE: 8 bytes
            for (int i = 7; i >=0; i--) {
                if (fputc((entry_size >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing  size. \n");
                    return -1;
                }
            }

            // WRITE THE METADATA: 12 bytes
            // Serialize the mode (file type and permissions) using masks
            uint32_t mode = (stat_buf.st_mode & (S_IFMT | S_IRWXU | S_IRWXG | S_IRWXO));
            for (int i = 3; i >= 0; i--) {
                if (fputc((mode >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing metadata of type and permissions. \n");
                    return -1;
                }
            }

            // Serialize the file size (st_size) as 64-bit value
            for (int i = 7; i >= 0; i--) {
                if (fputc(((uint64_t)stat_buf.st_size >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing size. \n");
                    return -1;
                }
            }

            // WRITE THE NAME
            for (int i = 0; i < len_string(de->d_name); i++) {
                if (fputc(*(de->d_name + i), stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing component name. \n");
                    return -1;
                }
            }

            if (serialize_directory(depth + 1) == -1) { // IF directoryy, then increment the depth because deeper
                if (path_pop() == -1) {
                    fprintf(stderr, "ERROR: Failed to pop component off path_buf. \n");
                    return -1; // Failed to restore path_buf
                }
                if (closedir(dir) == -1) {
                    fprintf(stderr, "ERROR: Failed to close directory. \n");
                    return -1;
                }
                return -1; // Failed to serialize sub-directory
            }
        } else if (S_ISREG(stat_buf.st_mode)) {
            // ^^^^^^^^^^^^^^WRITE RECORDS DIRECTORY ENTRY^^^^^^^^^^^^^ (writing for file too, because records directory regardless of what parsed)
            // Independent of if file or directory, record the current directory entry
            // WRITE THE MAGIC BYTES - always the same for every entry
            if (fputc(0x0C, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 1 when writing DIRECTORRY ENTRY - file.\n");
                return -1; // Magic byte 1
            }
            if (fputc(0x0D, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 2 when writing DIRECTORY ENTRY - file. \n");
                return -1; // Magic byte 2
            }
            if (fputc(0xED, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 3 when writing DIRECTORY ENTRY - file. \n");
                return -1; // Magic byte 3
            }

            // WRITE THE TYPE
            unsigned char directory_entry_type = 4; // DIRECTORY_ENTRY = 4
            if (fputc(directory_entry_type, stdout) == EOF) {
                fprintf(stderr, "ERROR: Unexpected EOF when writing type of DIRECTORY ENTRY 4. \n");
                return -1;
            }

            // WRITE THE DEPTH: 4 bytes
            for (int i = 3; i >= 0; i--) {
                if (fputc(((uint32_t) depth >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing depth. \n");
                    return -1;
                }
            }

            int name_length = len_string(de->d_name);
            uint64_t entry_size = name_length + 16 + 12; // REMEMBER the header is given as a constant size of 16 and the metadata collected above is given as a constant size of 12
            // WRITE THE SIZE: 8 bytes
            for (int i = 7; i >=0; i--) {
                if (fputc((entry_size >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing. \n");
                    return -1;
                }
            }

            // WRITE THE METADATA: 12 bytes
            // Serialize the mode (file type and permissions) using masks
            uint32_t mode = (stat_buf.st_mode & (S_IFMT | S_IRWXU | S_IRWXG | S_IRWXO));
            for (int i = 3; i >= 0; i--) {
                if (fputc((mode >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing metadata file type and permissions. \n");
                    return -1;
                }
            }

            // Serialize the file size (st_size) as 64-bit value
            for (int i = 7; i >= 0; i--) {
                if (fputc(((uint64_t)stat_buf.st_size >> (i * 8)) & 0xFF, stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing size. \n");
                    return -1;
                }
            }

            // WRITE THE NAME
            // write each character of the name
            for (int i = 0; i < len_string(de->d_name); i++) {
                if (fputc(*(de->d_name + i), stdout) == EOF) {
                    fprintf(stderr, "ERROR: Unexpected EOF when writing component name. \n");
                    return -1;
                }
            }

            // Serialize regular file
            if (serialize_file(depth, stat_buf.st_size) == -1) { // ELSE IF file then keep deth the same because searching at the same level
                if (path_pop() == -1) {
                    fprintf(stderr, "ERROR: Failed to pop component off path_buf. \n");
                    return -1; // Failed to restore path_buf
                }
                if (closedir(dir) == -1) {
                    fprintf(stderr, "ERROR: Failed to close directory. \n");
                    return -1;
                }
                return -1; // Failed to serialize file
            }
        } else { // Unkown type, not a file or a directory
            fprintf(stderr, "ERROR: Unknown type not a file or a directory.\n");
            return -1;
        }

        // Pop directory entry name from path_buf using pointer arithmetic
        if (path_pop() == -1) {
            fprintf(stderr, "ERROR: Failed to pop component off path_buf. \n");
            return -1; // Failed to restore path_buf
        }
    }

    // Close the directory
    if (closedir(dir) == -1) {
        fprintf(stderr, "ERROR: Failed to close directory. \n");
        return -1;
    }

    // Write the END OF DIRECTORY
    //******************WRITE THE RECORD DIRECTORY END**********************
    // WRITE THE MAGIC BYTES
    if (fputc(0x0C, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 1 of END OF DIRECTORY. \n");
        return -1; // Magic byte 1
    }
    if (fputc(0x0D, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 2 of END OF DIRECTORY. \n");
        return -1; // Magic byte 2
    }
    if (fputc(0xED, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF when writing magic byte 3 of END OF DIRECTORY. \n");
        return -1; // Magic byte 3
    }

    // WRITE THE TYPE
    unsigned char end_directory_type = 3; // END_OF_DIRECTORY = 3
    if (fputc(end_directory_type, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF when writing type 3 of END OF DIRECTORY. \n");
        return -1;
    }

    // WRITE THE DEPTH: 4 bytes
    for (int i = 3; i >= 0; i--) {
        if (fputc((node_depth >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing depth. \n");
            return -1;
        }
    }

    // WRITE THE SIZE: 8 bytes
    for (int i = 7; i >=0; i--) {
        if (fputc((header_size >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing size. \n");
            return -1;
        }
    }

    return 0; // Success
}

/*
 * @brief  Serialize the contents of a file as a single record written to the
 * standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * file to be serialized.  It serializes the contents of that file as a single
 * FILE_DATA record emitted to the standard output.
 *
 * @param depth  The value to be used in the depth field of the FILE_DATA record.
 * @param size  The number of bytes of data in the file to be serialized.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open the file, too many or not enough data bytes read
 * from the file, and I/O errors reading the file data or writing to standard output.
 */
int serialize_file(int depth, off_t size) {
    if (size == -1) { // Invalid write size
        fprintf(stderr, "ERROR: Invalid negative write size. \n");
        return -1; // Can't have a negative write size
    }

    // WRITE THE MAGIC BYTES - always the same for every entry
    if (fputc(0x0C, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF writing magic byte 1. \n");
        return -1; // Magic byte 1
    }
    if (fputc(0x0D, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF writing magic byte 2. \n");
        return -1; // Magic byte 2
    }
    if (fputc(0xED, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF writing magic byte 3. \n");
        return -1; // Magic byte 3
    }

    // WRITE THE TYPE
    unsigned char file_data_type = 5; // FILE_DATA = 5
    if (fputc(file_data_type, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF writing type 5 FILE DATA. \n");
        return -1;
    }

    // WRITE THE DEPTH: 4 bytes
    for (int i = 3; i >= 0; i--) {
        if (fputc(((uint32_t) depth >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing depth. \n");
            return -1;
        }
    }

    int size_total_file = size + 16; // Header size is given as a constant value
    // WRITE THE SIZE: 8 bytes
    for (int i = 7; i >=0; i--) {
        if (fputc(((uint64_t) size_total_file >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing size. \n");
            return -1;
        }
    }

    //**************PROCESS THE FILE*****************
    // Open the file for reading
    FILE *file = fopen(path_buf, "r");
    if (!file) {
        fprintf(stderr, "ERROR: Failed to open  file, not a file. \n");
        return -1; // Failed to open the file
    }

    // Read file data byte-by-byte and write to standard output
    int c;
    while (size > 0) {
        c = fgetc(file); // Read a single byte from the file
        if (c == EOF) {
            if (ferror(file)) {
                fclose(file);
                fprintf(stderr, "ERROR: I/O error occurred.\n");
                return -1; // I/O error occurred
            }
            fprintf(stderr, "ERROR: Unexpected EOF. \n");
            return -1; // Unexpected EOF
        }
        fputc(c, stdout); // Write the byte to the standard output
        size--;
    }

    if (fclose(file) == -1) {
        fprintf(stderr, "ERROR: Failed to close file. \n");
        return -1; // Close the file
    }

    // Verify that the correct number of bytes has been serialized
    if (size != 0) {
        fprintf(stderr, "ERROR: Not all bytes are read/written. \n");
        return -1; // Not all bytes were read/written
    }
    return 0; // Success
}

/**
 * @brief Serializes a tree of files and directories, writes
 * serialized data to standard output.
 * @details This function assumes path_buf has been initialized with the pathname
 * of a directory whose contents are to be serialized.  It traverses the tree of
 * files and directories contained in this directory (not including the directory
 * itself) and it emits on the standard output a sequence of bytes from which the
 * tree can be reconstructed.  Options that modify the behavior are obtained from
 * the global_options variable.
 *
 * @return 0 if serialization completes without error, -1 if an error occurs.
 */
int serialize() {
    // Write the START OF TRANSMISSION record first
    //**************WRITE RECORD START*********************************
    // WRITE THE MAGIC BYTES
    if (fputc(0x0C, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF magic byte 1 START OF TRANSMISSION. \n");
        return -1; // Magic byte 1
    }
    if (fputc(0x0D, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF magic byte 2 START OF TRANSMISSION. \n");
        return -1; // Magic byte 2
    }
    if (fputc(0xED, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF magic byte 3 START OF TRANSMISSION. \n");
        return -1; // Magic byte 3
    }

    // WRITE THE TYPE
    unsigned char start_record_type = 0; // START_OF_TRANSMISSION
    if (fputc(start_record_type, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF writing START OF TRANSMISSION type.\n");
        return -1;
    }

    // WRITE THE DEPTH: 4 bytes
    uint32_t start_end_depth = 0; // always 0 for the start and end
    for (int i = 3; i >= 0; i--) {
        if (fputc((start_end_depth >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing depth. \n");
            return -1;
        }
    }

    // WRITE THE SIZE: 8 bytes
    uint64_t header_size = 16;
    for (int i = 7; i >=0; i--) {
        if (fputc((header_size >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing size. \n");
            return -1;
        }
    }

    //****************SERIALIZATION BEGIN*******************************************
    // Start serialization of directory contents
    if (serialize_directory(1) == -1) { // in other words it returns an error at -1
        return -1; // Error occurred during serialization
    }

    // Write the END OF TRANSMISSION record last
    //**************WRITE RECORD END*********************************
    // WRITE THE MAGIC BYTES
    if (fputc(0x0C, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF magic byte 1 END OF TRANSMISSION. \n");
        return -1; // Magic byte 1
    }
    if (fputc(0x0D, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF magic byte 2 END OF TRANSMISSION. \n");
        return -1; // Magic byte 2
    }
    if (fputc(0xED, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF magic byte 3 END OF TRANSMISSION. \n");
        return -1; // Magic byte 3
    }

    // WRITE THE TYPE
    unsigned char end_record_type = 1; // END_OF_TRANSMISSION
    if (fputc(end_record_type, stdout) == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF writing END OF TRANSMISSION type.\n");
        return -1;
    }

    // WRITE THE DEPTH: 4 bytes
    for (int i = 3; i >= 0; i--) {
        if (fputc((start_end_depth >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing depth. \n");
            return -1;
        }
    }

    // WRITE THE SIZE: 8 bytes
    for (int i = 7; i >=0; i--) {
        if (fputc((header_size >> (i * 8)) & 0xFF, stdout) == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF writing size. \n");
            return -1;
        }
    }

    return 0; // Success
}

/**
 * @brief Reads serialized data from the standard input and reconstructs from it
 * a tree of files and directories.
 * @details  This function assumes path_buf has been initialized with the pathname
 * of a directory into which a tree of files and directories is to be placed.
 * If the directory does not already exist, it is created.  The function then reads
 * from from the standard input a sequence of bytes that represent a serialized tree
 * of files and directories in the format written by serialize() and it reconstructs
 * the tree within the specified directory.  Options that modify the behavior are
 * obtained from the global_options variable.
 *
 * @return 0 if deserialization completes without error, -1 if an error occurs.
 */
int deserialize() {
    // PROCESS THE START OF TRANSMISSION RECORD FIRST 
    // Step 1: Validate the magic sequence: magic byte 1=0x0C, 2=0x0D, 3=0xED
    unsigned char magic1_b = debug_getchar(); // first 3 bytes
    unsigned char magic2_b = debug_getchar();
    unsigned char magic3_b = debug_getchar();
    if (magic1_b == EOF || magic2_b == EOF || magic3_b == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF reading magic bytes. \n");
        return -1; // Check for EOF
    }
    if (magic1_b != 0x0C || magic2_b != 0x0D || magic3_b != 0xED) {
        fprintf(stderr, "ERROR: Magic bytes not as expected. \n");
        return -1; // Magic bytes mismatch
    }

    // STEP 2: check that the type is START_OF_TRANSMISSION
    unsigned char type_b = debug_getchar();  // 4th byte
    if (type_b != 0 || type_b == -1 || type_b == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF reading type of START OF TRANSMISSION. \n");
        return -1;
    }
    // At the start, start of transmission must be 0 type as defined by assignment

    // Step 3: Validate the depth (32-bit unsigned integer in big-endian)
    int depth = 0; // depth should be at 0 - next 4 bytes
    for (int i = 0; i < 4; i++) {
        int byte = debug_getchar();
        if (byte == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF reading depth. \n");
            return -1;
        }
        depth = (depth << 8) | byte;
    }
    if (depth != 0) {
        fprintf(stderr, "ERROR: Depth is not 0. \n");
        return -1;
    }

    // Step 4: Validate the size (32-bit unsigned integer in big-endian)
    int size = 0; // size should be 16 - next 8 bytes
    for (int i = 0; i < 8; i++) {
        int byte = debug_getchar();
        if (byte == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF reading size. \n");
            return -1;
        }
        size = (size << 8) | byte;
    }
    if (size != 16) {
        fprintf(stderr, "ERROR: Size is not 16. \n");
        return -1;
    }

    //****************************DESERIALIZATION BEGIN*************************************************
    // Start deserialization of directory contents
    if (deserialize_directory(1) != 0) { // DOUBLE CHECK if path_buf is considered as outside of the directory strucutre
        return -1; // Error occurred during deserialization (any other value that should be expected other than 0 is -1)
    }

    // Process the END OF TRANSMISSION record last
    // Step 1: Validate the magic sequence: magic byte 1=0x0C, 2=0x0D, 3=0xED
    unsigned char magic1_f = debug_getchar();
    unsigned char magic2_f = debug_getchar();
    unsigned char magic3_f = debug_getchar();
    if (magic1_f == EOF || magic2_f == EOF || magic3_f == EOF) {
        fprintf(stderr, "ERROR: Unexpected EOF reading magic bytes. \n");
        return -1; // Check for EOF
    }
    if (magic1_f != 0x0C || magic2_f != 0x0D || magic3_f != 0xED) {
        fprintf(stderr, "ERROR: Magic bytes not as expected. \n");
        return -1; // Magic bytes mismatch
    }

    // STEP 2: check that the file type is END_OF_TRANSMISSION
    unsigned char type_f = debug_getchar();
    if (type_f != 1 || type_f == -1 || type_f == EOF) {
        fprintf(stderr, "ERROR: Invalid type for END OF TRANSMISSION \n");
        return -1;
    }
    // At the end, end of transmission must be 1 type as defined by assignment

    // Step 3: Validate the depth (32-bit unsigned integer in big-endian)
    depth = 0; // depth should be at 0
    for (int i = 0; i < 4; i++) {
        int byte = debug_getchar();
        if (byte == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF reading depth.\n");
            return -1;
        }
        depth = (depth << 8) | byte;
    }
    if (depth != 0) {
        fprintf(stderr, "ERROR: Depth is not 0 \n");
        return -1;
    }

    // Step 4: Validate the size (32-bit unsigned integer in big-endian)
    size = 0; // size should be 16
    for (int i = 0; i < 8; i++) {
        int byte = debug_getchar();
        if (byte == EOF) {
            fprintf(stderr, "ERROR: Unexpected EOF reading size \n");
            return -1;
        }
        size = (size << 8) | byte;
    }
    if (size != 16) {
        fprintf(stderr, "ERROR: Size does not equal 16. \n");
        return -1;
    }

    return 0; // Success
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv) {
    // Initialize the provided global options to 0 (just in case)
    global_options = 0x0; // Bit representation

    // Check if no flag arguments are provided
    if (argc < 2 || argc == 1) { // argc always at least 1, because at index 1 of argv is the name of the program
        fprintf(stderr, "ERROR: No arguments were passed on command line. \n");
        return -1; // Must provide arguments in the command line
    }

    // Pointer to traverse the arguments - no array indexing only pointer arithmetic
    char *arg; // Does this have to be initialized to something? or can just declare?

    // Declare the flags command line arguments parsing
    int s_flag = 0; // Initilize to 0 because compiler not consistent behavior across
    int d_flag = 0; // Only global variables are initialized to 0x0 or null
    int c_flag = 0;
    int p_flag = 0;
    int pos_flag_found = 0; // Track if s or d flag has been found because only can have one
    char *dir_path = "."; // Default directory is the current working directory if the p flag is not specified
    // Pointers must be initialized before first reference

    // Traverse the arguments using pointer arithmetic and pointers
    for (char **current_arg = argv + 1; current_arg < argv + argc; current_arg++) {
        arg = *current_arg; // Initializing the pointer

        // Check for the -h flag
        if (*arg == '-' && *(arg + 1) == 'h' && *(arg + 2) == '\0') {
            if (current_arg != argv + 1) { // -h flag must be the first positional argument after the program name
                fprintf(stderr, "ERROR: -h flag must be the first positional argument after the program name. \n");
                return -1;
            }
            global_options |= 1 << 0; // Set the help flag
            // If the h flag is provided, it is the first positional argument after the program name and any other arguments that follow are ignored
            return 0; // Return success validation
        }

        // check for the s flag
        else if (*arg == '-' && *(arg + 1) == 's' && *(arg + 2) == '\0') {
            if  (s_flag) {
                fprintf(stderr, "ERROR: Duplicate -s flags passed and not as path attached to -p. \n");
                return -1; // Cannot have two -s flags
            }
            if (pos_flag_found) {
                fprintf(stderr, "ERROR: Cannot have both -s and -d flags. \n");
                return -1; // Cannot have both -s and -d flag
            }
            s_flag = 1;
            global_options |= 1 << 1; // Set the serialization flag
            pos_flag_found = 1;
            continue;
        }

        // check for the d flag
        else if (*arg == '-' && *(arg + 1) == 'd' && *(arg + 2) == '\0') {
            if  (d_flag) {
                fprintf(stderr, "ERROR: Duplicate -d flags passed and not as path attached to -p. \n");
                return -1; // Cannot have two -d flags
            }
            if (pos_flag_found) {
                fprintf(stderr, "ERROR: Cannon have both -s and -d flags. \n");
                return -1; // Cannot have both -s and -d flag
            }
            d_flag = 1;
            global_options |= 1 << 2; // Set the deserialization flag
            pos_flag_found = 1;  // Marks that a positional argument was found
            continue;
        }

        // Check for at least one positional argument found (already checked for h flag)
        else if (!pos_flag_found) {
            fprintf(stderr, "ERROR: Must have at one positional argument exactly before optional flags. \n");
            return -1; // Must have a positional argument before options
        }

        // Check for optional arguments after positional 
        else if (*arg == '-' && *(arg + 1) == 'c' && *(arg + 2) == '\0') {
            if (!d_flag) { // c flag can only be with deserialization
                fprintf(stderr, "ERROR: clobber -c flag can only be passed when -d flag is also passed. \n");
                return -1;
            }
            c_flag = 1;
            global_options |= 1 << 3;
            continue;
        }

        // Check for -p flag
        else if (*arg == '-' && *(arg + 1) == 'p' && *(arg + 2) == '\0') {
            // Ensure that -p is followed by a valid directory path
            if (current_arg + 1 >= argv + argc) {
                fprintf(stderr, "ERROR: A directory path must follow immediately after the -p flag. \n");
                return -1; // Missing directory path after -p
            }
            dir_path = *(++current_arg); // Move to the next argument for the path - the path can be a flag as specified by piazza
            p_flag = 1;
            // Copy the directory path into name_buf
            char *buff_ptr = name_buf;
            while (*dir_path != '\0' && buff_ptr < name_buf + NAME_MAX - 1) {
                *buff_ptr++ = *dir_path++;
            }
            *buff_ptr = '\0'; // Null-terminate the path
        }

        else {
            fprintf(stderr, "ERROR: Unkown flag or disallowed argument was passed on command line. \n");
            return -1; 
        }
    }

    // If -p flag is not passed as optional argument set path_buf to work from current working directory
    if (!p_flag) {
        dir_path = ".";
        // Copy the directory path into name_buf as the current working directory
        char *buff_ptr = name_buf;
        while(*dir_path != '\0' && buff_ptr < name_buf + NAME_MAX - 1) {
            *buff_ptr++ = *dir_path++;
        }
        *buff_ptr = '\0';
    }

    // Extra error handling of cases
    // Ensure exactly one of -s or -d was specified
    if (!s_flag && !d_flag) {
        fprintf(stderr, "ERROR: Neither -s or -d flag was passed. \n");
        return -1; // Error: Neither -s nor -d was specified
    }
    if (c_flag && !d_flag) {
        fprintf(stderr, "ERROR: -c flag must only be set with -d flag. \n");
        return -1; // c flag must be used with d flag
    }

    return 0; // Validation successful
}

/*
0000000 0c 0d ed 00 00 00 00 00 00 00 00 00 00 00 00 10
0000020 0c 0d ed 02 00 00 00 01 00 00 00 00 00 00 00 10
0000040 0c 0d ed 04 00 00 00 01 00 00 00 00 00 00 00 1f
0000060 00 00 41 fd 00 00 00 00 00 00 10 00 64 69 72 0c
0000100 0d ed 02 00 00 00 02 00 00 00 00 00 00 00 10 0c
0000120 0d ed 04 00 00 00 02 00 00 00 00 00 00 00 23 00
0000140 00 81 b4 00 00 00 00 00 00 00 09 67 6f 6f 64 62
0000160 79 65 0c 0d ed 05 00 00 00 02 00 00 00 00 00 00
0000200 00 19 47 6f 6f 64 62 79 65 21 0a 0c 0d ed 04 00
0000220 00 00 02 00 00 00 00 00 00 00 22 00 00 81 b4 00
0000240 00 00 00 00 00 00 07 68 65 6c 6c 6f 31 0c 0d ed
0000260 05 00 00 00 02 00 00 00 00 00 00 00 17 48 65 6c
0000300 6c 6f 31 0a 0c 0d ed 03 00 00 00 02 00 00 00 00
0000320 00 00 00 10 0c 0d ed 04 00 00 00 01 00 00 00 00
0000340 00 00 00 21 00 00 81 b4 00 00 00 00 00 00 00 06
0000360 68 65 6c 6c 6f 0c 0d ed 05 00 00 00 01 00 00 00
0000400 00 00 00 00 16 48 65 6c 6c 6f 0a 0c 0d ed 03 00
0000420 00 00 01 00 00 00 00 00 00 00 10 0c 0d ed 01 00
0000440 00 00 00 00 00 00 00 00 00 00 10
0000453
*/