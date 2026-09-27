#include <standardloop/testing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

#define ASSERT_TOKEN(expected_type, msg) \
    token = YAMLLex(lexer);              \
    TestCaseVerify(true, msg, token != NULL && token->type == expected_type)

#define ASSERT_SCALAR(expected_val, msg)                              \
    token = YAMLLex(lexer);                                           \
    TestCaseVerify(true, msg,                                         \
                   token != NULL && token->type == YAMLTokenScalar && \
                       token->literal != NULL &&                      \
                       strcmp(token->literal, expected_val) == 0)

static void testOnlyEmpty(void)
{
    FILE *file_ptr = fopen("./testfiles/only/empty.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenEOF, "Ensure empty file only has eof token");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyDocStart(void)
{
    FILE *file_ptr = fopen("./testfiles/only/doc-start.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartOfDocument, "Ensure first token is doc start");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure next token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyDocEnd(void)
{
    FILE *file_ptr = fopen("./testfiles/only/doc-end.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenEndOfDocument, "Ensure first token is doc end");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure next token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyListFlow(void)
{
    FILE *file_ptr = fopen("./testfiles/only/list-flow.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "Ensure token is start flow");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token scalar");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "Ensure token is end flow");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyListNormal(void)
{
    FILE *file_ptr = fopen("./testfiles/only/list-normal.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenListDash, "Ensure token is dash");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token scalar");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyMapNormal(void)
{
    FILE *file_ptr = fopen("./testfiles/only/map-normal.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "Ensure token value indicator");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyMapFlow(void)
{
    FILE *file_ptr = fopen("./testfiles/only/map-flow.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "Ensure token is flowstart");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "Ensure token value indicator");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "Ensure token is flow end");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyString(void)
{
    FILE *file_ptr = fopen("./testfiles/only/string.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenScalar, "Ensure first token scalar");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure next token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnlyNull(void)
{
    FILE *file_ptr = fopen("./testfiles/only/null.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenScalar, "Ensure first token scalar");
    ASSERT_TOKEN(YAMLTokenEOF, "Ensure next token is eof");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testOnly(void)
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

static void testMultiDocumentAndFlowContainers(void)
{
    FILE *file_ptr = fopen("./testfiles/multi-doc-flow.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    // --- Document 1 Start ---
    ASSERT_TOKEN(YAMLTokenStartOfDocument, "1. Start of Document (---)");
    ASSERT_TOKEN(YAMLTokenNewline, "2. Newline after doc start");

    // server:
    ASSERT_SCALAR("server", "3. Scalar 'server'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "4. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "5. Newline after server:");
    ASSERT_TOKEN(YAMLTokenIndent, "6. Indent under server");

    // url: http://localhost:8080/path
    ASSERT_SCALAR("url", "7. Scalar 'url'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "8. Value indicator (:)");
    ASSERT_SCALAR("http://localhost:8080/path",
                  "9. Scalar URL with embedded colon");
    ASSERT_TOKEN(YAMLTokenNewline, "10. Newline after url");

    // active: true
    ASSERT_SCALAR("active", "11. Scalar 'active'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "12. Value indicator (:)");
    ASSERT_SCALAR("true", "13. Scalar 'true'");
    ASSERT_TOKEN(YAMLTokenNewline, "14. Newline after active");

    // empty_containers: [ {}, [] ]
    ASSERT_SCALAR("empty_containers", "15. Scalar 'empty_containers'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "16. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart,
                 "17. Outer flow sequence start ([)");
    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "18. Empty flow mapping start ({)");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "19. Empty flow mapping end (})");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "20. Flow entry comma (,)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart,
                 "21. Empty flow sequence start ([)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "22. Empty flow sequence end (])");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "23. Outer flow sequence end (])");
    ASSERT_TOKEN(YAMLTokenNewline, "24. Newline after flow sequence");

    // Document 1 End
    ASSERT_TOKEN(YAMLTokenDedent, "25. Dedent before doc end");
    ASSERT_TOKEN(YAMLTokenEndOfDocument, "26. End of Document (...)");
    ASSERT_TOKEN(YAMLTokenNewline, "27. Newline after doc end");

    // --- Document 2 Start ---
    ASSERT_TOKEN(YAMLTokenStartOfDocument, "28. Start of Document (---)");
    ASSERT_TOKEN(YAMLTokenNewline, "29. Newline after doc start");

    // - { 'item': "quoted \"val\"", status: ok }
    ASSERT_TOKEN(YAMLTokenListDash, "30. List dash (-)");
    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "31. Flow mapping start ({)");
    ASSERT_SCALAR("item", "32. Single-quoted scalar 'item'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "33. Value indicator (:)");
    ASSERT_SCALAR("quoted \"val\"", "34. Double-quoted unescaped scalar");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "35. Flow entry comma (,)");
    ASSERT_SCALAR("status", "36. Unquoted key scalar 'status'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "37. Value indicator (:)");
    ASSERT_SCALAR("ok", "38. Unquoted value scalar 'ok'");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "39. Flow mapping end (})");
    ASSERT_TOKEN(YAMLTokenNewline, "40. Newline after flow mapping");

    // - [ unquoted string, 100 ]
    ASSERT_TOKEN(YAMLTokenListDash, "41. List dash (-)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "42. Flow sequence start ([)");
    ASSERT_SCALAR("unquoted string", "43. Plain scalar with space");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "44. Flow entry comma (,)");
    ASSERT_SCALAR("100", "45. Plain scalar '100'");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "46. Flow sequence end (])");
    ASSERT_TOKEN(YAMLTokenNewline, "47. Newline after flow sequence");

    // EOF
    ASSERT_TOKEN(YAMLTokenEOF, "48. EOF reached");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

extern void TestLexer(void)
{
    testOnly();
    testMultiDocumentAndFlowContainers();
}
