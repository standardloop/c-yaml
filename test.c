#include <stdio.h>
#include <stdlib.h>

#include <standardloop/logger.h>
#include "./yaml.h"

int main(void)
{
    InitLogger(TRACE, STANDARD_FMT, false, true, true, true);
    // Log(TRACE, "HI");
    YAML *yaml = YAMLFromFile("./examples/simple.yaml");
    // Log(FATAL, "foo");

    YAMLPrint(yaml);
    YAMLFree(yaml);

    return EXIT_SUCCESS;
}
