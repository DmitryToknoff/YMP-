#include "lexer.h"
#include <iostream>
#include <string>
#include <cctype>

Lexer::Lexer(const std::string& _in, HashTable& ht) : hashTable(ht), s(_in), cur_pos(0), line(1), position(1), is_error(false), paren_depth(0) {}

char Lexer::get_cur_char() {
    if (cur_pos < s.size()) {
        return s[cur_pos];
    }
    return '\0';
}


void Lexer::next_char() {
    if (cur_pos < s.size()) {
        if (s[cur_pos] == '\n') {
            line++;
            position = 1;
        } else {
            position++;
        }
        cur_pos++;
    }
}

void Lexer::skip_space() {
    while (cur_pos < s.size() && std::isspace(s[cur_pos])) {
        next_char();
    }
}


Token Lexer::check_number() {
    std::string val;
    int start_pos = position;
    int start_line = line;
    bool is_real = false, is_error = false;
    skip_space();
    
    while (cur_pos < s.size() && !std::isspace(s[cur_pos])) {
        val += s[cur_pos];
        next_char();
    }
    
    if (val[0] == '0' && val.size() > 1 && val[1] != '.') is_error = true;
    
    int cnt = 0;
    for (int i = 0; i < val.size(); ++i) {
        if (tolower(val[i]) != toupper(val[i])) is_error = true;
        else if (val[i] == '.') ++cnt;
    }
    
    if (cnt > 1) is_error = true;
    else if (cnt == 1) {
        int tmp = 0;
        for (int i = int(val.size()) - 1; i >= 0; --i) {
            if (std::isdigit(val[i])) ++tmp;
        }
        
        is_real = true;
        if (tmp == 0) is_error = true;
    }
    
    if (is_error) {
        return Token(TokenType::ERROR, "Is Not number " + val, start_line, start_pos);
    }
    
    return Token(is_real ? TokenType::REAL_NUMBER : TokenType::INT_NUMBER, val, start_line, start_pos);
}



Token Lexer::check_keyword() {
    std::string val;
    int start_pos = position;
    int start_line = line;
    
    while (cur_pos < s.size() && !std::isspace(s[cur_pos])) {
        val += s[cur_pos];
        next_char();
    }
    
    TokenType type = TokenType::IDENTIFIER;
    if (val == "PROGRAM") type = TokenType::PROGRAM;
    else if (val == "END") type = TokenType::END;
    else if (val == "INTEGER") type = TokenType::INTEGER;
    else if (val == "REAL") type = TokenType::REAL;
    
    bool ok = true;
    
    for (int i = 0; i < val.size() - (val.back() == ','); ++i) {
        if (tolower(val[i]) == toupper(val[i])) {
            ok = false;
            break;
        }
    }
    
    std::string sub_val = val.substr(0, 5);
    if (sub_val == "ITOR(" && val.back() == ')') type = TokenType::ITOR, ok = true;
    if (sub_val == "RTOI(" && val.back() == ')') type = TokenType::RTOI, ok = true;
    if (type == TokenType::IDENTIFIER) {
        if (!ok) {
            return Token(TokenType::ERROR, "Is not name varible " + val, line, position);
        }
    }
    
    return Token(type, val, start_line, start_pos);
}
Token Lexer::check_operator() {
    int start_line = line;
    int start_pos = position;
    char op = get_cur_char();
    next_char();

    switch (op) {
        case '=': return Token(TokenType::ASSIGN, "=", start_line, start_pos);
        case '+': return Token(TokenType::PLUS, "+", start_line, start_pos);
        case '-': return Token(TokenType::MINUS, "-", start_line, start_pos);
        case ',': return Token(TokenType::COMMA, ",", start_line, start_pos);
        case '(': return Token(TokenType::LPAREN, "(", start_line, start_pos);
        case ')': return Token(TokenType::RPAREN, ")", start_line, start_pos);
        default:
            return Token(TokenType::ERROR, std::string(1, op), start_line, start_pos);
    }
}

Token Lexer::get_next_token() {
    skip_space();
    
    if (cur_pos >= s.size()) {
        return Token(TokenType::END_FILE, "", line, position);
    }
    
    char current = get_cur_char();
    Token token;
    if (std::isalpha(current)) {
        
        token = check_keyword();
    } else if (std::isdigit(current)) {
        
        token = check_number();
    }
    else {
        
        token = check_operator();
    }
    
    hashTable.insert(token);

    return token;
}


void Lexer::start() {
    for (Token t = get_next_token(); t.type != TokenType::END_FILE; t = get_next_token()) {}
}
