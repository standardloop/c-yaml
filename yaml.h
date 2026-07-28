#ifndef STANDARDLOOP_YAML_H
#define STANDARDLOOP_YAML_H

#define STANDARDLOOP_YAML_H_MAJOR_VERSION 0
#define STANDARDLOOP_YAML_H_MINOR_VERSION 0
#define STANDARDLOOP_YAML_H_PATCH_VERSION 0
#define STANDARDLOOP_YAML_H_VERSION "0.0.0"

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

extern YAMLToken *YAMLTokenInit(enum YAMLTokenType, u_int32_t, u_int32_t, u_int32_t, char *);

extern char *YAMLTokenTypeToString(enum YAMLTokenType);
extern void YAMLTokenPrint(YAMLToken *);

enum YAMLLexerState
{
    YAMLLexerStateNormal,
    YAMLLexerStateIncomplete,

    YAMLLexerStateEatingComment,
    YAMLLexerStateInSingleQuotes,
    YAMLLexerStateInDoubleQuotes,
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

    bool is_last_chunk;

    // Hungry means the lexer needs more input before it can finish a token
    bool hungry;
    char *temp_input;
    size_t temp_input_len;

    // maybe
    enum YAMLLexerState state;
} YAMLLexer;

extern YAMLToken *YAMLLex(YAMLLexer *);
extern YAMLLexer *YAMLLexerInit();
extern void LexerReload(YAMLLexer *, char *, size_t, bool);
extern void YAMLLexerFree(YAMLLexer *);
extern bool IsLexerHungry(YAMLLexer *);

// ————————— LEXER END —————————

typedef struct
{
    float version;
    HashMap *values;
} YAMLDocument;

typedef struct
{
    YAMLDocument **documents;
} YAML;

extern YAML *YAMLInit();
extern YAML *StringToYAML(char *);
extern YAML *YAMLFromFile(char *, size_t);
extern char *YAMLToString(YAML *);

extern void YAMLFree(YAML *);
extern void YAMLPrint(YAML *);

#endif
