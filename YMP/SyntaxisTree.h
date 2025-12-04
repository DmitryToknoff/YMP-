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
    
    void next_token();
    void error(const std::string& msg);
    void synchronize();
    void match(TokenType expected);
    void print_tree(Node* node, std::ofstream& file_out, int depth);
    
    Node* parseProgram();
    Node* parseBegin();
    Node* parseEnd();
    Node* parseDescriptions();
    Node* parseDescr();
    Node* parseType();
    Node* parseVarList();
    Node* parseOperators();
    Node* parseOp();
    Node* parseExpr();
    Node* parseSimpleExpr();     
    
public:
    SyntaxisTree(Lexer& l, HashTable& ht);
    
    bool parse();
    const std::vector<std::string>& get_errors() const;
    Node* getTree() const;
    void print_tree(std::ofstream& fout);
};
