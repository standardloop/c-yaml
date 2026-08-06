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
