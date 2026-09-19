/**
 * @file yaml.h
 * @headerfile yaml.h <standardloop/yaml.h>
 * @brief A C library from YAML.
 */

#ifndef STANDARDLOOP_YAML_H
#define STANDARDLOOP_YAML_H

#include <standardloop/collections.h>

#define LEXER_MIN_BUFFER_SIZE 4096
#define LEXER_DEFAULT_BUFFER_SIZE 4096
#define LEXER_MAX_BUFFER_SIZE 1048576 // TODO

// ————————— LEXER START —————————
enum YAMLTokenType
{
    YAMLTokenStartOfDocument, // ---
    YAMLTokenEndOfDocument,   // ...

    YAMLTokenDirective, // %
    // %YAML
    // %TAG

    // spacing
    YAMLTokenIndent,
    YAMLTokenDedent,
    YAMLTokenSpace,   //
    YAMLTokenNewline, // do we need this?

    // general
    YAMLTokenKey,               // part before the colon
    YAMLTokenValueIndicator,    // :
    YAMLTokenFlowMappingStart,  // {
    YAMLTokenFlowMappingEnd,    // }
    YAMLTokenFlowSequenceStart, // [
    YAMLTokenFlowSequenceEnd,   // ]
    YAMLTokenListDash,          // -
    YAMLTokenComma,             // ,

    YAMLTokenValue,  // do we want general value? or the below
    YAMLTokenBool,   // TRUE, true, FALSE, false
    YAMLTokenNumber, // 1, 3.14, -10
    YAMLTokenString, // hello
    YAMLTokenNULL,   // null

    YAMLTokenSingleQuotes, // "
    YAMLTokenDoubleQuotes, // "

    YAMLTokenLiteralBlockStart, // |
    YAMLTokenFoldedBlockStart,  // >
    YAMLTokenListChompingDash,  // -
    YAMLTokenListKeepChomping,  // +
    YAMLTokenChompingNumber,    // 1-9

    YAMLTokenAlias,        // *
    YAMLTokenAnchor,       // &
    YAMLTokenKeyIndicator, // ? // TODO: understand this one more

    YAMLTokenTag, // !

    YAMLTokenComment, // #

    // reserved
    YAMLTokenAT,       // @
    YAMLTokenBacktick, // `
    // yaml 1.1
    YAMLTokenMerge, // <<

    YAMLTokenEOF,
    YAMLTokenIllegal
};

typedef struct
{
    enum YAMLTokenType type;
    u_int32_t start;
    u_int32_t end;
    u_int32_t line;
    char *literal;
} YAMLToken;

extern YAMLToken *YAMLTokenInit(enum YAMLTokenType, u_int32_t, u_int32_t,
                                u_int32_t, char *);

extern char *YAMLTokenTypeToString(enum YAMLTokenType);
extern void YAMLTokenPrint(YAMLToken *);

enum YAMLLexerState
{
    YAMLLexerStateNormal,
    YAMLLexerStateIncomplete,

    YAMLLexerStateEatingComment,
    YAMLLexerStateInSingleQuotes,
    YAMLLexerStateInDoubleQuotes,
    YAMLLexerStateWaitingForChompingDashOrPlus,
    YAMLLexerStateWaitSpaceAfterDash,
    YAMLLexerStateJustGotNewline,
    YAMLLexerStatePopDedent,
    YAMLLexerStateFoundEOFNeedToPopRemainingDedent,
    YAMLLexerStateWaitingForChompingNumber,
    YAMLLexerStateCurlyFlow,
    YAMLLexerStateSequenceFlow
};

typedef struct
{
    char *input;
    size_t input_len;
    char current_char;
    u_int32_t position;
    u_int32_t read_position;
    u_int32_t line;
    char *error;

    List *indent_stack;

    bool is_last_chunk;

    // Hungry means the lexer needs more input before it can finish a token
    bool hungry; // can this be a state?
    char *temp_input;
    size_t temp_input_len;

    u_int8_t sequential_dashes; // tracking document start
    u_int8_t sequential_dots;   // tracking document end
    int space_count;

    enum YAMLLexerState state;
} YAMLLexer;

/// @cond INTERNAL
extern void TestLexer(void);
/// @endcond

extern YAMLToken *YAMLLex(YAMLLexer *);
extern void YAMLTokenFree(YAMLToken *);
extern YAMLToken *YAMLTokenInit(enum YAMLTokenType, u_int32_t, u_int32_t,
                                u_int32_t, char *);
extern YAMLLexer *YAMLLexerInit();
extern void YAMLLexerReload(YAMLLexer *, char *, size_t, bool);
extern void YAMLLexerFree(YAMLLexer *);
extern bool IsLexerHungry(YAMLLexer *);

extern void YAMLLexerDebugTest(char *);

// ————————— LEXER END —————————

// ————————— PARSER START —————————

typedef struct
{
    YAMLLexer *lexer;
    YAMLToken *current_token;
    YAMLToken *peek_token;
    char *error_message;
} YAMLParser;

extern YAMLParser *YAMLParserInit(YAMLLexer *);
extern void YAMLParserFree(YAMLParser *);

// ————————— PARSER END —————————

enum YAMLValueType
{
    /**  */
    YAMLOBJ_t,
    /**  */
    YAMLNUMBER_INT_t,
    /**  */
    YAMLNUMBER_DOUBLE_t,
    /**  */
    YAMLSTRING_t,
    /**  */
    YAMLBOOL_t,
    /**  */
    YAMLNULL_t,
    /**  */
    YAMLLIST_t,
};

typedef struct
{
    enum YAMLValueType value_type;
    union
    {
        List *list;
        HashMap *map;
        int64_t *num_int;
        double *num_double;
        // void *null_yaml; // if null, then do need to hold it
        char *str;
        bool *boolean;
    };
} YAMLValue;

typedef struct
{
    // do we want a header for version?
    Item *root; // root->value will be of type YAMLValue
} YAMLDocument;

typedef struct
{
    List *documents; // list of YAMLDocument
} YAML;

extern YAML *YAMLInit();
extern YAML *StringToYAML(char *);
extern YAML *YAMLFromFile(FILE *, size_t);
extern YAML *YAMLFromSTDIN(size_t);
extern char *YAMLToString(YAML *);

extern void YAMLFree(YAML *);
extern void YAMLPrint(YAML *);

/// @cond INTERNAL
extern void TestYaml(void);
/// @endcond

extern YAML *YAMLParse(YAMLParser *parser);
extern YAML *YAMLParseFile(YAMLParser *parser, FILE *file_ptr,
                           size_t buffer_size);
#endif
