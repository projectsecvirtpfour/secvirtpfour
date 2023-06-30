#include "../include/pcheaders.h"
#include "../include/IPLBase.hpp"
#include "../include/AST.h"
#include "../include/SymbolTable.h"

SymbolTable::SymbolTable(){
    this->m_parent = NULL;
}

SymbolTable::~SymbolTable(){
    for(auto it : m_table)
        if(it.second.subtable != NULL)
            delete it.second.subtable;
}

int SymbolTable::getBitwidthFromExpression(const std::string& expression, const std::string& controlBlock) const{
    if(controlBlock != ""){
        auto it = m_table.find(controlBlock);
        Assert(it != m_table.end());
        Assert(it->second.subtable != NULL);
        return it->second.subtable->getBitwidthFromExpression(expression, "");
    }
    int result = -1;

    if(expression.find(".") != std::string::npos){
        std::string firstPart = expression.substr(0, expression.find("."));

        SymbolTable const *current = this;
        SymbolTableEntry const *entry = NULL;
        while(entry == NULL && current != NULL){
            if(current->m_table.find(firstPart) != current->m_table.end()){
                entry = &current->m_table.find(firstPart)->second;
            }
            current = current->m_parent;
        }
        Assert(entry != NULL);
        std::string instType = entry->instUserType;
        Assert(instType != "");
        current = this;
        entry = NULL;
        while(entry == NULL && current != NULL){
            if(current->m_table.find(instType) != current->m_table.end()){
                entry = &current->m_table.find(instType)->second;
            }
            current = current->m_parent;
        }
        Assert(entry != NULL && entry->subtable != NULL);
        result = entry->subtable->getBitwidthFromExpression(expression.substr(firstPart.size() + 1), "");
    }
    else{
        SymbolTable const *current = this;
        SymbolTableEntry const *entry = NULL;
        while(entry == NULL && current != NULL){
            if(current->m_table.find(expression) != current->m_table.end()){
                entry = &current->m_table.find(expression)->second;
            }
            current = current->m_parent;
        }
        if(entry->type == STET_INST && !entry->instIsUserType)
            result = entry->instBitwidth;
    }

    return result;
}

void SymbolTable::addEntry(SymbolTable *root, const std::string& key, AST *ast){
    SymbolTableEntry newEntry;
    newEntry.key = key;
    newEntry.subtable = NULL;
    newEntry.instBitwidth = 0;
    switch(ast->getNodeType()){
        case NT_HEADER_TYPE_DECLARATION:
            newEntry.type = STET_DEC;
            newEntry.dectype = STEDT_HEADER;
            newEntry.subtable = new SymbolTable;
            newEntry.subtable->m_parent = this;
            if(ast->getChildNT(NT_STRUCT_FIELD_LIST) != NULL){
                for(AST *c : ast->getChildNT(NT_STRUCT_FIELD_LIST)->getChildren()){
                    newEntry.subtable->addEntry(this, c->getValue(), c);
                }
            }
            this->m_table.emplace(key, newEntry);
            break;
        case NT_STRUCT_TYPE_DECLARATION:
            newEntry.type = STET_DEC;
            newEntry.dectype = STEDT_STRUCT;
            newEntry.subtable = new SymbolTable;
            newEntry.subtable->m_parent = this;
            if(ast->getChildNT(NT_STRUCT_FIELD_LIST) != NULL){
                for(AST *c : ast->getChildNT(NT_STRUCT_FIELD_LIST)->getChildren()){
                    newEntry.subtable->addEntry(this, c->getValue(), c);
                }
            }
            this->m_table.emplace(key, newEntry);
            break;
        case NT_CONTROL_DECLARATION:
            newEntry.type = STET_DEC;
            newEntry.dectype = STEDT_CONTROL;
            newEntry.subtable = new SymbolTable;
            newEntry.subtable->m_parent = this;
            if(ast->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST) != NULL){
                for(AST* parameter : ast->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->getChildren())
                    newEntry.subtable->addEntry(this, parameter->getValue(), parameter);
            }
            this->m_table.emplace(key, newEntry);
            break;
        case NT_STRUCT_FIELD:
        case NT_PARAMETER:
            newEntry.type = STET_INST;
            if(ast->getChildNT(NT_BASE_TYPE) != NULL){
                newEntry.instIsUserType = false;
                if(ast->getChildNT(NT_BASE_TYPE)->getValue() == "bit" && ast->getChildNT(NT_BASE_TYPE)->getChildNT(NT_LITERAL) != NULL){
                    newEntry.instBitwidth = IPLBase::intFromStr(ast->getChildNT(NT_BASE_TYPE)->getChildNT(NT_LITERAL)->str());
                }
            }
            else if(ast->getChildNT(NT_TYPE_NAME) != NULL){
                newEntry.instIsUserType = true;
                if(root->m_table.find(ast->getChildNT(NT_TYPE_NAME)->getValue()) != root->m_table.end()){
                    newEntry.instUserType = ast->getChildNT(NT_TYPE_NAME)->getValue();
                }
            }
            this->m_table.emplace(key, newEntry);
            break;
        default:
            break;
    }
}

void SymbolTable::addEntry(const std::string& key, AST *ast){
    this->addEntry(NULL, key, ast);    
}

void SymbolTable::addEntry(AST *ast){
    std::string key = ast->getValue();
    if(ast->getNodeType() == NT_CONTROL_DECLARATION)
        key = ast->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getValue();
    if(ast->getNodeType() == NT_PARSER_DECLARATION)
        key = ast->getChildNT(NT_PARSER_TYPE_DECLARATION)->getValue();
    this->addEntry(key, ast);
}

void SymbolTable::Print() const{
    for(auto it : m_table){
        SymbolTableEntry entry = it.second;
        printf("%s:\n", it.first.c_str());
        switch(entry.type){
            case STET_DEC:
                printf("Type: Declaration ");
                switch(entry.dectype){
                    case STEDT_HEADER:
                        printf("(Header)\n");
                        break;
                    case STEDT_STRUCT:
                        printf("(Struct)\n");
                        break;
                    case STEDT_PARSER:
                        printf("(Parser)\n");
                        break;
                    case STEDT_CONTROL:
                        printf("(Control)\n");
                        break;
                }
                break;
            case STET_INST:
                printf("Type: Instantiation\n");
                if(entry.instIsUserType){
                    printf("UserType: %s\n", entry.instUserType.c_str());
                }
                else{
                    printf("Bitwidth: %d\n", entry.instBitwidth);
                }
                break;
        }
        if(entry.subtable != NULL){
            printf("Subtable: {\n");
            entry.subtable->Print();
            printf("}");
        }
        printf("\n");
    }
}