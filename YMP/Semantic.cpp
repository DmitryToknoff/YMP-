#include <string>
#include "Semantic.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>

void Semantic::add_error(const std::string& msg, int line) {
    std::string ans = "ERROR at the line: " + std::to_string(line) + " " + msg;
    errors.push_back(ans);
}

void Semantic::checker_variable(const std::string& s, int line) {
    if (real.find(s) == real.end() && intager.find(s) == intager.end()) {
        add_error("Не сущ " + s, line);
    }
}


Semantic::Semantic() {}


void Semantic::start(Node* root) {
    analyze_program(root);
}


void Semantic::analyze_program(Node* node) {
    analyze_begin(node->child[0]);
    analyze_description(node->child[1]);
    analyze_operator(node->child[2]);
    analyze_end(node->child[3]);
}


void Semantic::analyze_begin(Node* node) {
    title = node->child[0]->token.value;
    postfix_code.push_back("PROGRAM " + title);
}


void Semantic::parse_descr(Node* node) {
    std::string type = node->child[0]->child[0]->name;
    std::vector<std::string> varlist;
    for (auto& id: node->child[1]->child) {
        if (real.find(id->token.value) != real.end() || intager.find(id->token.value) != intager.end() || id->token.value == title) {
            add_error("Уже есть такое название переменной/программы", id->token.line);
        } else {
            if (type == "REAL") real[id->token.value] = 1;
            else intager[id->token.value] = 1;
            
            varlist.push_back(id->token.value);
        }
    }
    
    std::string ans = type + " ";
    for (auto& s: varlist)
        ans += s + " ";
    
    ans += std::to_string(varlist.size() + 1) + " DECL";
    postfix_code.push_back(ans);
    
    
}



void Semantic::analyze_description(Node* node) {
    for (auto& c: node->child)
        parse_descr(c);
}


void Semantic::parse_op(Node *node) {
    
    
    std::string var_name = node->child[0]->token.value;
    checker_variable(var_name, node->child[0]->token.line);
    std::string var_type = (real.find(var_name) != real.end()) ? "REAL" : "INTEGER";
    
    std::vector<std::string> poliz;
    poliz.push_back(var_name);
    
    std::function<std::string(Node*)> dfs = [&](Node* cur) -> std::string {
        if (!cur) return "ERROR";
        
        if (cur->name == "Expr" && cur->child.size() == 3) {
            std::string left_type = dfs(cur->child[0]);
            
            std::string right_type = dfs(cur->child[2]);
            std::string op = cur->child[1]->name;
            
            if (left_type != "ERROR" && right_type != "ERROR") {
                if (left_type != right_type) {
                    add_error("Есть не явный каст ", cur->line);
                    return "ERROR";
                }
            } else {
                return "ERROR";
            }
            
            poliz.push_back(((op == "Plus") ? "+" : "-"));
            
            return left_type;
        }
        
        else if (cur->name == "SimpleExpr" ||
                (cur->name == "Expr" && cur->child.size() == 1)) {
            return dfs(cur->child[0]);
        }
        
        else if (cur->name == "ITOR") {
            std::string type = dfs(cur->child[0]);
            if (type == "ERROR") return "ERROR";
            
            if (type != "INTEGER") {
                add_error("ITORе ожидался инт ", cur->line);
                return "ERROR";
            }
            
            poliz.push_back("ITOR");
            poliz.push_back("CALL");
            
            return "REAL";
        }
        
        else if (cur->name == "RTOI") {
            std::string arg_type = dfs(cur->child[0]);
            
            if (arg_type == "ERROR") return "ERROR";
            
            if (arg_type != "REAL") {
                add_error("RTOI ожидался тип real", cur->line);
                return "ERROR";
            }
            
            poliz.push_back("RTOI");
            poliz.push_back("CALL");
            
            return "INTEGER";
        }
        
        else if (cur->name == "Id") {
            std::string var = cur->token.value;
            checker_variable(var, cur->line);
            
            std::string type = (real.find(var) != real.end()) ? "REAL" : "INTEGER";
            
            poliz.push_back(var);
            
            return type;
        }
        
        else if (cur->name == "Const") {
            std::string value = cur->token.value;
            
            std::string type;
            if (value.find('.') != std::string::npos) {
                type = "REAL";
            } else {
                type = "INTEGER";
            }
            
            poliz.push_back(value);
            
            return type;
        }
        
        return "ERROR";
    };
    
    std::string expr_type = dfs(node->child[1]);
    
    if (expr_type != "ERROR") {
        if (expr_type != var_type) {
            add_error("Отичаются типы до равно и после", node->line);
        }
    }
    
    poliz.push_back("=");
    
    std::string ans;
    for (const auto& token : poliz) {
        if (!ans.empty()) ans += " ";
        ans += token;
    }
    
    postfix_code.push_back(ans);
}
void Semantic::analyze_operator(Node* node) {
    
    for (auto& op: node->child)
        parse_op(op);
}


void Semantic::analyze_end(Node* node) {
    if (node->child[0]->token.value != title) add_error("Не совпадают название начала и конца программы ", node->child[0]->token.line);
    postfix_code.push_back({"END " + title});
}



void Semantic::print_file(std::ostream &os) {
    for (auto& s: postfix_code)
        os << s << std::endl;
}


std::vector<std::string> Semantic::get_errors() const {
    return errors;
}





