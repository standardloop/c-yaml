#include <standardloop/testing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

extern void TestParser()
{
    // FILE *file_ptr = fopen("./testfiles/playground.yaml", "rb");
    // TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    // if (!file_ptr)
    // {
    //     return;
    // }

    char *input_str = "---";

    YAMLParser *parser = YAMLParserInit(InputModeString, input_str);
    YAMLParserDebugTest(parser);
}
