#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Assuming the necessary 'String' and file reading helpers are included here
// from your original context (e.g., build.h/shl_read_file)
#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

// ---------------- Token Definitions ----------------

// Define all possible types of tokens in our lexer
typedef enum {
    TOKEN_INT,                 // Integer literals
    TOKEN_FLOAT,               // Floating-point literals
    TOKEN_IDENTIFIER,          // Variable or function names
    TOKEN_KEYWORD,             // Reserved keywords like 'int', 'if', 'return', etc.
    TOKEN_STRING,              // String literals ("..." or '...')
    TOKEN_IMPORT,              // Preprocessor import directives (#include)
    TOKEN_OPERATOR,            // Single or multi-character operators (+, ==, etc.)
    TOKEN_COMMENT,             // Comments (// ...)
    TOKEN_SPACE,               // Whitespace (spaces, tabs, newlines)
    TOKEN_FUNCTION_CALL,       // Function call: identifier followed by (
    TOKEN_FUNCTION_DEFINITION, // Function definition: type keyword followed by identifier and (
    TOKEN_UNKNOWN              // Anything we couldn't recognize
} TokenType;

// Token structure: stores type, string value, and numeric values if applicable
typedef struct {
    TokenType type;
    char *string;       // Pointer into source line (no copy for efficiency)
    int string_len;     // Length of the token string
    long int_number;    // Used if token is an integer
    double real_number; // Used if token is a float
} Token;

// Lexer structure: dynamically growing array of tokens
typedef struct {
    Token *tokens;
    size_t count;
    size_t capacity;
} Lexer;

// ---------------- Global Data ----------------

// 1. List of C keywords for fast lookup
static const char *const keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short",
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while"
};
#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))

// 2. List of multi-character operators
// Sorted by length descending to ensure greedy matching (e.g., ">>=" before ">>")
static const char *const multi_operators[] = {
    "<<=", ">>=", "...",
    "+=", "-=", "*=", "/=", "%=", "&=", "^=", "|=",
    "==", "!=", "<=", ">=", "&&", "||", "++", "--", "->", "<<" , ">>"
};
#define NUM_MULTI_OPERATORS (sizeof(multi_operators) / sizeof(multi_operators[0]))

// ---------------- Lexer Helpers ----------------

// Push a token onto the lexer's dynamic array, resizing if necessary
static void lexer_push(Lexer *lexer, Token t) {
    if (lexer->count >= lexer->capacity) {
        lexer->capacity = lexer->capacity ? lexer->capacity * 2 : 16;
        lexer->tokens = realloc(lexer->tokens, lexer->capacity * sizeof(Token));
    }
    lexer->tokens[lexer->count++] = t;
}

// Convert token type enum to readable string
const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_INT:                 return "INT";
        case TOKEN_FLOAT:               return "FLOAT";
        case TOKEN_IDENTIFIER:          return "IDENTIFIER";
        case TOKEN_KEYWORD:             return "KEYWORD";
        case TOKEN_STRING:              return "STRING";
        case TOKEN_IMPORT:              return "IMPORT";
        case TOKEN_OPERATOR:            return "OPERATOR";
        case TOKEN_COMMENT:             return "COMMENT";
        case TOKEN_SPACE:               return "SPACE";
        case TOKEN_FUNCTION_CALL:       return "FN_CALL";
        case TOKEN_FUNCTION_DEFINITION: return "FN_DEF";
        default:                        return "UNKNOWN";
    }
}

// Pretty-print all tokens for debugging
void pretty_print_lexer(Lexer *lexer) {
    for (size_t i = 0; i < lexer->count; ++i) {
        Token t = lexer->tokens[i];
        info("[%02zu] %-10s '", i, token_type_to_string(t.type));
        fwrite(t.string, 1, t.string_len, stdout); // raw token string
        printf("'");
        if (t.type == TOKEN_INT) printf(" (int: %ld)", t.int_number);
        else if (t.type == TOKEN_FLOAT) printf(" (float: %f)", t.real_number);
        printf("\n");
    }
}

// ---------------- Token Matching Rules ----------------

// Each matcher returns true if a token is recognized at the current position
typedef struct {
    TokenType type;
    const char *pattern; // unused here, but could hold regex or literal
    bool (*match)(char *line, size_t pos, size_t *length, Token *t);
} TokenMap;

// ---------------- Matchers ----------------

// Match integer or floating-point numbers
bool match_number(char *line, size_t pos, size_t *length, Token *t) {
    size_t i = pos;
    if (!isdigit(line[i])) return false;

    long val = 0;
    while (isdigit(line[i])) {
        val = val * 10 + (line[i] - '0');
        i++;
    }
    t->int_number = val;
    t->type = TOKEN_INT;

    // Check for fractional part (float)
    if (line[i] == '.') {
        double frac = 0.0, div = 10.0;
        t->type = TOKEN_FLOAT;
        i++;
        while (isdigit(line[i])) {
            frac += (line[i] - '0') / div;
            div *= 10.0;
            i++;
        }
        t->real_number = val + frac;
    }

    *length = i - pos;
    return true;
}

// Match identifiers (variables/functions) and detect keywords
bool match_identifier_or_keyword(char *line, size_t pos, size_t *length, Token *t) {
    size_t i = pos;
    if (!isalpha(line[i]) && line[i] != '_') return false;

    while (isalnum(line[i]) || line[i] == '_') i++;

    *length = i - pos;

    // Check if the matched string is a keyword
    for (size_t k = 0; k < NUM_KEYWORDS; k++) {
        if (*length == strlen(keywords[k]) && strncmp(&line[pos], keywords[k], *length) == 0) {
            t->type = TOKEN_KEYWORD;
            return true;
        }
    }

    // Otherwise it's a normal identifier
    t->type = TOKEN_IDENTIFIER;
    return true;
}

// Match string literals with escape handling
bool match_string(char *line, size_t pos, size_t *length, Token *t) {
    if (line[pos] != '"' && line[pos] != '\'') return false;

    char quote = line[pos];
    size_t i = pos + 1;
    bool escaped = false;

    while (line[i] != '\0') {
        if (escaped) escaped = false;
        else if (line[i] == '\\') escaped = true;
        else if (line[i] == quote) {
            i++; // Consume closing quote
            t->type = TOKEN_STRING;
            *length = i - pos;
            return true;
        }
        i++;
    }

    // Unterminated string: consume everything to end of line
    t->type = TOKEN_STRING;
    *length = i - pos;
    return true;
}

// Match multi-character operators (greedy)
bool match_multi_operator(char *line, size_t pos, size_t *length, Token *t) {
    for (size_t k = 0; k < NUM_MULTI_OPERATORS; k++) {
        size_t op_len = strlen(multi_operators[k]);
        if (strncmp(&line[pos], multi_operators[k], op_len) == 0) {
            t->type = TOKEN_OPERATOR;
            *length = op_len;
            return true;
        }
    }
    return false;
}

// Match #include directive (preprocessor)
bool match_import_directive(char *line, size_t pos, size_t *length, Token *t) {
    const char *inc = "#include";
    size_t inc_len = strlen(inc);
    if (strncmp(&line[pos], inc, inc_len) == 0) {
        t->type = TOKEN_IMPORT;
        *length = inc_len;
        return true;
    }
    return false;
}

// Match single-line comments (// ...)
bool match_comment(char *line, size_t pos, size_t *length, Token *t) {
    if (line[pos] == '/' && line[pos + 1] == '/') {
        size_t i = pos;
        while (line[i] != '\0') i++; // consume rest of line
        t->type = TOKEN_COMMENT;
        *length = i - pos;
        return true;
    }
    return false;
}

// Match single-character operators (+, -, {, }, etc.)
bool match_single_operator(char *line, size_t pos, size_t *length, Token *t) {
    if (ispunct(line[pos]) && line[pos] != '"' && line[pos] != '\'' && !isalnum(line[pos]) && line[pos] != '_') {
        t->type = TOKEN_OPERATOR;
        *length = 1;
        return true;
    }
    return false;
}

// ---------------- Lexer Core ----------------

bool lex(Lexer *lexer, String *file) {
    // Order of matchers matters for correct tokenization
    TokenMap token_map[] = {
        {TOKEN_COMMENT, NULL, match_comment},                  // 1. Comments
        {TOKEN_OPERATOR, NULL, match_multi_operator},          // 2. Multi-char operators
        {TOKEN_IMPORT, NULL, match_import_directive},          // 3. Preprocessor
        {TOKEN_STRING, NULL, match_string},                    // 4. Strings
        {TOKEN_INT, NULL, match_number},                       // 5. Numbers
        {TOKEN_IDENTIFIER, NULL, match_identifier_or_keyword}, // 6. Identifiers/Keywords
        {TOKEN_OPERATOR, NULL, match_single_operator},         // 7. Single-char operators
    };

    size_t num_rules = sizeof(token_map) / sizeof(token_map[0]);

    for (size_t i = 0; i < file->len; ++i) {
        char *line = file->data[i];
        size_t j = 0;

        while (line[j] != '\0') {
            // Skip spaces entirely - don't tokenize them
            while (line[j] == ' ' || line[j] == '\t' || line[j] == '\n' || line[j] == '\r') {
                j++;
            }
            
            // If we've reached the end of line after skipping spaces, break
            if (line[j] == '\0') break;
            
            Token t = {0};
            t.string = &line[j];
            t.string_len = 0;

            bool matched = false;

            // Try all matchers in precedence order
            for (size_t r = 0; r < num_rules; r++) {
                size_t len = 0;
                if (token_map[r].match(line, j, &len, &t)) {
                    t.string_len = len;
                    matched = true;
                    break;
                }
            }

            // Fallback: unknown token (consume one char)
            if (!matched) {
                t.type = TOKEN_UNKNOWN;
                t.string_len = 1;
            }

            // Check if identifier is actually a function call or function definition
            if (t.type == TOKEN_IDENTIFIER) {
                // Look ahead: skip spaces, check for '('
                size_t next_pos = j + t.string_len;
                while (line[next_pos] == ' ' || line[next_pos] == '\t') next_pos++;
                if (line[next_pos] == '(') {
                    // Check if this is a function definition by looking backwards
                    // Function definition: type keyword + identifier + (
                    // Function call: identifier + (
                    bool is_function_definition = false;
                    
                    // Look backwards through previous tokens to see if there's a type keyword
                    for (int k = lexer->count - 1; k >= 0; k--) {
                        Token prev_token = lexer->tokens[k];
                        if (prev_token.type == TOKEN_KEYWORD) {
                            // Check if it's a type keyword
                            const char *keyword_str = prev_token.string;
                            size_t keyword_len = prev_token.string_len;
                            
                            // Common C type keywords
                            if ((keyword_len == 3 && strncmp(keyword_str, "int", 3) == 0) ||
                                (keyword_len == 4 && strncmp(keyword_str, "char", 4) == 0) ||
                                (keyword_len == 5 && strncmp(keyword_str, "float", 5) == 0) ||
                                (keyword_len == 6 && strncmp(keyword_str, "double", 6) == 0) ||
                                (keyword_len == 4 && strncmp(keyword_str, "void", 4) == 0) ||
                                (keyword_len == 4 && strncmp(keyword_str, "long", 4) == 0) ||
                                (keyword_len == 5 && strncmp(keyword_str, "short", 5) == 0)) {
                                is_function_definition = true;
                                break;
                            }
                        } else if (prev_token.type != TOKEN_SPACE) {
                            // Stop looking if we hit a non-space, non-keyword token
                            break;
                        }
                    }
                    
                    t.type = is_function_definition ? TOKEN_FUNCTION_DEFINITION : TOKEN_FUNCTION_CALL;
                }
            }

            lexer_push(lexer, t);

            // Stop tokenizing the line after a comment
            if (t.type == TOKEN_COMMENT) break;

            j += t.string_len;
        }
    }

    return true;
}

// ---------------- Main ----------------

int main() {
    init_logger(LOG_DEBUG, false, !false);

    String file = {0};
    Lexer lexer = {0};

    if (!shl_read_file("./hello.c", &file)) return 1;
    if (!lex(&lexer, &file)) return 1;

    pretty_print_lexer(&lexer);

    free(lexer.tokens);
    return 0;
}
