#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

class SymbolTable;

enum SymbolTableEntryType {STET_DEC, STET_INST};
enum SymbolTableEntryDecType {STEDT_HEADER, STEDT_STRUCT, STEDT_PARSER, STEDT_CONTROL};

typedef struct _symbol_table_entry{
    std::string key;
    SymbolTableEntryType type;
    SymbolTable* subtable;
    
    //For declarations
    SymbolTableEntryDecType dectype;

    //For instantiations
    bool instIsUserType;
    std::string instUserType;
    int instBitwidth;
} SymbolTableEntry;

class SymbolTable{
    public:
        SymbolTable();
        ~SymbolTable();

        //Returns -1 if not found
        int getBitwidthFromExpression(const std::string& expression, const std::string& controlBlock) const;

        void addEntry(const std::string& key, AST *ast);
        void addEntry(AST *ast);

        void Print() const;

    private:
        SymbolTable *m_parent;
        std::unordered_map<std::string, SymbolTableEntry> m_table;

        void addEntry(SymbolTable *root, const std::string& key, AST *ast);
};

#endif
