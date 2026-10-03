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
extern void DynStringShiftLeft(DynString *str, size_t index);
extern DynString *DynStringDefaultInit();

// ————————— LEXER START —————————
enum YAMLTokenType
{
    YAMLTokenStartStream,

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

    YAMLTokenAlias,               // *
    YAMLTokenAnchor,              // &
    YAMLTokenComplexKeyIndicator, // ? // TODO: understand this one more

    YAMLTokenTag, // !

    // reserved
    YAMLTokenAT,       // @
    YAMLTokenBacktick, // `

    YAMLTokenEndStream,
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
    YAMLLexerStartStream = 0,
    YAMLLexerStateNormal = 1,
    YAMLLexerStateJustGotNewline = 2,
    YAMLLexerStatePopDedent = 3,
    YAMLLexerStateFoundEOFNeedToPopRemainingDedent = 4,
    YAMLLexerStateCurlyFlow = 5,
    YAMLLexerStateSequenceFlow = 6,
};

#define CHUNK_SIZE 4096
#define BUFFER_SIZE (CHUNK_SIZE * 2)

enum BlockScalarStyle
{
    BlockScalarStyleLiteral =
        '|', // '|' Every consumed \n in the body is appended directly as \n.
    BlockScalarStyleFolded =
        '>', // '>' \n line breaks between normal text lines are converted to
             // space characters (' '). Double line breaks (blank lines) remain
             // as
             // \n.
};

enum BlockScalarChomping
{
    BlockScalarStyleClip =
        0, // Keeps exactly one trailing \n. Strips any extra blank lines.
    BlockScalarStyleStrip = '-', //  Removes all trailing \n characters.
    BlockScalarStyleKeep =
        '+', // Retains all trailing \n characters and blank lines verbatim.
};

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
    u_int8_t this_indent_space_count; // You can use different indentation
                                      // inside of the same YAML document, as
                                      // long as it is the same for one level.
    int space_count;
    struct block_scalar_options_s
    {
        bool enabled;
        enum BlockScalarStyle style;
        enum BlockScalarChomping chomping;
        uint8_t explicit_indent; // 1 - 9
    } block_scalar_options;
} YAMLLexer;

/// @cond INTERNAL
extern void TestLexer(void);
extern void YAMLLexerDebugTest(char *file_name);
/// @endcond

extern YAMLToken *YAMLLex(YAMLLexer *lexer);
extern void YAMLTokenFree(YAMLToken *token);
extern YAMLToken *YAMLTokenInit(enum YAMLTokenType type, u_int32_t start,
                                u_int32_t end, u_int32_t line, char *literal);
extern YAMLLexer *YAMLLexerInit(FILE *file_ptr);
extern void YAMLLexerFree(YAMLLexer *lexer);

// ————————— LEXER END —————————

// ————————— PARSER START —————————

enum YAMLParserInputMode
{
    YAMLParserInputFile,
    // YAMLParserInputString
};

typedef struct
{
    // enum YAMLParserInputMode input_mode;
    // union
    // {
    // FILE *file_ptr;
    // char *string_ptr;
    // };
    YAMLLexer *lexer;
    YAMLToken *current_token;
    YAMLToken *peek_token;
    char *error_message;
} YAMLParser;

extern YAMLParser *YAMLParserInit(FILE *file_ptr);
extern void YAMLParserFree(YAMLParser *parser);

/// @cond INTERNAL
extern void TestParser(void);
extern void YAMLParserDebugTest(YAMLParser *parser);
/// @endcond

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
extern YAML *YAMLFromFile(FILE *file_ptr);
extern YAML *YAMLFromSTDIN(size_t);
extern char *YAMLToString(YAML *);

extern void YAMLFree(YAML *);
extern void YAMLPrint(YAML *);

/// @cond INTERNAL
extern void TestYaml(void);
/// @endcond

extern YAML *YAMLParse(YAMLParser *parser);

#endif
