#include <standardloop/testing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./yaml.h"

#define ASSERT_TOKEN(expected_type, msg) \
    token = YAMLLex(lexer);              \
    TestCaseVerify(true, msg, token != NULL && token->type == expected_type)

#define ASSERT_TOKEN_WITH_VAL(expected_type, expected_val, msg)     \
    token = YAMLLex(lexer);                                         \
    TestCaseVerify(true, msg,                                       \
                   token != NULL && token->type == expected_type && \
                       token->literal != NULL &&                    \
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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure empty file only has eof token");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenStartOfDocument, "Ensure first token is doc start");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure next token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenEndOfDocument, "Ensure first token is doc end");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure next token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "Ensure token is start flow");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token scalar");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "Ensure token is end flow");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenListDash, "Ensure token is dash");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token scalar");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "Ensure token value indicator");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "Ensure token is flowstart");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "Ensure token value indicator");
    ASSERT_TOKEN(YAMLTokenScalar, "Ensure token is scalar");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "Ensure token is flow end");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenScalar, "Ensure first token scalar");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure next token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenScalar, "Ensure first token scalar");
    ASSERT_TOKEN(YAMLTokenEndStream, "Ensure next token is eof");

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

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenStartOfDocument, "1. Start of Document (---)");
    ASSERT_TOKEN(YAMLTokenNewline, "2. Newline after doc start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "server", "3. Scalar 'server'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "4. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "5. Newline after server:");
    ASSERT_TOKEN(YAMLTokenIndent, "6. Indent under server");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "url", "7. Scalar 'url'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "8. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "http://localhost:8080/path",
                          "9. Scalar URL with embedded colon");
    ASSERT_TOKEN(YAMLTokenNewline, "10. Newline after url");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "active", "11. Scalar 'active'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "12. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "true", "13. Scalar 'true'");
    ASSERT_TOKEN(YAMLTokenNewline, "14. Newline after active");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "empty_containers",
                          "15. Scalar 'empty_containers'");
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

    ASSERT_TOKEN(YAMLTokenDedent, "25. Dedent before doc end");
    ASSERT_TOKEN(YAMLTokenEndOfDocument, "26. End of Document (...)");
    ASSERT_TOKEN(YAMLTokenNewline, "27. Newline after doc end");

    ASSERT_TOKEN(YAMLTokenStartOfDocument, "28. Start of Document (---)");
    ASSERT_TOKEN(YAMLTokenNewline, "29. Newline after doc start");

    ASSERT_TOKEN(YAMLTokenListDash, "30. List dash (-)");
    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "31. Flow mapping start ({)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "item",
                          "32. Single-quoted scalar 'item'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "33. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "quoted \"val\"",
                          "34. Double-quoted unescaped scalar");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "35. Flow entry comma (,)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "status",
                          "36. Unquoted key scalar 'status'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "37. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "ok",
                          "38. Unquoted value scalar 'ok'");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "39. Flow mapping end (})");
    ASSERT_TOKEN(YAMLTokenNewline, "40. Newline after flow mapping");

    ASSERT_TOKEN(YAMLTokenListDash, "41. List dash (-)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "42. Flow sequence start ([)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "unquoted string",
                          "43. Plain scalar with space");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "44. Flow entry comma (,)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "100", "45. Plain scalar '100'");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "46. Flow sequence end (])");

    ASSERT_TOKEN(YAMLTokenEndStream, "47. EOF reached");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testMixed(void)
{
    FILE *file_ptr = fopen("./testfiles/mixed.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    // foo: "bar with \"quotes\""
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "foo", "1. Scalar 'foo'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "bar with \"quotes\"",
                          "3. Unescaped double-quoted scalar");
    ASSERT_TOKEN(YAMLTokenNewline, "4. Newline after double quote");

    // fizz:
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "fizz", "5. Scalar 'fizz'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "6. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "7. Newline after fizz:");

    //   - 'buzz ''quote''' (2 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "8. Indent to 2 spaces");
    ASSERT_TOKEN(YAMLTokenListDash, "9. List dash (-)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "buzz 'quote'",
                          "10. Unescaped single-quoted scalar");
    ASSERT_TOKEN(YAMLTokenNewline, "11. Newline after single quote");

    //   - { bazz: qux, quux: [ corge, grault ] } (2 spaces)
    ASSERT_TOKEN(YAMLTokenListDash, "12. List dash (-)");
    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "13. Flow mapping start ({)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "bazz", "14. Key 'bazz'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "15. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "qux", "16. Value 'qux'");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "17. Flow entry comma (,)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "quux", "18. Key 'quux'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "19. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "20. Flow sequence start ([)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "corge",
                          "21. Sequence item 'corge'");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "22. Flow entry comma (,)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "grault",
                          "23. Sequence item 'grault'");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "24. Flow sequence end (])");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "25. Flow mapping end (})");
    ASSERT_TOKEN(YAMLTokenNewline, "26. Newline after flow mapping");

    //   - foo_bar: fizz:buzz (2 spaces)
    ASSERT_TOKEN(YAMLTokenListDash, "27. List dash (-)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "foo_bar", "28. Key 'foo_bar'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "29. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "fizz:buzz",
                          "30. Plain scalar with embedded colon");
    ASSERT_TOKEN(YAMLTokenNewline, "31. Newline after line");

    // EOF Indent Unwinding (2 spaces -> 0 spaces)
    ASSERT_TOKEN(YAMLTokenDedent, "32. Dedent at EOF (2 -> 0 spaces)");
    ASSERT_TOKEN(YAMLTokenEndStream, "33. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteralSimple()
{
    FILE *file_ptr = fopen("./testfiles/block/literal/simple.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "test", "1. Scalar 'test'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "foo\nbar\nfizz\nbuzz\nbazz\n",
                          "3. Literal block scalar payload");
    ASSERT_TOKEN(YAMLTokenEndStream, "4. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteralSimpleStrip()
{
    FILE *file_ptr = fopen("./testfiles/block/literal/simple-strip.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "test", "1. Scalar 'test'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(
        YAMLTokenScalar, "foo\nbar\nfizz\nbuzz\nbazz",
        "3. Strip block scalar payload (no trailing newline)");
    ASSERT_TOKEN(YAMLTokenEndStream, "4. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteralSimpleKeep()
{
    FILE *file_ptr = fopen("./testfiles/block/literal/simple-keep.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "test", "1. Scalar 'test'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(
        YAMLTokenScalar, "foo\nbar\nfizz\nbuzz\nbazz\n\n\n\n\n",
        "3. Keep block scalar payload (all trailing newlines preserved)");
    ASSERT_TOKEN(YAMLTokenEndStream, "4. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteralSimpleWithNumber()
{
    FILE *file_ptr =
        fopen("./testfiles/block/literal/simple-with-number.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "doc_explicit_indent",
                          "1. Scalar 'doc_explicit_indent'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar,
                          "  leading spaces are content\n  second line\n",
                          "3. Literal block scalar with explicit indent |2");
    ASSERT_TOKEN(YAMLTokenEndStream, "4. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteralSimpleNested()
{
    FILE *file_ptr =
        fopen("./testfiles/block/literal/simple-nested.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "foo", "1. Scalar 'foo'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "3. Newline after foo:");

    ASSERT_TOKEN(YAMLTokenIndent, "4. Indent to 2 spaces");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "bar", "5. Scalar 'bar'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "6. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "7. Newline after bar:");

    ASSERT_TOKEN(YAMLTokenIndent, "8. Indent to 4 spaces");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "example", "9. Scalar 'example'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "10. Value indicator (:)");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "hello\nthis is an example\n",
                          "11. Nested block scalar payload");

    ASSERT_TOKEN(YAMLTokenDedent, "12. Dedent (4 -> 2 spaces)");
    ASSERT_TOKEN(YAMLTokenDedent, "13. Dedent (2 -> 0 spaces)");
    ASSERT_TOKEN(YAMLTokenEndStream, "14. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteralSimpleWithLeadingLines()
{
    FILE *file_ptr =
        fopen("./testfiles/block/literal/simple-leading-newlines.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "example", "1. Scalar 'example'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(
        YAMLTokenScalar, "\n\nhello\n  world\n",
        "3. Payload with leading blank lines and relative indent");
    ASSERT_TOKEN(YAMLTokenEndStream, "4. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockScalarSimpleAfterDash()
{
    FILE *file_ptr =
        fopen("./testfiles/block/literal/simple-after-dash.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenListDash, "1. Block entry indicator (-)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "first line\nsecond line\n",
                          "2. Compact block scalar payload (2 spaces)");

    ASSERT_TOKEN(YAMLTokenListDash, "3. Block entry indicator (-)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "indented further\nsecond line\n",
                          "4. Compact block scalar payload (4 spaces)");

    ASSERT_TOKEN(YAMLTokenEndStream, "5. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteralSimpleWithDashAndNumber()
{
    FILE *file_ptr = fopen(
        "./testfiles/block/literal/simple-with-dash-and-number.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenListDash, "1. Sequence entry dash (-)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar,
                          "  extra indented content\n  second line\n",
                          "2. Scalar payload preserving 2 extra spaces");

    ASSERT_TOKEN(YAMLTokenListDash, "3. Sequence entry dash (-)");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "exact base indent\n",
                          "4. Scalar payload matching explicit base indent");

    ASSERT_TOKEN(YAMLTokenEndStream, "5. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockLiteral()
{
    testBlockLiteralSimple();
    testBlockLiteralSimpleStrip();
    testBlockLiteralSimpleKeep();
    testBlockLiteralSimpleWithNumber();
    testBlockLiteralSimpleNested();
    testBlockLiteralSimpleWithLeadingLines();
    testBlockScalarSimpleAfterDash();
    testBlockLiteralSimpleWithDashAndNumber();
}

static void testBlockFoldedSimple()
{
    FILE *file_ptr = fopen("./testfiles/block/folded/simple.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "test", "1. Scalar 'test'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(
        YAMLTokenScalar, "foo bar\n",
        "3. Folded block scalar payload (newlines folded to spaces)");
    ASSERT_TOKEN(YAMLTokenEndStream, "4. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockSimpleFoldedWithKeepChomping()
{
    FILE *file_ptr =
        fopen("./testfiles/block/folded/simple-with-keep-chomping.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "test", "1. Scalar 'test'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN_WITH_VAL(
        YAMLTokenScalar, "foo bar fizz buzz\nbazz\n\n\n",
        "3. Folded block scalar payload (newlines folded to spaces)");
    ASSERT_TOKEN(YAMLTokenEndStream, "4. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockSimpleFoldedAfterDash()
{
    FILE *file_ptr =
        fopen("./testfiles/block/folded/simple-after-dash.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN(YAMLTokenListDash, "1. Value dash (-)");
    ASSERT_TOKEN_WITH_VAL(
        YAMLTokenScalar, "first line second line\n",
        "2. Folded block scalar payload (newlines folded to spaces)");
    ASSERT_TOKEN(YAMLTokenEndStream, "3. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testBlockFolded()
{
    testBlockFoldedSimple();
    testBlockSimpleFoldedWithKeepChomping();
    testBlockSimpleFoldedAfterDash();
}

static void testDepartmentsNoAnchors()
{
    FILE *file_ptr = fopen("./testfiles/departments-no-anchors.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "company_name", "1");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Example, Inc.", "3");
    ASSERT_TOKEN(YAMLTokenNewline, "4");
    ASSERT_TOKEN(YAMLTokenNewline, "5");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "departments", "6");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "7");
    ASSERT_TOKEN(YAMLTokenNewline, "8");
    ASSERT_TOKEN(YAMLTokenIndent, "8");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "shipping", "9");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "12");
    ASSERT_TOKEN(YAMLTokenNewline, "25");
    ASSERT_TOKEN(YAMLTokenIndent, "8");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "name", "11");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "12");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Shipping Department", "13");
    ASSERT_TOKEN(YAMLTokenNewline, "14");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "legal_entity", "15");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "16");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Example, Inc.", "17");
    ASSERT_TOKEN(YAMLTokenNewline, "18");
    ASSERT_TOKEN(YAMLTokenDedent, "19");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "billing", "20");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "23");
    ASSERT_TOKEN(YAMLTokenNewline, "25");
    ASSERT_TOKEN(YAMLTokenIndent, "21");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "name", "22");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "23");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Billing Department", "24");
    ASSERT_TOKEN(YAMLTokenNewline, "25");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "legal_entity", "26");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "27");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Example, Inc.", "28");
    ASSERT_TOKEN(YAMLTokenNewline, "29");
    ASSERT_TOKEN(YAMLTokenDedent, "30");
    ASSERT_TOKEN(YAMLTokenDedent, "31");
    ASSERT_TOKEN(YAMLTokenEndStream, "32");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testDepartments()
{

    FILE *file_ptr = fopen("./testfiles/anchors/departments.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "company_name", "1");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAnchor, "corp", "2");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Example, Inc.", "3");
    ASSERT_TOKEN(YAMLTokenNewline, "4");
    ASSERT_TOKEN(YAMLTokenNewline, "5");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "departments", "6");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "7");
    ASSERT_TOKEN(YAMLTokenNewline, "8");
    ASSERT_TOKEN(YAMLTokenIndent, "8");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "shipping", "9");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "12");
    ASSERT_TOKEN(YAMLTokenNewline, "25");
    ASSERT_TOKEN(YAMLTokenIndent, "8");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "name", "11");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "12");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Shipping Department", "13");
    ASSERT_TOKEN(YAMLTokenNewline, "14");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "legal_entity", "15");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "16");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAlias, "corp", "17");
    ASSERT_TOKEN(YAMLTokenNewline, "18");
    ASSERT_TOKEN(YAMLTokenDedent, "19");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "billing", "20");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "23");
    ASSERT_TOKEN(YAMLTokenNewline, "25");
    ASSERT_TOKEN(YAMLTokenIndent, "21");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "name", "22");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "23");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "Billing Department", "24");
    ASSERT_TOKEN(YAMLTokenNewline, "25");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "legal_entity", "26");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "27");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAlias, "corp", "28");
    ASSERT_TOKEN(YAMLTokenNewline, "29");
    ASSERT_TOKEN(YAMLTokenDedent, "30");
    ASSERT_TOKEN(YAMLTokenDedent, "31");
    ASSERT_TOKEN(YAMLTokenEndStream, "32");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testSimpleMergeKey()
{
    FILE *file_ptr = fopen("./testfiles/anchors/simple-merge-key.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "defaults", "1");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAnchor, "base_settings", "2");
    ASSERT_TOKEN(YAMLTokenNewline, "4");

    ASSERT_TOKEN(YAMLTokenIndent, "8");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "timeout", "6");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "30", "9");

    ASSERT_TOKEN(YAMLTokenNewline, "8");
    ASSERT_TOKEN(YAMLTokenDedent, "19");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "production", "9");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2");
    ASSERT_TOKEN(YAMLTokenNewline, "25");

    ASSERT_TOKEN(YAMLTokenIndent, "8");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "<<", "11");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAlias, "base_settings", "17");
    ASSERT_TOKEN(YAMLTokenNewline, "14");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "timeout", "15");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "16");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "60", "20");
    ASSERT_TOKEN(YAMLTokenNewline, "18");

    ASSERT_TOKEN(YAMLTokenDedent, "30");
    ASSERT_TOKEN(YAMLTokenEndStream, "32");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testAliasesInList()
{
    FILE *file_ptr = fopen("./testfiles/anchors/aliases-in-list.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    ASSERT_TOKEN(YAMLTokenStartStream, "Start");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "test", "15");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "16");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAnchor, "default_port", "2");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "8080", "15");
    ASSERT_TOKEN(YAMLTokenNewline, "18");

    ASSERT_TOKEN_WITH_VAL(YAMLTokenScalar, "ports", "15");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "16");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "16");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAlias, "default_port", "17");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "16");
    ASSERT_TOKEN_WITH_VAL(YAMLTokenAlias, "default_port", "17");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "16");
    ASSERT_TOKEN(YAMLTokenNewline, "18");

    ASSERT_TOKEN(YAMLTokenEndStream, "32");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testAnchors()
{
    testDepartments();
    testSimpleMergeKey();
    testAliasesInList();
}

extern void TestLexer(void)
{
    // YAMLLexerDebugTest("./testfiles/anchors/simple.yaml");
    // YAMLLexerDebugTest("./testfiles/indent/different-levels.yaml");
    // exit(1);
    testOnly();
    testMultiDocumentAndFlowContainers();
    testMixed();
    testBlockLiteral();
    testBlockFolded();
    testDepartmentsNoAnchors();

    testAnchors();
    return;
}
