#include <assert.h>
#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>
#include <stdio.h>
#include <stdlib.h>

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

[[maybe_unused]] static YAMLValue *yamlValueInitBlank()
{
    return malloc(sizeof(YAMLValue));
}

[[maybe_unused]] static YAMLValue *yamlValueInit(enum YAMLValueType value_type,
                                                 void *value)
{
    YAMLValue *yaml_value = malloc(sizeof(YAMLValue));
    yaml_value->value_type = value_type;
    if (value_type == YAMLOBJ_t)
    {
        yaml_value->map = value;
    }
    else if (value_type == YAMLLIST_t)
    {
        yaml_value->list = value;
    }
    else if (value_type == YAMLSTRING_t)
    {
        yaml_value->str = value;
    }
    else if (value_type == YAMLNUMBER_INT_t)
    {
        yaml_value->num_int = value;
    }
    else if (value_type == YAMLNUMBER_DOUBLE_t)
    {
        yaml_value->num_double = value;
    }
    else if (value_type == YAMLBOOL_t)
    {
        yaml_value->boolean = value;
    }
    // else if (value_type == YAMLNULL_t)
    else
    {
    }
    return yaml_value;
}

static YAMLValue *parse(YAMLParser *parser)
{
    assert(parser != NULL);
    YAMLValue *yaml_value = NULL;
    if (parser->current_token->type == YAMLTokenStartStream)
    {
    }
    if (parser->current_token->type == YAMLTokenScalar)
    {
    }
    return yaml_value;
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
    yaml->root = parse(parser);

    return yaml;
}
