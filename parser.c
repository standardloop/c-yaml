#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>
#include <stdio.h>
#include <stdlib.h>

#include "./yaml.h"

static void nextYAMLToken(YAMLParser *parser);

extern YAMLParser *YAMLParserInit(YAMLLexer *lexer,
                                  enum YAMLParserInputMode input_mode,
                                  void *input_ptr, size_t buffer_size)
{
    Log(TRACE, "enterin YAMLParserInit");
    if (lexer == NULL)
    {
        return NULL;
    }

    YAMLParser *parser = malloc(sizeof(YAMLParser));
    if (parser == NULL)
    {
        return NULL;
    }
    parser->buffer_size = buffer_size;
    parser->input_mode = input_mode;

    // worry about file only for now
    if (parser->input_mode == YAMLParserInputFile)
    {
        parser->file_ptr = (FILE *)input_ptr;
    }
    else
    {
        return NULL;
    }
    // else if (parser->input_mode == YAMLParserInputString)
    // {
    //     parser->file_ptr = (FILE *)input_ptr;
    // }
    parser->lexer = lexer;
    parser->current_token = NULL;
    parser->peek_token = NULL;
    if (false)
    {
        nextYAMLToken(parser); // todo
    }
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

extern YAML *YAMLParserParse(YAMLParser *parser)
{
    Log(TRACE, "entering YAMLParserParse");
    if (parser == NULL)
    {
        return NULL;
    }
    Log(TRACE, "parser is not NULL");
    YAML *yaml = YAMLInit();
    if (yaml == NULL)
    {
        YAMLParserFree(parser);
    }

    Log(TRACE, "setting up initial buffers");
    char *raw_yaml_buffer_a = calloc(parser->buffer_size + 1, sizeof(char));
    if (raw_yaml_buffer_a == NULL)
    {
        // fclose(file_ptr);
        Log(ERROR, "no mem for raw_yaml_buffer_a");
        return NULL;
    }
    char *raw_yaml_buffer_b = calloc(parser->buffer_size + 1, sizeof(char));
    if (raw_yaml_buffer_b == NULL)
    {
        // fclose(file_ptr);
        Log(ERROR, "no mem for raw_yaml_buffer_b");
        return NULL;
    }

    char *current_buffer = raw_yaml_buffer_a;
    char *next_buffer = raw_yaml_buffer_b;

    size_t current_bytes = fread(current_buffer, sizeof(char),
                                 parser->buffer_size, parser->file_ptr);
    current_buffer[parser->buffer_size] = NULL_CHAR;

    if (current_bytes == 0)
    {

        Log(ERROR, "the file is empty");
        // fclose(file_ptr);
        return NULL; // fixme, maybe we can still return an initialized yaml
                     // struct
    }

    Log(TRACE, "starting the loop");
    bool done = false;
    while (ALWAYS)
    {
        size_t next_bytes = fread(next_buffer, sizeof(char),
                                  parser->buffer_size, parser->file_ptr);
        current_buffer[parser->buffer_size] = NULL_CHAR;
        next_buffer[parser->buffer_size] = NULL_CHAR;

        // Log(ERROR, "%s", current_buffer);
        // Log(ERROR, "%d", (int)next_bytes);
        YAMLLexerReload(parser->lexer, current_buffer, current_bytes,
                        next_bytes == 0);
        while (!IsLexerHungry(parser->lexer))
        {
            YAMLToken *token = YAMLLex(parser->lexer);
            if (token != NULL)
            {
                YAMLTokenPrint(token);
                if (token->type == YAMLTokenEOF)
                {
                    done = true;
                    break;
                }
            }
            // sleep(1);
        }
        // if (next_bytes == 0)
        // {
        //     break;
        // }

        if (done)
        {
            break;
        }

        // loop
        char *temp = current_buffer;
        current_buffer = next_buffer;
        next_buffer = temp;

        current_bytes = next_bytes;
    }

    YAMLParserFree(parser);
    return yaml;
}
