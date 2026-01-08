#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <arpa/inet.h>

typedef enum AppErrors_e {
    OK = 0,
    BADARGS,
    OOM,
    INFILEIO,
    TOOLONG,
    TOOBIG,
    OUTFILEIO,
} ApplicationError;

#ifdef CTH_BIG_ENDIAN
#define SPRINT_I16 sprint_be
const char* const ENDIANNESS = "big";
#else
#define SPRINT_I16 sprint_le
const char* const ENDIANNESS = "little";
#endif

void sprint_le(int16_t* buffer, int16_t value) {
    int8_t* asBytes = (int8_t*)buffer;
    asBytes[0] = (int8_t)((value & 0x00ff) >> 0);
    asBytes[1] = (int8_t)((value & 0xff00) >> 8);
}

void sprint_be(int16_t* buffer, int16_t value) {
    *buffer = htons(value);
}

ApplicationError process_line(char* inbuf, size_t buflen, int16_t* out) {
    if (buflen < strlen(inbuf)) {
        return TOOLONG;
    }
    
    int temp = atoi(&inbuf[1]); // skip over L/R
    if (temp < INT16_MIN || INT16_MAX < temp) {
        return TOOBIG;
    }

    int16_t value = (int16_t)temp;
    if (inbuf[0] == 'L') {
        value = -value; // I may actually just want to just set the high bit and not actually negate it?
    }

    SPRINT_I16(out, value);
    return OK;
}

// buf: where the output will be written.
// size: size of buf, in bytes
ApplicationError process_file(char* filename, int16_t* buf, size_t size, size_t* bytes_written_out) {
    ApplicationError err = 0;
    *bytes_written_out = 0;
    FILE* infile;
    infile = fopen(filename, "r");
    if (infile == NULL) {
        err = INFILEIO;
        perror("Could not open file.");
        return err; // perror probably doesn't overwrite errno, but I'd have to look it up.
    }

    // we print a terminator at the end. we could also (instead) print the count at the top.
    // the former is easier to check for on the DMG.

    // input matches /[LR]\d{1,5}/ -- honestly the max is probably 3 or 4, but this gives us full int16s.
    char linebuf[8] = {0};
    size_t line_no = 1;
    int16_t *next = buf;
    while(fgets(linebuf, sizeof(linebuf) - 1, infile) != NULL && ((size_t)(next - buf)) < size) {
        //printf("processing line into buf at %p\n", (next));
        err = process_line(linebuf, sizeof(linebuf) - 1, (next));
        ++line_no;
        if (err == TOOLONG) {
            fprintf(stderr, "line %lu exceeds expected length: %s(...)\n", line_no, linebuf);
            goto cleanup_infile;
        } else if (err == TOOBIG) {
            fprintf(stderr, "line %lu exceeds int16 size! %s\n", line_no, linebuf);
        }
        next += 1;
        //printf("next is now %p and bytes_written is %lu  ", next, bytes_written); 
        //printf("buf ptr index: %lu\n", (next - buf));
    }

    err = process_line("R0\n", 3, next);
    next += 1;
    //printf("huh?\n");

cleanup_infile:
    fclose(infile);
    infile = NULL;

    *bytes_written_out = (next - buf);

    return err;
}

int main(int argc, char** argv) {
    int app_err = 0;
    fprintf(stderr, "Compiled with %s endianness.\n", ENDIANNESS);
    if (argc < 2) {
        fprintf(stderr, "Usage: %s INPUTFILE\n", argv[0]);
        return BADARGS;
    }

    // the whole point is to fit in a single DMG rom bank, so it better...
    const size_t OUTBUF_SIZE = 16 * 1024;
    void* outbuf = malloc(OUTBUF_SIZE);
    if (outbuf == NULL) {
        return OOM;
    }
    memset(outbuf, OUTBUF_SIZE, '0');

    size_t bytes_written = 0;
    app_err = process_file(argv[1], (int16_t*)outbuf, OUTBUF_SIZE, &bytes_written);
    if (app_err) goto cleanup_alloc;

    if ((8 * 1024) <= bytes_written && bytes_written < (12 * 1024)) {
        fprintf(stderr, "Caution. The binary file is going to be pretty big... =\\\n");
    } else if ((12 * 1024) <= bytes_written) {
        fprintf(stderr, "Oh no. The binary file is going to be huge... =(\n ");
    }

    if (!isatty(STDOUT_FILENO)) {
        fwrite(outbuf, sizeof(int8_t), bytes_written, stdout);
    } else {
        FILE* outfile = fopen("out.bin", "wb");
        if (outfile == NULL) {
            app_err = OUTFILEIO;
            fprintf(stderr, "Couldn't open out.bin\n");
            goto cleanup_outfile;
        }
        fwrite(outbuf, sizeof(int8_t), bytes_written, outfile);
        printf("Done. See out.bin\n");
cleanup_outfile:
        fclose(outfile);
        outfile = NULL;
    }

cleanup_alloc:
    free(outbuf);
    outbuf = NULL;

    return app_err;
}