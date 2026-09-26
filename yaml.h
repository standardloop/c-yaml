/**
 * @file yaml.h
 * @headerfile yaml.h <standardloop/yaml.h>
 * @brief A C library from YAML.
 */

#ifndef STANDARDLOOP_YAML_H
#define STANDARDLOOP_YAML_H

#include <standardloop/collections.h>

typedef struct
{
    char *value;
    size_t size;
    // size_t chars;
    // size_t cur_idx;
} DynString;

extern char *DynStringToCString(DynString *str);
extern void DynStringFree(DynString *str);
extern void DynStringPrint(DynString *str);
extern void DynStringAddCharAt(DynString *str, size_t idx, char c);
extern void DynStringTrimEnd(DynString *str);
extern DynString *DynStringDefaultInit();

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
    YAMLTokenNewline, // do we need this?

    // general
    YAMLTokenScalar,            // anything
    YAMLTokenValueIndicator,    // :
    YAMLTokenFlowMappingStart,  // {
    YAMLTokenFlowMappingEnd,    // }
    YAMLTokenFlowSequenceStart, // [
    YAMLTokenFlowSequenceEnd,   // ]
    YAMLTokenListDash,          // -
    YAMLTokenFlowEntry,         // ,

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
    YAMLLexerStateNormal = 0,
    YAMLLexerStateJustGotNewline = 1,
    YAMLLexerStatePopDedent = 2,
    YAMLLexerStateFoundEOFNeedToPopRemainingDedent = 3,
    YAMLLexerStateCurlyFlow = 4,
    YAMLLexerStateSequenceFlow = 5,
};

#define CHUNK_SIZE 4096
#define BUFFER_SIZE (CHUNK_SIZE * 2)

typedef struct
{
    FILE *file_ptr;
    char buffer[BUFFER_SIZE];
    size_t cursor;
    size_t bytes_in_buffer;
    u_int32_t line;
    char current_char;
    bool eof_reached;
    enum YAMLLexerState state;
    List *flow_stack;
    List *indent_stack;
    int space_count;
} YAMLLexer;

/// @cond INTERNAL
extern void TestLexer(void);
/// @endcond

extern YAMLToken *YAMLLex(YAMLLexer *lexer);
extern void YAMLTokenFree(YAMLToken *token);
extern YAMLToken *YAMLTokenInit(enum YAMLTokenType type, u_int32_t start,
                                u_int32_t end, u_int32_t line, char *literal);
extern YAMLLexer *YAMLLexerInit(FILE *file_ptr);
extern void YAMLLexerFree(YAMLLexer *lexer);

extern void YAMLLexerDebugTest(char *file_name);

// ————————— LEXER END —————————

// ————————— PARSER START —————————

enum YAMLParserInputMode
{
    YAMLParserInputFile,
    // YAMLParserInputString
};

typedef struct
{
    enum YAMLParserInputMode input_mode;
    union
    {
        FILE *file_ptr;
        // char *string_ptr;
    };
    size_t buffer_size;
    YAMLLexer *lexer;
    YAMLToken *current_token;
    YAMLToken *peek_token;
    char *error_message;
    char *current_buffer;
    char *next_buffer;
    size_t current_bytes;
} YAMLParser;

extern YAMLParser *YAMLParserInit();

extern void YAMLParserFree(YAMLParser *parser);

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
        ComplexHashMap *map;
        int64_t *num_int;
        double *num_double;
        // void *null_yaml; // if null, then do need to hold it
        char *str;
        bool *boolean;
    };
} YAMLValue;

typedef struct
{
    YAMLValue *root;
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

extern YAML *YAMLParserParse(YAMLParser *parser);
extern YAML *YAMLParseFile(YAMLParser *parser, FILE *file_ptr,
                           size_t buffer_size);

#endif
