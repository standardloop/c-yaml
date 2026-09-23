#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>
#include <stdio.h>
#include <stdlib.h>

#include "./yaml.h"

// static void nextYAMLToken(YAMLParser *parser);

extern YAMLParser *YAMLParserInit()
{
    Log(TRACE, "enterin YAMLParserInit");

    YAMLParser *parser = malloc(sizeof(YAMLParser));

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

// static void nextYAMLToken(YAMLParser *parser)
// {
//     Log(TRACE, "nextYAMLToken");
//     if (parser == NULL)
//     {
//         return;
//     }
//     // YAMLTokenFree(parser->current_token);
//     parser->current_token = parser->peek_token;
//     YAMLToken *peek_token = NULL;
//     YAMLToken *token = YAMLLex(parser->lexer);
//     peek_token = token;
//     parser->peek_token = peek_token;
// }

extern YAML *YAMLParserParse(YAMLParser *parser)
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

    return yaml;
}
