#include <standardloop/testing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

static void testOnlyEmpty()
{
    FILE *file_ptr = fopen("./testfiles/only/empty.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = YAMLLex(lexer);

    TestCaseVerify(true, "Ensure empty file only has eof token",
                   token->type == YAMLTokenEOF);
    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyDocStart()
{
    FILE *file_ptr = fopen("./testfiles/only/doc-start.yaml", "rb");
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

static void testOnlyDocEnd()
{
    FILE *file_ptr = fopen("./testfiles/only/doc-end.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;
    token = YAMLLex(lexer);

    TestCaseVerify(true, "Ensure first token is doc end",
                   token->type == YAMLTokenEndOfDocument);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure next token is eof",
                   token->type == YAMLTokenEOF);
    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyListFlow()
{
    FILE *file_ptr = fopen("./testfiles/only/list-flow.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);

    YAMLToken *token = NULL;

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is start flow",
                   token->type == YAMLTokenFlowSequenceStart);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token scalar", token->type == YAMLTokenScalar);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is end flow",
                   token->type == YAMLTokenFlowSequenceEnd);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is eof", token->type == YAMLTokenEOF);

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyListNormal()
{
    FILE *file_ptr = fopen("./testfiles/only/list-normal.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);

    YAMLToken *token = NULL;

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is dash",
                   token->type == YAMLTokenListDash);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token scalar", token->type == YAMLTokenScalar);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is eof", token->type == YAMLTokenEOF);

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyMapNormal()
{
    FILE *file_ptr = fopen("./testfiles/only/map-normal.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);

    YAMLToken *token = NULL;

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is scalar",
                   token->type == YAMLTokenScalar);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token value indicator",
                   token->type == YAMLTokenValueIndicator);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is scalar",
                   token->type == YAMLTokenScalar);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is eof", token->type == YAMLTokenEOF);

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyMapFlow()
{
    FILE *file_ptr = fopen("./testfiles/only/map-flow.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);

    YAMLToken *token = NULL;

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is flowstart",
                   token->type == YAMLTokenFlowMappingStart);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is scalar",
                   token->type == YAMLTokenScalar);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token value indicator",
                   token->type == YAMLTokenValueIndicator);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is scalar",
                   token->type == YAMLTokenScalar);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is flow end",
                   token->type == YAMLTokenFlowMappingEnd);

    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure token is eof", token->type == YAMLTokenEOF);

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyString()
{
    FILE *file_ptr = fopen("./testfiles/only/string.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;
    token = YAMLLex(lexer);

    TestCaseVerify(true, "Ensure first token scalar",
                   token->type == YAMLTokenScalar);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure next token is eof",
                   token->type == YAMLTokenEOF);
    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyNull()
{
    FILE *file_ptr = fopen("./testfiles/only/null.yaml", "rb");
    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;
    token = YAMLLex(lexer);

    TestCaseVerify(true, "Ensure first token scalar",
                   token->type == YAMLTokenScalar);
    token = YAMLLex(lexer);
    TestCaseVerify(true, "Ensure next token is eof",
                   token->type == YAMLTokenEOF);
    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnly()
{
    testOnlyEmpty();
    testOnlyDocStart();
    testOnlyDocEnd();
    testOnlyListFlow();
    testOnlyListNormal();
    testOnlyMapFlow();
    testOnlyMapNormal();
    testOnlyString();
    testOnlyNull();
}

static void testSimple() {}

extern void TestLexer()
{
    // testEmptyFile();
    // testOnly();
    // YAMLLexerDebugTest("./testfiles/playground.yaml");
    // YAMLLexerDebugTest("./testfiles/only/map-flow.yaml");
    testOnly();
    testSimple();
}
