#include <_stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <standardloop/collections.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>
#include <sys/_types/_u_int8_t.h>

#include "./yaml.h"

static void checkOtherBlockScalarOptions(YAMLLexer *lexer);

// this function is silly, but serves as documentation in the code
static inline void maintainLexerState(YAMLLexer *lexer,
                                      enum YAMLLexerState state)
{
    assert(lexer->state == state);
}

[[maybe_unused]] static void printchar(unsigned char c);

static bool isInFlow(YAMLLexer *lexer)
{
    return lexer->flow_stack->size > 0;
}

static YAMLToken *handleYAMLLexerStateJustGotNewline(YAMLLexer *lexer);

static void advanceChar(YAMLLexer *lexer);
static void resetLexerState(YAMLLexer *lexer);

static void handleComment(YAMLLexer *lexer)
{
    assert(lexer->current_char == '#');

    while (lexer->current_char != NULL_CHAR &&
           lexer->current_char != NEWLINE_CHAR)
    {
        advanceChar(lexer);
    }
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
        // printf("reloading!");
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

    // If target position extends past loaded bytes, attempt to top-off the
    // buffer
    while (target_pos >= lexer->bytes_in_buffer && !lexer->eof_reached)
    {
        size_t space_left = BUFFER_SIZE - lexer->bytes_in_buffer;

        // Cannot fit more lookahead in the buffer window
        if (space_left == 0)
        {
            break;
        }
        // Read into the unused trailing space of the buffer
        size_t read_bytes = fread(lexer->buffer + lexer->bytes_in_buffer, 1,
                                  space_left, lexer->file_ptr);

        lexer->bytes_in_buffer += read_bytes;

        if (read_bytes == 0)
        {
            lexer->eof_reached = true;
        }
    }
    // Return character if target is within loaded bounds, else EOF
    if (target_pos < lexer->bytes_in_buffer)
    {
        return lexer->buffer[target_pos];
    }

    return NULL_CHAR;
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

static void resetLexerBlockScalarOptions(YAMLLexer *lexer)
{
    assert(lexer != NULL);
    lexer->block_scalar_options.enabled = false;
    lexer->block_scalar_options.style = BlockScalarStyleLiteral;
    lexer->block_scalar_options.chomping = BlockScalarStyleClip;
}

extern YAMLLexer *YAMLLexerInit(FILE *file_ptr)
{
    assert(file_ptr != NULL);
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

    lexer->flow_stack = ListInitDefault();
    lexer->space_count = 0;
    lexer->state = YAMLLexerStateJustGotNewline;
    resetLexerBlockScalarOptions(lexer);
    return lexer;
}

static void blockScalarPostProcess(enum BlockScalarChomping chomping,
                                   DynString *str)
{
    assert(str != NULL);
    if (chomping == BlockScalarStyleKeep)
    {
        return;
    }
    size_t i = str->size - 1;
    for (; i >= 0; i--)
    {
        if (str->value[i] == NEWLINE_CHAR)
        {
            break;
        }
    }
    if (str->value[i] != NEWLINE_CHAR)
    {
        Log(FATAL, "%d", __LINE__);
    }

    while (str->value[i] == NEWLINE_CHAR)
    {
        str->value[i] = NULL_CHAR;
        i--;
    }
    if (chomping == BlockScalarStyleClip)
    {
        // keep the last one
        i++;
        str->value[i] = NEWLINE_CHAR;
    }
}

static char *eatBlockScalar(YAMLLexer *lexer)
{
    assert(lexer != NULL);
    assert(lexer->current_char == NEWLINE_CHAR);
    assert(lexer->block_scalar_options.enabled == true);
    assert(lexer->block_scalar_options.style == BlockScalarStyleLiteral ||
           lexer->block_scalar_options.style == BlockScalarStyleFolded);

    advanceChar(lexer); // move past newline

    bool just_got_newline = false;
    bool is_first_itr = true;

    DynString *str = DynStringDefaultInit();
    size_t chars_found = 0;

    int parent_indent = *(int *)ListGetFirst(lexer->indent_stack)->value;

    // skip leading newlines
    while (lexer->current_char == NEWLINE_CHAR)
    {
        DynStringAddCharAt(str, chars_found, lexer->current_char);
        chars_found++;
        advanceChar(lexer);
    }

    int amount_spaces_before_content = 0;

    if (lexer->block_scalar_options.explicit_indent != 0)
    {
        amount_spaces_before_content =
            parent_indent + lexer->block_scalar_options.explicit_indent;
        just_got_newline = true;
        is_first_itr = false;
    }
    else
    {
        // first line with space indent is our base indent
        assert(lexer->block_scalar_options.explicit_indent == 0);
        while (lexer->current_char == SPACE_CHAR)
        {
            amount_spaces_before_content++;
            advanceChar(lexer);
        }
    }

    // base space indent should always be more than the parents
    if (amount_spaces_before_content < parent_indent)
    {
        Log(FATAL, "%d", __LINE__);
    }

    // Log(DEBUG, "%d", first_list_count_space);

    while (ALWAYS)
    {
        if (lexer->current_char == NULL_CHAR)
        {
            // found_end = true;
            break;
        }

        if (just_got_newline && !is_first_itr)
        {
            while (lexer->current_char == NEWLINE_CHAR)
            {
                DynStringAddCharAt(str, chars_found, lexer->current_char);
                chars_found++;
                advanceChar(lexer);
            }
            just_got_newline = false;
            if (lexer->current_char != SPACE_CHAR)
            {
                break;
            }
            else
            {
                for (int i = 0; i < amount_spaces_before_content; i++)
                {
                    if (lexer->current_char != SPACE_CHAR)
                    {
                        // printchar(lexer->current_char);
                        // DynStringPrint(str);
                        // TODO
                        // according to yaml spec, we need to just skip over
                        // this?
                        Log(FATAL, "%d", __LINE__);
                    }
                    advanceChar(lexer);
                }
            }
        }
        is_first_itr = false;
        DynStringAddCharAt(str, chars_found, lexer->current_char);
        chars_found++;
        advanceChar(lexer);

        just_got_newline = lexer->current_char == NEWLINE_CHAR;
    }
    blockScalarPostProcess(lexer->block_scalar_options.chomping, str);
    // post processing now
    resetLexerBlockScalarOptions(lexer);
    char *ret_val = str->value;
    free(str); // only free the pointer to the DynString, not the
               // DynString->value
    return ret_val;
}

static char *eatScalar(YAMLLexer *lexer)
{
    assert(lexer != NULL);

    DynString *str = DynStringDefaultInit();
    char quote_char = NULL_CHAR;

    if (lexer->current_char == '\"' || lexer->current_char == '\'')
    {
        quote_char = lexer->current_char;
        advanceChar(lexer);
    }
    bool in_quotes = quote_char != NULL_CHAR;

    size_t chars_found = 0;
    while (ALWAYS)
    {
        if (lexer->current_char == NULL_CHAR)
        {
            break;
        }
        if (isInFlow(lexer) && !in_quotes)
        {
            if (lexer->current_char == COMMA_CHAR ||
                lexer->current_char == BRACKET_CLOSE_CHAR ||
                lexer->current_char == CURLY_CLOSE_CHAR)
            {
                break;
            }
        }
        if (!in_quotes)
        {
            if (lexer->current_char == SPACE_CHAR && peek(lexer, 1) == '#')
            {
                advanceChar(lexer);
                break;
            }
            else if (lexer->current_char == COLON_CHAR &&
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
        else if (in_quotes && lexer->current_char == '\\')
        {
            advanceChar(lexer);
        }
        else if (in_quotes && lexer->current_char == quote_char)
        {
            if (quote_char == SINGLE_QUOTES_CHAR &&
                peek(lexer, 1) == SINGLE_QUOTES_CHAR)
            {
                advanceChar(lexer);
                // Log(DEBUG, "TODO");
            }
            else
            {
                advanceChar(lexer);
                break;
            }
        }

        DynStringAddCharAt(str, chars_found, lexer->current_char);
        chars_found++;
        advanceChar(lexer);
    }

    DynStringTrimEnd(str);
    char *ret_val = str->value;
    free(str); // only free the pointer to the DynString, not the
               // DynString->value
    return ret_val;
}

static YAMLToken *handleBlockScalar(YAMLLexer *lexer)
{
    assert(lexer->current_char == '|' || lexer->current_char == '>');
    u_int32_t curr_pos = lexer->cursor;
    lexer->block_scalar_options.style = lexer->current_char;
    lexer->block_scalar_options.enabled = true;
    advanceChar(lexer);

    // will need to throughly test this part
    checkOtherBlockScalarOptions(lexer);

    if (lexer->current_char != NEWLINE_CHAR &&
        lexer->current_char != SPACE_CHAR && lexer->current_char != '#')
    {
        return YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->cursor,
                             lexer->line, NULL);
    }
    else
    {
        while (lexer->current_char == SPACE_CHAR)
        {
            advanceChar(lexer);
        }
        if (lexer->current_char == '#')
        {
            handleComment(lexer);
        }
    }
    assert(lexer->current_char == NEWLINE_CHAR ||
           lexer->current_char == NULL_CHAR);

    if (lexer->current_char == NULL_CHAR)
    {
        Log(FATAL, "%d", __LINE__);
    }
    char *scalar = eatBlockScalar(lexer);
    return YAMLTokenInit(YAMLTokenScalar, curr_pos, lexer->cursor, lexer->line,
                         scalar);
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

extern YAMLToken *
handleYAMLLexerStateFoundEOFNeedToPopRemainingDedent(YAMLLexer *lexer)
{
    u_int32_t curr_pos = lexer->cursor;
    assert(lexer->state == YAMLLexerStateFoundEOFNeedToPopRemainingDedent);
    if (lexer->indent_stack->size == 1)
    {
        return YAMLTokenInit(YAMLTokenEOF, curr_pos, lexer->cursor + 1,
                             lexer->line, NULL);
    }
    else
    {
        Item *dedent_item = ListPopFirst(lexer->indent_stack);
        ItemFree(dedent_item);
        // if (dedent_item_value > lexer->space_count)
        // {

        lexer->state =
            YAMLLexerStateFoundEOFNeedToPopRemainingDedent; // maintain
                                                            // state
        return YAMLTokenInit(YAMLTokenDedent, curr_pos, lexer->cursor + 1,
                             lexer->line, NULL);
        // }
    }
}

static bool isExplicitIndentCharNumber(char c)
{
    return c == '1' || c == '2' || c == '3' || c == '4' || c == '5' ||
           c == '6' || c == '7' || c == '8' || c == '9';
}

static void peekForChompingOptions(YAMLLexer *lexer)
{
    lexer->block_scalar_options.chomping = BlockScalarStyleClip;
    if (lexer->current_char == '-' || lexer->current_char == '+')
    {
        lexer->block_scalar_options.chomping = lexer->current_char;
    }
    if (peek(lexer, 1) == '-' || peek(lexer, 1) == '+')
    {
        if (lexer->block_scalar_options.chomping != BlockScalarStyleClip)
        {
            Log(FATAL, "double chomping option...");
        }
        lexer->block_scalar_options.chomping = lexer->current_char;
    }
}

static void peekForIndentOptions(YAMLLexer *lexer)
{
    lexer->block_scalar_options.explicit_indent = 0;
    if (isExplicitIndentCharNumber(lexer->current_char))
    {
        lexer->block_scalar_options.explicit_indent = lexer->current_char - '0';
    }
    if (isExplicitIndentCharNumber(peek(lexer, 1)))
    {
        if (lexer->block_scalar_options.explicit_indent != 0)
        {
            Log(FATAL, "duplicate number option found");
        }
        lexer->block_scalar_options.explicit_indent = peek(lexer, 1) - '0';
    }
}

static void checkOtherBlockScalarOptions(YAMLLexer *lexer)
{
    peekForChompingOptions(lexer);
    peekForIndentOptions(lexer);

    if (lexer->block_scalar_options.explicit_indent != 0)
    {
        advanceChar(lexer);
    }
    if (lexer->block_scalar_options.chomping != BlockScalarStyleClip)
    {
        advanceChar(lexer);
    }
}

static void skipSpaceAndNewline(YAMLLexer *lexer)
{
    while (lexer->current_char == SPACE_CHAR ||
           lexer->current_char == NEWLINE_CHAR)
    {
        advanceChar(lexer);
    }
}

static YAMLToken *handleYAMLLexerStateNormal(YAMLLexer *lexer)
{
    assert(lexer->state == YAMLLexerStateNormal);
    u_int32_t curr_pos = lexer->cursor;

    if (isInFlow(lexer))
    {
        skipSpaceAndNewline(lexer);
        // fall though
    }

    if (lexer->current_char == NEWLINE_CHAR)
    {
        assert(!isInFlow(lexer));

        // move past
        advanceChar(lexer);
        lexer->state = YAMLLexerStateJustGotNewline;
        return YAMLTokenInit(YAMLTokenNewline, curr_pos, lexer->cursor + 1,
                             lexer->line, NULL);
    }

    if (isDocStart(lexer))
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
    else if (lexer->current_char == DASH_MINUS_CHAR)
    {
        // Log(FATAL, "%s", lexer->buffer);
        if (peek(lexer, 1) == SPACE_CHAR)
        {
            advanceChar(lexer);
            advanceChar(lexer);
            return YAMLTokenInit(YAMLTokenListDash, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
        }
        else
        {
            advanceChar(lexer);
            Log(DEBUG, "%d", __LINE__);
            return YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
        }
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
                maintainLexerState(lexer, YAMLLexerStateNormal);
                advanceChar(lexer); // go past colon
                advanceChar(lexer); // go past space
            }
            // if (lexer->current_char == '#')
            // {
            //     handleComment(lexer);
            // }

            return YAMLTokenInit(YAMLTokenValueIndicator, curr_pos,
                                 lexer->cursor, lexer->line, NULL);
        }
        else
        {
            advanceChar(lexer);
            Log(DEBUG, "%d", __LINE__);
            return YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->cursor,
                                 lexer->line, NULL);
        }
    }
    else if (lexer->current_char == COMMA_CHAR)
    {
        if (!isInFlow(lexer))
        {
            Log(FATAL, "%d", __LINE__);
        }
        advanceChar(lexer);
        skipSpaceAndNewline(lexer);

        return YAMLTokenInit(YAMLTokenFlowEntry, curr_pos, lexer->cursor,
                             lexer->line, NULL);
    }
    else if (lexer->current_char == BRACKET_OPEN_CHAR)
    {
        // push char
        char *flow_entry_char = malloc(sizeof(char));
        *flow_entry_char = BRACKET_OPEN_CHAR;
        Item *flow_entry =
            ItemInit(flow_entry_char, &ItemValueStringOperations);
        ListAddFirst(lexer->flow_stack, flow_entry);

        advanceChar(lexer);
        skipSpaceAndNewline(lexer);

        return YAMLTokenInit(YAMLTokenFlowSequenceStart, curr_pos,
                             lexer->cursor, lexer->line, NULL);
        // Log(FATAL, "%d", __LINE__);
    }
    else if (lexer->current_char == BRACKET_CLOSE_CHAR)
    {
        if (lexer->flow_stack == 0)
        {
            Log(FATAL, "%d", __LINE__);
        }
        else
        {
            if (*(char *)ListGetFirst(lexer->flow_stack)->value !=
                BRACKET_OPEN_CHAR)
            {
                Log(FATAL, "%d", __LINE__);
            }
            else
            {
                ItemFree(ListPopFirst(lexer->flow_stack));
            }
        }
        advanceChar(lexer);
        while (lexer->current_char == SPACE_CHAR)
        {
            advanceChar(lexer);
        }
        if (isInFlow(lexer))
        {
            skipSpaceAndNewline(lexer);
        }

        return YAMLTokenInit(YAMLTokenFlowSequenceEnd, curr_pos, lexer->cursor,
                             lexer->line, NULL);
        // Log(FATAL, "%d", __LINE__);
    }
    else if (lexer->current_char == CURLY_OPEN_CHAR)
    {
        char *flow_entry_char = malloc(sizeof(char));
        *flow_entry_char = CURLY_OPEN_CHAR;
        Item *flow_entry =
            ItemInit(flow_entry_char, &ItemValueStringOperations);
        ListAddFirst(lexer->flow_stack, flow_entry);
        advanceChar(lexer);
        skipSpaceAndNewline(lexer);
        return YAMLTokenInit(YAMLTokenFlowMappingStart, curr_pos, lexer->cursor,
                             lexer->line, NULL);
    }
    else if (lexer->current_char == CURLY_CLOSE_CHAR)
    {
        if (lexer->flow_stack == 0)
        {
            Log(FATAL, "%d", __LINE__);
        }
        else
        {
            if (*(char *)ListGetFirst(lexer->flow_stack)->value !=
                CURLY_OPEN_CHAR)
            {
                Log(FATAL, "%d", __LINE__);
            }
            else
            {
                ItemFree(ListPopFirst(lexer->flow_stack));
            }
        }
        advanceChar(lexer);
        while (lexer->current_char == SPACE_CHAR)
        {
            advanceChar(lexer);
        }
        if (isInFlow(lexer))
        {
            skipSpaceAndNewline(lexer);
        }

        return YAMLTokenInit(YAMLTokenFlowMappingEnd, curr_pos, lexer->cursor,
                             lexer->line, NULL);
    }
    else if (lexer->current_char == '|' || lexer->current_char == '>')
    {
        if (isInFlow(lexer))
        {
            advanceChar(lexer);
            return YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->cursor,
                                 lexer->line, NULL);
        }
        return handleBlockScalar(lexer);
    }
    else if (lexer->current_char == AND_CHAR)
    {
        Log(FATAL, "TODO %d", __LINE__);
        advanceChar(lexer);
        char *scalar = eatScalar(lexer);
        // lexer->state = YAMLLexerStateJustGotNewline;
        return YAMLTokenInit(YAMLTokenAnchor, curr_pos, lexer->cursor,
                             lexer->line, scalar);
    }
    else if (lexer->current_char == '*')
    {
        Log(FATAL, "TODO %d", __LINE__);
        advanceChar(lexer);
        char *scalar = eatScalar(lexer);
        // lexer->state = YAMLLexerStateJustGotNewline;
        return YAMLTokenInit(YAMLTokenAlias, curr_pos, lexer->cursor,
                             lexer->line, scalar);
    }
    else if (lexer->current_char == QUESTION_CHAR)
    {
        advanceChar(lexer);
        return YAMLTokenInit(YAMLTokenComplexKeyIndicator, curr_pos,
                             lexer->cursor, lexer->line, NULL);
    }

    // printchar(lexer->current_char);
    char *scalar = eatScalar(lexer);
    return YAMLTokenInit(YAMLTokenScalar, curr_pos, lexer->cursor, lexer->line,
                         scalar);
}

static YAMLToken *handleYAMLLexerStatePopDedent(YAMLLexer *lexer)
{
    assert(lexer->state == YAMLLexerStatePopDedent);
    u_int32_t curr_pos = lexer->cursor;
    if (lexer->indent_stack->size == 1)
    {
        resetLexerState(lexer);
        return handleYAMLLexerStateNormal(lexer);
    }
    else
    {
        Item *dedent_item = ListPopFirst(lexer->indent_stack);
        int dedent_item_value = *(int *)dedent_item->value;
        if (dedent_item_value > lexer->space_count)
        {
            ItemFree(dedent_item);
            if (*(int *)ListGetFirst(lexer->indent_stack)->value ==
                lexer->space_count)
            {
                resetLexerState(lexer);
            }
            else
            {
                maintainLexerState(lexer, YAMLLexerStatePopDedent);
            }
            return YAMLTokenInit(YAMLTokenDedent, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
        }
        else
        {
            ItemFree(dedent_item);
            resetLexerState(lexer); //  TODO
            Log(DEBUG, "%d", __LINE__);
            return YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
        }
    }
}

static YAMLToken *handleYAMLLexerStateJustGotNewline(YAMLLexer *lexer)
{
    assert(lexer->state == YAMLLexerStateJustGotNewline);
    u_int32_t curr_pos = lexer->cursor;
    // Log(DEBUG, "here");
    if (lexer->current_char == NULL_CHAR)
    {
        lexer->state = YAMLLexerStateFoundEOFNeedToPopRemainingDedent;
        return handleYAMLLexerStateFoundEOFNeedToPopRemainingDedent(lexer);
    }
    else if (lexer->current_char == SPACE_CHAR)
    {
        // Log(DEBUG, "here");
        while (lexer->current_char == SPACE_CHAR)
        {
            lexer->space_count++;
            advanceChar(lexer);
        }
        if (lexer->current_char == '#')
        {
            lexer->space_count = 0;
            handleComment(lexer);
            if (lexer->current_char == NULL_CHAR)
            {
                lexer->state = YAMLLexerStateFoundEOFNeedToPopRemainingDedent;
                return handleYAMLLexerStateFoundEOFNeedToPopRemainingDedent(
                    lexer);
            }
            else if (lexer->current_char == NEWLINE_CHAR)
            {
                lexer->state = YAMLLexerStateJustGotNewline;
                // advanceChar(lexer); // do we need this
                return handleYAMLLexerStateJustGotNewline(lexer);
            }
            else
            {
                Log(FATAL, "%d", __LINE__);
            }
        }
        else
        {
            // fall through
        }
    }

    if (lexer->current_char == TAB_CHAR)
    {
        Log(DEBUG, "%d", __LINE__);
        resetLexerState(lexer);
        return YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->cursor + 1,
                             lexer->line, NULL);
    }
    else if (lexer->current_char == NEWLINE_CHAR)
    {
        lexer->state = YAMLLexerStateJustGotNewline;
        advanceChar(lexer);
        return YAMLTokenInit(YAMLTokenNewline, curr_pos, lexer->cursor + 1,
                             lexer->line, NULL);
    }
    else if (isInFlow(lexer))
    {
        lexer->space_count = 0;
        resetLexerState(lexer);
        return handleYAMLLexerStateNormal(lexer);
    }
    else
    {
        // Log(DEBUG, "%d", __LINE__);
        int top_of_stack_value =
            *(int *)ListGetFirst(lexer->indent_stack)->value;
        if (lexer->space_count == top_of_stack_value)
        {
            // Log(DEBUG, "same indent %d", __LINE__);
            lexer->space_count = 0;
            resetLexerState(lexer);
            return handleYAMLLexerStateNormal(lexer);
        }
        else if (lexer->space_count > top_of_stack_value)
        {
            // Log(DEBUG, "%d", __LINE__);
            int *new_top = malloc(sizeof(int));
            *new_top = lexer->space_count;
            Item *new_top_item = ItemInit(new_top, &ItemValueIntOperations);
            ListAddFirst(lexer->indent_stack, new_top_item);
            // ListPrint(lexer->indent_stack);

            lexer->space_count = 0;

            resetLexerState(lexer);
            // advanceChar(lexer);
            return YAMLTokenInit(YAMLTokenIndent, curr_pos, lexer->cursor + 1,
                                 lexer->line, NULL);
        }
        else if (lexer->space_count < top_of_stack_value)
        {
            // Log(DEBUG, "%d", lexer->space_count);
            if (lexer->indent_stack->size > 1)
            {
                lexer->state = YAMLLexerStatePopDedent;
                return handleYAMLLexerStatePopDedent(lexer);
            }
            else
            {
                resetLexerState(lexer);
                return handleYAMLLexerStateNormal(lexer);
            }
        }
        else
        {
            Log(FATAL, "IMPOSSIBLE %d", __LINE__);
        }
    }
    Log(FATAL, "IMPOSSIBLE %d", __LINE__);
    return NULL;
}

extern YAMLToken *YAMLLex(YAMLLexer *lexer)
{
    assert(lexer != NULL);
    if (lexer->current_char == '#')
    {
        // Log(DEBUG, "found a comment!");
        handleComment(lexer);
        // putchar(lexer->current_char);
        // if (lexer->current_char == NEWLINE_CHAR)
        // {
        //     advanceChar(lexer);
        //     lexer->state = YAMLLexerStateJustGotNewline;
        // }
        if (lexer->current_char == NULL_CHAR)
        {
            // written below, but serves as documentation here for now
            lexer->state = YAMLLexerStateFoundEOFNeedToPopRemainingDedent;
        }
    }
    else if (lexer->current_char == NULL_CHAR)
    {
        // Log(DEBUG, "%d", __LINE__);
        lexer->state = YAMLLexerStateFoundEOFNeedToPopRemainingDedent;
    }

    if (lexer->state == YAMLLexerStateFoundEOFNeedToPopRemainingDedent)
    {
        return handleYAMLLexerStateFoundEOFNeedToPopRemainingDedent(lexer);
    }
    else if (lexer->state == YAMLLexerStateJustGotNewline)
    {
        return handleYAMLLexerStateJustGotNewline(lexer);
    }
    else if (lexer->state == YAMLLexerStatePopDedent)
    {
        return handleYAMLLexerStatePopDedent(lexer);
    }
    else if (lexer->state == YAMLLexerStateNormal)
    {
        return handleYAMLLexerStateNormal(lexer);
    }

    return NULL;
}

extern void YAMLLexerDebugTest(char *file_name)
{
    // char *file_name = "./testfiles/playground.yaml";
    FILE *file_ptr = fopen(file_name, "rb");
    if (file_ptr == NULL)
    {
        Log(FATAL, "file_ptr is NULL in %s", __FUNCTION__);
    }

    YAMLLexer *lexer = YAMLLexerInit(file_ptr);

    while (true)
    {
        YAMLToken *token = YAMLLex(lexer);
        YAMLTokenPrint(token);
        if (token == NULL)
        {
            Log(FATAL, "null token...");
        }
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
            printf(":");
            size_t literal_len = strlen(token->literal);
            for (size_t i = 0; i < literal_len; i++)
            {
                printchar(token->literal[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\n");
        }
    }
    fflush(stdout);
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
    else if (type == YAMLTokenFlowEntry)
    {
        return "YAMLTokenFlowEntry";
    }
    else if (type == YAMLTokenAlias)
    {
        return "YAMLTokenAlias";
    }
    else if (type == YAMLTokenAnchor)
    {
        return "YAMLTokenAnchor";
    }
    else if (type == YAMLTokenComplexKeyIndicator)
    {
        return "YAMLTokenComplexKeyIndicator";
    }
    else if (type == YAMLTokenTag)
    {
        return "YAMLTokenTag";
    }
    else if (type == YAMLTokenAT)
    {
        return "YAMLTokenAT";
    }
    else if (type == YAMLTokenBacktick)
    {
        return "YAMLTokenBacktick";
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
        printf("\\n");
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
