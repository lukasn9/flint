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

void tokenize(const char *code) {
    token_count = 0;
    int i = 0;
    while (code[i] != '\0') {
        if (isspace(code[i])) {
            i++;
            continue;
        }

        if (isalpha(code[i])) {
            char buffer[MAX_TOKEN_LENGTH];
            int j = 0;
            while (isalnum(code[i]) || code[i] == '_') {
                buffer[j++] = code[i++];
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
            }
            buffer[j] = '\0';
            add_token("NUMBER", buffer);
            continue;
        }

        if (code[i] == '/' && code[i + 1] == '/') {
            while (code[i] != '\n' && code[i] != '\0') i++;
            continue;
        }

        if (code[i] == '/' && code[i + 1] == '*') {
            i += 2;
            while (!(code[i] == '*' && code[i + 1] == '/')) {
                if (code[i] == '\0') {
                    fprintf(stderr, "Unterminated comment\n");
                    exit(1);
                }
                i++;
            }
            i += 2;
            continue;
        }

        if (code[i] == '"') {
            char buffer[MAX_TOKEN_LENGTH];
            int j = 0;
            i++;
            while (code[i] != '"') {
                if (code[i] == '\0') {
                    fprintf(stderr, "Unterminated string\n");
                    exit(1);
                }
                buffer[j++] = code[i++];
            }
            buffer[j] = '\0';
            i++;
            add_token("STRING", buffer);
            continue;
        }

        if (strchr("=+->*/,{}:;()[]", code[i])) {
            char buffer[3] = {code[i], '\0', '\0'};

            if ((code[i] == '+' || code[i] == '-' || code[i] == '*' || code[i] == '/' || code[i] == '%' || code[i] == '*') && code[i + 1] == '>') {
                buffer[1] = '>';
                i++;
            } else if (code[i] == '/' && code[i + 1] == '/') {
                buffer[1] = '/';
                i++;
            } else if (code[i] == '*' && code[i + 1] == '*') {
                buffer[1] = '>';
                i++;
            }

            add_token("OPERATOR", buffer);
            i++;
            continue;
        }

        fprintf(stderr, "Unknown character: %c\n", code[i]);
        exit(1);
    }
}

typedef struct ASTNode {
    char type[MAX_TOKEN_LENGTH];
    char value[MAX_TOKEN_LENGTH];
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode *create_node(const char *type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    strcpy(node->type, type);
    strcpy(node->value, value);
    node->left = NULL;
    node->right = NULL;
    return node;
}

void print_ast(ASTNode *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("Node(Type=%s, Value=%s)\n", node->type, node->value);
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
}

ASTNode *parse_tokens(Token *tokens, int count) {
    ASTNode *root = create_node("Program", "");
    ASTNode *current = root;
    for (int i = 0; i < count; i++) {
        if (strcmp(tokens[i].type, "KEYWORD") == 0) {
            if (strcmp(tokens[i].value, "let") == 0) {
                ASTNode *node = create_node("Declaration", tokens[i + 1].value);
                current->left = node;
                i += 2;
            } else if (strcmp(tokens[i].value, "set") == 0) {
                ASTNode *node = create_node("Assignment", tokens[i + 1].value);
                current->right = node;
                i += 2;
            }
        }
    }
    return root;
}

int main() {
    const char *code = "let x : num; set x = 5;";

    tokenize(code);

    for (int i = 0; i < token_count; i++) {
        printf("Token(Type=%s, Value=%s)\n", tokens[i].type, tokens[i].value);
    }

    ASTNode *ast = parse_tokens(tokens, token_count);
    print_ast(ast, 0);

    return 0;
}