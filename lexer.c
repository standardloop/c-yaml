#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <strings.h>
#include <assert.h>

#include <standardloop/util.h>
#include <standardloop/logger.h>

#include "./yaml.h"

static void advanceChar(YAMLLexer *);
static void backtrackChar(YAMLLexer *);
static bool isAtEndOfLexerInput(YAMLLexer *);
static char *captureValueOrKey(YAMLLexer *);
static char *createStringLiteral(YAMLLexer *, size_t);
static void resetLexerState(YAMLLexer *);
static void handleComment(YAMLLexer *);
static bool checkIfGettingHungry(YAMLLexer *);
static bool isAllowedChompingNumber(char);
static void resetLexerDashes(YAMLLexer *);
static void resetLexerDots(YAMLLexer *);

static bool isAllowedChompingNumber(char to_check)
{
    int to_check_as_int = to_check - '0';
    return (to_check_as_int >= 1 && to_check_as_int <= 9);
}

static bool checkIfGettingHungry(YAMLLexer *lexer)
{
    return lexer->current_char == NULL_CHAR && !lexer->is_last_chunk;
}

static void resetLexerState(YAMLLexer *lexer)
{
    assert(lexer != NULL);
    lexer->state = YAMLLexerStateNormal;
}

static void resetLexerDashes(YAMLLexer *lexer)
{
    lexer->sequential_dashes = 0;
}

static void resetLexerDots(YAMLLexer *lexer)
{
    lexer->sequential_dots = 0;
}

extern bool IsLexerHungry(YAMLLexer *lexer)
{
    assert(lexer != NULL);
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
YAMLLexerReload(YAMLLexer *lexer, char *buffer, size_t size, bool is_last_chunk)
{
    if (lexer->temp_input != NULL && lexer->temp_input_len > 0)
    {
        // printf("temp buffer: ");
        // PrintBuffer(lexer->temp_input, lexer->temp_input_len, true);
        // printf("\n");
        // printf("incoming buffer: ");
        // PrintBuffer(buffer, size, true);
        // printf("\n");
        size_t new_size = lexer->temp_input_len + size + 1; // -1 for extra null char?
        char *larger_input = calloc(new_size, sizeof(char));

        // Log(ERROR, "%s", lexer->temp_input);
        // Log(ERROR, "%s", buffer);

        // Log(ERROR, "%d", lexer->temp_input_len);
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
        larger_input[new_size - 1] = NULL_CHAR;
        free(lexer->temp_input);
        lexer->temp_input = NULL;
        lexer->temp_input_len = 0;

        lexer->input = larger_input;
        lexer->input_len = new_size;
        // printf("combined: ");
        // PrintBuffer(lexer->input, new_size, true);
        // printf("\n");

        // Log(ERROR, "%s", larger_input);
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
    // resetLexerDashes(lexer);
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

    // lexer->indent_stack = NULL;
    lexer->indent_stack = ListInitDefault();
    int *zero_int = malloc(sizeof(int));
    *zero_int = 0;
    Item *indent_zero_start = ItemInit(zero_int, NULL, NULL);
    ListAddFirst(lexer->indent_stack, indent_zero_start);

    lexer->space_count = 0;

    // lexer->state = YAMLLexerStateJustGotNewline;
    resetLexerDashes(lexer);
    resetLexerDots(lexer);
    resetLexerState(lexer);

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
    if (lexer->position > 0)
    {
        lexer->position -= 1;
        lexer->read_position -= 1;
        lexer->current_char = lexer->input[lexer->position];
    }
}

static void handleComment(YAMLLexer *lexer)
{
    bool did_break = false;
    do
    {
        advanceChar(lexer);
        if (checkIfGettingHungry(lexer))
        {
            lexer->hungry = true;
            lexer->state = YAMLLexerStateEatingComment;
            did_break = true;
            break;
        }
    } while (lexer->current_char != NULL_CHAR && lexer->current_char != NEWLINE_CHAR);

    if (!did_break)
    {
        if (lexer->current_char == NEWLINE_CHAR)
        {
            backtrackChar(lexer);
        }
        resetLexerState(lexer);
    }
}

static char *captureValueOrKey(YAMLLexer *lexer)
{
    size_t start_position = lexer->position;
    // char prev_char = lexer->current_char;
    // bool is_error = false;
    if (lexer->state == YAMLLexerStateInDoubleQuotes || lexer->state == YAMLLexerStateInSingleQuotes)
    {
        advanceChar(lexer);
    }
    bool broke_with_matching_quotes = false;

    while (ALWAYS)
    {
        if (lexer->current_char == NULL_CHAR)
        {
            break;
        }

        if (lexer->state == YAMLLexerStateNormal)
        {
            if (lexer->current_char == NEWLINE_CHAR || lexer->current_char == COLON_CHAR)
            {
                break;
            }
            else if (lexer->current_char == SPACE_CHAR)
            {
                advanceChar(lexer);
                if (checkIfGettingHungry(lexer))
                {
                    return createStringLiteral(lexer, start_position);
                }
                if (lexer->current_char == '#')
                {
                    backtrackChar(lexer);
                    break;
                }
                else
                {
                    continue;
                }
            }
        }

        if (lexer->state == YAMLLexerStateInDoubleQuotes && lexer->current_char == DOUBLE_QUOTES_CHAR)
        {
            advanceChar(lexer);
            broke_with_matching_quotes = true;
            break;
        }
        else if (lexer->state == YAMLLexerStateInSingleQuotes && lexer->current_char == SINGLE_QUOTES_CHAR)
        {
            advanceChar(lexer);
            broke_with_matching_quotes = true;
            break;
        }

        advanceChar(lexer);
    }

    if (broke_with_matching_quotes)
    {
        resetLexerState(lexer);
    }

    return createStringLiteral(lexer, start_position);
}

static char *createStringLiteral(YAMLLexer *lexer, size_t start_position)
{
    u_int32_t string_literal_size = (lexer->position - start_position) + 1;
    char *string_literal = malloc(sizeof(char) * string_literal_size);
    if (string_literal == NULL)
    {
        return NULL;
    }
    copyString(lexer->input, string_literal, string_literal_size, start_position);
    string_literal[string_literal_size - 1] = NULL_CHAR;
    return string_literal;
}

extern YAMLToken *YAMLLex(YAMLLexer *lexer)
{

    if (isAtEndOfLexerInput(lexer))
    {
        pass;
        // lexer->hungry = true;
        // return NULL;
    }

    // Log(DEBUG, "%c", lexer->current_char);
    //  if (lexer->current_char == NULL_CHAR)
    //  {
    //      return NULL;
    //  }
    if (lexer->state != YAMLLexerStatePopDedent && lexer->state != YAMLLexerStateFoundEOFNeedToPopRemainingDedent && lexer->state != YAMLLexerStateFoundEOFNeedToPopRemainingDedent)
    {
        advanceChar(lexer);
    }
    u_int32_t curr_pos = lexer->position;
    YAMLToken *token = NULL;

    if (lexer->state == YAMLLexerStateJustGotNewline)
    {
        if (lexer->current_char == SPACE_CHAR)
        {
            lexer->space_count++;
            // return NULL;
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
            token = YAMLTokenInit(YAMLTokenNewline, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else if (lexer->current_char == '#')
        {
            Log(DEBUG, "TODO");
        }
        else
        {
            int top_of_stack_value = *(int *)ListGetFirst(lexer->indent_stack)->value;
            if (lexer->space_count > top_of_stack_value)
            {
                int *new_top = malloc(sizeof(int));
                *new_top = lexer->space_count;
                Item *new_top_item = ItemInit(new_top, NULL, NULL);
                ListAddFirst(lexer->indent_stack, new_top_item);

                lexer->space_count = 0;

                token = YAMLTokenInit(YAMLTokenIndent, curr_pos, lexer->position + 1, lexer->line, NULL);
                resetLexerState(lexer);
                backtrackChar(lexer);
            }
            else if (lexer->space_count < top_of_stack_value)
            {
                lexer->state = YAMLLexerStatePopDedent;
                backtrackChar(lexer);
            }
            else
            {
                backtrackChar(lexer);
                resetLexerState(lexer);
            }
        }
    }
    else if (lexer->state == YAMLLexerStatePopDedent || lexer->state == YAMLLexerStateFoundEOFNeedToPopRemainingDedent)
    {
        if (lexer->indent_stack->size > 1)
        {
            assert(lexer->indent_stack->items[0] != NULL);

            Item *dedent_item = ListPopFirst(lexer->indent_stack);
            int dedent_item_value = *(int *)dedent_item->value;
            if (dedent_item_value > lexer->space_count)
            {
                token = YAMLTokenInit(YAMLTokenDedent, curr_pos, lexer->position + 1, lexer->line, NULL);
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
            token = YAMLTokenInit(YAMLTokenEOF, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            int minumum_indent = *(int *)ListGetFirst(lexer->indent_stack)->value;
            if (minumum_indent != lexer->space_count)
            {
                Log(FATAL, "HUH");
            }
            lexer->space_count = 0;
            resetLexerState(lexer);
        }
    }
    else
    {
        if (lexer->current_char != DASH_MINUS_CHAR && lexer->current_char != PLUS_CHAR && lexer->state == YAMLLexerStateWaitingForChompingDashOrPlus)
        {
            // Log(DEBUG, "only | or > was found, no |-, |+ or >-, >+... reseting lexer state");
            resetLexerState(lexer);
        }
        if (lexer->current_char == DASH_MINUS_CHAR)
        {
            lexer->sequential_dashes++;
        }
        else
        {
            resetLexerDashes(lexer);
        }

        if (lexer->current_char == DOT_CHAR)
        {
            lexer->sequential_dots++;
        }
        else
        {
            resetLexerDots(lexer);
        }

        // Log(DEBUG, "%d", lexer->current_char);
        if (lexer->current_char == NULL_CHAR)
        {
            // Log(ERROR, "%s", lexer->is_last_chunk ? "true" : "false");
            if (!lexer->is_last_chunk)
            {
                lexer->hungry = true;
            }
            else
            {
                lexer->state = YAMLLexerStateFoundEOFNeedToPopRemainingDedent;
                // token = YAMLTokenInit(YAMLTokenEOF, curr_pos, lexer->position + 1, lexer->line, NULL);
            }
        }
        else if (lexer->current_char == '#' || lexer->state == YAMLLexerStateEatingComment)
        {
            if (lexer->current_char == '#')
            {
                Log(TRACE, "Found a comment....");
            }
            else
            {
                Log(TRACE, "Continuing to munch a comment...");
            }
            handleComment(lexer);
        }
        else if (lexer->current_char == COLON_CHAR)
        {
            token = YAMLTokenInit(YAMLTokenValueIndicator, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        // I want to revist this one
        else if (lexer->current_char == SPACE_CHAR)
        {
            if (lexer->position == 0 && lexer->state == YAMLLexerStateWaitSpaceAfterDash)
            {
                token = YAMLTokenInit(YAMLTokenSpace, curr_pos, lexer->position + 1, lexer->line, NULL);
                resetLexerState(lexer);
            }
            else
            {
                if (lexer->position > 0)
                {
                    backtrackChar(lexer);
                    if (lexer->current_char == COLON_CHAR || lexer->current_char == DASH_MINUS_CHAR)
                    {
                        token = YAMLTokenInit(YAMLTokenSpace, curr_pos, lexer->position + 1, lexer->line, NULL);
                    }
                    advanceChar(lexer);
                }
                else
                {
                    Log(FATAL, "what to do?");
                }
            }
        }
        else if (lexer->current_char == NEWLINE_CHAR)
        {
            lexer->line++;
            token = YAMLTokenInit(YAMLTokenNewline, curr_pos, lexer->position + 1, lexer->line, NULL);
            lexer->state = YAMLLexerStateJustGotNewline;
        }
        else if (lexer->current_char == PLUS_CHAR)
        {
            if (lexer->state == YAMLLexerStateWaitingForChompingDashOrPlus)
            {
                token = YAMLTokenInit(YAMLTokenListKeepChomping, curr_pos, lexer->position + 1, lexer->line, NULL);
                resetLexerState(lexer);
                lexer->state = YAMLLexerStateWaitingForChompingNumber;
            }
            else
            {
                token = YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
                resetLexerState(lexer);
            }
        }
        else if (lexer->current_char == DASH_MINUS_CHAR)
        {
            // Log(DEBUG, "%d", lexer->sequential_dashes);
            assert(lexer->sequential_dashes >= 1);

            if (lexer->sequential_dashes == 3)
            {
                resetLexerState(lexer);
                token = YAMLTokenInit(YAMLTokenStartOfDocument, curr_pos, lexer->position + 1, lexer->line, NULL);
                resetLexerDashes(lexer);
            }
            else if (lexer->sequential_dashes == 2)
            {
                resetLexerState(lexer);
                advanceChar(lexer);
                if (checkIfGettingHungry(lexer))
                {
                    lexer->hungry = true;
                }
                else if (lexer->current_char == DASH_MINUS_CHAR)
                {
                    token = YAMLTokenInit(YAMLTokenStartOfDocument, curr_pos, lexer->position + 1, lexer->line, NULL);
                    resetLexerDashes(lexer);
                }
                else
                {
                    token = YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
                    resetLexerDashes(lexer);
                    backtrackChar(lexer);
                }
            }
            else if (lexer->state == YAMLLexerStateWaitingForChompingDashOrPlus)
            {
                token = YAMLTokenInit(YAMLTokenListChompingDash, curr_pos, lexer->position + 1, lexer->line, NULL);
                lexer->state = YAMLLexerStateWaitingForChompingNumber;
            }
            else
            {
                resetLexerState(lexer);
                advanceChar(lexer);
                if (checkIfGettingHungry(lexer))
                {
                    lexer->state = YAMLLexerStateWaitSpaceAfterDash;
                    lexer->hungry = true;
                }
                else if (lexer->current_char == SPACE_CHAR)
                {
                    token = YAMLTokenInit(YAMLTokenListDash, curr_pos, lexer->position + 1, lexer->line, NULL);
                    resetLexerDashes(lexer);
                    backtrackChar(lexer);
                }
                else if (lexer->current_char == DASH_MINUS_CHAR)
                {
                    backtrackChar(lexer);
                }
                else
                {
                    token = YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
                    resetLexerDashes(lexer);
                    backtrackChar(lexer);
                }
            }
        }
        else if (lexer->current_char == CURLY_OPEN_CHAR || lexer->state == YAMLLexerStateCurlyFlow)
        {
            lexer->state = YAMLLexerStateCurlyFlow;
            Log(FATAL, "TODO");
        }
        else if (lexer->current_char == CURLY_CLOSE_CHAR)
        {
            Log(FATAL, "TODO");
        }
        else if (lexer->current_char == BRACKET_OPEN_CHAR)
        {
            lexer->state = YAMLLexerStateSequenceFlow;
            token = YAMLTokenInit(YAMLTokenFlowSequenceStart, curr_pos, lexer->position + 1, lexer->line, NULL);
            Log(FATAL, "TODO");
        }
        else if (lexer->current_char == BRACKET_CLOSE_CHAR)
        {
            token = YAMLTokenInit(YAMLTokenFlowSequenceEnd, curr_pos, lexer->position + 1, lexer->line, NULL);
            Log(FATAL, "TODO");
        }
        else if (lexer->current_char == '|')
        {
            token = YAMLTokenInit(YAMLTokenLiteralBlockStart, curr_pos, lexer->position + 1, lexer->line, NULL);
            lexer->state = YAMLLexerStateWaitingForChompingDashOrPlus;
        }
        else if (lexer->current_char == '>')
        {
            token = YAMLTokenInit(YAMLTokenFoldedBlockStart, curr_pos, lexer->position + 1, lexer->line, NULL);
            lexer->state = YAMLLexerStateWaitingForChompingDashOrPlus;
        }
        else if (lexer->current_char == '*')
        {
            Log(FATAL, "TODO");
        }
        else if (lexer->current_char == '&')
        {
            Log(FATAL, "TODO");
        }
        else if (lexer->current_char == QUESTION_CHAR)
        {
            Log(FATAL, "TODO");
        }
        else if (lexer->current_char == DOT_CHAR)
        {
            assert(lexer->sequential_dots >= 1);

            if (lexer->sequential_dots == 3)
            {
                token = YAMLTokenInit(YAMLTokenEndOfDocument, curr_pos, lexer->position + 1, lexer->line, NULL);
            }
            else if (lexer->sequential_dots == 2)
            {
                resetLexerState(lexer);
                advanceChar(lexer);
                if (checkIfGettingHungry(lexer))
                {
                    lexer->hungry = true;
                }
                else if (lexer->current_char == DOT_CHAR)
                {
                    token = YAMLTokenInit(YAMLTokenEndOfDocument, curr_pos, lexer->position + 1, lexer->line, NULL);
                    resetLexerDashes(lexer);
                }
                else
                {
                    token = YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
                    resetLexerDashes(lexer);
                    backtrackChar(lexer);
                }
            }
            else if (lexer->sequential_dots == 1)
            {
                resetLexerState(lexer);
                advanceChar(lexer);
                if (checkIfGettingHungry(lexer))
                {
                    lexer->hungry = true;
                }
                else if (lexer->current_char == DOT_CHAR)
                {
                    // catch it next loop
                    backtrackChar(lexer);
                }
                else
                {
                    token = YAMLTokenInit(YAMLTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
                    resetLexerDashes(lexer);
                    backtrackChar(lexer);
                }
            }
        }
        else if (lexer->current_char == '%')
        {
            Log(FATAL, "TODO");
        }
        else
        {
            if (lexer->state == YAMLLexerStateWaitingForChompingNumber && isAllowedChompingNumber(lexer->current_char))
            {
                // Log(TRACE, "asd");
                char *chomping_number_string = malloc(sizeof(char) * 2);
                if (chomping_number_string == NULL)
                {
                    Log(FATAL, "chomping_number_string == NULL");
                }
                else
                {
                    chomping_number_string[0] = lexer->current_char;
                    chomping_number_string[1] = NULL_CHAR;
                    token = YAMLTokenInit(YAMLTokenChompingNumber, curr_pos, lexer->position + 1, lexer->line, chomping_number_string);
                    resetLexerState(lexer);
                }
            }
            else
            {
                if (lexer->current_char == DOUBLE_QUOTES_CHAR)
                {
                    lexer->state = YAMLLexerStateInDoubleQuotes;
                }
                else if (lexer->current_char == SINGLE_QUOTES_CHAR)
                {
                    lexer->state = YAMLLexerStateInSingleQuotes;
                }

                // must be a value or key
                char *value_or_key = captureValueOrKey(lexer);
                // Log(DEBUG, "%s", value_or_key);
                // PrintBuffer(value_or_key, strlen(value_or_key), true);
                // how to we know if we captured the full value?
                // did we run out of buffer?
                if (checkIfGettingHungry(lexer))
                {
                    lexer->hungry = true;
                    lexer->temp_input = value_or_key;
                    lexer->temp_input_len = strlen(value_or_key);
                    return NULL;
                }

                // if after the string there is colon, then this is a key
                if (lexer->current_char == COLON_CHAR)
                {
                    token = YAMLTokenInit(YAMLTokenKey, curr_pos, lexer->position + 1, lexer->line, value_or_key);
                    backtrackChar(lexer);
                }
                // this is a value // TODO -> we may want have a function for any whitespace here
                else if (lexer->current_char == NEWLINE_CHAR || lexer->current_char == SPACE_CHAR)
                {
                    token = YAMLTokenInit(YAMLTokenValue, curr_pos, lexer->position + 1, lexer->line, value_or_key);
                    backtrackChar(lexer);
                }
                else if (lexer->current_char == NULL_CHAR)
                {
                    token = YAMLTokenInit(YAMLTokenValue, curr_pos, lexer->position + 1, lexer->line, value_or_key);
                    lexer->hungry = true;
                }
            }
        }
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
    else if (type == YAMLTokenNewline)
    {
        return "YAMLTokenNewline";
    }
    return "ERROR";
}

extern void YAMLLexerDebugTest(char *input_str)
{
    YAMLLexer *lexer = YAMLLexerInit();
    char *string_copy = QuickAllocatedString(input_str);

    // pass full string
    lexer->input = string_copy;
    lexer->input_len = strlen(string_copy);
    lexer->is_last_chunk = true;

    while (ALWAYS)
    {
        YAMLToken *token = YAMLLex(lexer);
        if (token != NULL)
        {
            YAMLTokenPrint(token);
            if (token->type == YAMLTokenEOF)
            {
                YAMLTokenFree(token);
                break;
            }
            YAMLTokenFree(token);
        }
    }
    YAMLLexerFree(lexer);
}
