#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <standardloop/util.h>

#include "./yaml.h"

static void advanceChar(YAMLLexer *);
static void backtrackChar(YAMLLexer *);

extern YAMLLexer *YAMLLexerInit()
{
    YAMLLexer *lexer = malloc(sizeof(YAMLLexer));
    if (lexer == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    lexer->input = NULL;
    lexer->input_len = 0;
    lexer->current_char = NULL_CHAR;
    lexer->position = -2;
    lexer->read_position = -1;
    lexer->line = 1;
    lexer->error = NULL;

    return lexer;
}

extern void YAMLLexerFree(YAMLLexer *lexer)
{
    if (lexer != NULL)
    {
        if (lexer->error != NULL)
        {
            // can we keep this on the stack?
            free(lexer->error);
        }
        free(lexer);
    }
}

static void advanceChar(YAMLLexer *lexer)
{
    if (lexer->read_position >= lexer->input_len)
    {
        lexer->current_char = NULL_CHAR;
    }
    else
    {
        lexer->current_char = lexer->input[lexer->read_position];
    }
    lexer->position = lexer->read_position;
    lexer->read_position++;
}

static void backtrackChar(YAMLLexer *lexer)
{
    lexer->position -= 2;
    lexer->read_position--;
    lexer->current_char = lexer->input[lexer->read_position];
}

extern YAMLToken *YAMLLex(YAMLLexer *lexer)
{
    advanceChar(lexer);
    if (lexer->current_char == NULL_CHAR)
    {
        return NULL;
    }

    u_int32_t curr_pos = lexer->position;
    YAMLToken *token = NULL;
    if (lexer->current_char == COLON_CHAR)
    {
        advanceChar(lexer);
        if (lexer->current_char != SPACE_CHAR)
        {
            token = YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
    }
    else if (lexer->current_char == DASH_MINUS_CHAR)
    {
    }
    else if (lexer->current_char == CURLY_OPEN_CHAR)
    {
    }
    else if (lexer->current_char == CURLY_CLOSE_CHAR)
    {
    }
    else if (lexer->current_char == BRACKET_OPEN_CHAR)
    {
    }
    else if (lexer->current_char == BRACKET_OPEN_CHAR)
    {
    }
    else if (lexer->current_char == DOUBLE_QUOTES_CHAR)
    {
    }
    else if (lexer->current_char == SINGLE_QUOTES_CHAR)
    {
    }
    else if (lexer->current_char == '|')
    {
    }
    else if (lexer->current_char == '*')
    {
    }
    else if (lexer->current_char == '&')
    {
    }
    else if (lexer->current_char == QUESTION_CHAR)
    {
    }
    else if (lexer->current_char == '#')
    {
    }
    else if (lexer->current_char == '#')
    {
    }
    else if (lexer->current_char == '.')
    {
    }
    else if (lexer->current_char == '%')
    {
    }
    else if (lexer->current_char == NULL_CHAR)
    {
    }
    else
    {
        // must be a value
    }
}

extern YAMLToken *YAMLTokenInit(enum YAMLTokenType type, u_int32_t start, u_int32_t end, u_int32_t line, char *literal)
{
    YAMLToken *token = malloc(sizeof(YAMLToken));
    if (token == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    token->type = type;
    token->start = start;
    token->end = end;
    token->literal = literal;
    token->line = line;
    return token;
}

extern void YAMLTokenFree(YAMLToken *token)
{
    if (token != NULL)
    {
        // maybe thinking about freeing literals
        free(token);
    }
}
