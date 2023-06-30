#ifndef TYPEDEF_TABLE_H
#define TYPEDEF_TABLE_H

typedef struct simbolInfo{
    AST *ast;
} TypedefTableEntry;

class TypedefTable{
    public:
        TypedefTable();
        ~TypedefTable();
    
        //Returns false if entry already exists
        bool addEntry(const std::string& key, const TypedefTableEntry entry);

        //Push a scope
        void pushTypedefScope();
        //Pop a scope
        void popTypedefScope();

        //Begins search on current scope to global scope. Returns NULL if not found
        TypedefTableEntry *getEntry(const std::string& key);

        std::vector<std::unordered_map<std::string, TypedefTableEntry>> *getTable();

        void Print();

    private:
        std::vector<std::unordered_map<std::string, TypedefTableEntry>> table;
};

#endif
