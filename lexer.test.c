#include <standardloop/testing.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

extern void TestLexer()
{
    YAMLLexerDebugTest("./testfiles/playground.yaml");
}
