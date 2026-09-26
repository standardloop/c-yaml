#include <standardloop/testing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

static void testEmptyFile()
{
    FILE *file_ptr = fopen("./testfiles/blank.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = YAMLLex(lexer);

    TestCaseVerify(true, "Ensure empty file only has eof token",
                   token->type == YAMLTokenEOF);
    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyDocStart()
{
    FILE *file_ptr = fopen("./testfiles/only-doc-start.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;
    token = YAMLLex(lexer);

    TestCaseVerify(true, "Ensure first token is doc start",
                   token->type == YAMLTokenStartOfDocument);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure next token is eof",
                   token->type == YAMLTokenEOF);
    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

extern void TestLexer()
{
    testEmptyFile();
    testOnlyDocStart();
    // YAMLLexerDebugTest("./testfiles/playground.yaml");
}
