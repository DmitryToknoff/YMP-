#pragma once
#include "Token.h"
#include "hash_table.h"
#include <string>
#include "lexer.h"
#include <vector>

struct Node {
    std::string name;
    Token token;
    int line;
    std::vector<Node*> child;
    
    Node(const std::string& s, const Token& token, int l) : name(s), token(token), line(l) {}
    void add(Node* t) {
        child.push_back(t);
    }
};


class SyntaxisTree {
    Lexer& lexer;
    HashTable& hashTable;
    Token cur_token;
    std::vector<std::string> errors;
    Node* root = nullptr;
    int d = 0;
    
    void next_token();
    void error(const std::string& msg);
    void _continue();
    void checker(TokenType expected);
    void print_tree(Node* node, std::ofstream& file_out, int depth);
    
    Node* parse_program();
    Node* parse_begin();
    Node* parse_end();
    Node* parse_descriptions();
    Node* parse_descr();
    Node* parse_type();
    Node* parse_var_list();
    Node* parse_operator();
    Node* parse_op();
    Node* parse_expr();
    Node* parse_simple_expr();
    
public:
    SyntaxisTree(Lexer& l, HashTable& ht);
    
    bool parse();
    const std::vector<std::string>& get_errors() const;
    Node* getTree() const;
    void print_tree(std::ofstream& fout);
};
