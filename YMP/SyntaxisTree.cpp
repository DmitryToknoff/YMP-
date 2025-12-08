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
void SyntaxisTree::synchronize() {
    while (cur_token.type != END_FILE && cur_token.type != END && cur_token.type != INTEGER && cur_token.type != REAL && cur_token.type != IDENTIFIER) {
        next_token();
    }
}

Node* SyntaxisTree::parseProgram() {
    Node* node = new Node("Program", Token(), cur_token.line);
    
    node->add(parseBegin());
    node->add(parseDescriptions());
    node->add(parseOperators());
    node->add(parseEnd());
    
    return node;
}

bool SyntaxisTree::parse() {
    root = parseProgram();
    
    return errors.empty();
}



void SyntaxisTree::match(TokenType expected) {
    if (cur_token.type == expected) {
        next_token();
    } else {
        std::string expectedStr;
        switch(expected) {
            case PROGRAM: expectedStr = "PROGRAM"; break;
            case END: expectedStr = "END"; break;
            case INTEGER: expectedStr = "INTEGER"; break;
            case REAL: expectedStr = "REAL"; break;
            case ITOR: expectedStr = "ITOR"; break;
            case RTOI: expectedStr = "RTOI"; break;
            case IDENTIFIER: expectedStr = "идентификатор"; break;
            case INT_NUMBER: expectedStr = "целое число"; break;
            case REAL_NUMBER: expectedStr = "вещественное число"; break;
            case ASSIGN: expectedStr = "оператор присваивания (=)"; break;
            case PLUS: expectedStr = "плюс (+)"; break;
            case MINUS: expectedStr = "минус (-)"; break;
            case LPAREN: expectedStr = "открывающая скобка (()"; break;
            case RPAREN: expectedStr = "закрывающая скобка ())"; break;
            case END_FILE: expectedStr = "конец файла"; break;
            default: expectedStr = "токен"; break;
        }
        
        std::string gotStr = cur_token.value.empty() ?
            std::to_string(static_cast<int>(cur_token.type)) :
            "'" + cur_token.value + "'";
            
        error("Ожидался " + expectedStr + ", получен " + gotStr);
        synchronize();
    }
}


Node* SyntaxisTree::parseBegin() {
    Node* node = new Node("Begin", Token(), cur_token.line);
    
    
    match(PROGRAM);
    
    if (cur_token.type == IDENTIFIER) {
        Node* idNode = new Node("Id", cur_token, cur_token.line);
        node->add(idNode);
        hashTable.insert(cur_token);
        next_token();
    } else {
        error("Ожидался идентификатор после PROGRAM");
    }
    
    return node;
}

Node* SyntaxisTree::parseEnd() {
    Node* node = new Node("End", Token(), cur_token.line);
    
    
    if (cur_token.type != END && errors.size() == 0) {
        error("Неправильно вырожение");
        return node;
    }
    
    match(END);
    
    if (cur_token.type == IDENTIFIER) {
        Node* idNode = new Node("Id", cur_token, cur_token.line);
        node->add(idNode);
        hashTable.insert(cur_token);
        next_token();
    } else {
        error("Ожидался идентификатор после END");
    }
    
    return node;
}
Node* SyntaxisTree::parseDescriptions() {
    Node* node = new Node("Descriptions", Token(), cur_token.line);
    bool ok = false;
    while (cur_token.type == INTEGER || cur_token.type == REAL) {
        node->add(parseDescr());
        ok = true;
    }
    
    
    if (!ok) {
        error("Нету VARLIST/TYPE");
    }
    return node;
}


Node* SyntaxisTree::parseType() {
    Node* node = new Node("Type", Token(), cur_token.line);
    
    if (cur_token.type == INTEGER) {
        Node* typeNode = new Node("INTEGER", cur_token, cur_token.line);
        node->add(typeNode);
        match(INTEGER);
    }
    else if (cur_token.type == REAL) {
        Node* typeNode = new Node("REAL", cur_token, cur_token.line);
        node->add(typeNode);
        match(REAL);
    }
    else {
        error("Ожидался тип (INTEGER или REAL)");
    }
    
    return node;
}

Node* SyntaxisTree::parseVarList() {
    Node* node = new Node("VarList", Token(), cur_token.line);
    
    if (cur_token.type == IDENTIFIER) {
        bool ok = cur_token.value.back() == ',';
        if (ok) cur_token.value.pop_back();
        Node* idNode = new Node("Id", cur_token, cur_token.line);
        node->add(idNode);
        hashTable.insert(cur_token);
        match(IDENTIFIER);
        
        while (ok && cur_token.type == IDENTIFIER) {
            ok = cur_token.value.back() == ',';
            if (ok) cur_token.value.pop_back();
            if (cur_token.type == IDENTIFIER) {
                Node* nextIdNode = new Node("Id", cur_token, cur_token.line);
                node->add(nextIdNode);
                hashTable.insert(cur_token);
                match(IDENTIFIER);
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

Node* SyntaxisTree::parseDescr() {
    Node* node = new Node("Descr", Token(), cur_token.line);
    
    
    node->add(parseType());
    node->add(parseVarList());
    
    return node;
}
Node* SyntaxisTree::parseOperators() {
    Node* node = new Node("Operators", Token(), cur_token.line);
    bool ok = false;
    while (cur_token.type == IDENTIFIER) {
        node->add(parseOp());
        ok = true;
    }
    
    if (!ok) {
        error("Нету выражений, никаких");
    }
    
    return node;
}

Node* SyntaxisTree::parseOp() {
    Node* node = new Node("Op", Token(), cur_token.line);
    
    if (cur_token.type == IDENTIFIER) {
        if (cur_token.value.back() == ',')
            error("Synt error: name " + cur_token.value);
    }
    if (cur_token.type == IDENTIFIER) {
        Node* idNode = new Node("Id", cur_token, cur_token.line);
        node->add(idNode);
        hashTable.insert(cur_token);
        match(IDENTIFIER);
        
        if (cur_token.type == ASSIGN) {
            match(ASSIGN);
            
            
            node->add(parseExpr());
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


Node* SyntaxisTree::parseExpr() {
    Node* node = new Node("Expr", Token(), cur_token.line);
    
    node->add(parseSimpleExpr());
    
    if (cur_token.type == PLUS || cur_token.type == MINUS) {
        Node* opNode = new Node(
            cur_token.type == PLUS ? "Plus" : "Minus",
            cur_token, cur_token.line);
        node->add(opNode);
        next_token();
    
        node->add(parseExpr());
    }
    
    if (cur_token.type != IDENTIFIER && cur_token.type != END) {
        if (cur_token.type != RPAREN)
            error("Неправильное вырожение");
    }
    
    
    
    return node;
}

Node* SyntaxisTree::parseSimpleExpr() {
    Node* node = new Node("SimpleExpr", Token(), cur_token.line);
    
    switch (cur_token.type) {
        case IDENTIFIER: {
            Node* id = new Node("Id", cur_token, cur_token.line);
            if (cur_token.value.back() == ',')
                error("Synt error: name " + cur_token.value);
            node->add(id);
            hashTable.insert(cur_token);
            match(IDENTIFIER);
            break;
        }
        case INT_NUMBER:
        case REAL_NUMBER: {
            Node* tmp = new Node("Const", cur_token, cur_token.line);
            node->add(tmp);
            hashTable.insert(cur_token);
            next_token();
            break;
        }
        case LPAREN: {
            match(LPAREN);
            ++d;
            node->add(parseExpr());
            match(RPAREN);
//            --d;
            break;
        }
        case ITOR:
        case RTOI: {
            Node* tmp = new Node(
                cur_token.type == ITOR ? "ITOR" : "RTOI",
                cur_token, cur_token.line);
            match((tmp->token.type == ITOR ? ITOR : RTOI));
            match(LPAREN);
//            ++d;
            tmp->add(parseExpr());
            match(RPAREN);
//            --d;
            node->add(tmp);
            break;
        }
        default: {
            error("Неправильно вырожение");
            break;
        }
    }
    
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
