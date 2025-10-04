#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

// ---------------- Token Definitions ----------------

// The different types of tokens our lexer can recognize
typedef enum {
    TOKEN_INT,        // 123
    TOKEN_FLOAT,      // 123.45
    TOKEN_IDENTIFIER, // variable names, function names
    TOKEN_STRING,     // "Hello", 'a'
    TOKEN_IMPORT,     // #include <stdio.h> or #include "file.h"
    TOKEN_OPERATOR,   // +, -, *, /, =, {, }, etc.
    TOKEN_COMMENT,    // // This is a comment
    TOKEN_SPACE,      // whitespace (optional, can be ignored)
    TOKEN_UNKNOWN     // anything not recognized
} TokenType;

// Each token stores its type, the raw string in the source, and optionally numeric values
typedef struct {
    TokenType type;
    char *string;      // pointer to the string in the original source line
    int string_len;    // length of the token
    long int_number;   // used if token is integer
    double real_number;// used if token is float
} Token;

// The lexer itself is just a dynamic array of tokens
typedef struct {
    Token *tokens;
    size_t count;
    size_t capacity;
} Lexer;

// ---------------- Lexer Helpers ----------------

// Push a token onto the dynamic array, resizing if needed
static void lexer_push(Lexer *lexer, Token t) {
    if (lexer->count >= lexer->capacity) {
        lexer->capacity = lexer->capacity ? lexer->capacity * 2 : 16;
        lexer->tokens = realloc(lexer->tokens, lexer->capacity * sizeof(Token));
    }
    lexer->tokens[lexer->count++] = t;
}

// Convert a token type enum into a human-readable string
const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_INT: return "INT";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IMPORT: return "IMPORT";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_COMMENT: return "COMMENT";
        case TOKEN_SPACE: return "SPACE";
        default: return "UNKNOWN";
    }
}

// Print all tokens in the lexer in a human-readable format
void pretty_print_lexer(Lexer *lexer) {
    for (size_t i = 0; i < lexer->count; ++i) {
        Token t = lexer->tokens[i];
        printf("[%02zu] %-10s '", i, token_type_to_string(t.type));
        fwrite(t.string, 1, t.string_len, stdout); // raw token string
        printf("'");
        if (t.type == TOKEN_INT) printf(" (int: %ld)", t.int_number);
        else if (t.type == TOKEN_FLOAT) printf(" (float: %f)", t.real_number);
        printf("\n");
    }
}

// ---------------- Token Matching Rules ----------------

// Each rule can be either:
// - a fixed string to match, OR
// - a custom function that checks if a token is present at a given position
typedef struct {
    TokenType type;
    const char *pattern;
    bool (*match)(char *line, size_t pos, size_t *length, Token *t);
} TokenMap;

// ---------------- Matchers ----------------

// Match integer or floating-point numbers
bool match_number(char *line, size_t pos, size_t *length, Token *t) {
    size_t i = pos;
    if (!isdigit(line[i])) return false;

    // Parse integer part
    long val = 0;
    while (isdigit(line[i])) {
        val = val * 10 + (line[i] - '0');
        i++;
    }
    t->int_number = val;
    t->type = TOKEN_INT;

    // Parse fractional part, if any
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

// Match variable/function identifiers
bool match_identifier(char *line, size_t pos, size_t *length, Token *t) {
    size_t i = pos;
    if (!isalpha(line[i]) && line[i] != '_') return false;

    while (isalnum(line[i]) || line[i] == '_') i++;
    t->type = TOKEN_IDENTIFIER;
    *length = i - pos;
    return true;
}

// Match string literals ("..." or '...')
bool match_string(char *line, size_t pos, size_t *length, Token *t) {
    if (line[pos] != '"' && line[pos] != '\'') return false;

    char quote = line[pos];
    size_t i = pos + 1;

    while (line[i] != '\0' && line[i] != quote) i++;
    if (line[i] == quote) i++;

    t->type = TOKEN_STRING;
    *length = i - pos;
    return true;
}

// Match #include imports: only strings following #include
bool match_import(char *line, size_t pos, size_t *length, Token *t) {
    const char *inc = "#include";
    size_t inc_len = strlen(inc);

    // Skip leading whitespace
    size_t start = 0;
    while (isspace(line[start])) start++;

    // Only match if line starts with #include
    if (strncmp(&line[start], inc, inc_len) != 0) return false;

    // Must be <...> or "..."
    char open = line[pos], close;
    if (open == '<') close = '>';
    else if (open == '"') close = '"';
    else return false;

    size_t i = pos + 1;
    while (line[i] != '\0' && line[i] != close) i++;
    if (line[i] == close) i++;

    t->type = TOKEN_IMPORT;
    *length = i - pos;
    return true;
}

// Match C-style single-line comments (//)
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

// Match single-character operators (e.g., +, -, *, =, {, }, etc.)
bool match_operator(char *line, size_t pos, size_t *length, Token *t) {
    if (ispunct(line[pos]) && line[pos] != '"' && line[pos] != '\'') {
        t->type = TOKEN_OPERATOR;
        *length = 1;
        return true;
    }
    return false;
}

// ---------------- Lexer Core ----------------

// Scan a file line by line and tokenize it
bool lex(Lexer *lexer, String *file) {
    TokenMap token_map[] = {
        {TOKEN_COMMENT, "//", match_comment},   // comment first, to consume full line
        {TOKEN_IMPORT, NULL, match_import},     // includes
        {TOKEN_STRING, NULL, match_string},     // string literals
        {TOKEN_INT, NULL, match_number},        // numbers
        {TOKEN_FLOAT, NULL, match_number},      // floats (handled in match_number)
        {TOKEN_IDENTIFIER, NULL, match_identifier}, // identifiers
        {TOKEN_OPERATOR, NULL, match_operator}, // operators
    };

    size_t num_rules = sizeof(token_map) / sizeof(token_map[0]);

    for (size_t i = 0; i < file->len; ++i) {
        char *line = file->data[i];
        size_t j = 0;

        while (line[j] != '\0') {
            // Skip whitespace for tokenization
            if (isspace(line[j])) { j++; continue; }

            Token t = {0};
            t.string = &line[j];
            t.string_len = 0;

            bool matched = false;

            // Try all matchers in order
            for (size_t r = 0; r < num_rules; r++) {
                if (token_map[r].pattern) {
                    size_t pat_len = strlen(token_map[r].pattern);
                    if (strncmp(&line[j], token_map[r].pattern, pat_len) == 0) {
                        t.type = token_map[r].type;
                        t.string_len = pat_len;
                        matched = true;
                        break;
                    }
                } else if (token_map[r].match) {
                    size_t len = 0;
                    if (token_map[r].match(line, j, &len, &t)) {
                        t.string_len = len;
                        matched = true;
                        break;
                    }
                }
            }

            // If nothing matched, mark as unknown
            if (!matched) {
                t.type = TOKEN_UNKNOWN;
                t.string_len = 1;
            }

            lexer_push(lexer, t);

            // Stop further tokenizing after comment
            if (t.type == TOKEN_COMMENT) break;

            j += t.string_len;
        }
    }

    return true;
}

// ---------------- Main ----------------

int main() {
    init_logger(LOG_DEBUG, true, false);

    String file = {0};
    Lexer lexer = {0};

    if (!shl_read_file("./hello.c", &file)) return 1;
    if (!lex(&lexer, &file)) return 1;

    pretty_print_lexer(&lexer);

    free(lexer.tokens);
    return 0;
}
