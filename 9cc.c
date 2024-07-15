#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n"); 
    return 0;
}

// val declaration

Token *token; //なんかこっちでも宣言しないと未定義で怒られる泣
char *user_input;

// func declaration
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    // char型が1byteなので入力文字列の先頭アドレスから現在アドレスの差分が文字数を表している
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char *op){
    if(token->kind != TK_RESERVED ||
    strlen(op) != token->len ||
    memcmp(token->str, op, token->len)) // token->strとopの先頭token->len文字が一致したら0を返す
    return false;
    token = token->next;
    return true;
}

void expect(char *op){
    if(token->kind != TK_RESERVED ||
    strlen(op) != token->len ||
    memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません", op);
    token = token->next;
}

int expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next; //tokenはグローバル変数のような扱
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len){ //curは今のトークン
    Token *tok = calloc(1, sizeof(Token)); //動的な配列のためのメモリの確保
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }

        if(memcmp(p, "==", 2)==0 || memcmp(p, "<=", 2)==0 || memcmp(p, ">=", 2)==0 || memcmp(p, "!=", 2)==0){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p+=2;
            continue;
        }

        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' ||
        *p == '>' || *p == '<') {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if(isdigit(*p)){ //文字列以外の場合0を返す
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr(){
    Node *node = equality();
    return node;
}

Node *equality(){
    Node *node = relational();
    for(;;){
        if(consume("=="))
            node = new_node(ND_EQU, node, relational());
        else if(consume("!="))
            node = new_node(ND_NEQ, node, relational());
        else
            return node;
    }
}

Node *relational(){
    Node *node = add();
    for(;;){
        if(consume("<"))
            node = new_node(ND_SML, node, add());
        else if(consume("<="))
            node = new_node(ND_ESM, node, add());
        else if(consume(">"))
            node = new_node(ND_SML, add(), node);
        else if(consume(">="))
            node = new_node(ND_ESM, add(), node);
        else 
            return node;
    }
}
Node *add(){
    Node *node = mul();
    for(;;){
        if(consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul(){
    Node *node = unary();
    for(;;){
        if(consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary(){
    if(consume("+"))
        return unary();
    if(consume("-"))
        return new_node(ND_SUB, new_node_num(0), unary());
    return primary();
}

Node *primary(){
    if (consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    return new_node_num(expect_number());
}

void gen(Node *node){
    if(node->kind == ND_NUM){
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind){
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    
    case ND_EQU:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NEQ:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_SML:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_ESM:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}
