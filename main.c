#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./yaml.h"
#include <standardloop/logger.h>
#include <standardloop/util.h>

static bool isBufferSizeOkay(size_t);

static bool isBufferSizeOkay(size_t buffer_size)
{
    if (buffer_size < LEXER_MIN_BUFFER_SIZE)
    {
        Log(ERROR, "buffer_size needs to be greater than or equal to %d",
            (int)LEXER_MIN_BUFFER_SIZE);
        return false;
    }
    else if (buffer_size >= LEXER_MAX_BUFFER_SIZE)
    {
        Log(ERROR, "buffer_size needs to be less than %d",
            (int)LEXER_MAX_BUFFER_SIZE);
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    InitLoggerEasy(TRACE);

    FILE *file_ptr = NULL;
    size_t buffer_size = LEXER_DEFAULT_BUFFER_SIZE;
    if (isatty(fileno(stdin)))
    {
        if (argc == 1)
        {
            Log(FATAL, "need an arg for filename");
        }
        char *filename = argv[1];
        if (argc >= 3)
        {
            size_t buffer_size_from_argv =
                (size_t)atoi(argv[2]); // TODO, maybe use strtoull instead?
            if (isBufferSizeOkay(buffer_size_from_argv))
            {
                buffer_size = buffer_size_from_argv;
            }
            else
            {
                Log(ERROR, "invalid buffer size, will use default");
            }
        }
        Log(DEBUG, "filename: %s", filename);
        file_ptr = fopen(filename, "rb");
    }
    else
    {
        if (argc >= 2)
        {
            size_t buffer_size_from_argv =
                (size_t)atoi(argv[1]); // TODO, maybe use strtoull instead?
            if (isBufferSizeOkay(buffer_size_from_argv))
            {
                buffer_size = buffer_size_from_argv;
            }
            else
            {
                Log(ERROR, "invalid buffer size, will use default");
            }
        }
        // Log(FATAL, "foo");
        file_ptr = stdin;
        Log(DEBUG, "taking input from stdin");
    }

    Log(DEBUG, "buffer_size: %d", (int)buffer_size);
    YAML *yaml = YAMLFromFile(file_ptr, buffer_size);
    // Log(FATAL, "foo");

    YAMLPrint(yaml);
    YAMLFree(yaml);
    if (file_ptr != stdin)
    {
        fclose(file_ptr);
    }

    // YAMLLexerDebugTest(QuickAllocatedString("---\nfoo: bar\n...\n"));
    // exit(0);
    return EXIT_SUCCESS;
}
