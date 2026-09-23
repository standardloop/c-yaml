#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>

#include "./yaml.h"

static void advanceChar(YAMLLexer *lexer);

void reload(YAMLLexer *lexer)
{

    printf("reloading!");
    size_t current_bytes = fread(lexer->input, sizeof(char),
                                 lexer->max_input_len, lexer->file_ptr);
    lexer->input[current_bytes] = NULL_CHAR;

    lexer->is_last_chunk = current_bytes < lexer->max_input_len;
}

static void advanceChar(YAMLLexer *lexer)
{
    if (lexer->read_position == lexer->input_len)
    {
        if (lexer->is_last_chunk)
        {
            lexer->current_char = NULL_CHAR;
        }
        else
        {
            reload(lexer);
            lexer->current_char = NULL_CHAR;
            lexer->position = -1;
            lexer->read_position = 0;
            lexer->current_char = lexer->input[lexer->read_position];
            lexer->position = lexer->read_position;
            lexer->read_position++;
        }
    }
    else
    {
        lexer->current_char = lexer->input[lexer->read_position];
        lexer->position = lexer->read_position;
        lexer->read_position++;
    }
}

static void printchar(unsigned char c)
{
    switch (c)
    {
    case '\n':
        printf("\\n\n");
        break;
    case '\r':
        printf("\\r");
        break;
    case '\t':
        printf("\\t");
        break;
    default:
        if ((c < 0x20) || (c > 0x7f))
        {
            printf("\\%03o", (unsigned char)c);
        }
        else
        {
            printf("%c", c);
        }
        break;
    }
}

extern YAMLLexer *YAMLLexerInit(FILE *file_ptr, size_t buffer_size)
{
    char *raw_yaml_buffer_a = calloc(buffer_size, sizeof(char));
    if (raw_yaml_buffer_a == NULL)
    {
        return NULL;
    }
    // char *raw_yaml_buffer_b = calloc(buffer_size, sizeof(char));
    // if (raw_yaml_buffer_b == NULL)
    // {
    //     return NULL;
    // }

    char *current_buffer = raw_yaml_buffer_a;
    // char *next_buffer = raw_yaml_buffer_b;

    size_t current_bytes =
        fread(current_buffer, sizeof(char), buffer_size, file_ptr);
    current_buffer[buffer_size] = NULL_CHAR;

    if (current_bytes == 0)
    {
        return NULL;
    }
    printf("%d\n", (int)current_bytes);
    YAMLLexer *lexer = malloc(sizeof(YAMLLexer));
    lexer->max_input_len = buffer_size;
    lexer->file_ptr = file_ptr;
    lexer->input = current_buffer;
    lexer->input_len = current_bytes;
    lexer->is_last_chunk = current_bytes < buffer_size;
    lexer->current_char = NULL_CHAR;
    lexer->position = -1;
    lexer->read_position = 0;
    lexer->line = 1;
    return lexer;
    return lexer;
}

extern void YAMLLexerFree(YAMLLexer *lexer)
{
    if (lexer != NULL)
    {
        // if (lexer->error != NULL)
        // {
        //     // can we keep this on the stack?
        //     free(lexer->error);
        // }
        free(lexer);
    }
}

extern YAMLToken *YAMLTokenInit(enum YAMLTokenType type, u_int32_t start,
                                u_int32_t end, u_int32_t line, char *literal)
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
    else if (type == YAMLTokenNewline)
    {
        return "YAMLTokenNewline";
    }
    return "ERROR";
}

extern void YAMLLexerDebugTest(char *file_name)
{
    // char *file_name = "./testfiles/playground.yaml";
    FILE *file_ptr = fopen(file_name, "rb");

    YAMLLexer *lexer = YAMLLexerInit(file_ptr, 4096);

    advanceChar(lexer);
    while (true)
    {
        printchar(lexer->current_char);
        if (lexer->current_char == NULL_CHAR)
        {
            break;
        }
        advanceChar(lexer);
    }
    YAMLLexerFree(lexer);
}
