#ifndef HEADER_H
#define HEADER_H

// enum
typedef enum{ 
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;

// struct
typedef struct Token Token; //これ書いとかないとToken内で未定義で怒られる
typedef struct Node Node;

struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

// global val
extern Token *token; 
extern char *user_input;

// func
// Tokenを触るやつら
void error_at(char *loc, char *fmt, ...);
bool consume(char op);
void expect(char op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);

// Nodeを触るやつら
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);


#endif