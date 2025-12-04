#pragma once
#include <string>

enum TokenType {
    PROGRAM,
    END,
    INTEGER,
    REAL,
    ITOR,
    RTOI,
    IDENTIFIER,
    INT_NUMBER,
    REAL_NUMBER,
    ASSIGN,
    PLUS,
    MINUS,
    COMMA,
    LPAREN,
    RPAREN,
    END_FILE,
    ERROR
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int pos;
    
    Token(TokenType t = TokenType::END_FILE, const std::string& s = "", int l = 1, int p = 1) : type(t), value(s), line(l), pos(p) {}
};
