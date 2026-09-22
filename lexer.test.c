#include <standardloop/testing.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

extern void TestLexer()
{
    YAMLLexer *lexer = YAMLLexerInit(strdup("---"), 3);
    lexer->is_last_chunk = true;
    TestCaseVerify(true, "", lexer != NULL);
    YAMLToken *token = NULL;
    token = YAMLLex(lexer);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "", token->type == YAMLTokenStartOfDocument);
    YAMLLexerFree(lexer);
}
