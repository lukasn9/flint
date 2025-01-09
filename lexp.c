#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 1024
#define MAX_TOKEN_LENGTH 256

typedef struct {
    char type[MAX_TOKEN_LENGTH];
    char value[MAX_TOKEN_LENGTH];
} Token;

Token tokens[MAX_TOKENS];
int token_count = 0;

void add_token(const char *type, const char *value) {
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].type, type);
        strcpy(tokens[token_count].value, value);
        token_count++;
    } else {
        fprintf(stderr, "Too many tokens\n");
        exit(1);
    }
}

void handle_error(const char *message, int line, int column) {
    fprintf(stderr, "Error: %s at line %d, column %d\n", message, line, column);
    exit(1);
}

void tokenize(const char *code) {
    token_count = 0;
    int i = 0, line = 1, column = 1;

    while (code[i] != '\0') {
        if (isspace(code[i])) {
            if (code[i] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            i++;
            continue;
        }

        if (isalpha(code[i])) {
            char buffer[MAX_TOKEN_LENGTH];
            int j = 0;
            while (isalnum(code[i]) || code[i] == '_') {
                buffer[j++] = code[i++];
                column++;
            }
            buffer[j] = '\0';

            if (strcmp(buffer, "let") == 0 || strcmp(buffer, "set") == 0 ||
                strcmp(buffer, "check") == 0 || strcmp(buffer, "othercheck") == 0 ||
                strcmp(buffer, "other") == 0 || strcmp(buffer, "echo") == 0 ||
                strcmp(buffer, "loop") == 0 || strcmp(buffer, "stop") == 0 ||
                strcmp(buffer, "funcdo") == 0 || strcmp(buffer, "done") == 0 ||
                strcmp(buffer, "run") == 0 || strcmp(buffer, "catch") == 0 ||
                strcmp(buffer, "module") == 0 || strcmp(buffer, "return") == 0) {
                add_token("KEYWORD", buffer);
            } else {
                add_token("IDENTIFIER", buffer);
            }
            continue;
        }

        if (isdigit(code[i])) {
            char buffer[MAX_TOKEN_LENGTH];
            int j = 0;
            while (isdigit(code[i]) || code[i] == '.') {
                buffer[j++] = code[i++];
                column++;
            }
            buffer[j] = '\0';
            add_token("NUMBER", buffer);
            continue;
        }

        if (code[i] == '/' && code[i + 1] == '/') {
            while (code[i] != '\n' && code[i] != '\0') {
                i++;
                column++;
            }
            continue;
        }

        if (code[i] == '/' && code[i + 1] == '*') {
            i += 2;
            column += 2;
            while (!(code[i] == '*' && code[i + 1] == '/')) {
                if (code[i] == '\0') {
                    handle_error("Unterminated comment", line, column);
                }
                if (code[i] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                i++;
            }
            i += 2;
            column += 2;
            continue;
        }

        if (code[i] == '"') {
            char buffer[MAX_TOKEN_LENGTH];
            int j = 0;
            i++;
            column++;
            while (code[i] != '"') {
                if (code[i] == '\0') {
                    handle_error("Unterminated string", line, column);
                }
                buffer[j++] = code[i++];
                column++;
            }
            buffer[j] = '\0';
            i++;
            column++;
            add_token("STRING", buffer);
            continue;
        }

        if (strchr("=+->*/,{}:;()[]?%", code[i])) {
            char buffer[3] = {code[i], '\0', '\0'};

            if ((code[i] == '+' || code[i] == '-' || code[i] == '*' || code[i] == '/' || code[i] == '%') && code[i + 1] == '>') {
                buffer[1] = '>';
                i++;
            }

            add_token("OPERATOR", buffer);
            i++;
            continue;
        }

        if (code[i] == '.') {
            add_token("DOT", ".");
            i++;
            column++;
            continue;
        }

        char error_message[50];
        sprintf(error_message, "Unknown character '%c'", code[i]);
        handle_error(error_message, line, column);
    }
}

void parse_statement(int *index);

void parse_program(int *index) {
    while (*index < token_count) {
        parse_statement(index);
    }
}

void parse_variable_declaration(int *index) {
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].type, "IDENTIFIER") != 0) {
        fprintf(stderr, "Error: Expected identifier after 'let'\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].value, ":") != 0) {
        fprintf(stderr, "Error: Expected ':' after identifier\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].type, "IDENTIFIER") != 0) {
        fprintf(stderr, "Error: Expected type after ':'\n");
        exit(1);
    }
    (*index)++;
}

void parse_variable_assignment(int *index) {
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].type, "IDENTIFIER") != 0) {
        fprintf(stderr, "Error: Expected identifier after 'set'\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].value, "=") != 0) {
        fprintf(stderr, "Error: Expected '=' after identifier\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count) {
        fprintf(stderr, "Error: Expected value after '='\n");
        exit(1);
    }
    (*index)++;
}

void parse_if_statement(int *index) {
    (*index)++;
    if (*index >= token_count) {
        fprintf(stderr, "Error: Expected condition after 'check'\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].value, "{") != 0) {
        fprintf(stderr, "Error: Expected '{' after condition\n");
        exit(1);
    }
    (*index)++;
    while (*index < token_count && strcmp(tokens[*index].value, "}") != 0) {
        parse_statement(index);
    }
    if (*index >= token_count) {
        fprintf(stderr, "Error: Unterminated 'check' block\n");
        exit(1);
    }
    (*index)++;
}

void parse_loop(int *index) {
    (*index)++;
    if (*index >= token_count) {
        fprintf(stderr, "Error: Expected 'range' or '{' after 'loop'\n");
        exit(1);
    }
    if (strcmp(tokens[*index].value, "range") == 0) {
        (*index)++;
        if (*index >= token_count || strcmp(tokens[*index].value, "{") != 0) {
            fprintf(stderr, "Error: Expected '{' after 'range'\n");
            exit(1);
        }
        (*index)++;
        while (*index < token_count && strcmp(tokens[*index].value, "}") != 0) {
            parse_statement(index);
        }
    } else if (strcmp(tokens[*index].value, "{") == 0) {
        (*index)++;
        while (*index < token_count && strcmp(tokens[*index].value, "}") != 0) {
            parse_statement(index);
        }
    } else {
        fprintf(stderr, "Error: Unexpected token after 'loop'\n");
        exit(1);
    }
    (*index)++;
}

void parse_function(int *index) {
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].type, "IDENTIFIER") != 0) {
        fprintf(stderr, "Error: Expected IDENTIFIER after 'funcdo'");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].value, "(") != 0) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        exit(1);
    }
    (*index)++;
    while (*index < token_count && strcmp(tokens[*index].value, ")") != 0) {
        (*index)++;
    }
    if (*index >= token_count) {
        fprintf(stderr, "Error: Unterminated parameter list\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].value, ":") != 0) {
        fprintf(stderr, "Error: Expected ':' after parameter list\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].type, "IDENTIFIER") != 0) {
        fprintf(stderr, "Error: Expected return type after ':'\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].value, "{") != 0) {
        fprintf(stderr, "Error: Expected '{' to start function body\n");
        exit(1);
    }
    (*index)++;
    while (*index < token_count && strcmp(tokens[*index].value, "}") != 0) {
        parse_statement(index);
    }
    if (*index >= token_count) {
        fprintf(stderr, "Error: Unterminated function body\n");
        exit(1);
    }
    (*index)++;
}

void parse_module(int *index) {
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].type, "IDENTIFIER") != 0) {
        fprintf(stderr, "Error: Expected module name after 'module'\n");
        exit(1);
    }
    (*index)++;
    if (*index >= token_count || strcmp(tokens[*index].value, "{") != 0) {
        fprintf(stderr, "Error: Expected '{' to start module body\n");
        exit(1);
    }
    (*index)++;
    while (*index < token_count && strcmp(tokens[*index].value, "}") != 0) {
        if (strcmp(tokens[*index].value, "funcdo") == 0) {
            parse_function(index);
        } else {
            fprintf(stderr, "Error: Only functions are allowed inside modules\n");
            exit(1);
        }
    }
    if (*index >= token_count) {
        fprintf(stderr, "Error: Unterminated module body\n");
        exit(1);
    }
    (*index)++;
}

void parse_echo(int *index) {
    (*index)++;
    if (*index >= token_count) {
        fprintf(stderr, "Error: Expected value after 'echo'\n");
        exit(1);
    }
    (*index)++;
}

void parse_return(int *index) {
    (*index)++;
    if (*index >= token_count) {
        fprintf(stderr, "Error: Expected value after 'return'\n");
        exit(1);
    }
    (*index)++;
}

void parse_stop(int *index) {
    (*index)++;
}

void parse_statement(int *index) {
    if (*index >= token_count) return;

    if (strcmp(tokens[*index].value, "let") == 0) {
        parse_variable_declaration(index);
    } else if (strcmp(tokens[*index].value, "set") == 0) {
        parse_variable_assignment(index);
    } else if (strcmp(tokens[*index].value, "check") == 0) {
        parse_if_statement(index);
    } else if (strcmp(tokens[*index].value, "loop") == 0) {
        parse_loop(index);
    } else if (strcmp(tokens[*index].value, "funcdo") == 0) {
        parse_function(index);
    } else if (strcmp(tokens[*index].value, "module") == 0) {
        parse_module(index);
    } else if (strcmp(tokens[*index].value, "echo") == 0) {
        parse_echo(index);
    } else if (strcmp(tokens[*index].value, "return") == 0) {
        parse_return(index);
    } else if (strcmp(tokens[*index].value, "stop") == 0) {
        parse_stop(index);
    } else {
        fprintf(stderr, "Error: Unsupported statement '%s'\n", tokens[*index].value);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.fl>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    if (strlen(filename) < 4 || strcmp(filename + strlen(filename) - 3, ".fl") != 0) {
        fprintf(stderr, "Error: File extension must be .fl\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *code = (char *)malloc(length + 1);
    if (!code) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    fread(code, 1, length, file);
    code[length] = '\0';
    fclose(file);

    tokenize(code);
    free(code);

    int index = 0;
    parse_program(&index);

    printf("Parsing completed successfully.\n");
    return 0;
}