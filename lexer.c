#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <strings.h>

#include <standardloop/util.h>
#include <standardloop/logger.h>

#include "./yaml.h"

static void advanceChar(YAMLLexer *);
static void backtrackChar(YAMLLexer *);
static bool isAtEndOfLexerInput(YAMLLexer *);
static char *captureValueOrKey(YAMLLexer *);

extern bool IsLexerHungry(YAMLLexer *lexer)
{
    return lexer->hungry;
}

static void copyString(char *, char *, size_t, size_t);

static void copyString(char *src, char *des, size_t len, size_t src_offset)
{
    if (src == NULL || des == NULL || len <= 0)
    {
        errno = EINVAL;
        return;
    }

    char *src_it = src + src_offset;
    size_t size = 0;
    while (size < len)
    {
        *des = *src_it;
        des++;
        src_it++;
        size++;
    }
}

static bool isAtEndOfLexerInput(YAMLLexer *lexer)
{
    return lexer->read_position >= lexer->input_len;
}

extern void
LexerReload(YAMLLexer *lexer, char *buffer, size_t size, bool is_last_chunk)
{

    if (lexer->temp_input != NULL && lexer->temp_input_len > 0)
    {
        size_t new_size = lexer->temp_input_len + size + 1;
        char *larger_input = calloc(new_size, sizeof(char));

        // Log(ERROR, "%s", lexer->temp_input);
        // Log(ERROR, "%s", buffer);

        for (size_t i = 0; i < new_size; i++)
        {
            if (i < lexer->temp_input_len)
            {
                larger_input[i] = lexer->temp_input[i];
            }
            else
            {
                larger_input[i] = buffer[i - lexer->temp_input_len];
            }
        }
        free(lexer->temp_input);
        lexer->temp_input = NULL;
        lexer->temp_input_len = 0;

        lexer->input = larger_input;
        lexer->input_len = new_size;

        // Log(DEBUG, "%s", larger_input);
    }
    else
    {
        lexer->input = buffer;
        lexer->input_len = size;

        lexer->temp_input = NULL;
        lexer->temp_input_len = 0;
    }
    lexer->hungry = false;
    lexer->position = -1;
    lexer->read_position = 0;
    lexer->is_last_chunk = is_last_chunk;
}

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
    lexer->position = -1;
    lexer->read_position = 0;
    lexer->line = 1;
    lexer->error = NULL;

    lexer->is_last_chunk = false;

    lexer->hungry = false;
    lexer->temp_input = NULL;
    lexer->temp_input_len = 0;

    lexer->state = YAMLLexerStateNormal;

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

static char *captureValueOrKey(YAMLLexer *lexer)
{
    size_t start_position = lexer->position;
    // char prev_char = lexer->current_char;
    // bool is_error = false;

    while (ALWAYS)
    {
        if (lexer->current_char == NEWLINE_CHAR || lexer->current_char == NULL_CHAR || lexer->current_char == COLON_CHAR)
        {
            break;
        }
        advanceChar(lexer);
    }
    // Log(DEBUG, "%d", lexer->current_char);
    // Log(DEBUG, "%d", (int)lexer->input_len);

    u_int32_t string_literal_size = (lexer->position - start_position) + 1;
    char *string_literal = malloc(sizeof(char) * string_literal_size);
    if (string_literal == NULL)
    {
        return NULL;
    }
    copyString(lexer->input, string_literal, string_literal_size, start_position);
    string_literal[string_literal_size - 1] = NULL_CHAR;
    // Log(DEBUG, "%s", string_literal);
    return string_literal;
}

extern YAMLToken *YAMLLex(YAMLLexer *lexer)
{
    if (false)
    {
        backtrackChar(lexer);
    }

    if (isAtEndOfLexerInput(lexer))
    {
        lexer->hungry = true;
        return NULL;
    }
    advanceChar(lexer);
    // Log(DEBUG, "%c", lexer->current_char);
    //  if (lexer->current_char == NULL_CHAR)
    //  {
    //      return NULL;
    //  }

    u_int32_t curr_pos = lexer->position;
    YAMLToken *token = NULL;
    // printf("%c", lexer->current_char);
    if (lexer->current_char == COLON_CHAR)
    {
        token = YAMLTokenInit(YAMLTokenValueIndicator, curr_pos, lexer->position + 1, lexer->line, NULL);
    }
    else if (lexer->current_char == SPACE_CHAR)
    {
        token = YAMLTokenInit(YAMLTokenSpace, curr_pos, lexer->position + 1, lexer->line, NULL);
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
        if (lexer->current_char == NULL_CHAR && !lexer->is_last_chunk)
        {
            lexer->hungry = true;
        }
        else
        {
            token = YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
    }
    else
    {
        // if (lexer->current_char == DOUBLE_QUOTES_CHAR)
        // {

        //     lexer->state =
        // }
        // else if (lexer->current_char == SINGLE_QUOTES_CHAR)
        // {
        // }

        // must be a value or key
        char *value_or_key = captureValueOrKey(lexer);
        // how to we know if we captured the full value?
        // did we run out of buffer?
        if (lexer->current_char == NULL_CHAR && !lexer->is_last_chunk)
        {
            // this should mean that we aren't done
            lexer->hungry = true;
            lexer->temp_input = value_or_key;
            lexer->temp_input_len = strlen(value_or_key); // FIXME lazy
            lexer->state = YAMLLexerStateIncomplete;
            return NULL;
        }

        if (lexer->current_char == COLON_CHAR)
        {
            token = YAMLTokenInit(YAMLTokenKey, curr_pos, lexer->position + 1, lexer->line, value_or_key);
        }
        else
        {
            token = YAMLTokenInit(YAMLTokenValue, curr_pos, lexer->position + 1, lexer->line, value_or_key);
        }
        backtrackChar(lexer);
    }
    return token;
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

extern void YAMLTokenPrint(YAMLToken *token)
{
    if (token != NULL)
    {
        printf("%s ", YAMLTokenTypeToString(token->type));
        if (token->literal)
        {
            printf(": %s\n", token->literal);
        }
        else
        {
            printf("\n");
        }
    }
}

extern char *YAMLTokenTypeToString(enum YAMLTokenType type)
{
    if (type == YAMLTokenStartOfDocument)
    {
        return "YAMLTokenStartOfDocument";
    }
    else if (type == YAMLTokenEndOfDocument)
    {
        return "YAMLTokenEndOfDocument";
    }
    else if (type == YAMLTokenDirective)
    {
        return "YAMLTokenDirective";
    }
    else if (type == YAMLTokenIndent)
    {
        return "YAMLTokenIndent";
    }
    else if (type == YAMLTokenDedent)
    {
        return "YAMLTokenDedent";
    }
    else if (type == YAMLTokenSpace)
    {
        return "YAMLTokenSpace";
    }
    else if (type == YAMLTokenKey)
    {
        return "YAMLTokenKey";
    }
    else if (type == YAMLTokenValueIndicator)
    {
        return "YAMLTokenValueIndicator";
    }
    else if (type == YAMLTokenFlowMappingStart)
    {
        return "YAMLTokenFlowMappingStart";
    }
    else if (type == YAMLTokenFlowMappingEnd)
    {
        return "YAMLTokenFlowMappingEnd";
    }
    else if (type == YAMLTokenFlowSequenceStart)
    {
        return "YAMLTokenFlowSequenceStart";
    }
    else if (type == YAMLTokenFlowSequenceEnd)
    {
        return "YAMLTokenFlowSequenceEnd";
    }
    else if (type == YAMLTokenListDash)
    {
        return "YAMLTokenListDash";
    }
    else if (type == YAMLTokenComma)
    {
        return "YAMLTokenComma";
    }
    else if (type == YAMLTokenValue)
    {
        return "YAMLTokenValue";
    }
    else if (type == YAMLTokenBool)
    {
        return "YAMLTokenBool";
    }
    else if (type == YAMLTokenNumber)
    {
        return "YAMLTokenNumber";
    }
    else if (type == YAMLTokenString)
    {
        return "YAMLTokenString";
    }
    else if (type == YAMLTokenNULL)
    {
        return "YAMLTokenNULL";
    }
    else if (type == YAMLTokenSingleQuotes)
    {
        return "YAMLTokenSingleQuotes";
    }
    else if (type == YAMLTokenDoubleQuotes)
    {
        return "YAMLTokenDoubleQuotes";
    }
    else if (type == YAMLTokenLiteralBlockStart)
    {
        return "YAMLTokenLiteralBlockStart";
    }
    else if (type == YAMLTokenFoldedBlockStart)
    {
        return "YAMLTokenFoldedBlockStart";
    }
    else if (type == YAMLTokenListChompingDash)
    {
        return "YAMLTokenListChompingDash";
    }
    else if (type == YAMLTokenListKeepChomping)
    {
        return "YAMLTokenListKeepChomping";
    }
    else if (type == YAMLTokenChompingNumber)
    {
        return "YAMLTokenChompingNumber";
    }
    else if (type == YAMLTokenAlias)
    {
        return "YAMLTokenAlias";
    }
    else if (type == YAMLTokenAnchor)
    {
        return "YAMLTokenAnchor";
    }
    else if (type == YAMLTokenKeyIndicator)
    {
        return "YAMLTokenKeyIndicator";
    }
    else if (type == YAMLTokenTag)
    {
        return "YAMLTokenTag";
    }
    else if (type == YAMLTokenComment)
    {
        return "YAMLTokenComment";
    }
    else if (type == YAMLTokenAT)
    {
        return "YAMLTokenAT";
    }
    else if (type == YAMLTokenBacktick)
    {
        return "YAMLTokenBacktick";
    }
    else if (type == YAMLTokenMerge)
    {
        return "YAMLTokenMerge";
    }
    else if (type == YAMLTokenEOF)
    {
        return "YAMLTokenEOF";
    }
    else if (type == YAMLTokenIllegal)
    {
        return "YAMLTokenIllegal";
    }
    return "ERROR";
}
