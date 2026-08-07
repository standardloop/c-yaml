#include <stdio.h>
#include <stdlib.h>

#include <standardloop/logger.h>
#include <standardloop/util.h>
#include "./yaml.h"

int main(int argc, char **argv)
{
    InitLogger(TRACE, STANDARD_FMT, false, true, true, true);

    // YAMLLexerDebugTest(QuickAllocatedString("---\nfoo: bar\n...\n"));
    // exit(0);

    if (argc == 1)
    {
        Log(FATAL, "need an arg for filename");
    }
    char *filename = argv[1];
    size_t buffer_size = LEXER_DEFAULT_BUFFER_SIZE;
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

    // Log(TRACE, "HI");
    YAML *yaml = YAMLFromFile(filename, buffer_size);
    // Log(FATAL, "foo");

    YAMLPrint(yaml);
    YAMLFree(yaml);

    return EXIT_SUCCESS;
}
