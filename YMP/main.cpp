#include <iostream>
#include <fstream>
#include "hash_table.h"
#include "lexer.h"
#include "SyntaxisTree.h"
#include <string>

using namespace std;

int main() {
    ifstream fin("input.txt");
    std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    HashTable ht;
    Lexer lexer(text, ht);
    
    SyntaxisTree st(lexer, ht);
    st.parse();
    ofstream fout("output.txt");
    st.print_tree(fout);
    fout << endl << endl << endl << endl;
    for (auto& nxt: st.get_errors())
        fout << nxt << endl;
        
    
    
    
    
    
    return 0;
}
