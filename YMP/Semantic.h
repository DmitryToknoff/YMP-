// Semantic.h
#pragma once

#include "SyntaxisTree.h"
#include <string>
#include <vector>
#include <map>
#include "hash_table.h"
#include <fstream>
#include <iostream>

class Semantic {
    std::string title;
    std::vector<std::string> errors;
    std::vector<std::string> postfix_code;
    HashTable ht;

    void add_error(const std::string& msg, int line);
    
    void analyze_program(Node* node);
    void analyze_begin(Node* node);
    void analyze_description(Node* node);
    void analyze_operator(Node* node);
    
    void analyze_end(Node* node);
    void analyze_expr(Node* node, std::vector<std::string>& p);
    
    void parse_op(Node* node);
    void parse_descr(Node* node);
    void parse_varlist(Node* node);
    
    void checker_variable(const std::string& s, int line);
public:

    Semantic();
    
    
    void start(Node* root);
    void print_file(std::ostream& os);
    std::vector<std::string> get_errors() const;
};
