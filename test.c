#include <stdio.h>
#include <stdlib.h>

#include <standardloop/logger.h>
#include "./yaml.h"

int main(int argc, char **argv)
{
    InitLogger(TRACE, STANDARD_FMT, false, true, true, true);
    if (argc == 1)
    {
        Log(FATAL, "need an arg for filename");
    }
    char *filename = argv[1];

    // Log(TRACE, "HI");
    YAML *yaml = YAMLFromFile(filename);
    // Log(FATAL, "foo");

    YAMLPrint(yaml);
    YAMLFree(yaml);

    return EXIT_SUCCESS;
}
