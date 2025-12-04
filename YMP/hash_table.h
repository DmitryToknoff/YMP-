#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Token.h"
#include <string>

struct Hash {
    TokenType type;
    std::string lex;
    int index;
    Hash* next;
    
    Hash() : type(TokenType::ERROR), index(-1), next(nullptr) {}
    Hash(TokenType t, const std::string& lex, int idx)
        : type(t), lex(lex), index(idx), next(nullptr) {}
};

class HashTable {
    const int size = 500;
    Hash** t;
    int cur;
    
    int _hash(const std::string& key);
    
public:
    HashTable();
    ~HashTable();
    
    int insert(const Token& token);
    Hash* find(const Token& token);
    int get_size() const { return size; }
    Hash** get_table() const { return t; }
};

#endif
