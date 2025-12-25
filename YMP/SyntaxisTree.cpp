#pragma once
#include "SyntaxisTree.h"
#include <sstream>
#include <fstream>
SyntaxisTree::SyntaxisTree(Lexer& l, HashTable& ht) : lexer(l), hashTable(ht) {
    next_token();
}

void SyntaxisTree::next_token() {
    cur_token = lexer.get_next_token();
}
void SyntaxisTree::error(const std::string& msg) {
    std::stringstream ss;
    ss << "Error at line " << cur_token.line << ", position " << cur_token.pos << ": " << msg;
    errors.push_back(ss.str());
}
void SyntaxisTree::_continue() {
    while (cur_token.type != END_FILE && cur_token.type != END && cur_token.type != INTEGER && cur_token.type != REAL && cur_token.type != IDENTIFIER) {
        next_token();
    }
}

Node* SyntaxisTree::parse_program() {
    Node* node = new Node("Program", Token(), cur_token.line);
    
    node->add(parse_begin());
    node->add(parse_descriptions());
    node->add(parse_operator());
    node->add(parse_end());
    
    return node;
}

bool SyntaxisTree::parse() {
    root = parse_program();
    
    return errors.empty();
}



void SyntaxisTree::checker(TokenType exp) {
    if (cur_token.type == exp) {
        next_token();
    } else {
        std::string exp_s;
        
        if (exp == PROGRAM) exp_s = "PROGRAM";
        else if (exp == END) exp_s = "END";
        else if (exp == ASSIGN) exp_s = "оператор присваивания";
        else if (exp == INTEGER) exp_s = "INTEGER";
        else if (exp == REAL) exp_s = "REAL";
        else if (exp == ITOR) exp_s = "ITOR";
        else if (exp == ASSIGN) exp_s = "оператор присваивания";
        else if (exp == RTOI) exp_s = "RTOI";
        else if (exp == INT_NUMBER) exp_s = "целое число";
        else if (exp == REAL_NUMBER) exp_s = "вещественное число";
        else if (exp == MINUS) exp_s = "минус";
        else if (exp == PLUS) exp_s = "плюс";
        else if (exp == RPAREN) exp_s = "закрывающая скобка";
        else if (exp == LPAREN) exp_s = "открывающая скобка";
        else if (exp == END_FILE) exp_s = "конец файла";
        else exp_s = "токен";
    
        error("Ожидался " + exp_s);
        _continue();
    }
}

Node* SyntaxisTree::parse_begin() {
    Node* node = new Node("Begin", Token(), cur_token.line);
    
    
    checker(PROGRAM);
    
    if (cur_token.type == IDENTIFIER) {
        Node* inode = new Node("Id", cur_token, cur_token.line);
        node->add(inode);
        hashTable.insert(cur_token);
        next_token();
    } else {
        error("Ожидался идентификатор после PROGRAM");
    }
    
    return node;
}

Node* SyntaxisTree::parse_end() {
    Node* node = new Node("End", Token(), cur_token.line);
    
    
    if (cur_token.type != END && errors.size() == 0) {
        error("Неправильно вырожение");
        return node;
    }
    
    checker(END);
    
    if (cur_token.type == IDENTIFIER) {
        Node* inode = new Node("Id", cur_token, cur_token.line);
        node->add(inode);
        hashTable.insert(cur_token);
        next_token();
    } else {
        error("Ожидался идентификатор после END");
    }
    
    return node;
}
Node* SyntaxisTree::parse_descriptions() {
    Node* node = new Node("Descriptions", Token(), cur_token.line);
    bool ok = false;
    while (cur_token.type == INTEGER || cur_token.type == REAL) {
        node->add(parse_descr());
        ok = true;
    }
    
    
    if (!ok) {
        error("Нету VARLIST/TYPE");
    }
    return node;
}


Node* SyntaxisTree::parse_type() {
    Node* node = new Node("Type", Token(), cur_token.line);
    
    if (cur_token.type == INTEGER) {
        Node* ntype = new Node("INTEGER", cur_token, cur_token.line);
        node->add(ntype);
        checker(INTEGER);
    }
    else if (cur_token.type == REAL) {
        Node* ntype = new Node("REAL", cur_token, cur_token.line);
        node->add(ntype);
        checker(REAL);
    }
    else {
        error("Ожидался тип (INTEGER или REAL)");
    }
    
    return node;
}

Node* SyntaxisTree::parse_var_list() {
    Node* node = new Node("VarList", Token(), cur_token.line);
    
    if (cur_token.type == IDENTIFIER) {
        bool ok = cur_token.value.back() == ',';
        if (ok) cur_token.value.pop_back();
        Node* inode = new Node("Id", cur_token, cur_token.line);
        node->add(inode);
        hashTable.insert(cur_token);
        checker(IDENTIFIER);
        
        while (ok && cur_token.type == IDENTIFIER) {
            ok = cur_token.value.back() == ',';
            if (ok) cur_token.value.pop_back();
            if (cur_token.type == IDENTIFIER) {
                Node* ninode = new Node("Id", cur_token, cur_token.line);
                node->add(ninode);
                hashTable.insert(cur_token);
                checker(IDENTIFIER);
            }
        }
        
        while (cur_token.type == ERROR) {
            error(cur_token.value);
            next_token();
        }
    } else {
        error("Ожидался идентификатор в списке переменных");
    }
    
    return node;
}

Node* SyntaxisTree::parse_descr() {
    Node* node = new Node("Descr", Token(), cur_token.line);
    
    
    node->add(parse_type());
    node->add(parse_var_list());
    
    return node;
}
Node* SyntaxisTree::parse_operator() {
    Node* node = new Node("Operators", Token(), cur_token.line);
    bool ok = false;
    while (cur_token.type == IDENTIFIER) {
        node->add(parse_op());
        ok = true;
    }
    
    if (!ok) {
        error("Нету выражений, никаких");
    }
    
    return node;
}

Node* SyntaxisTree::parse_op() {
    Node* node = new Node("Op", Token(), cur_token.line);
    
    if (cur_token.type == IDENTIFIER) {
        if (cur_token.value.back() == ',')
            error("Synt error: name " + cur_token.value);
    }
    if (cur_token.type == IDENTIFIER) {
        Node* inode = new Node("Id", cur_token, cur_token.line);
        node->add(inode);
        hashTable.insert(cur_token);
        checker(IDENTIFIER);
        
        if (cur_token.type == ASSIGN) {
            checker(ASSIGN);
            
            
            node->add(parse_expr());
        } else {
            error("Ожидался оператор присваивания '='");
        }
    } else {
        error("Ожидался идентификатор в левой части присваивания");
    }
    
    Node* tmp = node;
    while (tmp->child.size() > 0)
        tmp = tmp->child.back();
    
    if (tmp->token.type != RTOI && tmp->token.type != ITOR && tmp->token.type != IDENTIFIER && tmp->token.type != REAL_NUMBER && tmp->token.type != INT_NUMBER) error("Synt error: " + tmp->token.value);
    
    return node;
}


Node* SyntaxisTree::parse_expr() {
    Node* node = new Node("expr", Token(), cur_token.line);
    
    node->add(parse_simple_expr());
    
    if (cur_token.type == PLUS || cur_token.type == MINUS) {
        Node* opNode = new Node(
            cur_token.type == PLUS ? "Plus" : "Minus",
            cur_token, cur_token.line);
        node->add(opNode);
        next_token();
    
        node->add(parse_expr());
    }
    
    if (cur_token.type != IDENTIFIER && cur_token.type != END) {
        if (cur_token.type != RPAREN)
            error("Неправильное вырожение");
    }
    
    
    
    return node;
}

Node* SyntaxisTree::parse_simple_expr() {
    Node* node = new Node("dop_expr", Token(), cur_token.line);
    
    if (cur_token.type == IDENTIFIER) {
        Node* id = new Node("Id", cur_token, cur_token.line);
        if (cur_token.value.back() == ',')
            error("Synt error: name " + cur_token.value);
        node->add(id);
        hashTable.insert(cur_token);
        checker(IDENTIFIER);
    } else if (cur_token.type == INT_NUMBER || cur_token.type == REAL_NUMBER) {
        Node* tmp = new Node("Const", cur_token, cur_token.line);
        node->add(tmp);
        hashTable.insert(cur_token);
        next_token();
    } else if (cur_token.type == LPAREN) {
        checker(LPAREN);
//        ++d;
        node->add(parse_expr());
        checker(RPAREN);
//        --d;
    } else if (cur_token.type == RTOI || cur_token.type == ITOR) {
        Node* tmp = new Node(cur_token.type == ITOR ? "ITOR" : "RTOI", cur_token, cur_token.line);
        checker((tmp->token.type == ITOR ? ITOR : RTOI));
        checker(LPAREN);
        //            ++d;
        tmp->add(parse_expr());
        checker(RPAREN);
        //            --d;
        node->add(tmp);
    } else error("Неправильно вырожение");

    return node;
}


const std::vector<std::string>& SyntaxisTree::get_errors() const {
    return errors;
}

Node* SyntaxisTree::getTree() const {
    return root;
}

void SyntaxisTree::print_tree(Node* node, std::ofstream& file_out, int depth) {
    if (!node) return;
    
    std::string s(depth * 2, ' ');
    
    file_out << s << node->name;
    
    if (!node->token.value.empty()) {
            file_out << " [" << node->token.value << "]";
    }
    
    file_out << std::endl;
//    file_out << node->line << std::endl;
    
    for (const auto& child : node->child) {
        print_tree(child, file_out, depth + 1);
    }
}

void SyntaxisTree::print_tree(std::ofstream& fout) {
    
    print_tree(root, fout, 0);
    
}
