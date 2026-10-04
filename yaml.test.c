#include <errno.h>
#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./yaml.h"

extern void TestYaml(void)
{
    YAML *test = YAMLFromFile("./testfiles/only/string.yaml");
    YAMLPrint(test);
    YAMLFree(test);
    return;
}
