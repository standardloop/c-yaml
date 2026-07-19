#ifndef STANDARDLOOP_YAML_H
#define STANDARDLOOP_YAML_H

#define STANDARDLOOP_YAML_H_MAJOR_VERSION 0
#define STANDARDLOOP_YAML_H_MINOR_VERSION 0
#define STANDARDLOOP_YAML_H_PATCH_VERSION 0
#define STANDARDLOOP_YAML_H_VERSION "0.0.0"

enum YAMLTokenType
{
    YAMLTokenEOF,
    YAMLTokenColon,           // :
    YAMLTokenOpenCurlyBrace,  // {
    YAMLTokenCloseCurlyBrace, // }
    YAMLTokenOpenBracket,     // [
    YAMLTokenCloseBracket,    // ]
    YAMLTokenDash,            // -
    YAMLTokenComma,           // ,
    YAMLTokenBool,            // TRUE, true, FALSE, false
    YAMLTokenNumber,          // 1, 3.14, -10
    YAMLTokenString,          // hello
    YAMLTokenNULL,            // null
    YAMLTokenPipe,            // |
    YAMLTokenGreaterThan,     // >
    YAMLTokenAsterisk,        // *
    YAMLTokenAmpersand,       // &
    YAMLTokenPound,           // #
    YAMLTokenStartOfDocument, // ---
    YAMLTokenEndOfDocument,   // ...
    YAMLTokenLessThan,        // <
    YAMLTokenQuestionMarch,   // ?
    YAMLTokenDirective,       // %
    YAMLTokenIllegal
};

typedef struct
{

} YAML;

extern YAML *YAMLInit();
extern YAML *StringToYAML(char *);
extern YAML *YAMLFromFile(char *);
extern char *YAMLToString(YAML *);

extern void FreeYAML(YAML *);
extern void PrintYAML(YAML *);

#endif
