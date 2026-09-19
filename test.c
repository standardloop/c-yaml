#include <standardloop/testing.h>
#include <stdlib.h>

#include "./yaml.h"

int main(void)
{
    FILE *file_ptr = NULL;
    file_ptr = fopen("./examples/playground.yaml", "rb");
    YAML *yaml = YAMLFromFile(file_ptr, LEXER_DEFAULT_BUFFER_SIZE);

    YAMLFree(yaml);
    // TestingInit();
    // TestLexer();
    // TestYaml();
    // TestingTearDown();

    return EXIT_SUCCESS;
}
