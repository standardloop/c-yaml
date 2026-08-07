#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <standardloop/logger.h>
#include <standardloop/util.h>
#include "./yaml.h"

int main(int argc, char **argv)
{
    InitLogger(TRACE, STANDARD_FMT, false, true, true, true);

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
            buffer_size = (size_t)atoi(argv[2]); // TODO, maybe use strtoull instead?
            if (buffer_size < LEXER_MIN_BUFFER_SIZE)
            {
                Log(FATAL, "buffer_size needs to be greater than or equal to %d", (int)LEXER_MIN_BUFFER_SIZE);
            }
            else if (buffer_size >= LEXER_MAX_BUFFER_SIZE)
            {
                Log(FATAL, "buffer_size needs to be less than %d", (int)LEXER_MAX_BUFFER_SIZE);
            }
        }
        Log(DEBUG, "filename: %s", filename);
        Log(DEBUG, "buffer_size: %d", (int)buffer_size);

        file_ptr = fopen(filename, "rb");
    }
    else
    {
        // Log(FATAL, "foo");
        file_ptr = stdin;
        Log(DEBUG, "expecting piped data from stdin");
    }
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
