#ifndef HEADER_H
#define HEADER_H

// enum
typedef enum{ 
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_LVAR, // ローカル変数
    ND_NUM,
    ND_EQU,
    ND_NEQ,
    ND_SML,
    ND_ESM,
} NodeKind;

// struct
typedef struct Token Token; //これ書いとかないとToken内で未定義で怒られる
typedef struct Node Node;

struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

// global val
extern Token *token; 
extern char *user_input;
extern Node *code[100];

// func
// Tokenを触るやつら
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
bool consume_ident(); // Tokenがローカル変数か検証する
void expect(char *op);
int expect_number();
char expect_ident(); // Tokenから変数名をもらって一つ進める
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

// Nodeを触るやつら
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_lvar(); // 新しいローカル変数のノードを作る
void *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);


#endif