#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

typedef struct _define_macro{
    std::string name;
    std::vector<std::string> argNames;
    std::string body;
} PPDefineMacro;

class PreProcessor {
    public:
        PreProcessor(const std::string& filePath, const std::vector<std::string>& p4includePaths);

        void Process();
        void ProcessStripComments();
        void Print(FILE *out) const;

        P4Architecture getArch() const;
        
    private:
        P4Architecture m_p4architecture;
        std::string m_filePath;
        std::vector<std::string> m_fileLines;
        std::vector<std::string> m_p4includePaths;
        std::map<std::string, PPDefineMacro> m_defineMacros;

        void ProcessInclude(const int line, const std::string& includeFile, std::vector<std::string>* currentFilePathStack);
        void ProcessDefine(const int line, const std::vector<std::string>& stringSplit);

        void ReplaceDefine(const int startLine, const PPDefineMacro& ppdm);

        static bool EvalIfStringSplit(const std::vector<std::string>& stringSplit);

        static void PrintDefineMacro(const PPDefineMacro& ppdm);
        static std::vector<std::string> readFileLines(const std::string& filePath);
};

inline P4Architecture PreProcessor::getArch() const { return this->m_p4architecture; }

#endif