#include <standardloop/testing.h>
#include <stdlib.h>

#include "./yaml.h"

int main(void)
{
    TestingInit();
    TestYaml();
    TestingTearDown();
    return EXIT_SUCCESS;
}
