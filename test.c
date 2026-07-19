#include <stdio.h>
#include <stdlib.h>

#include "./yaml.h"

int main(void)
{
    YAML *yaml = YAMLFromFile("./examples/simple-but-larger.yaml");

    YAMLPrint(yaml);
    YAMLFree(yaml);

    return EXIT_SUCCESS;
}
