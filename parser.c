#include <assert.h>
#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_u_int8_t.h>

#include "./yaml.h"

static void nextYAMLToken(YAMLParser *parser);

extern YAMLParser *YAMLParserInit(FILE *file_ptr)
{
    // Log(TRACE, "%s", __FUNCTION__);

    YAMLParser *parser = malloc(sizeof(YAMLParser));

    parser->lexer = YAMLLexerInit(file_ptr);
    parser->error_message = NULL;
    parser->current_token = NULL;
    parser->peek_token = NULL;
    nextYAMLToken(parser);
    nextYAMLToken(parser);

    return parser;
}

extern void YAMLParserFree(YAMLParser *parser)
{
    if (parser != NULL)
    {
        if (parser->lexer != NULL)
        {
            YAMLLexerFree(parser->lexer);
        }
        free(parser);
    }
}

static void nextYAMLToken(YAMLParser *parser)
{
    // Log(TRACE, "%s", __FUNCTION__);
    assert(parser != NULL);
    // YAMLTokenFree(parser->current_token);
    parser->current_token = parser->peek_token;
    YAMLToken *peek_token = NULL;
    YAMLToken *token = YAMLLex(parser->lexer);
    peek_token = token;
    parser->peek_token = peek_token;
}

extern void YAMLParserDebugTest(YAMLParser *parser)
{
    while (parser->current_token != NULL &&
           parser->current_token->type != YAMLTokenEndStream)
    {
        YAMLTokenPrint(parser->current_token);
        nextYAMLToken(parser);
    }
}

static void skipNewlines(YAMLParser *parser)
{
    while (parser->current_token->type == YAMLTokenNewline)
    {
        nextYAMLToken(parser);
    }
}

static bool isAllowedEndings(enum YAMLTokenType type)
{
    return type == YAMLTokenEndOfDocument || type == YAMLTokenStartOfDocument;
}

static YAMLValue *parseScalar(YAMLParser *parser)
{
    Log(TRACE, "%s", __FUNCTION__);
    assert(parser->current_token->type == YAMLTokenScalar);
    YAMLValue *return_value =
        ItemInit(parser->current_token->literal, &ItemValueStringOperations);
    skipNewlines(parser);
    YAMLTokenPrint(parser->current_token);
    nextYAMLToken(parser);
    YAMLTokenPrint(parser->current_token);
    if (parser->current_token->type == YAMLTokenEndStream)
    {
        return return_value;
    }
    else
    {
        assert(isAllowedEndings(parser->current_token->type));
        nextYAMLToken(parser);
        Log(FATAL, "wip");
        return NULL;
    }
}

static YAMLValue *parseFlowSequence(YAMLParser *parser)
{
    assert(parser != NULL);
    return NULL;
}

static YAMLValue *parseFlowMapping(YAMLParser *parser)
{
    assert(parser != NULL);
    return NULL;
}

static YAMLValue *parseMap(YAMLParser *parser)
{
    assert(parser != NULL);
    return NULL;
}

static YAMLValue *parseList(YAMLParser *parser)
{
    assert(parser != NULL);
    return NULL;
}

//
static YAMLValue *parse(YAMLParser *parser)
{
    assert(parser != NULL);
    // nextYAMLToken(parser);
    if (parser->current_token->type == YAMLTokenScalar &&
        parser->peek_token->type != YAMLTokenValueIndicator)
    {
        return parseScalar(parser);
    }
    else if (parser->current_token->type == YAMLTokenScalar &&
             parser->peek_token->type == YAMLTokenValueIndicator)
    {
        return parseMap(parser);
    }
    else if (parser->current_token->type == YAMLTokenListDash)
    {
        return parseList(parser);
    }
    else if (parser->current_token->type == YAMLTokenFlowSequenceStart)
    {
        return parseFlowSequence(parser);
    }
    else if (parser->current_token->type == YAMLTokenFlowMappingStart)
    {
        return parseFlowMapping(parser);
    }
    else
    {
        return NULL;
    }
}

static bool parserDone(YAMLParser *parser)
{
    return parser->current_token->type == YAMLTokenEndStream;
}

extern YAML *YAMLParse(YAMLParser *parser)
{
    if (parser == NULL)
    {
        // nextYAMLToken(parser);
        return NULL;
    }
    YAML *yaml = YAMLInit();
    if (yaml == NULL)
    {
        YAMLParserFree(parser);
    }
    u_int8_t doc_count = 0;
    assert(parser->current_token->type == YAMLTokenStartStream);
    nextYAMLToken(parser);
    while (ALWAYS)
    {
        Log(DEBUG, "looping");
        YAMLValue *yaml_item = parse(parser);
        ListAddAtIndex(yaml, yaml_item, doc_count);
        doc_count++;
        if (parserDone(parser))
        {
            break;
        }
        exit(1);
    }
    YAMLParserFree(parser);

    return yaml;
}
