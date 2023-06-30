#ifndef RUNTIME_CFG_H
#define RUNTIME_CFG_H

#define RUNTIME_EXCEPT_CLIENT_CFG_NOT_FOUND_MSG "NOT_FOUND"
#define RUNTIME_EXCEPT_TABLE_FULL "TABLE_RESOURCE_EXHAUSTED"
#define RUNTIME_EXCEPT_MALFORMED_REQUEST "MALFORMED_REQUEST"

enum VirtTableType {VTT_NOT_SHARED = 0, VTT_HIDDEN_SHARED = 1, VTT_SHARED = 2};

typedef struct _virt_table_data{
    std::string name;
    VirtTableType type;
    std::set<int> programIDs;
    int totalSize;
} VirtTableData;

typedef struct _program_table_data{
    std::string name;
    std::string virtName;
    bool isSharedOwner;
    bool canWrite;

    int curProgSize;
    int maxProgSize;
} ProgramTableData;

typedef struct _program_action_data{
    std::string name;
    std::string virtName;
} ProgramActionData;

typedef struct _client_data{
    int clientProgID;
    P4InfoDesc* clientP4InfoDesc;
} ClientData;

class RuntimeCFG{
    public:
        RuntimeCFG(const std::string& clientCFGPath, const std::string& virtCFGPath, const std::string& virtP4InfoPath);
        ~RuntimeCFG();
        
        void Update(const std::string& clientCFGPath, const std::string& virtCFGPath, const std::string& virtP4InfoPath);

        p4::v1::WriteRequest Translate(const std::string& client, const p4::v1::WriteRequest* request);
        p4::v1::ReadRequest Translate(const std::string& client, const p4::v1::ReadRequest* request);

        p4::v1::ReadResponse Translate(const std::string& client, const p4::v1::ReadResponse* response);

    private:
        std::map<std::string, std::map<std::string, std::string>> m_clientsTableNameTranslator;
        std::map<std::string, std::map<std::string, std::string>> m_clientsActionNameTranslator;
        
        std::map<std::string, std::map<std::string, std::string>> m_clientsTableNameTranslatorReverse;
        std::map<std::string, std::map<std::string, std::string>> m_clientsActionNameTranslatorReverse;
        
        std::map<std::string, ClientData> m_clientsP4Info;
        std::map<std::string, VirtTableData> m_virtTableDatas;
        std::map<std::string, std::map<std::string, ProgramTableData>> m_programTableDatas;
        P4InfoDesc m_virtP4Info;

        static VirtTableData getVirtTableDataFromLine(const std::string& line);
        static ProgramTableData getProgramTableDataFromLine(const std::string& line);
        static ProgramActionData getProgramActionDataFromLine(const std::string& line);
};

#endif