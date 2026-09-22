#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>
#include <stdio.h>
#include <stdlib.h>

#include "./yaml.h"

static void nextYAMLToken(YAMLParser *parser);

extern YAMLParser *YAMLParserInit(enum YAMLParserInputMode input_mode,
                                  void *input_ptr, size_t buffer_size)
{
    Log(TRACE, "enterin YAMLParserInit");

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
    parser->current_token = NULL;
    parser->peek_token = NULL;
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
        return NULL;
    }
    parser->current_buffer = current_buffer;
    parser->next_buffer = next_buffer;
    parser->current_bytes = current_bytes;

    parser->lexer = YAMLLexerInit(parser->current_buffer, current_bytes);
    if (current_bytes < buffer_size)
    {
        parser->lexer->is_last_chunk = true;
    }
    // printf("[JOSH]: %s, %d %d\n", parser->current_buffer, (int)current_bytes,
    //        (int)buffer_size);
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
    Log(TRACE, "nextYAMLToken");
    if (parser == NULL)
    {
        return;
    }
    // YAMLTokenFree(parser->current_token);
    parser->current_token = parser->peek_token;
    YAMLToken *peek_token = NULL;

    if (!IsLexerHungry(parser->lexer))
    {
        Log(TRACE, "lexer is hungry");
        size_t next_bytes = fread(parser->next_buffer, sizeof(char),
                                  parser->buffer_size, parser->file_ptr);
        parser->current_buffer[parser->buffer_size] = NULL_CHAR;
        parser->next_buffer[parser->buffer_size] = NULL_CHAR;

        // Log(ERROR, "%s", current_buffer);
        // Log(ERROR, "%d", (int)next_bytes);
        YAMLLexerReload(parser->lexer, parser->current_buffer,
                        parser->current_bytes, next_bytes == 0);

        parser->current_bytes = next_bytes;
        char *temp = parser->current_buffer;
        parser->current_buffer = parser->next_buffer;
        parser->next_buffer = temp;
    }
    else
    {
        Log(TRACE, "lexer is not hungry");
    }
    YAMLToken *token = YAMLLex(parser->lexer);
    peek_token = token;

    parser->peek_token = peek_token;
}

extern YAML *YAMLParserParse(YAMLParser *parser)
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

    Log(TRACE, "going into loop");
    while (ALWAYS)
    {
        nextYAMLToken(parser);
        YAMLTokenPrint(parser->current_token);
        if (parser->current_token->type == YAMLTokenEOF)
        {
            break;
        }
    }

    YAMLParserFree(parser);
    return yaml;
}
