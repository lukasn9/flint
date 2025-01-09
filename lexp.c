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
FILE *output;

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

void translate_program_to_c();

void parse_program(int *index) {
    translate_program_to_c();
}

void translate_program_to_c() {
    fprintf(output, "#include <stdio.h>\n#include <stdlib.h>\n\nint main() {\n");
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i].type, "KEYWORD") == 0) {
            if (strcmp(tokens[i].value, "echo") == 0) {
                i++;
                fprintf(output, "printf(\"%%s\\n\", %s);\n", tokens[i].value);
            } else if (strcmp(tokens[i].value, "let") == 0) {
                fprintf(output, "int %s;\n", tokens[++i].value);
            } else if (strcmp(tokens[i].value, "set") == 0) {
                fprintf(output, "%s = %s;\n", tokens[++i].value, tokens[++i + 1].value);
            }
        }
    }
    fprintf(output, "return 0;\n}");
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

    output = fopen("translated.c", "w");
    if (!output) {
        fprintf(stderr, "Error: Could not create output file\n");
        return 1;
    }

    int index = 0;
    parse_program(&index);

    fclose(output);
    printf("Translation completed successfully. Output: translated.c\n");
    return 0;
}