#include <stdio.h>
#include <stdlib.h>

#include "./yaml.h"

extern YAMLParser *YAMLParserInit(YAMLLexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }

    YAMLParser *parser = malloc(sizeof(YAMLParser));
    if (parser == NULL)
    {
        return NULL;
    }
    parser->lexer = lexer;
    parser->current_token = NULL;
    parser->peek_token = NULL;
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
        YAMLParserFree(parser);
    }
}

static void nextYAMLToken(YAMLParser *parser)
{
    if (parser == NULL)
    {
        return;
    }
    YAMLTokenFree(parser->current_token);
    parser->current_token = parser->peek_token;
    YAMLToken *peek_token = NULL;
    while (peek_token == NULL) // also check lexer error here maybe errno
    {
        peek_token = YAMLLex(parser->lexer);
    }
    parser->peek_token = peek_token;
}

extern YAML *YAMLParse(YAMLParser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    YAML *yaml = YAMLInit();
    if (yaml == NULL)
    {
        YAMLParserFree(parser);
    }
    // yaml = parse();
    YAMLParserFree(parser);
    return yaml;
}
