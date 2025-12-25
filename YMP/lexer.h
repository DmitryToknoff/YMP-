#pragma once
#include "Token.h"
#include "hash_table.h"
#include <string>

class Lexer {
private:
    HashTable& hashTable;
    int paren_depth = 0;
    
    std::string s;
    int cur_pos;
    int line;
    int pos;
    bool is_error;
    
    char get_cur_char();
    void next_char();
    void skip_space();
    Token number();
    Token keyword();
    Token op();
    void print_error(const std::string& s);
    
public:
    Lexer(const std::string& _in, HashTable& ht);
    
    Token get_next_token();
    bool get_is_error() const { return is_error; }
    int get_line() const { return line; }
    int get_pos() const { return pos; }
    void start();
};
