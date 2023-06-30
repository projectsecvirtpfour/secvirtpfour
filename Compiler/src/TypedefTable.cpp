#include "../include/pcheaders.h"
#include "../include/AST.h"
#include "../include/TypedefTable.h"

TypedefTable::TypedefTable(){
    std::unordered_map<std::string, TypedefTableEntry> templ;
    this->table.push_back(templ);
}

TypedefTable::~TypedefTable(){

}

bool TypedefTable::addEntry(const std::string& key, const TypedefTableEntry entry){
    if(this->table[table.size() - 1].find(key) != this->table[table.size() - 1].end())
        return false;

    this->table[table.size() - 1].emplace(key, entry);
    return true;
}

void TypedefTable::pushTypedefScope(){
    std::unordered_map<std::string, TypedefTableEntry> templ;
    this->table.push_back(templ);
}

void TypedefTable::popTypedefScope(){
    if(this->table.size() > 1)
        this->table.erase(this->table.end() - 1);
}

TypedefTableEntry* TypedefTable::getEntry(const std::string& key){
    int i = this->table.size() - 1;

    while(i >= 0){
        auto it = this->table[i].find(key);
        if(it != this->table[i].end())
            return &(it->second);
        i--;
    }

    return NULL;
}

std::vector<std::unordered_map<std::string, TypedefTableEntry>>* TypedefTable::getTable(){
    return &this->table;
}

void TypedefTable::Print(){
    for(int i = 0; i < (int) this->table.size(); i++){
        auto tableScope = this->table[i];
        for(auto& entry : tableScope){
            for(int j = 0; j < i; j++)
                printf("-");
            printf("%s...\n", entry.first.c_str());
        }
    }
}

