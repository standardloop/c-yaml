#include <standardloop/testing.h>
#include <stdlib.h>

#include "./yaml.h"
#include <standardloop/logger.h>

int main(void)
{
    // InitLoggerEasy(TRACE);
    // Log(TRACE, "starting up");
    // FILE *file_ptr = fopen("./examples/playground.yaml", "rb");
    // Log(TRACE, "opened the file");
    // YAML *yaml = YAMLFromFile(file_ptr, LEXER_DEFAULT_BUFFER_SIZE);
    // Log(TRACE, "got yaml");

    // YAMLFree(yaml);
    TestingInit();
    TestLexer();
    // TestYaml();
    TestingTearDown();

    return EXIT_SUCCESS;
}
