#include <assert.h>
#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>
#include <stdio.h>
#include <stdlib.h>

#define ENTER_FUNC_LOG Log(TRACE, "entering %s...", __PRETTY_FUNCTION__);

#include "./yaml.h"

static void nextYAMLToken(YAMLParser *parser);
static YAMLValue *parse(YAMLParser *parser);

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

static inline void skipNewlines(YAMLParser *parser)
{
    while (parser->current_token->type == YAMLTokenNewline)
    {
        nextYAMLToken(parser);
    }
}

[[maybe_unused]] static bool isAllowedEndings(enum YAMLTokenType type)
{
    return type == YAMLTokenEndOfDocument || type == YAMLTokenStartOfDocument;
}

static YAMLValue *parseScalar(YAMLParser *parser)
{
    // Log(TRACE, "%s", __FUNCTION__);
    assert(parser->current_token->type == YAMLTokenScalar);
    YAMLValue *return_value =
        ItemInit(parser->current_token->literal, &SL_COLLECTIONS_TYPE_STRING,
                 &ItemValueStringOperations);
    nextYAMLToken(parser);
    return return_value;

    // YAMLTokenPrint(parser->current_token);
    // if (parser->current_token->type == YAMLTokenEndStream)
    // {
    //     return return_value;
    // }
    // else
    // {
    //     assert(isAllowedEndings(parser->current_token->type));
    //     nextYAMLToken(parser);
    //     Log(FATAL, "wip");
    //     return NULL;
    // }
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
    ENTER_FUNC_LOG
    assert(parser != NULL);
    assert(parser->current_token->type == YAMLTokenScalar);
    assert(parser->peek_token->type == YAMLTokenValueIndicator);

    // create the map
    ComplexHashMap *map = ComplexHashMapInitDefault();
    YAMLValue *yaml_value_item =
        ItemInit(map, &SL_COLLECTIONS_TYPE_COMPLEX_HASHMAP,
                 &ItemValueComplexHashMapOperations);
    while (ALWAYS)
    {

        YAMLTokenPrint(parser->current_token);

        skipNewlines(parser);

        if (parser->current_token->type == YAMLTokenEndStream ||
            parser->current_token->type == YAMLTokenDedent) // handle nested
        {
            break;
        }

        if (parser->current_token->type == YAMLTokenScalar)
        {
            Item *key = ItemInit(parser->current_token->literal,
                                 &SL_COLLECTIONS_TYPE_STRING,
                                 &ItemValueStringOperations);

            nextYAMLToken(parser);
            Log(DEBUG, "should be value indicator");
            assert(parser->current_token->type == YAMLTokenValueIndicator);
            nextYAMLToken(parser);
            Log(DEBUG, "should be value or nested now");
            if (parser->current_token->type == YAMLTokenScalar)
            {
                Log(DEBUG, "it is value");
                ComplexHashMapInsert(map, key,
                                     ItemInit(parser->current_token->literal,
                                              &SL_COLLECTIONS_TYPE_STRING,
                                              &ItemValueStringOperations));
                nextYAMLToken(parser);
            }
            else
            {
                skipNewlines(parser);

                if (parser->current_token->type == YAMLTokenIndent)
                {
                    Log(DEBUG, "nested?");
                    nextYAMLToken(parser);
                    assert(parser->current_token->type == YAMLTokenScalar);
                    ComplexHashMapInsert(map, key, parse(parser));
                    nextYAMLToken(parser);
                }
                else
                {
                    YAMLTokenPrint(parser->current_token);
                    Log(FATAL, "not ready for this yet");
                }
            }
        }
        else // if (parser->current_token->type == YAMLTokenIndent)
        {
            YAMLTokenPrint(parser->current_token);
            Log(FATAL, "not ready for this yet");
        }
    }
    return yaml_value_item;
}

// FIXME: eventually we will need to handle a null for example

/*
```yaml
-
-
-
```
is equal to
```json
[
  null,
  null,
  null
]
```
*/
static YAMLValue *parseList(YAMLParser *parser)
{
    ENTER_FUNC_LOG
    assert(parser != NULL);
    assert(parser->current_token->type == YAMLTokenListDash);
    // create the list
    List *list = ListInitDefault();
    assert(list != NULL);
    YAMLValue *yaml_value_item =
        ItemInit(list, &SL_COLLECTIONS_TYPE_LIST, &ItemValueListOperations);
    u_int32_t idx = 0;
    while (ALWAYS)
    {
        skipNewlines(parser);
        if (parser->current_token->type == YAMLTokenEndStream)
        {
            break;
        }

        if (parser->current_token->type == YAMLTokenListDash)
        {
            nextYAMLToken(parser);
            Item *entry = parse(parser);
            ListAddAtIndex(list, entry, idx);
            idx++;
            nextYAMLToken(parser);
        }
        else
        {
            Log(FATAL, "HUH");
        }
    }

    return yaml_value_item;
}

//
static YAMLValue *parse(YAMLParser *parser)
{
    ENTER_FUNC_LOG
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
    if (parser->current_token->type == YAMLTokenStartOfDocument)
    {
        nextYAMLToken(parser);
    }
    while (parser->current_token->type == YAMLTokenNewline)
    {
        nextYAMLToken(parser);
    }
    while (ALWAYS)
    {
        // YAMLTokenPrint(parser->current_token);
        Log(DEBUG, "looping");
        YAMLValue *yaml_item = parse(parser);
        if (yaml_item == NULL)
        {
            Log(FATAL, "null yaml_item");
        }
        ListAddAtIndex(yaml, yaml_item, doc_count);
        doc_count++;
        if (parserDone(parser))
        {
            break;
        }
    }
    YAMLParserFree(parser);

    return yaml;
}
