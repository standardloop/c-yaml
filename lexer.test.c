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

    // EOF
    ASSERT_TOKEN(YAMLTokenEOF, "47. EOF reached");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testNestedWithFlow(void)
{
    FILE *file_ptr = fopen("./testfiles/nested-with-flow.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    // root:
    ASSERT_SCALAR("root", "1. Root scalar 'root'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "3. Newline after root:");

    //   level1: (2 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "4. Indent to level 1");
    ASSERT_SCALAR("level1", "5. Scalar 'level1'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "6. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "7. Newline after level1:");

    //     - level2_item1: (4 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "8. Indent to level 2");
    ASSERT_TOKEN(YAMLTokenListDash, "9. List dash (-)");
    ASSERT_SCALAR("level2_item1", "10. Scalar 'level2_item1'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "11. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "12. Newline after level2_item1:");

    //         level3: (8 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "13. Indent to level 3");
    ASSERT_SCALAR("level3", "14. Scalar 'level3'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "15. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "16. Newline after level3:");

    //           - [ { deep_key: "nested \"value\"" }, 42 ] (10 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "17. Indent to level 4");
    ASSERT_TOKEN(YAMLTokenListDash, "18. List dash (-)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "19. Flow sequence start ([)");
    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "20. Flow mapping start ({)");
    ASSERT_SCALAR("deep_key", "21. Flow key scalar 'deep_key'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "22. Value indicator (:)");
    ASSERT_SCALAR("nested \"value\"", "23. Double-quoted scalar with escape");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "24. Flow mapping end (})");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "25. Flow entry comma (,)");
    ASSERT_SCALAR("42", "26. Plain scalar '42'");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "27. Flow sequence end (])");
    ASSERT_TOKEN(YAMLTokenNewline,
                 "28. Newline after line (ignoring # comment)");

    //     - level2_item2: end (drops from 10 spaces back to 4 spaces)
    ASSERT_TOKEN(YAMLTokenDedent, "29. First dedent (10 -> 8 spaces)");
    ASSERT_TOKEN(YAMLTokenDedent, "30. Second dedent (8 -> 4 spaces)");
    ASSERT_TOKEN(YAMLTokenListDash, "31. List dash (-)");
    ASSERT_SCALAR("level2_item2", "32. Scalar 'level2_item2'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "33. Value indicator (:)");
    ASSERT_SCALAR("end", "34. Scalar 'end'");
    ASSERT_TOKEN(YAMLTokenNewline, "35. Newline after last item");

    // EOF Indent Unwinding (stack unwinds from 4 spaces to 0)
    ASSERT_TOKEN(YAMLTokenDedent, "36. EOF dedent (4 -> 2 spaces)");
    ASSERT_TOKEN(YAMLTokenDedent, "37. EOF dedent (2 -> 0 spaces)");
    ASSERT_TOKEN(YAMLTokenEOF, "38. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

static void testNested(void)
{
    FILE *file_ptr = fopen("./testfiles/nested.yaml", "rb");
    TestCaseVerify(true, "File opened successfully", file_ptr != NULL);
    if (!file_ptr)
    {
        return;
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);
    YAMLToken *token = NULL;

    // foo:
    ASSERT_SCALAR("foo", "1. Scalar 'foo'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "3. Newline after foo:");

    //   bar: (2 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "4. Indent to 2 spaces");
    ASSERT_SCALAR("bar", "5. Scalar 'bar'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "6. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "7. Newline after bar:");

    //     - fizz: (4 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "8. Indent to 4 spaces");
    ASSERT_TOKEN(YAMLTokenListDash, "9. List dash (-)");
    ASSERT_SCALAR("fizz", "10. Scalar 'fizz'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "11. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "12. Newline after fizz:");

    //         buzz: (8 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "13. Indent to 8 spaces");
    ASSERT_SCALAR("buzz", "14. Scalar 'buzz'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "15. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "16. Newline after buzz:");

    //           - bazz (10 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "17. Indent to 10 spaces");
    ASSERT_TOKEN(YAMLTokenListDash, "18. List dash (-)");
    ASSERT_SCALAR("bazz", "19. Scalar 'bazz'");
    ASSERT_TOKEN(YAMLTokenNewline, "20. Newline after bazz");

    //           - qux: (10 spaces)
    ASSERT_TOKEN(YAMLTokenListDash, "21. List dash (-)");
    ASSERT_SCALAR("qux", "22. Scalar 'qux'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "23. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "24. Newline after qux:");

    //               quux: corge (14 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "25. Indent to 14 spaces");
    ASSERT_SCALAR("quux", "26. Scalar 'quux'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "27. Value indicator (:)");
    ASSERT_SCALAR("corge", "28. Scalar 'corge'");
    ASSERT_TOKEN(YAMLTokenNewline, "29. Newline after corge");

    //     - grault (drops 14 spaces -> 4 spaces across 3 stack levels)
    ASSERT_TOKEN(YAMLTokenDedent, "30. First dedent (14 -> 10 spaces)");
    ASSERT_TOKEN(YAMLTokenDedent, "31. Second dedent (10 -> 8 spaces)");
    ASSERT_TOKEN(YAMLTokenDedent, "32. Third dedent (8 -> 4 spaces)");
    ASSERT_TOKEN(YAMLTokenListDash, "33. List dash (-)");
    ASSERT_SCALAR("grault", "34. Scalar 'grault'");
    ASSERT_TOKEN(YAMLTokenNewline, "35. Newline after grault");

    // EOF Indent Unwinding (unwinds 4 spaces -> 2 spaces -> 0 spaces)
    ASSERT_TOKEN(YAMLTokenDedent, "36. EOF dedent (4 -> 2 spaces)");
    ASSERT_TOKEN(YAMLTokenDedent, "37. EOF dedent (2 -> 0 spaces)");
    ASSERT_TOKEN(YAMLTokenEOF, "38. End of file");

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

    // foo: "bar with \"quotes\""
    ASSERT_SCALAR("foo", "1. Scalar 'foo'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "2. Value indicator (:)");
    ASSERT_SCALAR("bar with \"quotes\"", "3. Unescaped double-quoted scalar");
    ASSERT_TOKEN(YAMLTokenNewline, "4. Newline after double quote");

    // fizz:
    ASSERT_SCALAR("fizz", "5. Scalar 'fizz'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "6. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenNewline, "7. Newline after fizz:");

    //   - 'buzz ''quote''' (2 spaces)
    ASSERT_TOKEN(YAMLTokenIndent, "8. Indent to 2 spaces");
    ASSERT_TOKEN(YAMLTokenListDash, "9. List dash (-)");
    ASSERT_SCALAR("buzz 'quote'", "10. Unescaped single-quoted scalar");
    ASSERT_TOKEN(YAMLTokenNewline, "11. Newline after single quote");

    //   - { bazz: qux, quux: [ corge, grault ] } (2 spaces)
    ASSERT_TOKEN(YAMLTokenListDash, "12. List dash (-)");
    ASSERT_TOKEN(YAMLTokenFlowMappingStart, "13. Flow mapping start ({)");
    ASSERT_SCALAR("bazz", "14. Key 'bazz'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "15. Value indicator (:)");
    ASSERT_SCALAR("qux", "16. Value 'qux'");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "17. Flow entry comma (,)");
    ASSERT_SCALAR("quux", "18. Key 'quux'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "19. Value indicator (:)");
    ASSERT_TOKEN(YAMLTokenFlowSequenceStart, "20. Flow sequence start ([)");
    ASSERT_SCALAR("corge", "21. Sequence item 'corge'");
    ASSERT_TOKEN(YAMLTokenFlowEntry, "22. Flow entry comma (,)");
    ASSERT_SCALAR("grault", "23. Sequence item 'grault'");
    ASSERT_TOKEN(YAMLTokenFlowSequenceEnd, "24. Flow sequence end (])");
    ASSERT_TOKEN(YAMLTokenFlowMappingEnd, "25. Flow mapping end (})");
    ASSERT_TOKEN(YAMLTokenNewline, "26. Newline after flow mapping");

    //   - foo_bar: fizz:buzz (2 spaces)
    ASSERT_TOKEN(YAMLTokenListDash, "27. List dash (-)");
    ASSERT_SCALAR("foo_bar", "28. Key 'foo_bar'");
    ASSERT_TOKEN(YAMLTokenValueIndicator, "29. Value indicator (:)");
    ASSERT_SCALAR("fizz:buzz", "30. Plain scalar with embedded colon");
    ASSERT_TOKEN(YAMLTokenNewline, "31. Newline after line");

    // EOF Indent Unwinding (2 spaces -> 0 spaces)
    ASSERT_TOKEN(YAMLTokenDedent, "32. Dedent at EOF (2 -> 0 spaces)");
    ASSERT_TOKEN(YAMLTokenEOF, "33. End of file");

    YAMLLexerFree(lexer);
    fclose(file_ptr);
}

extern void TestLexer(void)
{
    YAMLLexerDebugTest("./testfiles/folded/simple.yaml");
    testOnly();
    testMultiDocumentAndFlowContainers();
    testNested();
    testNestedWithFlow();
    testMixed();
}
