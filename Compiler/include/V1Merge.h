#ifndef V1_MERGE_H
#define V1_MERGE_H

class V1Merge {
    public:
        static bool MergeV1(const std::vector<AST*> programs, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts);
    private:
        static bool GetPipeline(AST *ast, std::vector<AST*> *Pipeline);
        static bool MergePipelines(const std::vector<AST*>& programs, const std::vector<std::vector<AST*>>& pipelines, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts);
};

#endif
