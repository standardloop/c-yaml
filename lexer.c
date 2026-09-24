#include <_stdlib.h>
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
static void resetLexerState(YAMLLexer *lexer);

static void handleComment(YAMLLexer *lexer)
{
    // bool did_break = false;
    do
    {
        advanceChar(lexer);
    } while (lexer->current_char != NULL_CHAR &&
             lexer->current_char != NEWLINE_CHAR);

    // resetLexerState(lexer);
}

static void resetLexerState(YAMLLexer *lexer)
{
    assert(lexer != NULL);
    lexer->state = YAMLLexerStateNormal;
}

void reload(YAMLLexer *lexer)
{

    if (lexer->cursor >= CHUNK_SIZE)
    {
        printf("reloading!");
        size_t shift = CHUNK_SIZE;
        size_t keep = (lexer->bytes_in_buffer > shift)
                          ? (lexer->bytes_in_buffer - shift)
                          : 0;

        if (keep > 0)
        {
            memmove(lexer->buffer, lexer->buffer + shift, keep);
        }

        lexer->cursor -= shift;
        lexer->bytes_in_buffer = keep;

        if (!lexer->eof_reached)
        {
            size_t read_bytes = fread(lexer->buffer + lexer->bytes_in_buffer, 1,
                                      CHUNK_SIZE, lexer->file_ptr);
            lexer->bytes_in_buffer += read_bytes;
            if (read_bytes < CHUNK_SIZE)
            {
                lexer->eof_reached = true;
            }
        }
    }
}

char peek(YAMLLexer *lexer, size_t offset)
{
    size_t target_pos = lexer->cursor + offset;

    if (target_pos >= lexer->bytes_in_buffer)
    {
        return NULL_CHAR; // End of available buffer or EOF
    }

    return lexer->buffer[target_pos];
}

static void advanceChar(YAMLLexer *lexer)
{
    if (lexer->current_char != NULL_CHAR)
    {
        lexer->cursor++;
        reload(lexer);
        lexer->current_char = peek(lexer, 0);
    }
}

extern YAMLLexer *YAMLLexerInit(FILE *file_ptr)
{
    YAMLLexer *lexer = malloc(sizeof(YAMLLexer));
    lexer->file_ptr = file_ptr;
    lexer->cursor = 0;
    lexer->bytes_in_buffer = 0;
    lexer->eof_reached = false;
    lexer->state = YAMLLexerStateJustGotNewline;

    // Read initial 4096 bytes into Left Half
    size_t read_bytes = fread(lexer->buffer, 1, CHUNK_SIZE, lexer->file_ptr);
    lexer->bytes_in_buffer = read_bytes;
    if (read_bytes < CHUNK_SIZE)
    {
        lexer->eof_reached = true;
    }

    // Assign first character directly
    if (lexer->bytes_in_buffer > 0)
    {
        lexer->current_char = lexer->buffer[0];
    }
    else
    {
        lexer->current_char = NULL_CHAR;
    }

    lexer->indent_stack = ListInitDefault();
    int *zero_int = malloc(sizeof(int));
    *zero_int = 0;
    Item *indent_zero_start = ItemInit(zero_int, &ItemValueIntOperations);
    ListAddFirst(lexer->indent_stack, indent_zero_start);

    lexer->space_count = 0;
    lexer->state = YAMLLexerStateJustGotNewline;
    return lexer;
}

static char *eatScalar(YAMLLexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }
    DynString *str = DynStringDefaultInit();
    char quote_char = NULL_CHAR;

    if (lexer->current_char == '\"' || lexer->current_char == '\'')
    {
        quote_char = lexer->current_char;
    }
    bool in_quotes = quote_char != NULL_CHAR;

    size_t chars_found = 0;
    while (ALWAYS)
    {
        // printf("lol\n");
        if (!in_quotes)
        {
            if (lexer->current_char == COLON_CHAR &&
                (peek(lexer, 1) == SPACE_CHAR ||
                 peek(lexer, 1) == NEWLINE_CHAR))
            {
                break;
            }
            else if (lexer->current_char == NEWLINE_CHAR)
            {
                break;
            }
        }
        else if (lexer->current_char == quote_char)
        {
            break;
        }
        else if (lexer->current_char == NULL_CHAR)
        {
            break;
        }

        DynStringAddCharAt(str, chars_found, lexer->current_char);
        chars_found++;
        advanceChar(lexer);
    }

    char *ret_val = str->value;
    free(str); // only free the pointer to the DynString, not the
               // DynString->value
    return ret_val;
}

static inline bool isWhiteSpaceOrBreak(char c)
{
    return c == SPACE_CHAR || c == TAB_CHAR || c == NEWLINE_CHAR ||
           c == CARRIAGE_CHAR || c == NULL_CHAR;
}

static bool isDocStart(YAMLLexer *lexer)
{
    return lexer->current_char == DASH_MINUS_CHAR &&
           peek(lexer, 0) == DASH_MINUS_CHAR &&
           peek(lexer, 1) == DASH_MINUS_CHAR &&
           isWhiteSpaceOrBreak(peek(lexer, 3));
}

static bool isEndOfDocStart(YAMLLexer *lexer)
{
    return lexer->current_char == DOT_CHAR && peek(lexer, 0) == DOT_CHAR &&
           peek(lexer, 1) == DOT_CHAR && isWhiteSpaceOrBreak(peek(lexer, 3));
}

extern YAMLToken *YAMLLex(YAMLLexer *lexer)
{
    // Log(ERROR, "lexer->state = %d\n", lexer->state);
    if (lexer == NULL)
    {
        return NULL;
    }
    YAMLToken *token = NULL;
    u_int32_t curr_pos = lexer->cursor;
    if (lexer->current_char == NULL_CHAR)
    {
        lexer->state = YAMLLexerStateFoundEOFNeedToPopRemainingDedent;
        // token = YAMLTokenInit(YAMLTokenEOF, curr_pos, lexer->cursor + 1,
        //                       lexer->line, NULL);
    }

    if (lexer->state == YAMLLexerStateJustGotNewline)
    {
        if (lexer->current_char == SPACE_CHAR)
        {
            // Log(DEBUG, "here");
            advanceChar(lexer);
            lexer->space_count++;
        }
        else if (lexer->current_char == TAB_CHAR)
        {
            Log(FATAL, "tab is not supported WIP");
        }
        else if (lexer->current_char == '#')
        {
            handleComment(lexer);
        }
        else if (lexer->current_char == NEWLINE_CHAR)
        {
            lexer->state = YAMLLexerStateJustGotNewline;
            advanceChar(lexer);
            return YAMLTokenInit(YAMLTokenNewline, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
        }
        else
        {
            // Log(DEBUG, "here");
            int top_of_stack_value =
                *(int *)ListGetFirst(lexer->indent_stack)->value;
            if (lexer->space_count > top_of_stack_value)
            {
                int *new_top = malloc(sizeof(int));
                *new_top = lexer->space_count;
                Item *new_top_item = ItemInit(new_top, &ItemValueIntOperations);
                ListAddFirst(lexer->indent_stack, new_top_item);

                lexer->space_count = 0;

                resetLexerState(lexer);
                // advanceChar(lexer);
                return YAMLTokenInit(YAMLTokenIndent, curr_pos,
                                     lexer->cursor + 1, lexer->line, NULL);
            }
            else if (lexer->space_count < top_of_stack_value)
            {
                lexer->state = YAMLLexerStatePopDedent;
            }
            else
            {
                Log(DEBUG, "%d", __LINE__);
                resetLexerState(lexer);
            }
        }
    }
    else if (lexer->state == YAMLLexerStatePopDedent ||
             lexer->state == YAMLLexerStateFoundEOFNeedToPopRemainingDedent)
    {
        if (lexer->indent_stack->size > 1)
        {
            assert(lexer->indent_stack->items[0] != NULL);

            Item *dedent_item = ListPopFirst(lexer->indent_stack);
            int dedent_item_value = *(int *)dedent_item->value;
            if (dedent_item_value > lexer->space_count)
            {
                return YAMLTokenInit(YAMLTokenDedent, curr_pos,
                                     lexer->cursor + 1, lexer->line, NULL);
            }
            else
            {
                Log(DEBUG, "idk fam");
            }
            ItemFree(dedent_item);
        }
        else if (lexer->state == YAMLLexerStateFoundEOFNeedToPopRemainingDedent)
        {
            resetLexerState(lexer);
            return YAMLTokenInit(YAMLTokenEOF, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
        }
        else
        {
            int minumum_indent =
                *(int *)ListGetFirst(lexer->indent_stack)->value;
            if (minumum_indent != lexer->space_count)
            {
                Log(FATAL, "HUH");
            }
            lexer->space_count = 0;
            resetLexerState(lexer);
        }
    }
    else if (lexer->state == YAMLLexerStateNormal)
    {
        if (lexer->current_char == NEWLINE_CHAR)
        {
            // move past
            advanceChar(lexer);
            return YAMLTokenInit(YAMLTokenNewline, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
            lexer->state = YAMLLexerStateJustGotNewline;
        }
        else if (isDocStart(lexer))
        {
            advanceChar(lexer);
            advanceChar(lexer);

            // move past
            advanceChar(lexer);
            return YAMLTokenInit(YAMLTokenStartOfDocument, curr_pos,
                                 lexer->cursor + 1, lexer->line, NULL);
        }
        else if (isEndOfDocStart(lexer))
        {
            advanceChar(lexer);
            advanceChar(lexer);

            // move past
            advanceChar(lexer);
            return YAMLTokenInit(YAMLTokenEndOfDocument, curr_pos,
                                 lexer->cursor + 1, lexer->line, NULL);
        }
        else if (lexer->current_char == COLON_CHAR)
        {
            // move past last for next run
            if (isWhiteSpaceOrBreak(peek(lexer, 1)))
            {
                if (peek(lexer, 1) == NEWLINE_CHAR)
                {
                    lexer->state = YAMLLexerStateJustGotNewline;
                    advanceChar(lexer); // go past colon
                }
                else
                {
                    advanceChar(lexer); // go past colon
                    advanceChar(lexer); // go past space
                }

                return YAMLTokenInit(YAMLTokenValueIndicator, curr_pos,
                                     lexer->cursor, lexer->line, NULL);
            }
            else
            {
                advanceChar(lexer);
                return YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->cursor,
                                     lexer->line, NULL);
            }
        }
        else
        {
            char *scalar = eatScalar(lexer);
            return YAMLTokenInit(YAMLTokenScalar, curr_pos, lexer->cursor,
                                 lexer->line, scalar);
        }
    }

    return token;
}

extern void YAMLLexerDebugTest(char *file_name)
{
    // char *file_name = "./testfiles/playground.yaml";
    FILE *file_ptr = fopen(file_name, "rb");

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);

    while (true)
    {
        YAMLToken *token = YAMLLex(lexer);
        YAMLTokenPrint(token);
        if (token != NULL && token->type == YAMLTokenEOF)
        {
            break;
        }
    }
    YAMLLexerFree(lexer);
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
        printf("%s", YAMLTokenTypeToString(token->type));
        if (token->literal)
        {
            printf(":%s\n", token->literal);
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
    else if (type == YAMLTokenScalar)
    {
        return "YAMLTokenScalar";
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

[[maybe_unused]] static void printchar(unsigned char c)
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
