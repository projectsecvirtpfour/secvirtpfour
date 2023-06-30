#ifndef TOFINO_MERGE_H
#define TOFINO_MERGE_H

class TofinoMerge {
    public:
        static bool MergeTofino(const std::vector<AST*> programs, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts);
    private:
        static bool RemoveSwitchInstantiation(AST *ast);
        static bool GetPipelines(AST *ast, std::vector<AST*> *Pipelines);
        static bool MergePipelines(const std::vector<AST*>& programs, const std::vector<AST*>& pipelines, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts);
        };

#endif
