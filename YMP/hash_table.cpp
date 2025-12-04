#include "hash_table.h"
#include <fstream>
#include <iostream>

HashTable::HashTable() : cur(0) {
    t = new Hash*[size];
    for (int i = 0; i < size; i++) {
        t[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i < size; i++) {
        Hash* node = t[i];
        while (node != nullptr) {
            Hash* temp = node;
            node = node->next;
            delete temp;
        }
    }
    delete[] t;
}

int HashTable::_hash(const std::string& key) {
    long long hash = 0;
    long long mod = 1e9 + 7;
    long long x = 257;
    for (char c : key) {
        hash = ((hash * x + c) % mod) % size;
    }
    return static_cast<int>(hash);
}

int HashTable::insert(const Token& token) {
    Hash* ex = find(token);
    if (ex != nullptr) {
        return ex->index;
    }
    
    int hash = _hash(token.value);
    int index = cur++;
    
    Hash* new_node = new Hash(token.type, token.value, index);
    
    new_node->next = t[hash];
    t[hash] = new_node;
    
    return index;
}

Hash* HashTable::find(const Token& token) {
    int hash = _hash(token.value);
    Hash* node = t[hash];
    
    while (node != nullptr) {
        if (node->lex == token.value) {
            return node;
        }
        node = node->next;
    }
    
    return nullptr;
}
