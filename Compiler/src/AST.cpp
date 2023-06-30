#include "../include/pcheaders.h"
#include "../include/AST.h"
#include "../include/SymbolTable.h"
#include "../include/TypedefTable.h"
#include "../include/TofinoMerge.h"
#include "../include/V1Merge.h"
#include "../include/Log.h"
#include "../include/CFG.h"

#include "../include/IPLBase.hpp"

std::string AST::TAB_SPACES = "";

std::string AST::NextVirtTypeID(){
    static int x = 0;
    char buffer[256];
    sprintf(buffer, "VirtType%d", x);
    x++;
    return std::string(buffer);
}

AST::AST(){

}

AST::AST(LexVal *v, const NodeType nType){
    this->nType = nType;

    if(v != NULL){
        this->value = *v;
        delete v;
    }
}

AST::AST(const std::string& svalue, const NodeType nType){
    this->nType = nType;
    this->value = svalue;
}

AST::~AST(){
    for(AST* c : this->children)
        NullDelete(c);
}

AST* AST::Clone(){
    AST* newAST = new AST(this->value, this->nType);
    newAST->programType = this->programType;

    newAST->children.resize(this->children.size(), NULL);
    for(int i = 0; i < (int) this->children.size(); i++){
        if(this->children[i] != NULL){
            newAST->children[i] = this->children[i]->Clone();
        }
    }

    return newAST;
}

std::string AST::NodeTypeToString(NodeType nt){
    std::string result;
    switch (nt){
        case NT_NONE:
            result = "NT_NONE";
            break;
        case NT_PROGRAM:
            result = "NT_PROGRAM";
            break;
        case NT_EXTERN_DECLARATION:
            result = "NT_EXTERN_DECLARATION";
            break;
        case NT_CONTROL_DECLARATION:
            result = "NT_CONTROL_DECLARATION";
            break;
        case NT_CONTROL_TYPE_DECLARATION:
            result = "NT_CONTROL_TYPE_DECLARATION";
            break;
        case NT_ANNOTATION_LIST:
            result = "NT_ANNOTATION_LIST";
            break;
        case NT_ANNOTATION:
            result = "NT_ANNOTATION";
            break;
        case NT_ANNOTATION_BODY:
            result = "NT_ANNOTATION_BODY";
            break;
        case NT_PARSER_DECLARATION:
            result = "NT_PARSER_DECLARATION";
            break;
        case NT_PARSER_TYPE_DECLARATION:
            result = "NT_PARSER_TYPE_DECLARATION";
            break;
        case NT_PACKAGE_TYPE_DECLARATION:
            result = "NT_PACKAGE_TYPE_DECLARATION";
            break;
        case NT_HEADER_TYPE_DECLARATION:
            result = "NT_HEADER_TYPE_DECLARATION";
            break;
        case NT_HEADER_UNION_DECLARATION:
            result = "NT_HEADER_UNION_DECLARATION";
            break;
        case NT_STRUCT_TYPE_DECLARATION:
            result = "NT_STRUCT_TYPE_DECLARATION";
            break;
        case NT_TYPEDEF_DECLARATION:
            result = "NT_TYPEDEF_DECLARATION";
            break;
        case NT_ENUM_DECLARATION:
            result = "NT_ENUM_DECLARATION";
            break;
        case NT_ERROR_DECLARATION:
            result = "NT_ERROR_DECLARATION";
            break;
        case NT_MATCH_KIND_DECLARATION:
            result = "NT_MATCH_KIND_DECLARATION";
            break;
        case NT_IDENTIFIER_LIST:
            result = "NT_IDENTIFIER_LIST";
            break;
        case NT_ANNOTATION_TOKEN_LIST:
            result = "NT_ANNOTATION_TOKEN_LIST";
            break;
        case NT_ANNOTATION_TOKEN:
            result = "NT_ANNOTATION_TOKEN";
            break;
        case NT_CONTROL_LOCAL_DEC_LIST:
            result = "NT_CONTROL_LOCAL_DEC_LIST";
            break;
        case NT_DIRECTION:
            result = "NT_DIRECTION";
            break;
        case NT_PARAMETER:
            result = "NT_PARAMETER";
            break;
        case NT_PARAMETER_LIST:
            result = "NT_PARAMETER_LIST";
            break;
        case NT_ARGUMENT:
            result = "NT_ARGUMENT";
            break;
        case NT_ARGUMENT_LIST:
            result = "NT_ARGUMENT_LIST";
            break;
        case NT_ASSIGN_STATEMENT:
            result = "NT_ASSIGN_STATEMENT";
            break;
        case NT_SWITCH_STATEMENT:
            result = "NT_SWITCH_STATEMENT";
            break;
        case NT_SWITCH_CASE:
            result = "NT_SWITCH_CASE";
            break;
        case NT_SWITCH_CASE_LIST:
            result = "NT_SWITCH_CASE_LIST";
            break;
        case NT_METHOD_CALL_STATEMENT:
            result = "NT_METHOD_CALL_STATEMENT";
            break;
        case NT_CONDITIONAL_STATEMENT:
            result = "NT_CONDITIONAL_STATEMENT";
            break;
        case NT_METHOD_PROTOTYPE:
            result = "NT_METHOD_PROTOTYPE";
            break;
        case NT_EXIT_STATEMENT:
            result = "NT_EXIT_STATEMENT";
            break;
        case NT_RETURN_STATEMENT:
            result = "NT_RETURN_STATEMENT";
            break;
        case NT_EMPTY_STATEMENT:
            result = "NT_EMPTY_STATEMENT";
            break;
        case NT_STAT_OR_DEC_LIST:
            result = "NT_STAT_OR_DEC_LIST";
            break;
        case NT_EXPRESSION:
            result = "NT_EXPRESSION";
            break;
        case NT_BASE_TYPE:
            result = "NT_BASE_TYPE";
            break;
        case NT_TUPLE_TYPE:
            result = "NT_TUPLE_TYPE";
            break;
        case NT_TYPE_NAME:
            result = "NT_TYPE_NAME";
            break;
        case NT_LITERAL:
            result = "NT_LITERAL";
            break;
        case NT_STRUCT_FIELD_LIST:
            result = "NT_STRUCT_FIELD_LIST";
            break;
        case NT_STRUCT_FIELD:
            result = "NT_STRUCT_FIELD";
            break;
        case NT_PARSER_STATEMENT_LIST:
            result = "NT_PARSER_STATEMENT_LIST";
            break;
        case NT_PARSER_LOCAL_ELEMENT_LIST:
            result = "NT_PARSER_LOCAL_ELEMENT_LIST";
            break;
        case NT_PARSER_STATE:
            result = "NT_PARSER_STATE";
            break;
        case NT_PARSER_STATE_LIST:
            result = "NT_PARSER_STATE_LIST";
            break;
        case NT_VARIABLE_DECLARATION:
            result = "NT_VARIABLE_DECLARATION";
            break;
        case NT_CONSTANT_DECLARATION:
            result = "NT_CONSTANT_DECLARATION";
            break;
        case NT_ACTION_DECLARATION:
            result = "NT_ACTION_DECLARATION";
            break;
        case NT_TABLE_DECLARATION:
            result = "NT_TABLE_DECLARATION";
            break;
        case NT_INSTANTIATION:
            result = "NT_INSTANTIATION";
            break;
        case NT_TABLE_PROPERTY_LIST:
            result = "NT_TABLE_PROPERTY_LIST";
            break;
        case NT_TABLE_PROPERTY:
            result = "NT_TABLE_PROPERTY";
            break;
        case NT_ACTION_LIST:
            result = "NT_ACTION_LIST";
            break;
        case NT_ACTION_REF:
            result = "NT_ACTION_REF";
            break;
        case NT_ENTRIES_LIST:
            result = "NT_ENTRIES_LIST";
            break;
        case NT_ENTRY:
            result = "NT_ENTRY";
            break;
        case NT_KEY_ELEMENT_LIST:
            result = "NT_KEY_ELEMENT_LIST";
            break;
        case NT_KEY_ELEMENT:
            result = "NT_KEY_ELEMENT";
            break;
        case NT_SELECT_EXPRESSION:
            result = "NT_SELECT_EXPRESSION";
            break;
        case NT_MASK:
            result = "NT_MASK";
            break;
        case NT_RANGE:
            result = "NT_RANGE";
            break;
        case NT_SIMPLE_KEYSET_DEFAULT:
            result = "NT_SIMPLE_KEYSET_DEFAULT";
            break;
        case NT_SIMPLE_KEYSET_DONTCARE:
            result = "NT_SIMPLE_KEYSET_DONTCARE";
            break;
        case NT_SIMPLE_EXPRESSION_LIST:
            result = "NT_SIMPLE_EXPRESSION_LIST";
            break;
        case NT_TUPLE_KEYSET_EXPRESSION:
            result = "NT_TUPLE_KEYSET_EXPRESSION";
            break;
        case NT_SELECT_CASE:
            result = "NT_SELECT_CASE";
            break;
        case NT_SELECT_CASE_LIST:
            result = "NT_SELECT_CASE_LIST";
            break;
        case NT_NAME_STATE_EXPRESSION:
            result = "NT_NAME_STATE_EXPRESSION";
            break;
        case NT_DIRECT_APPLICATION:
            result = "NT_DIRECT_APPLICATION";
            break;
        case NT_VALUE_SET_DECLARATION:
            result = "NT_VALUE_SET_DECLARATION";
            break;
        case NT_TYPEARG_VOID:
            result = "NT_TYPEARG_VOID";
            break;
        case NT_TYPEARG_DONTCARE:
            result = "NT_TYPEARG_DONTCARE";
            break;
        case NT_TYPE_ARGUMENT_LIST:
            result = "NT_TYPE_ARGUMENT_LIST";
            break;
        case NT_TYPEARG_NONTYPENAME:
            result = "NT_TYPEARG_NONTYPENAME";
            break;
        case NT_EXPRESSION_LIST:
            result = "NT_EXPRESSION_LIST";
            break;
        case NT_METHOD_PROTOTYPE_LIST:
            result = "NT_METHOD_PROTOTYPE_LIST";
            break;
        case NT_CONSTRUCTOR_METHOD_PROTOTYPE:
            result = "NT_CONSTRUCTOR_METHOD_PROTOTYPE";
            break;
        case NT_FUNCTION_PROTOTYPE:
            result = "NT_FUNCTION_PROTOTYPE";
            break;
        case NT_FUNCTION_DECLARATION:
            result = "NT_FUNCTION_DECLARATION";
            break;
        case NT_ABSTRACT_FUNCTION_PROTOTYPE:
            result = "NT_ABSTRACT_FUNCTION_PROTOTYPE";
            break;
        case NT_TYPE_OR_VOID:
            result = "NT_TYPE_OR_VOID";
            break;
        case NT_TYPE_PARAMETER:
            result = "NT_TYPE_PARAMETER";
            break;
        case NT_TYPE_PARAMETER_LIST:
            result = "NT_TYPE_PARAMETER_LIST";
            break;
        case NT_SPECIFIED_IDENTIFIER_LIST:
            result = "NT_SPECIFIED_IDENTIFIER_LIST";
            break;
        case NT_SPECIFIED_IDENTIFIER:
            result = "NT_SPECIFIED_IDENTIFIER";
            break;
    }
    return result;
}

bool BothOrNoneStart(AST* state, AST* state2){
    const std::string n1 = state->getValue().substr(state->getValue().find("_") + 1);
    const std::string n2 = state2->getValue().substr(state2->getValue().find("_") + 1);
    return ((n1 != "start" && n2 != "start") || (n1 == n2));
}

void AST::MergeParserEqualStates(AST *stateList, std::vector<std::vector<int>>& statePrograms){
    char buffer[BUFFER_SIZE];
    
    std::vector<std::set<int>> equalStates;
    for(int i = 0; i < (int) stateList->children.size() - 1; i++){
        AST *state = stateList->children[i];
        for(int j = i + 1; j < (int) stateList->children.size(); j++){
            AST *state2 =  stateList->children[j];
            if(AST::Equals(state, state2) && BothOrNoneStart(state, state2)){
                bool found = false;
                for(std::set<int>& eqs : equalStates){
                    if(eqs.find(i) != eqs.end()){
                        found = true;
                        if(eqs.find(j) == eqs.end())
                            eqs.emplace(j);
                    }
                }
                if(!found){
                    equalStates.push_back({i, j});
                }
            }
        }
    }
    
    std::set<AST*> deleteStates;
    for(std::set<int> eqSet : equalStates){
        std::set<std::string> names;
        std::string newStateName = "P";
        std::vector<int> newStateProgs;
        for(int x: eqSet){
            deleteStates.emplace(stateList->children[x]);
            names.emplace(stateList->children[x]->value);
            for(int program : statePrograms[x]){
                sprintf(buffer, "s%d", program + 1);
                newStateProgs.push_back(program);
                newStateName = newStateName + std::string(buffer);
            }
        }

        int ulp = 2;
        while((*names.begin())[ulp] != '_')
            ulp++;
        newStateName = newStateName + (*names.begin()).substr(ulp);

        for(int i = 0; i < (int) stateList->children.size(); i++){
            AST* state = stateList->children[i];
            if(state->children[1]->nType == NT_NAME_STATE_EXPRESSION){
                if(names.find(state->children[1]->value) != names.end()){
                    state->children[1]->value = newStateName;
                }
            }
            else{
                for(AST *selectCase : state->children[1]->children[1]->children){
                    if(names.find(selectCase->value) != names.end()){
                        selectCase->value = newStateName;
                    }
                }
            }
        }

        AST* newState = stateList->children[*eqSet.begin()]->Clone();
        newState->value = newStateName;
        stateList->addChild(newState);
        statePrograms.push_back(newStateProgs);

        std::string logM = "Merged states [ ";
        for(const std::string& name : names)
            logM = logM + name + " ";
        logM = logM + "] into state \"" + newStateName + "\"";
        Log::MSG(logM);
    }

    int sc = 0;
    while(sc < (int) stateList->children.size()){
        AST* state = stateList->children[sc];
        if(deleteStates.find(state) != deleteStates.end()){
            delete state;
            stateList->children.erase(stateList->children.begin() + sc);
            statePrograms.erase(statePrograms.begin() + sc);
            sc--;
        }
        sc++;
    }

    if(equalStates.size() > 0)
        MergeParserEqualStates(stateList, statePrograms);
}

bool AST::MergeParserSemiEqualStates(AST *stateList, std::vector<std::vector<int>>& statePrograms, const std::vector<std::vector<std::pair<int, int>>>& continuousPorts, AST* IG_PORT_EXP){
    char buffer[256];
    
    std::vector<std::set<int>> semiEqualStates;
    for(int i = 0; i < (int) stateList->children.size() - 1; i++){
        AST *state = stateList->children[i];
        for(int j = i + 1; j < (int) stateList->children.size(); j++){
            AST *state2 =  stateList->children[j];
            if(AST::Equals(state->getChildNT(NT_PARSER_STATEMENT_LIST), state2->getChildNT(NT_PARSER_STATEMENT_LIST)) && BothOrNoneStart(state, state2)){
                bool canMerge = false;
                if(state->children[1]->nType == NT_NAME_STATE_EXPRESSION || state2->children[1]->nType == NT_NAME_STATE_EXPRESSION)
                    canMerge = true;
                //Expression lists must be the same
                else if(AST::Equals(state->children[1]->children[0], state2->children[1]->children[0]))
                    canMerge = true;

                if(canMerge){
                    bool found = false;
                    for(std::set<int>& s : semiEqualStates){
                        if(s.find(i) != s.end()){
                            if(s.find(j) == s.end())
                                s.emplace(j);
                            found = true;
                        }
                    }
                    if(!found){
                        semiEqualStates.push_back({i, j});
                    }
                }
            }
        }
    }

    for(const std::set<int>& ses : semiEqualStates){
        std::vector<AST*> states;
        std::set<std::string> names;
        AST* newState;
        for(int i : ses){
            states.push_back(stateList->children[i]);
            names.emplace(stateList->children[i]->value);
        }
        
        int firstSE = 0;
        while(firstSE < (int) states.size() && states[firstSE]->children[1]->nType != NT_SELECT_EXPRESSION)
            firstSE++;
        
        if(firstSE == (int) states.size()){ //Only transitions without select
            newState = states[0]->Clone();
            delete newState->children[1];
            AST *newTransition = new AST("select", NT_SELECT_EXPRESSION);
            newState->children[1] = newTransition;
            AST *newExpList = new AST(NULL, NT_EXPRESSION_LIST);
            if(IG_PORT_EXP != NULL)
                newExpList->addChild(IG_PORT_EXP->Clone());
            newTransition->addChild(newExpList);
            AST *newSelectCaseList = new AST(NULL, NT_SELECT_CASE_LIST);
            newTransition->addChild(newSelectCaseList);

            for(int stateN : ses){
                for(int program : statePrograms[stateN]){
                    for(const std::pair<int, int>& pr : continuousPorts[program]){
                        AST *newSCCase = new AST(stateList->children[stateN]->children[1]->value, NT_SELECT_CASE);
                        if(pr.first == pr.second){
                            sprintf(buffer, "%d", pr.first);
                            newSCCase->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                        }
                        else{
                            AST *rangeAST = new AST("..", NT_RANGE);
                            sprintf(buffer, "%d", pr.first);
                            rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                            sprintf(buffer, "%d", pr.second);
                            rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                            newSCCase->addChild(rangeAST);
                        }
                        newSelectCaseList->addChild(newSCCase);
                    }
                }
            }
        }
        else{
            newState = states[firstSE]->Clone();
            AST* newExpList = newState->getChildNT(NT_SELECT_EXPRESSION)->getChildNT(NT_EXPRESSION_LIST);
            if(IG_PORT_EXP != NULL)
                newExpList->addChildStart(IG_PORT_EXP->Clone());

            std::vector<AST*> selectCaseLists(states.size());

            AST *newSelectCaseList = new AST(NULL, NT_SELECT_CASE_LIST);
            for(int stateN : ses){
                if(stateList->children[stateN]->children[1]->nType == NT_NAME_STATE_EXPRESSION){
                    for(int program : statePrograms[stateN]){
                        for(const std::pair<int, int>& portRange : continuousPorts[program]){
                            AST *newSC = new AST(stateList->children[stateN]->children[1]->value, NT_SELECT_CASE);
                            newSC->addChild(new AST(NULL, NT_TUPLE_KEYSET_EXPRESSION));
                            newSC->children[0]->addChild(new AST(NULL, NT_SIMPLE_EXPRESSION_LIST));
                            if(portRange.first == portRange.second){
                                sprintf(buffer, "%d", portRange.first);
                                newSC->children[0]->children[0]->addChildStart(new AST(std::string(buffer), NT_EXPRESSION));
                            }
                            else{
                                AST *rangeAST = new AST("..", NT_RANGE);
                                sprintf(buffer, "%d", portRange.first);
                                rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                                sprintf(buffer, "%d", portRange.second);
                                rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                                newSC->children[0]->children[0]->addChildStart(rangeAST);
                            }
                            for(int i = 1; i < (int) newState->getChildNT(NT_SELECT_EXPRESSION)->getChildNT(NT_EXPRESSION_LIST)->children.size(); i++){
                                newSC->children[0]->children[0]->addChild(new AST("_", NT_SIMPLE_KEYSET_DONTCARE));
                            }
                            newSelectCaseList->addChild(newSC);
                        }
                    }
                }
                else{
                    for(int program : statePrograms[stateN]){
                        for(AST *sCase : stateList->children[stateN]->getChildNT(NT_SELECT_EXPRESSION)->getChildNT(NT_SELECT_CASE_LIST)->children){
                            AST *newSC = sCase->Clone();
                            AST *aux = newSC->children[0];
                            newSC->children[0] = new AST(NULL, NT_TUPLE_KEYSET_EXPRESSION);
                            newSC->children[0]->addChild(new AST(NULL, NT_SIMPLE_EXPRESSION_LIST));
                            newSC->children[0]->children[0]->addChild(aux);
                            if(aux->nType == NT_SIMPLE_KEYSET_DEFAULT){
                                aux->nType = NT_SIMPLE_KEYSET_DONTCARE;
                                aux->value = "_";
                            }
                            for(const std::pair<int, int>& portRange : continuousPorts[program]){
                                AST *newSCCase = newSC->Clone();
                                if(portRange.first == portRange.second){
                                    sprintf(buffer, "%d", portRange.first);
                                    newSCCase->children[0]->children[0]->addChildStart(new AST(std::string(buffer), NT_EXPRESSION));
                                }
                                else{
                                    AST *rangeAST = new AST("..", NT_RANGE);
                                    sprintf(buffer, "%d", portRange.first);
                                    rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                                    sprintf(buffer, "%d", portRange.second);
                                    rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                                    newSCCase->children[0]->children[0]->addChildStart(rangeAST);
                                }
                                newSelectCaseList->addChild(newSCCase);
                            }
                            delete newSC;
                        }
                    }
                }
            }

            delete newState->children[1]->children[1];
            newState->children[1]->children[1] = newSelectCaseList;
        }

        stateList->addChild(newState);
        statePrograms.push_back({});

        std::string newStateName = "P";
        for(int stateN : ses){
            for(int program : statePrograms[stateN]){
                sprintf(buffer, "s%d", program + 1);
                newStateName = newStateName + std::string(buffer);
                statePrograms[statePrograms.size() - 1].push_back(program);
            }
        }
        newStateName = newStateName + newState->value.substr(newState->value.find("_"));

        for(int i = 0; i < (int) stateList->children.size(); i++){
            AST* state = stateList->children[i];
            if(state->children[1]->nType == NT_NAME_STATE_EXPRESSION){
                if(names.find(state->children[1]->value) != names.end()){
                    state->children[1]->value = newStateName;
                }
            }
            else{
                for(AST *selectCase : state->children[1]->children[1]->children){
                    if(names.find(selectCase->value) != names.end()){
                        selectCase->value = newStateName;
                    }
                }
            }
        }
        newState->value = newStateName;

        std::string LogM = "Merged states [";
        for(std::string name : names)
            LogM = LogM + " " + name;
        LogM = LogM + " ] into state \"" + newStateName + "\"";
        Log::MSG(LogM);
    }

    std::set<int> toDelete;
    for(const std::set<int>& ses : semiEqualStates)
        for(int s : ses)
            toDelete.emplace(s);

    int dc = 0;
    int sc = 0;
    while(sc < (int) stateList->children.size()){
        if(toDelete.find(sc + dc) != toDelete.end()){
            delete stateList->children[sc];
            stateList->children.erase(stateList->children.begin() + sc);
            statePrograms.erase(statePrograms.begin() + sc);
            sc--;
            dc++;
        }
        sc++;
    }

    return (semiEqualStates.size() > 0);
}

bool AST::MergeRange(AST *r1, AST *r2){
    if(r1->nType == NT_RANGE && r2->nType == NT_RANGE){
        const int r11 = IPLBase::intFromStr(r1->children[0]->str());
        const int r12 = IPLBase::intFromStr(r1->children[1]->str());
        const int r21 = IPLBase::intFromStr(r2->children[0]->str());
        const int r22 = IPLBase::intFromStr(r2->children[1]->str());
        
        if((r11 <= r22 && r21 - 1 <= r12) || (r21 <= r12 && r11 - 1 <= r22)){
            r1->children[0]->value = r11 < r21 ? r1->children[0]->value : r2->children[0]->value;
            r1->children[1]->value = r12 > r22 ? r1->children[1]->value : r2->children[1]->value;
            return true;
        }
    }

    return false;
}

void AST::WriteParserDOT(const std::string& fileName){
    char buffer[256];

    if(this->nType != NT_PARSER_DECLARATION)
        return;

    FILE *fp = fopen(fileName.c_str(), "w");
    Assert(fp != NULL);
    if(!fp)
        return;

    std::unordered_map<std::string, std::set<std::string>> transitions; 

    for(AST* state : this->getChildNT(NT_PARSER_STATE_LIST)->children){
        if(state->getChildNT(NT_NAME_STATE_EXPRESSION) != NULL){
            transitions.emplace(state->value, std::set<std::string>({state->getChildNT(NT_NAME_STATE_EXPRESSION)->value}));
        }
        else{
            transitions.emplace(state->value, std::set<std::string>());
            AST* SCL = state->getChildNT(NT_SELECT_EXPRESSION)->getChildNT(NT_SELECT_CASE_LIST);
            for(AST *nextState : SCL->children)
                transitions.find(state->value)->second.emplace(nextState->value);
        }
    }

    fprintf(fp, "digraph AST {\n");
    for(AST *state : this->getChildNT(NT_PARSER_STATE_LIST)->children){
        sprintf(buffer, "%s", state->value.c_str());
        fprintf(fp, "\"%s\" [ shape=record, style=\"filled\", fillcolor=cornsilk, label=\"%s\" ];\n", buffer, buffer);
    }
    fprintf(fp, "accept [ shape=record, style=\"filled\", fillcolor=cornsilk, label=\"accept\" ];\n");
    fprintf(fp, "reject [ shape=record, style=\"filled\", fillcolor=cornsilk, label=\"reject\" ];\n");
    for(AST *state : this->getChildNT(NT_PARSER_STATE_LIST)->children){
        sprintf(buffer, "%s", state->value.c_str());
        for(const std::string& transition : transitions.find(state->value)->second){
            char buffer2[256];
            sprintf(buffer2, "%s", transition.c_str());
            fprintf(fp, "\"%s\" -> \"%s\"\n", buffer, buffer2);
        }
    }
    fprintf(fp, "}\n");
    fclose(fp);
}

std::vector<AST*> AST::MergeParsers(std::vector<AST*> programs, std::vector<AST*> parsers, const std::vector<AST*>& headersDec, const std::string& newParserName, const std::vector<std::set<int>>& virtualSwitchPorts, AST* IG_PORT_EXP) {
    Assert(programs.size() >= 2);
    Assert(programs.size() == parsers.size() && programs.size() == virtualSwitchPorts.size());
    
    std::string LogM;
    LogM = "Merging Parsers [ ";
    for(int i = 0; i < (int) parsers.size(); i++)
        LogM = LogM + "\"" + parsers[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value + "\" ";
    LogM = LogM + "] into \"" + newParserName + "\"";
    Log::MSG(LogM);
    Log::Push();

    char buffer[256];
    std::vector<AST*> newNodes;
    
    //Determine continuous port allocations
    std::vector<std::vector<std::pair<int, int>>> continuousPorts;
    for(const std::set<int>& ports : virtualSwitchPorts)
        continuousPorts.push_back(AST::GetContinuous(ports));

    //Determine first and last ports used
    int FIRST_SWITCH_PORT = 100000;
    int LAST_SWITCH_PORT = 0;
    for(const std::set<int>& ports : virtualSwitchPorts){
        for(int p : ports){
            if(p < FIRST_SWITCH_PORT)
                FIRST_SWITCH_PORT = p;
            if(p > LAST_SWITCH_PORT)
                LAST_SWITCH_PORT = p;
        }
    }

    std::vector<AST*> TempLocalElementLists;
    std::vector<AST*> TempStateLists;
    for(AST* parser : parsers){
        TempLocalElementLists.push_back(NULL_CLONE(parser->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST)));
        TempStateLists.push_back(NULL_CLONE(parser->getChildNT(NT_PARSER_STATE_LIST)));
    }
    
    AST *newParser = new AST(NULL, NT_PARSER_DECLARATION);
    AST *ParserTypeDec = new AST(NULL, NT_PARSER_TYPE_DECLARATION);
    newParser->addChild(ParserTypeDec);
    ParserTypeDec->value = newParserName;
    for(int j = 0; j < 2; j++)
        ParserTypeDec->addChild(NULL_CLONE(parsers[0]->getChildNT(NT_PARSER_TYPE_DECLARATION)->children[j]));
    AST *ParserParameterList = new AST(NULL, NT_PARAMETER_LIST);
    ParserTypeDec->addChild(ParserParameterList);

    int minParamN = (int) parsers[0]->getChildNT(NT_PARSER_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children.size();
    for(int j = 1; j < (int) parsers.size(); j++)
        minParamN = MIN(minParamN, (int) parsers[0]->getChildNT(NT_PARSER_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children.size());

    //TODO: Merge other params (different number of params)
    for(int j = 0; j < minParamN; j++){
        std::vector<AST*> params;
        for(AST *parser : parsers)
            params.push_back(parser->getChildNT(NT_PARSER_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[j]);
        
        sprintf(buffer, "VirtParam%d", j + 1);
        AST *newParam = new AST(std::string(buffer), NT_PARAMETER);
        ParserParameterList->addChild(newParam);
        newParam->addChild(NULL); //TODO: Merge annotations?
        
        AST *direction = params[0]->getChildNT(NT_DIRECTION);
        bool allEqual = true;
        for(int pn = 1; pn < (int) parsers.size(); pn++)
            if(!AST::Equals(direction, params[pn]->getChildNT(NT_DIRECTION)))
                allEqual = false;
        if(allEqual)
            newParam->addChild(NULL_CLONE(direction));
        else
            newParam->addChild(new AST("inout", NT_DIRECTION));
        
        allEqual = true;
        for(int pn = 1; pn < (int) parsers.size(); pn++)
            if(!AST::Equals(params[0]->children[2], params[pn]->children[2]))
                allEqual = false;

        if(!allEqual){
            std::vector<std::string> paramTypes;
            for(int pn = 0; pn < (int) parsers.size(); pn++)
                paramTypes.push_back(params[pn]->children[2]->value);

            bool hasNull = false;
            std::vector<AST*> hPs(parsers.size(), NULL);
            for(int pn = 0; pn < (int) parsers.size(); pn++){
                for(AST *c : headersDec)
                    if(c != NULL && c->value == paramTypes[pn])
                        hPs[pn] = c;
                if(hPs[pn] == NULL)
                    hasNull = true;
            }
            if(hasNull){
                for(AST *node : newNodes)
                    delete node;
                delete newParser;
                for(AST *TempLEL : TempLocalElementLists)
                    NullDelete(TempLEL);
                for(AST *TempSL : TempStateLists)
                    NullDelete(TempSL);
                Log::Pop();
                return std::vector<AST*>();
            } 

            AST *newHeaderType = new AST(AST::NextVirtTypeID(), hPs[0]->nType);
            newHeaderType->addChild(NULL); //Annotations
            newHeaderType->addChild(NULL); //TypeParameters
            
            newHeaderType->addChild(AST::MergeStructFieldListByNames(headersDec, paramTypes));

            for(int x = 0; x < (int) TempLocalElementLists.size(); x++){
                if(TempLocalElementLists[x] != NULL)
                    TempLocalElementLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, paramTypes[x]);
                if(TempStateLists[x] != NULL)
                    TempStateLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, paramTypes[x]);
            }

            for(int pn = 0; pn < (int) parsers.size(); pn++){
                if(hPs[pn]->children[2] != NULL){
                    for(AST *c : hPs[pn]->children[2]->children){
                        int i = 0;
                        while(!AST::Equals(newHeaderType->children[2]->children[i], c))
                            i++;
                        if(newHeaderType->children[2]->children[i]->value != c->value){
                            if(TempLocalElementLists[pn] != NULL)
                                TempLocalElementLists[pn]->SubstituteIdentifierWithType(c->value, newHeaderType->children[2]->children[i]->value, "");
                            if(TempStateLists[pn] != NULL)
                                TempStateLists[pn]->SubstituteIdentifierWithType(c->value, newHeaderType->children[2]->children[i]->value, "");
                        }
                    }
                }
            }

            newNodes.push_back(newHeaderType);

            newParam->addChild(new AST(newHeaderType->value, NT_TYPE_NAME));
        
            for(int x = 0; x < (int) TempLocalElementLists.size(); x++){
                if(TempLocalElementLists[x] != NULL)
                    TempLocalElementLists[x]->RenameProgramTypes(hPs[x]->value, newHeaderType->value);
                if(TempStateLists[x] != NULL)
                    TempStateLists[x]->RenameProgramTypes(hPs[x]->value, newHeaderType->value);
            }
        }
        else{
            newParam->addChild(params[0]->children[2]->Clone());
            for(int x = 0; x < (int) TempLocalElementLists.size(); x++){
                if(TempLocalElementLists[x] != NULL)
                    TempLocalElementLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, params[x]->children[2]->value);
                if(TempStateLists[x] != NULL)
                    TempStateLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, params[x]->children[2]->value);
            }
        }
    }

    //Merge local elements
    AST *LocalElementList = NULL;;
    for(int pn = 0; pn < (int) parsers.size(); pn++){
        if(TempLocalElementLists[pn] != NULL){
            if(LocalElementList == NULL){
                LocalElementList = TempLocalElementLists[pn]->Clone();
                for(AST *LocalElement : LocalElementList->children){
                    sprintf(buffer, "Ps%d_%s", pn + 1, LocalElement->str());
                    std::string newName = std::string(buffer);
                    LocalElementList->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);
                    TempStateLists[pn]->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);
                    LocalElement->value = newName;
                }
            }
            else{
                std::unordered_set<AST*> Matched;
                for(AST *localElement : TempLocalElementLists[pn]->children){
                    bool matchedD;
                    std::string newName;
                    switch(localElement->nType){
                        case NT_VARIABLE_DECLARATION:
                        case NT_CONSTANT_DECLARATION:
                        case NT_INSTANTIATION:
                            matchedD = false;
                            for(AST *LocalElement : LocalElementList->children){
                                if(!matchedD && Matched.find(LocalElement) == Matched.end() && AST::Equals(localElement, LocalElement)){
                                    Matched.emplace(LocalElement);
                                    newName = LocalElement->value;
                                    sprintf(buffer, "s%d", pn+1);
                                    newName = newName.substr(0, newName.find("_")) + std::string(buffer) + newName.substr(newName.find("_"));
                                    LocalElementList->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);
                                    for(int pn2 = 0; pn2 < pn; pn2++)
                                        TempStateLists[pn2]->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);
                                    TempLocalElementLists[pn]->SubstituteIdentifierWithType(localElement->value, newName, LocalElement->programType);
                                    TempStateLists[pn]->SubstituteIdentifierWithType(localElement->value, newName, LocalElement->programType);
                                    LocalElement->value = newName;
                                    matchedD = true;
                                }
                            }
                            if(!matchedD){
                                AST *newElement = localElement->Clone();
                                sprintf(buffer, "Ps%d_%s", pn + 1, localElement->str());
                                newName = std::string(buffer);
                                TempLocalElementLists[pn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                if(TempStateLists[pn] != NULL)
                                    TempStateLists[pn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                newElement->value = newName;
                                LocalElementList->addChild(newElement);
                            }
                            break;
                        default:
                            LocalElementList->addChild(localElement->Clone());
                            break;
                    }
                }
            }
        }
    }
    newParser->addChild(LocalElementList);

    //Merge States
    std::vector<std::vector<int>> statePrograms;
    AST *StateList = new AST(NULL, NT_PARSER_STATE_LIST);
    newParser->addChild(StateList);
    for(int pn = 0; pn < (int) parsers.size(); pn++){
        for(AST *state : TempStateLists[pn]->children){
            AST *cloneS = state->Clone();
            StateList->addChild(cloneS);
            statePrograms.push_back({pn});
            sprintf(buffer, "P%d_%s", pn + 1, cloneS->value.c_str());
            cloneS->value = std::string(buffer);
            if(cloneS->children[1]->nType == NT_NAME_STATE_EXPRESSION){
                if(cloneS->children[1]->value != "accept" && cloneS->children[1]->value != "reject"){
                    sprintf(buffer, "P%d_%s", pn + 1, cloneS->children[1]->str());
                    cloneS->children[1]->value = std::string(buffer);
                }
            }
            else{
                for(AST *selectCase : cloneS->children[1]->children[1]->children){
                    if(selectCase->value != "accept" && selectCase->value != "reject"){
                        sprintf(buffer, "P%d_%s", pn + 1, selectCase->value.c_str());
                        selectCase->value = std::string(buffer);
                    }
                }
            }
        }
    }

    bool tryAgain = true;
    while(tryAgain){
        MergeParserEqualStates(StateList, statePrograms);
        tryAgain = MergeParserSemiEqualStates(StateList, statePrograms, continuousPorts, IG_PORT_EXP);
    }

    //Check if we made all select statements the same somehow...
    for(AST *state : StateList->children){
        if(state->children[1]->nType == NT_SELECT_EXPRESSION){
            std::string nextStateName = state->children[1]->children[1]->children[0]->value;
            bool allSame = true;
            for(int j = 1; j < (int) state->children[1]->children[1]->children.size(); j++){
                if(nextStateName != state->children[1]->children[1]->children[j]->value)
                    allSame = false;
            }
            if(allSame){
                delete state->children[1];
                state->children[1] = new AST(nextStateName, NT_NAME_STATE_EXPRESSION);
            }
        }
    }

    //Unify port Ranges
    if(IG_PORT_EXP != NULL){
        AST* PORT_EXPRESSION_AST = IG_PORT_EXP->Clone();
        for(AST *state : StateList->children){
            if(state->children[1]->nType == NT_SELECT_EXPRESSION){
                AST* SCL = state->children[1]->getChildNT(NT_SELECT_CASE_LIST);
                AST* EL = state->children[1]->getChildNT(NT_EXPRESSION_LIST);
                if(AST::Equals(EL->children[0], PORT_EXPRESSION_AST) && EL->children.size() > 1){
                    tryAgain = true;
                    while(tryAgain){
                        std::set<AST*> SCsRemove;
                        tryAgain = false;
                        for(int j = 0; j < (int) SCL->children.size(); j++){
                            AST* SCJRange = SCL->children[j]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[0];
                            if(SCJRange->nType == NT_RANGE){
                                if(IPLBase::intFromStr(SCJRange->children[0]->str()) == FIRST_SWITCH_PORT && IPLBase::intFromStr(SCJRange->children[1]->str()) == LAST_SWITCH_PORT){
                                    delete SCJRange->children[0];
                                    delete SCJRange->children[1];
                                    SCJRange->children.clear();
                                    SCJRange->nType = NT_SIMPLE_KEYSET_DONTCARE;
                                    SCJRange->value = "_";
                                }
                            }

                            for(int k = j + 1; k < (int) SCL->children.size(); k++){
                                bool restEqual = SCL->children[j]->value == SCL->children[k]->value;
                                for(int e = 1; e < (int) EL->children.size(); e++){
                                    if((k != j + 1 && SCL->children[j]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[e]->nType == NT_SIMPLE_KEYSET_DONTCARE) || !AST::Equals(SCL->children[j]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[e], SCL->children[k]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[e]))
                                        restEqual = false;
                                }
                                if(restEqual){
                                    if(AST::MergeRange(SCL->children[j]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[0], SCL->children[k]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[0])){
                                        tryAgain = true;
                                        SCsRemove.emplace(SCL->children[k]);
                                    }
                                }
                            }
                        }
                        int scp = 0;
                        while(scp < (int) SCL->children.size()){
                            if(SCsRemove.find(SCL->children[scp]) != SCsRemove.end()){
                                delete SCL->children[scp];
                                SCL->children.erase(SCL->children.begin() + scp);
                                scp--;
                            }
                            scp++;
                        }
                    }
                    bool allDontCare = true;
                    for(int j = 0; j < (int) SCL->children.size(); j++){
                        if(SCL->children[j]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[0]->nType != NT_SIMPLE_KEYSET_DONTCARE)
                            allDontCare = false;
                    }
                    if(allDontCare){
                        if(EL->children.size() == 2){
                            delete EL->children[0];
                            EL->children.erase(EL->children.begin());
                            for(int j = 0; j < (int) SCL->children.size(); j++){
                                AST *newValue = SCL->children[j]->getChildNT(NT_TUPLE_KEYSET_EXPRESSION)->getChildNT(NT_SIMPLE_EXPRESSION_LIST)->children[1]->Clone();
                                delete SCL->children[j]->children[0];
                                SCL->children[j]->children[0] = newValue;
                                if(newValue->nType == NT_SIMPLE_KEYSET_DONTCARE){
                                    newValue->nType = NT_SIMPLE_KEYSET_DEFAULT;
                                    newValue->value = "default";
                                }
                            }
                        }
                        else{
                            //TODO
                        }
                    }
                }
            }
        }
        delete PORT_EXPRESSION_AST;
    }

    //Determine start state
    std::unordered_map<AST*, int> stateOrder;
    std::vector<AST*> stateExpandList;

    int hasEqualStartState = 0;
    for(AST *state : StateList->children){
        if(state->value.substr(state->value.find("_") + 1) == "start"){
            hasEqualStartState++;
            stateOrder.emplace(state, 1);
            stateExpandList.push_back(state);
        }
    }
    Assert(hasEqualStartState > 0);
    if(hasEqualStartState > 1){
        AST *startState = new AST("start", NT_PARSER_STATE);
        stateOrder.emplace(startState, 0);
        stateExpandList.push_back(startState);
        startState->addChild(NULL);
        AST *newTransition = new AST("select", NT_SELECT_EXPRESSION);
        startState->addChild(newTransition);
        AST *newExpList = new AST(NULL, NT_EXPRESSION_LIST);
        AST *newExpression = new AST(".", NT_EXPRESSION);
        newExpression->addChild(new AST("a", NT_EXPRESSION));
        newExpression->addChild(new AST("b", NT_NONE));
        if(IG_PORT_EXP != NULL){
            delete newExpression;
            newExpression = IG_PORT_EXP->Clone();
        }
        newExpList->addChild(newExpression);
        newTransition->addChild(newExpList);
        AST *newSelectCaseList = new AST(NULL, NT_SELECT_CASE_LIST);
        newTransition->addChild(newSelectCaseList);

        for(int i = 0; i < (int) StateList->children.size(); i++){
            AST *state = StateList->children[i];
            if(state->value.find("_start") != std::string::npos){
                for(int program : statePrograms[i]){
                    for(const std::pair<int, int>& portRange : continuousPorts[program]){
                        AST *newSCCase = new AST(state->value, NT_SELECT_CASE);
                        if(portRange.first == portRange.second){
                            sprintf(buffer, "%d", portRange.first);
                            newSCCase->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                        }
                        else{
                            AST *rangeAST = new AST("..", NT_RANGE);
                            sprintf(buffer, "%d", portRange.first);
                            rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                            sprintf(buffer, "%d", portRange.second);
                            rangeAST->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                            newSCCase->addChild(rangeAST);
                        }
                        newSelectCaseList->addChild(newSCCase);
                    }
                }
            }
        }
        StateList->addChildStart(startState);
    }
    else{
        stateExpandList[0]->value = "start";
    }

    std::unordered_map<std::string, std::set<std::string>> stateTransitions;

    //Order states
    while(stateExpandList.size() > 0){
        AST *s = stateExpandList[stateExpandList.size() - 1];
        stateExpandList.pop_back();

        if(stateTransitions.find(s->value) == stateTransitions.end())
            stateTransitions.emplace(s->value, std::set<std::string>());

        if(s->children[1]->nType == NT_NAME_STATE_EXPRESSION){
            stateTransitions.find(s->value)->second.insert(s->children[1]->value);
            for(AST* n : StateList->children){
                if(n->value == s->children[1]->value){
                    auto it = stateOrder.find(n);
                    if(it == stateOrder.end()){
                        stateOrder.emplace(n, stateOrder.find(s)->second + 1);
                    }
                    else
                        it->second = (stateOrder.find(s)->second + 1 > it->second) ? stateOrder.find(s)->second + 1 : it->second;
                    stateExpandList.push_back(n);
                }
            }
        }
        else{
            for(AST *sc : s->children[1]->children[1]->children){
                stateTransitions.find(s->value)->second.insert(sc->value);
                for(AST* n : StateList->children){
                    if(n->value == sc->value){
                        auto it = stateOrder.find(n);
                        if(it == stateOrder.end()){
                            stateOrder.emplace(n, stateOrder.find(s)->second + 1);
                        }
                        else
                            it->second = (stateOrder.find(s)->second + 1 > it->second) ? stateOrder.find(s)->second + 1 : it->second;
                        stateExpandList.push_back(n);
                    }
                }
            }
        }
    }
    for(int i = 1; i < (int) StateList->children.size(); i++){
        int j = i - 1;
        while(j >= 0 && stateOrder.find(StateList->children[j])->second > stateOrder.find(StateList->children[j+1])->second){
            AST *auxS = StateList->children[j];
            StateList->children[j] = StateList->children[j+1];
            StateList->children[j+1] = auxS;
            j--;
        }
    }

    newNodes.push_back(newParser);

    for(AST *TempLEL : TempLocalElementLists)
        NullDelete(TempLEL);
    for(AST *TempSL : TempStateLists)
        NullDelete(TempSL);
    Log::Pop();

    return newNodes;
}

std::vector<AST*> AST::Linearize(AST *sList){
    std::vector<AST*> result;
    for(AST* a : sList->children){
        if(a->nType != NT_CONDITIONAL_STATEMENT)
            result.push_back(a->Clone());
        else{
            AST *ifb = new AST("IF", NT_NONE);
            ifb->addChild(a->children[0]->Clone());
            result.push_back(ifb);
            if(a->children[1] != NULL){
                std::vector<AST*> tempR = AST::Linearize(a->children[1]);
                for(AST* r : tempR)
                    result.push_back(r);
            }
            ifb = new AST("ELSE", NT_NONE);
            result.push_back(ifb);
            if(a->children[2] != NULL){
                std::vector<AST*> tempR = AST::Linearize(a->children[2]);
                    for(AST* r : tempR)
                        result.push_back(r);
            }
            ifb = new AST("DONE", NT_NONE);
            result.push_back(ifb);
        }
    }
    return result;
}

void AST::RemoveEmptyIfElses(){
    if(this->nType == NT_STAT_OR_DEC_LIST){
        int i = 0;
        while(i < (int) this->children.size()){
            if(this->children[i] != NULL && this->children[i]->nType == NT_CONDITIONAL_STATEMENT){
                if(this->children[i]->children[2] != NULL && this->children[i]->children[2]->children.size() == 0){
                    delete this->children[i]->children[2];
                    this->children[i]->children[2] = NULL;
                }
                if(this->children[i]->children[2] == NULL && (this->children[i]->children[1] == NULL || this->children[i]->children[1]->children.size() == 0)){
                    delete this->children[i];
                    this->children.erase(this->children.begin() + i);
                    i--;
                }
            }
            i++;
        }
    }

    for(AST *c : this->children)
        if(c != NULL)
            c->RemoveEmptyIfElses();
}

void AST::RemoveEmptySwitches(){
    if(this->nType == NT_STAT_OR_DEC_LIST){
        int i = 0;
        while(i < (int) this->children.size()){
            if(this->children[i] != NULL && this->children[i]->nType == NT_SWITCH_STATEMENT){
                bool removeSS = false;
                if(this->children[i]->getChildNT(NT_SWITCH_CASE_LIST) == NULL)
                    removeSS = true;
                else{
                    AST *SCL = this->children[i]->getChildNT(NT_SWITCH_CASE_LIST);
                    int j = 0;
                    while(j < (int) SCL->children.size()){
                        if(SCL->children[j]->children[1] == NULL || SCL->children[j]->children[1]->children.size() == 0){
                            delete SCL->children[j];
                            SCL->children.erase(SCL->children.begin() + j);
                            j--;
                        }
                        j++;
                    }
                    if(j == 0)
                        removeSS = true;
                    else if(j == 1){ //Substitute switch -> if
                        AST *newCS = new AST("IF", NT_CONDITIONAL_STATEMENT);
                        AST *condEXP = new AST("==", NT_EXPRESSION);
                        condEXP->addChild(this->children[i]->children[0]->Clone());
                        condEXP->addChild(SCL->children[0]->children[0]->Clone());
                        newCS->addChild(condEXP);
                        newCS->addChild(SCL->children[0]->children[1]->Clone());
                        newCS->addChild(NULL); //else
                        delete this->children[i];
                        this->children[i] = newCS;
                    }
                }

                if(removeSS){
                    delete this->children[i];
                    this->children.erase(this->children.begin() + i);
                    i--;
                }
            }
            i++;
        }
    }

    for(AST *c : this->children)
        if(c != NULL)
            c->RemoveEmptySwitches();
}

std::vector<AST*> AST::SplitApply(AST* apply, const std::vector<std::string>& sharedTNs, int *maxIfDepth){
    char buffer[BUFFER_SIZE];
    int ifcount = 0;
    
    std::vector<AST*> result;
    AST* tempSTL = new AST(NULL, NT_STAT_OR_DEC_LIST);
    AST* rootSTL = tempSTL;
    result.push_back(tempSTL);
    
    int tCounter = 0;

    //TODO: Switch statements
    std::vector<bool> ifStack;
    std::vector<AST*> ifChain;
    std::vector<int> ifNumStack;
    std::vector<AST*> linear = AST::Linearize(apply);
    
    for(int x = 0; x < (int) linear.size(); x++){
        if(linear[x]->value == "IF"){
            ifcount++;
            if(ifcount > *maxIfDepth)
                *maxIfDepth = ifcount;
            ifStack.push_back(true);
            ifNumStack.push_back(ifcount);
            AST* ifS = new AST("IF", NT_CONDITIONAL_STATEMENT);
            ifS->addChild(linear[x]->children[0]->Clone());
            ifS->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
            ifS->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
            ifChain.push_back(ifS);
            tempSTL->addChild(ifS);
            tempSTL = ifS->children[1];
            tempSTL->addChild(new AST("=", NT_ASSIGN_STATEMENT));
            sprintf(buffer, "VirtIF_%d", ifcount);
            tempSTL->children[tempSTL->children.size() - 1]->addChild(new AST(std::string(buffer), NT_NONE));
            tempSTL->children[tempSTL->children.size() - 1]->addChild(new AST("true", NT_EXPRESSION));
        }
        else if(linear[x]->value == "ELSE"){
            ifStack.pop_back();
            ifStack.push_back(false);
            tempSTL = ifChain[ifChain.size() - 1]->children[2];
        }
        else if(linear[x]->value == "DONE"){
            ifStack.pop_back();
            ifChain.pop_back();
            ifNumStack.pop_back();
            if(ifChain.size() > 0)
                tempSTL = ifChain[ifChain.size() - 1]->children[1];
            else
                tempSTL = rootSTL;
        }
        else{
            if(linear[x]->nType == NT_METHOD_CALL_STATEMENT && linear[x]->children[0]->children.size() > 0){
                int newTCounter = tCounter;
                while(newTCounter < (int) sharedTNs.size() && linear[x]->children[0]->children[0]->value != sharedTNs[newTCounter])
                    newTCounter++;
                if(newTCounter < (int) sharedTNs.size()){
                    while(tCounter != newTCounter){
                        tCounter++;
                        result.push_back(NULL);
                    }

                    sprintf(buffer, "VirtApplyT%d", tCounter + 1);
                    tempSTL->addChild(new AST("=", NT_ASSIGN_STATEMENT));
                    tempSTL->children[tempSTL->children.size() - 1]->addChild(new AST(std::string(buffer), NT_NONE));
                    tempSTL->children[tempSTL->children.size() - 1]->addChild(new AST("true", NT_EXPRESSION));
                    tempSTL = new AST(NULL, NT_STAT_OR_DEC_LIST);
                    rootSTL = tempSTL;
                    result.push_back(tempSTL);
                    tCounter++;
                    for(int i = 0; i < (int) ifStack.size(); i++){
                        AST *newIF = new AST("IF", NT_CONDITIONAL_STATEMENT);
                        ifChain[i] = newIF;
                        if(ifStack[i])
                            sprintf(buffer, "VirtIF_%d", ifNumStack[i]);
                        else
                            sprintf(buffer, "!(VirtIF_%d)", ifNumStack[i]);
                        newIF->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                        newIF->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
                        newIF->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
                        tempSTL->addChild(newIF);
                        tempSTL = newIF->children[1];
                    }
                }
                else{
                    tempSTL->addChild(linear[x]->Clone());
                }
            }
            else{
                tempSTL->addChild(linear[x]->Clone());
            }
        }
    }

    while(result.size() < sharedTNs.size() + 1)
        result.push_back(NULL);

    for(AST* ast : linear)
        delete ast;
    
    return result;
}

int findPrefix(const std::string& str){
    int ulp = str.find("_");

    if(ulp == (int) std::string::npos || str.size() < 2 || str[0] != 'P')
        return -1;

    if((str[1] == 'S' && str[2] == '_') || IPLBase::intFromStr(str.substr(1, ulp - 1).c_str()) != -1)
        return ulp;

    if(str[1] != 's')
        return -1;
    
    std::string tS;
    int i = 2;
    while(i < ulp){
        if(str[i] == 's'){
            if(IPLBase::intFromStr(tS.c_str()) == -1)
                return -1;
            tS.clear();
        }
        else{
            tS.push_back(str[i]);
        }
        i++;
    }

    return ulp;
}

std::vector<AST*> AST::MergeControls(const std::vector<AST*>& programs, const std::vector<AST*>& controls, const std::vector<AST*>& headersDec, const std::string& newControlName, const std::vector<std::set<int>>& virtualSwitchPorts) {
    Assert(programs.size() >= 2);
    Assert(programs.size() == controls.size() && programs.size() == virtualSwitchPorts.size());
    
    std::string LogM;
    LogM = "Merging Control Blocks [ ";
    for(int i = 0; i < (int) controls.size(); i++)
        LogM = LogM + "\"" + controls[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value + "\" ";
    LogM = LogM + "] into \"" + newControlName + "\"";
    Log::MSG(LogM);
    Log::Push();
    
    char buffer[256];
    std::vector<AST*> newNodes;

    //Determine continuous port allocations
    std::vector<std::vector<std::pair<int, int>>> continuousPorts;
    for(const std::set<int>& ports : virtualSwitchPorts)
        continuousPorts.push_back(AST::GetContinuous(ports));
    
    std::vector<AST*> TempLocalElementLists;
    std::vector<AST*> TempApplyLists;
    for(AST* control : controls){
        TempLocalElementLists.push_back(NULL_CLONE(control->getChildNT(NT_CONTROL_LOCAL_DEC_LIST)));
        TempApplyLists.push_back(NULL_CLONE(control->getChildNT(NT_STAT_OR_DEC_LIST)));
    }

    AST *newControl = new AST(NULL, NT_CONTROL_DECLARATION);
    AST *ControlTypeDec = new AST(NULL, NT_CONTROL_TYPE_DECLARATION);
    newControl->addChild(ControlTypeDec);
    newControl->addChild(NULL); //optConstructorParameters
    ControlTypeDec->value = newControlName;
    
    for(int j = 0; j < 2; j++)
        ControlTypeDec->addChild(NULL_CLONE(controls[0]->children[0]->children[j]));
    AST *ControlParameterList = new AST(NULL, NT_PARAMETER_LIST);
    ControlTypeDec->addChild(ControlParameterList);

    int minParamN = (int) controls[0]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children.size();
    for(int j = 1; j < (int) controls.size(); j++)
        minParamN = MIN(minParamN, (int) controls[0]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children.size());

    //TODO: Merge other params (different number of params)
    for(int j = 0; j < minParamN; j++){
        std::vector<AST*> params;
        for(AST *control : controls)
            params.push_back(control->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[j]);

        sprintf(buffer, "VirtParam%d", j + 1);
        AST *newParam = new AST(std::string(buffer), NT_PARAMETER);
        ControlParameterList->addChild(newParam);
        newParam->addChild(NULL); //TODO: Merge annotations?

        AST *direction = params[0]->getChildNT(NT_DIRECTION);
        bool allEqual = true;
        for(int pn = 1; pn < (int) controls.size(); pn++)
            if(!AST::Equals(direction, params[pn]->getChildNT(NT_DIRECTION)))
                allEqual = false;
        if(allEqual)
            newParam->addChild(NULL_CLONE(direction));
        else
            newParam->addChild(new AST("inout", NT_DIRECTION));
        
        allEqual = true;
        for(int pn = 1; pn < (int) controls.size(); pn++)
            if(!AST::Equals(params[0]->children[2], params[pn]->children[2]))
                allEqual = false;

        if(!allEqual){
            std::vector<std::string> paramTypes;
            for(int pn = 0; pn < (int) controls.size(); pn++)
                paramTypes.push_back(params[pn]->children[2]->value);

            bool hasNull = false;
            std::vector<AST*> hPs(controls.size(), NULL);
            for(int pn = 0; pn < (int) controls.size(); pn++){
                for(AST *c : headersDec)
                    if(c != NULL && c->value == paramTypes[pn])
                        hPs[pn] = c;
                if(hPs[pn] == NULL)
                    hasNull = true;
            }
            if(hasNull){
                for(AST *node : newNodes)
                    delete node;
                delete newControl;
                for(AST *TempLEL : TempLocalElementLists)
                    NullDelete(TempLEL);
                for(AST *TempAL : TempApplyLists)
                    NullDelete(TempAL);
                Log::Pop();
                return std::vector<AST*>();
            }
            

            AST *newHeaderType = new AST(AST::NextVirtTypeID(), hPs[0]->nType);
            newHeaderType->addChild(NULL); //Annotations
            newHeaderType->addChild(NULL); //TypeParameters
            
            newHeaderType->addChild(AST::MergeStructFieldListByNames(headersDec, paramTypes));

            for(int x = 0; x < (int) TempLocalElementLists.size(); x++){
                if(TempLocalElementLists[x] != NULL)
                    TempLocalElementLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, paramTypes[x]);
                if(TempApplyLists[x] != NULL)
                    TempApplyLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, paramTypes[x]);
            }

            for(int pn = 0; pn < (int) controls.size(); pn++){
                if(hPs[pn]->children[2] != NULL){
                    for(AST *c : hPs[pn]->children[2]->children){
                        int i = 0;
                        while(!AST::Equals(newHeaderType->children[2]->children[i], c))
                            i++;
                        if(newHeaderType->children[2]->children[i]->value != c->value){
                            if(TempLocalElementLists[pn] != NULL)
                                TempLocalElementLists[pn]->SubstituteIdentifierWithType(c->value, newHeaderType->children[2]->children[i]->value, "");
                            if(TempApplyLists[pn] != NULL)
                                TempApplyLists[pn]->SubstituteIdentifierWithType(c->value, newHeaderType->children[2]->children[i]->value, "");
                        }
                    }
                }
            }

            newNodes.push_back(newHeaderType);

            newParam->addChild(new AST(newHeaderType->value, NT_TYPE_NAME));
        
            for(int x = 0; x < (int) TempLocalElementLists.size(); x++){
                if(TempLocalElementLists[x] != NULL)
                    TempLocalElementLists[x]->RenameProgramTypes(hPs[x]->value, newHeaderType->value);
                if(TempApplyLists[x] != NULL)
                    TempApplyLists[x]->RenameProgramTypes(hPs[x]->value, newHeaderType->value);
            }
        }
        else{
            newParam->addChild(params[0]->children[2]->Clone());
            for(int x = 0; x < (int) TempLocalElementLists.size(); x++){
                if(TempLocalElementLists[x] != NULL)
                    TempLocalElementLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, params[x]->children[2]->value);
                if(TempApplyLists[x] != NULL)
                    TempApplyLists[x]->SubstituteIdentifierWithType(params[x]->value, newParam->value, params[x]->children[2]->value);
            }
        }
    }

    //Merge local elements
    AST *LocalElementList = NULL;;
    for(int cn = 0; cn < (int) controls.size(); cn++){
        if(TempLocalElementLists[cn] != NULL){
            if(LocalElementList == NULL){
                LocalElementList = TempLocalElementLists[cn]->Clone();
                for(AST *LocalElement : LocalElementList->children){
                    sprintf(buffer, "Ps%d_%s", cn + 1, LocalElement->str());
                    std::string newName = std::string(buffer);

                    if(LocalElement->nType == NT_TABLE_DECLARATION){
                        CFG::AddProgramTable(cn, LocalElement, newName, controls[cn]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, newControlName);
                    }
                    else if(LocalElement->nType == NT_ACTION_DECLARATION){
                        CFG::AddProgramAction(cn, LocalElement, newName, controls[cn]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, newControlName);
                    }

                    LocalElementList->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);
                    if(TempApplyLists[cn] != NULL)
                        TempApplyLists[cn]->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);
                    
                    LogM = "Added local element \"" + LocalElement->value + "\" from program \"" + programs[cn]->value + "\" as \"" + newName + "\""; 
                    Log::MSG(LogM);

                    LocalElement->value = newName;
                }
            }
            else{
                std::unordered_set<AST*> Matched;
                for(AST *localElement : TempLocalElementLists[cn]->children){
                    bool matchedD;
                    std::string newName;
                    switch(localElement->nType){
                        case NT_VARIABLE_DECLARATION:
                        case NT_INSTANTIATION:
                        case NT_CONSTANT_DECLARATION:
                        case NT_ACTION_DECLARATION:
                            matchedD = false;
                            for(AST *LocalElement : LocalElementList->children){
                                if(!matchedD && Matched.find(LocalElement) == Matched.end() && AST::Equals(localElement, LocalElement)){
                                    LogM = "Found equal element of \"" + localElement->value + "\" from \"" + programs[cn]->value + "\" (" + LocalElement->value + "). Merged into \"";

                                    Matched.emplace(LocalElement);
                                    newName = LocalElement->value;
                                    sprintf(buffer, "s%d", cn+1);
                                    newName = newName.substr(0, newName.find("_")) + std::string(buffer) + newName.substr(newName.find("_"));
                                    LocalElementList->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);
                                    
                                    for(int cn2 = 0; cn2 < cn; cn2++)
                                        if(TempLocalElementLists[cn2] != NULL)
                                            TempLocalElementLists[cn2]->SubstituteIdentifierWithType(LocalElement->value, newName, LocalElement->programType);

                                    TempLocalElementLists[cn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                    if(TempApplyLists[cn] != NULL)
                                        TempApplyLists[cn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                    
                                    if(localElement->nType == NT_ACTION_DECLARATION){
                                        CFG::ChangeVActionName(newControlName, LocalElement->value, newName);
                                        CFG::AddProgramAction(cn, localElement, newName, controls[cn]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, newControlName);
                                    }
                                    
                                    LocalElement->value = newName;
                                    matchedD = true;
                                
                                    LogM = LogM + newName + "\"";
                                    Log::MSG(LogM);
                                }
                            }
                            if(!matchedD){
                                AST *newElement = localElement->Clone();
                                sprintf(buffer, "Ps%d_%s", cn + 1, localElement->str());
                                newName = std::string(buffer);
                                TempLocalElementLists[cn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                if(TempApplyLists[cn] != NULL)
                                    TempApplyLists[cn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                newElement->value = newName;
                                LocalElementList->addChild(newElement);

                                if(localElement->nType == NT_ACTION_DECLARATION)
                                    CFG::AddProgramAction(cn, localElement, newName, controls[cn]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, newControlName);

                                LogM = "Added local element \"" + localElement->value + "\" from program \"" + programs[cn]->value + "\" as \"" + newName + "\""; 
                                Log::MSG(LogM);
                            }
                            break;
                        case NT_TABLE_DECLARATION:
                            {
                            matchedD = false;
                            const int LELSize = (int) LocalElementList->children.size();
                            for(int lelp = 0; lelp < LELSize; lelp++){
                                AST *LocalElement = LocalElementList->children[lelp];
                                if(!matchedD && Matched.find(LocalElement) == Matched.end()){
                                    AST* mergedTable = AST::TablesMerge(LocalElement, localElement);
                                    if(mergedTable != NULL){
                                        matchedD = true;
                                        Matched.emplace(LocalElement);
                                        newName = LocalElement->value;

                                        LogM = "Found compatible table of \"" + localElement->value + "\" from \"" + programs[cn]->value + "\" (" + LocalElement->value + "). Merged into \"";

                                        sprintf(buffer, "s%d", cn+1);
                                        newName = newName.substr(0, newName.find("_")) + std::string(buffer) + newName.substr(newName.find("_"));
                                        mergedTable->value = newName;
                                        LocalElementList->addChild(mergedTable);

                                        CFG::ChangeVTableName(newControlName, LocalElement->value, mergedTable->value);
                                        CFG::AddProgramTable(cn, localElement, mergedTable->value, controls[cn]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, newControlName);

                                        for(int cn2 = 0; cn2 < cn; cn2++){
                                            if(TempLocalElementLists[cn2] != NULL)
                                                TempLocalElementLists[cn2]->SubstituteIdentifierWithType(LocalElement->value, mergedTable->value, LocalElement->programType);
                                            if(TempApplyLists[cn2] != NULL)
                                                TempApplyLists[cn2]->SubstituteIdentifierWithType(LocalElement->value, mergedTable->value, LocalElement->programType);
                                        }
                                        if(TempLocalElementLists[cn] != NULL)
                                            TempLocalElementLists[cn]->SubstituteIdentifierWithType(localElement->value, mergedTable->value, localElement->programType);
                                        if(TempApplyLists[cn] != NULL)
                                            TempApplyLists[cn]->SubstituteIdentifierWithType(localElement->value, mergedTable->value, localElement->programType);
                                    
                                        LogM = LogM + newName + "\"";
                                        Log::MSG(LogM);
                                    }
                                }
                            }
                            if(!matchedD){
                                AST *newElement = localElement->Clone();
                                sprintf(buffer, "Ps%d_%s", cn + 1, localElement->str());
                                newName = std::string(buffer);
                                TempLocalElementLists[cn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                if(TempApplyLists[cn] != NULL)
                                    TempApplyLists[cn]->SubstituteIdentifierWithType(localElement->value, newName, localElement->programType);
                                newElement->value = newName;
                                LocalElementList->addChild(newElement);

                                LogM = "Added local element \"" + localElement->value + "\" from program \"" + programs[cn]->value + "\" as \"" + newName + "\""; 
                                Log::MSG(LogM);

                                CFG::AddProgramTable(cn, localElement, newName, controls[cn]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, newControlName);
                            }
                            }
                            break;
                        default:
                            Assert(false);
                            break;
                    }
                }
                for(AST *table : Matched){
                    if(table->nType == NT_TABLE_DECLARATION){
                        int x = 0;
                        while(LocalElementList->children[x] != table)
                            x++;
                        LocalElementList->children.erase(LocalElementList->children.begin() + x);
                        delete table;
                    }
                }
            }
        }
    }
    newControl->addChild(LocalElementList);

    //Apply
    if(newControl->getChildNT(NT_CONTROL_LOCAL_DEC_LIST) != NULL){
        AST* newApply = new AST(NULL, NT_STAT_OR_DEC_LIST);
        newControl->addChild(newApply);

        AST* templateSwitchS = new AST(NULL, NT_SWITCH_STATEMENT);
        templateSwitchS->addChild(new AST(TEMP_PROGRAM_CODE, NT_EXPRESSION));
        templateSwitchS->addChild(new AST(NULL, NT_SWITCH_CASE_LIST));
        for(int p = 0; p < (int) programs.size(); p++){
            AST* newSC = new AST(NULL, NT_SWITCH_CASE);
            templateSwitchS->getChildNT(NT_SWITCH_CASE_LIST)->addChild(newSC);
            sprintf(buffer, "%d", p + 1);
            newSC->addChild(new AST(std::string(buffer), NT_EXPRESSION));
            newSC->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
        }
        
        std::vector<std::string> sharedTableNames;
        for(AST* le : newControl->getChildNT(NT_CONTROL_LOCAL_DEC_LIST)->children){
            if(le->nType == NT_TABLE_DECLARATION && findPrefix(le->value) > 4){
                sharedTableNames.push_back(le->value);
            }
        }
        std::vector<std::vector<AST*>> splitedApply(programs.size());
        int maxIfDepth = 0;
        for(int p = 0; p < (int) programs.size(); p++){
            if(TempApplyLists[p] != NULL){
                splitedApply[p] = SplitApply(TempApplyLists[p], sharedTableNames, &maxIfDepth);
                Assert(splitedApply[p].size() == sharedTableNames.size() + 1);
            }
            else{
                splitedApply[p].resize(sharedTableNames.size() + 1, NULL);
            }
        }

        for(int i = 0; i < maxIfDepth; i++){
            sprintf(buffer, "VirtIF_%d", i + 1);
            AST *ifVarDec = new AST(std::string(buffer), NT_VARIABLE_DECLARATION);
            ifVarDec->addChild(NULL); //annotations
            ifVarDec->addChild(new AST("bool", NT_BASE_TYPE));
            ifVarDec->addChild(new AST("false", NT_EXPRESSION));
            newApply->addChild(ifVarDec);
        }
        for(int i = 0; i < (int) sharedTableNames.size(); i++){
            sprintf(buffer, "VirtApplyT%d", i + 1);
            AST *atVarDec = new AST(std::string(buffer), NT_VARIABLE_DECLARATION);
            atVarDec->addChild(NULL); //annotations
            atVarDec->addChild(new AST("bool", NT_BASE_TYPE));
            atVarDec->addChild(new AST("false", NT_EXPRESSION));
            newApply->addChild(atVarDec);
        }

        for(int i = 0; i < (int) sharedTableNames.size() + 1; i++){
            //TODO: Could check if equal...
            AST* newSS = templateSwitchS->Clone();
            for(int p = 0; p < (int) programs.size(); p++){
                if(splitedApply[p][i] != NULL){
                    delete newSS->getChildNT(NT_SWITCH_CASE_LIST)->children[p]->children[1];
                    newSS->getChildNT(NT_SWITCH_CASE_LIST)->children[p]->children[1] = splitedApply[p][i];
                }
            }
            newSS->RemoveEmptyIfElses();
            newApply->addChild(newSS);
            if(i < (int) sharedTableNames.size()){
                AST *tableApplyCondition = new AST("IF", NT_CONDITIONAL_STATEMENT);
                sprintf(buffer, "VirtApplyT%d", i + 1);
                tableApplyCondition->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                tableApplyCondition->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
                tableApplyCondition->addChild(NULL); //else
                AST *applyCall = new AST(NULL, NT_METHOD_CALL_STATEMENT);
                tableApplyCondition->children[1]->addChild(applyCall);
                applyCall->addChild(new AST(".", NT_NONE));
                applyCall->children[0]->addChild(new AST(sharedTableNames[i], NT_NONE));
                applyCall->children[0]->addChild(new AST("apply", NT_NONE));
                applyCall->addChild(NULL); //arguments
                newApply->addChild(tableApplyCondition);
            }
        }
        delete templateSwitchS;
    }
    else{
        std::set<int> VerifiedApplies;
        std::vector<std::vector<int>> EqualApplies;
        for(int i = 0; i < (int) programs.size(); i++){
            if(VerifiedApplies.find(i) == VerifiedApplies.end()){
                VerifiedApplies.emplace(i);
                EqualApplies.push_back({i});
                for(int j = i + 1; j < (int) programs.size(); j++){
                    if(AST::Equals(TempApplyLists[i], TempApplyLists[j])){
                        VerifiedApplies.emplace(j);
                        EqualApplies[EqualApplies.size() - 1].push_back(j);
                    }
                }
            }
        }

        if(EqualApplies.size() == 1){
            newControl->addChild(NULL_CLONE(TempApplyLists[0]));
        }
        else{
            AST *newApply = new AST(NULL, NT_STAT_OR_DEC_LIST);
            AST *branchProgram = new AST(NULL, NT_SWITCH_STATEMENT);
            branchProgram->addChild(new AST(TEMP_PROGRAM_CODE, NT_EXPRESSION));
            branchProgram->addChild(new AST(NULL, NT_SWITCH_CASE_LIST));
            for(const std::vector<int>& ea : EqualApplies){
                for(int x = 0; x < (int) ea.size(); x++){
                    sprintf(buffer, "%d", ea[x] + 1);
                    AST *newSC = new AST(NULL, NT_SWITCH_CASE);
                    newSC->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                    newSC->addChild((x == (int) ea.size() - 1) ? (TempApplyLists[ea[x]] == NULL ? new AST(NULL, NT_STAT_OR_DEC_LIST) : TempApplyLists[ea[x]]->Clone()) : NULL);
                    branchProgram->children[1]->addChild(newSC);
                }
            }
            newApply->addChild(branchProgram);
            newControl->addChild(newApply);
        }
    }
    newNodes.push_back(newControl);
    for(AST *TempLEL : TempLocalElementLists)
        NullDelete(TempLEL);
    for(AST *TempAL : TempApplyLists)
        NullDelete(TempAL);
    Log::Pop();

    return newNodes;
}

void AST::MergeUnifyErrorMKDeclarations(AST *root){
    std::set<std::string> errorIdentifiers;
    std::set<std::string> mkIdentifiers;
    
    int i = 0;
    while(i < (int) root->children.size()){
        AST *c = root->children[i];
        if(c->nType == NT_ERROR_DECLARATION || c->nType == NT_MATCH_KIND_DECLARATION){
            if(c->children[0] != NULL){
                for(AST *identifier : c->children[0]->children){
                    if(c->nType == NT_ERROR_DECLARATION){
                        if(errorIdentifiers.find(identifier->value) == errorIdentifiers.end())
                            errorIdentifiers.emplace(identifier->value);
                    }
                    else{
                        if(mkIdentifiers.find(identifier->value) == mkIdentifiers.end())
                            mkIdentifiers.emplace(identifier->value);
                    }
                }
            }
            delete root->children[i];
            root->children.erase(root->children.begin() + i);
            i--;
        }
        i++;
    }

    if(!errorIdentifiers.empty()){
        AST *errorDec = new AST(NULL, NT_ERROR_DECLARATION);
        AST *errIdentifierList = new AST(NULL, NT_IDENTIFIER_LIST);
        errorDec->addChild(errIdentifierList);
        for(const std::string& s : errorIdentifiers)
            errIdentifierList->addChild(new AST(s, NT_NONE));
        root->addChildStart(errorDec);
    }
    
    if(!mkIdentifiers.empty()){
        AST *mkDec = new AST(NULL, NT_MATCH_KIND_DECLARATION);
        AST *mkIdentifierList = new AST(NULL, NT_IDENTIFIER_LIST);
        mkDec->addChild(mkIdentifierList);

        for(const std::string& s : mkIdentifiers)
            mkIdentifierList->addChild(new AST(s, NT_NONE));
        root->addChildStart(mkDec);
    }
}

bool AST::TablesMergeable(AST *t1, AST *t2){
    if(t1 == NULL || t2 == NULL || t1->nType != NT_TABLE_DECLARATION || t2->nType != NT_TABLE_DECLARATION || t1->getChildNT(NT_TABLE_PROPERTY_LIST) == NULL || t2->getChildNT(NT_TABLE_PROPERTY_LIST) == NULL)
        return false;

    //Check is both tables are "VirtShared" compatible
    std::string vsharedName = "";
    if(t1->getChildNT(NT_ANNOTATION_LIST) != NULL){
        int i = 0;
        while(i < (int) t1->getChildNT(NT_ANNOTATION_LIST)->children.size() && t1->getChildNT(NT_ANNOTATION_LIST)->children[i]->value != VIRT_SHARED_ANNOTATION)
            i++;
        if(i < (int) t1->getChildNT(NT_ANNOTATION_LIST)->children.size()){
            AST *ATL = t1->getChildNT(NT_ANNOTATION_LIST)->children[i]->getChildNT(NT_ANNOTATION_TOKEN_LIST);
            if(ATL == NULL || ATL->children.size() < 0)
                return false;
            vsharedName = ATL->children[0]->value;
        }
    }
    if(t2->getChildNT(NT_ANNOTATION_LIST) != NULL){
        int i = 0;
        while(i < (int) t2->getChildNT(NT_ANNOTATION_LIST)->children.size() && t2->getChildNT(NT_ANNOTATION_LIST)->children[i]->value != VIRT_SHARED_ANNOTATION)
            i++;
        if(i < (int) t2->getChildNT(NT_ANNOTATION_LIST)->children.size()){
            AST *ATL = t2->getChildNT(NT_ANNOTATION_LIST)->children[i]->getChildNT(NT_ANNOTATION_TOKEN_LIST);
            if(ATL == NULL || ATL->children.size() < 0)
                return false;
            if(vsharedName != ATL->children[0]->value)
                return false;
        }
    }
    else if(vsharedName != ""){
        return false;
    }

    //Check if "const default_action" is the same
    std::string constDefaultAction1 = "";
    for(AST *c : t1->getChildNT(NT_TABLE_PROPERTY_LIST)->children)
        if(c != NULL && c->value == "const default_action")
            constDefaultAction1 = c->children[0]->value;
    if(constDefaultAction1 == "")
        return false;
    std::string constDefaultAction2 = "";
    for(AST *c : t2->getChildNT(NT_TABLE_PROPERTY_LIST)->children)
        if(c != NULL && c->value == "const default_action")
            constDefaultAction2 = c->children[0]->value;
    if(constDefaultAction1 != constDefaultAction2)
        return false;

    //Check if both tables define a size
    bool hasSize = false;
    for(AST *c : t1->getChildNT(NT_TABLE_PROPERTY_LIST)->children)
        if(c != NULL && c->value == "size")
            hasSize = true;
    if(!hasSize)
        return false;
    hasSize = false;
    for(AST *c : t2->getChildNT(NT_TABLE_PROPERTY_LIST)->children)
        if(c != NULL && c->value == "size")
            hasSize = true;
    if(!hasSize)
        return false;

    //Check if keys are the same
    AST *keyProperty1 = t1->getChildNT(NT_TABLE_PROPERTY_LIST)->findChildrenByValue("key", {NT_TABLE_PROPERTY});
    AST *keyProperty2 = t2->getChildNT(NT_TABLE_PROPERTY_LIST)->findChildrenByValue("key", {NT_TABLE_PROPERTY});
    Assert(keyProperty1 != NULL && keyProperty2 != NULL);
    Assert(keyProperty1->children[0] != NULL && keyProperty2->children[0] != NULL);

    std::unordered_set<AST*> matchedT1Keys;
    for(AST *keyElement : keyProperty1->children[0]->children)
        if(keyElement->children[0]->value == TEMP_PROGRAM_CODE)
            matchedT1Keys.emplace(keyElement);
    for(AST *keyElement : keyProperty2->children[0]->children){
        if(keyElement->children[0]->value != TEMP_PROGRAM_CODE){
            bool matched = false;
            for(AST* t1KeyElement : keyProperty1->children[0]->children){
                if(matchedT1Keys.find(t1KeyElement) == matchedT1Keys.end() && AST::Equals(keyElement, t1KeyElement)){
                    matchedT1Keys.emplace(t1KeyElement);
                    matched = true;
                }
            }
            if(!matched)
                return false;
        }
    }
    for(AST *keyElement : keyProperty1->children[0]->children)
        if(matchedT1Keys.find(keyElement) == matchedT1Keys.end())
            return false;
    return true;
}

AST* AST::TablesMerge(AST *t1, AST *t2){
    if(!AST::TablesMergeable(t1, t2)){
        return NULL;
    }

    AST *newTable = t1->Clone();

    bool isVirtShared = false;
    AST* AnnotationList = newTable->getChildNT(NT_ANNOTATION_LIST);
    if(AnnotationList != NULL){
        int i = 0;
        while(i < (int) AnnotationList->children.size() && newTable->getChildNT(NT_ANNOTATION_LIST)->children[i]->value != VIRT_SHARED_ANNOTATION)
            i++;
        if(i < (int) AnnotationList->children.size()){
            isVirtShared = true;
        }
    }

    if(!isVirtShared){
        AST *keyProperty = newTable->getChildNT(NT_TABLE_PROPERTY_LIST)->findChildrenByValue("key", {NT_TABLE_PROPERTY});
        Assert(keyProperty != NULL && keyProperty->children[0] != NULL);

        if(keyProperty->children[0]->children[0]->children[0]->value != TEMP_PROGRAM_CODE){
            AST *newKP = new AST("exact", NT_KEY_ELEMENT);
            newKP->addChild(new AST(TEMP_PROGRAM_CODE, NT_EXPRESSION));
            keyProperty->children[0]->addChildStart(newKP);
        }

        //Merge actions
        AST *actionProperty = newTable->getChildNT(NT_TABLE_PROPERTY_LIST)->findChildrenByValue("actions", {NT_TABLE_PROPERTY});
        AST *actionPropertyT2 = t2->getChildNT(NT_TABLE_PROPERTY_LIST)->findChildrenByValue("actions", {NT_TABLE_PROPERTY});
        Assert(actionProperty != NULL && actionPropertyT2 != NULL);
        Assert(actionProperty->children[0] != NULL && actionPropertyT2->children[0] != NULL);
        
        std::unordered_set<std::string> actionNames;
        for(AST *actionR : actionProperty->children[0]->children)
            actionNames.emplace(actionR->value);
        for(AST *actionR : actionPropertyT2->children[0]->children)
            if(actionNames.find(actionR->value) == actionNames.end())
                actionProperty->children[0]->addChild(actionR->Clone());

        //Merge size
        AST *sizeProperty = newTable->getChildNT(NT_TABLE_PROPERTY_LIST)->findChildrenByValue("size", {NT_TABLE_PROPERTY});
        AST *sizePropertyT2 = t2->getChildNT(NT_TABLE_PROPERTY_LIST)->findChildrenByValue("size", {NT_TABLE_PROPERTY});
        Assert(sizeProperty->children[0]->children.size() == 0 && sizePropertyT2->children[0]->children.size() == 0);
        const int size1 = IPLBase::intFromStr(sizeProperty->children[0]->str());
        const int size2 = IPLBase::intFromStr(sizePropertyT2->children[0]->str());
        Assert(size1 > 0 && size2 > 0);

        char buffer[128];
        sprintf(buffer, "%d", size1 + size2);
        sizeProperty->children[0]->value = std::string(buffer);
    }

    return newTable;
}

bool AST::EqualActionDeclarations(AST* a1, AST* a2){
    if(!Equals(a1->children[0], a2->children[0]) || !Equals(a1->children[1], a2->children[1]))
        return false;
    
    AST *tempA2 = a2->Clone();

    if(a1->getChildNT(NT_PARAMETER_LIST) != NULL && a2->getChildNT(NT_STAT_OR_DEC_LIST) != NULL){
        for(int i = 0; i < (int) a1->getChildNT(NT_PARAMETER_LIST)->children.size(); i++){
            tempA2->getChildNT(NT_STAT_OR_DEC_LIST)->SubstituteValue(tempA2->getChildNT(NT_PARAMETER_LIST)->children[i]->value, a1->getChildNT(NT_PARAMETER_LIST)->children[i]->value);
            tempA2->getChildNT(NT_PARAMETER_LIST)->children[i]->value = a1->getChildNT(NT_PARAMETER_LIST)->children[i]->value;
        }
    }

    bool result = AST::Equals(a1->getChildNT(NT_STAT_OR_DEC_LIST), tempA2->getChildNT(NT_STAT_OR_DEC_LIST));
    delete tempA2;
    return result;
}

bool AST::Equals(AST *a1, AST *a2){
    if(a1 == NULL || a2 == NULL)
        return (a1 == NULL && a2 == NULL);
    if(a1->nType != a2->nType || a1->children.size() != a2->children.size())
        return false;
    switch(a1->nType){
        case NT_STRUCT_FIELD_LIST:
        case NT_HEADER_TYPE_DECLARATION:
        //case NT_EXTERN_DECLARATION:
        case NT_STRUCT_TYPE_DECLARATION:
        case NT_STRUCT_FIELD:
        case NT_METHOD_PROTOTYPE_LIST:
        case NT_METHOD_PROTOTYPE:
        case NT_PARAMETER_LIST:
        case NT_TYPE_PARAMETER_LIST:
        case NT_CONSTRUCTOR_METHOD_PROTOTYPE:
        case NT_PARSER_STATEMENT_LIST:
        case NT_METHOD_CALL_STATEMENT:
        case NT_PARSER_STATE:
        case NT_INSTANTIATION:
        case NT_VARIABLE_DECLARATION:
        case NT_PARAMETER:
        case NT_CONTROL_TYPE_DECLARATION:
            for(int i = 0; i < (int) a1->children.size(); i++)
                if(!Equals(a1->children[i], a2->children[i]))
                    return false;
            return true;
            break;
        case NT_ACTION_DECLARATION:
            return AST::EqualActionDeclarations(a1, a2);
            break;
        /*
        case NT_FUNCTION_PROTOTYPE:
        case NT_BASE_TYPE:
        case NT_PARAMETER:
        */
        case NT_TYPE_NAME:
        case NT_LITERAL:
        case NT_DIRECTION:
        case NT_TYPE_OR_VOID:
        case NT_TYPE_PARAMETER:
            return a1->value == a2->value;
            break;
        default:
            if(a1->value != a2->value)
                return false;
            for(int i = 0; i < (int) a1->children.size(); i++)
                if(!Equals(a1->children[i], a2->children[i]))
                    return false;
            return true;
            break;
        
    }
    return false;
}

bool AST::EqualsParserDeclaration(AST *a1, AST *a2, AST *p1, AST *p2, const std::vector<AST*>& headersDec){

    //Parser statement lists must be equal
    if(!AST::Equals(p1->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST), p1->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST)))
        return false;

    std::vector<AST*> newNodes = MergeParsers({a1, a2}, {p1, p2}, headersDec, "TempName", {std::set<int>({0}), std::set<int>({1})}, NULL);
    if(newNodes.size() == 0)
        return false;

    bool result = p1->children[2]->children.size() == newNodes[newNodes.size() - 1]->children[2]->children.size();

    for(AST *node : newNodes)
        delete node;

    return result;
}

bool AST::EqualsControlDeclaration(AST *a1, AST *a2, AST *c1, AST *c2, const std::vector<AST*>& headersDec){
    //TODO: Better?
    return AST::Equals(c1, c2);
}

std::vector<AST*> AST::MergeUnifyHeaders(const std::vector<AST*>& programs){
    char buffer[256];
    std::vector<AST*> newNodes;
    std::vector<std::vector<AST*>> pHeaders(programs.size());
    std::vector<std::map<AST*, AST*>> deleteHeaders(programs.size());

    Log::MSG("Unifying equal Structs and Headers");
    Log::Push();

    for(int i = 0; i < (int) programs.size(); i++){
        for(AST *c : programs[i]->children){
            if(c != NULL && (c->nType == NT_HEADER_TYPE_DECLARATION || c->nType == NT_STRUCT_TYPE_DECLARATION))
            pHeaders[i].push_back(c);
        }
    }

    for(int i = 0; i < (int) programs.size(); i++){
        for(int hi = 0; hi < (int) pHeaders[i].size(); hi++){
            bool repeated = false;
            for(AST *n : newNodes){
                if(Equals(pHeaders[i][hi], n)){
                    sprintf(buffer, "s%d", i+1);
                    std::string newName = n->value.substr(0, n->value.find("_")) + std::string(buffer) + n->value.substr(n->value.find("_"));
                    
                    Log::MSG("Headers \"" + n->value + "\" and \"" + pHeaders[i][hi]->value + "\" are equal. Renaming to \"" + newName + "\"");
                    
                    n->value = newName;
                    deleteHeaders[i].emplace(std::make_pair(pHeaders[i][hi], n));
                    repeated = true;
                }
            }
            if(!repeated){
                newNodes.push_back(pHeaders[i][hi]->Clone());
                deleteHeaders[i].emplace(std::make_pair(pHeaders[i][hi], newNodes[newNodes.size() - 1]));
            }
        }
    }

    for(AST *n : newNodes){
        if(n->getChildNT(NT_STRUCT_FIELD_LIST) != NULL){
            for(AST* sf : n->getChildNT(NT_STRUCT_FIELD_LIST)->children){
                sf->value = n->value.substr(0, n->value.find("_")) + sf->value.substr(sf->value.find("_"));
                if(sf->children[1]->nType == NT_TYPE_NAME)
                    for(int p = 0; p < (int) deleteHeaders.size(); p++)
                        for(auto it : deleteHeaders[p])
                            if(sf->children[1]->value == it.first->value)
                                sf->children[1]->value = it.second->value;
            }
        }
    }

    for(int i = 0; i < (int) programs.size(); i++){
        for(auto it : deleteHeaders[i])
            programs[i]->SubstituteHeaderType(it.first, it.second);
        
        int cc = 0;
        while(cc < (int) programs[i]->children.size()){
            if(deleteHeaders[i].find(programs[i]->children[cc]) != deleteHeaders[i].end()){
                delete programs[i]->children[cc];
                programs[i]->children.erase(programs[i]->children.begin() + cc);
                cc--;
            }
            cc++;
        }
    }

    Log::Pop();

    return newNodes;
}

void ASTChangeExternName(AST *a, const std::string oldN, const std::string newN){
    if(a->getProgramType() == oldN)
        a->setProgramType(newN);
    if(a->getNodeType() == NT_TYPE_NAME && a->getValue() == oldN)
        a->setValue(newN);
    for(AST *c : a->getChildren())
        if(c != NULL)
            ASTChangeExternName(c, oldN, newN);
}

std::vector<AST*> AST::MergeUnifyExterns(const std::vector<AST*>& programs){
    std::vector<AST*> newNodes;
    std::vector<std::vector<AST*>> externs(programs.size());

    for(int i = 0; i < (int) programs.size(); i++){
        for(AST *c : programs[i]->children)
            if(c != NULL && c->nType == NT_EXTERN_DECLARATION)
                externs[i].push_back(c);
    }

    std::vector<std::map<AST*, AST*>> deleteExterns(programs.size());

    for(int i = 0; i < (int) programs.size(); i++){
        for(int ei = 0; ei < (int) externs[i].size(); ei++){
            bool repeated = false;
            for(int ni = 0; ni < (int) newNodes.size(); ni++){
                AST *n = newNodes[ni];
                if(AST::Equals(externs[i][ei], n)){
                    repeated = true;
                    deleteExterns[i].emplace(std::make_pair(externs[i][ei], n));
                }
            }
            if(!repeated){
                std::string eName = (externs[i][ei]->children.size() == 3) ? externs[i][ei]->value : externs[i][ei]->children[1]->value;
                newNodes.push_back(externs[i][ei]->Clone());
                deleteExterns[i].emplace(std::make_pair(externs[i][ei], newNodes[newNodes.size() - 1]));
            
                newNodes[newNodes.size() - 1]->writeCode(stdout);
                printf("\n***********\n");
            }
        }
    }

    for(int i = 0; i < (int) deleteExterns.size(); i++){
        for(auto it : deleteExterns[i])
            ASTChangeExternName(programs[i], it.first->value, it.second->value);

        int cc = 0;
        while(cc < (int) programs[i]->children.size()){
            if(deleteExterns[i].find(programs[i]->children[cc]) != deleteExterns[i].end()){
                delete programs[i]->children[cc];
                programs[i]->children.erase(programs[i]->children.begin() + cc);
                cc--;
            }
            cc++;
        }
    }

    return newNodes;
}

void AST::SubstituteHeaderType(AST *oldHeader, AST *newHeader){
    if(this->programType == oldHeader->value)
        this->programType = newHeader->value;

    if(this->nType == NT_TYPE_NAME && this->value == oldHeader->value)
        this->value = newHeader->value;
    else if(this->nType == NT_EXPRESSION || this->nType == NT_NONE){
        if(this->value == "."){
            if(this->children[0]->programType == oldHeader->value){
                if(this->children[1] != NULL){
                    for(int i = 0; i < (int) newHeader->children[2]->children.size(); i++){
                        if(this->children[1]->value == oldHeader->children[2]->children[i]->value)
                            this->children[1]->value = newHeader->children[2]->children[i]->value;
                    }
                }
            }
        }
    }
    for(AST *c : this->children)
        if(c != NULL)
            c->SubstituteHeaderType(oldHeader, newHeader);
}

void ASTRemoveHeaderFieldsPrefixRec(AST *ast, const std::string headerName, const std::string longestP, const AST* parent){
    if(parent != NULL && parent->getNodeType() != NT_METHOD_CALL_STATEMENT && (ast->getValue() == ".") && (ast->getNodeType() == NT_NONE || ast->getNodeType() == NT_EXPRESSION) && ast->getChildren().size() == 2){
        if(ast->getChildren()[0]->getProgramType() == headerName){
            ast->getChildren()[1]->setValue(ast->getChildren()[1]->getValue().substr(longestP.size()));
        }
    }

    for(AST *c : ast->getChildren())
        if(c != NULL)
            ASTRemoveHeaderFieldsPrefixRec(c, headerName, longestP, ast);
}

void AST::RemoveHeaderFieldsPrefix(AST *header){
    if(header->children[2] == NULL)
        return;

    std::string longestPrefix = header->children[2]->children[0]->value;
    for(int i = 1; i < (int) header->children[2]->children.size(); i++){
        AST *structField = header->children[2]->children[i];
        int j = 0;
        while(j < (int) structField->value.size() && j < (int) longestPrefix.size() && longestPrefix[j] == structField->value[j])
            j++;
        if(j < (int) longestPrefix.size())
            longestPrefix = longestPrefix.substr(0, j);
    }

    while(longestPrefix.size() > 0 && longestPrefix[longestPrefix.size() - 1] != '_')
        longestPrefix.pop_back();

    if(longestPrefix.size() > 0){
        for(AST *structField : header->children[2]->children)
            structField->value = structField->value.substr(longestPrefix.size(), structField->value.size() - longestPrefix.size());
            
        ASTRemoveHeaderFieldsPrefixRec(this, header->value, longestPrefix, NULL);
    }
}

bool UnusedTypeName(AST *ast, const std::string& typeName){
    if(ast->getNodeType() == NT_TYPE_NAME && ast->getValue() == typeName)
        return false;
    for(AST *c : ast->getChildren()){
        if(c != NULL && !UnusedTypeName(c, typeName))
            return false;
    }
    return true;
}

void AST::RemoveHeaderPrefix(const std::string headerName){
    int ulp = findPrefix(headerName);
    Assert(ulp != -1);
    if(this->value == headerName)
        this->value = headerName.substr(ulp + 1);
    if(this->programType == headerName)
        this->programType = headerName.substr(ulp + 1);

    for(AST *c : this->children)
        if(c != NULL)
            c->RemoveHeaderPrefix(headerName);
}

AST* AST::findChildrenByValue(const LexVal& value, const std::unordered_set<NodeType>& validTypes){
    AST* child = NULL;
    for(AST *c : this->children)
        if(c != NULL && c->value == value && (validTypes.empty() || validTypes.find(c->nType) != validTypes.end()))
            child = c;
    return child;
}

void AST::SubstituteValue(const std::string& oldValue, const std::string& newValue){
    if(this->value == oldValue)
        this->value = newValue;
    if(this->programType == oldValue)
        this->programType == newValue;
    for(AST *c : this->children)
        if(c != NULL)
            c->SubstituteValue(oldValue, newValue);
}

void AST::RemoveMergePrefixes(){
    static const std::unordered_set<std::string> prefixesSet = {"PS_", "P1_", "P2_"};
    switch(this->nType){
        case NT_PARSER_DECLARATION:
            if(this->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST) != NULL){
                for(AST* LocalElement : this->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST)->children){
                    int ulp = findPrefix(LocalElement->value);
                    if(ulp != -1){
                        const std::string candidateName = LocalElement->value.substr(ulp + 1);
                        bool noConflicts = true;
                        for(AST* LocalElement2 : this->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST)->children){
                            if(LocalElement2 != LocalElement && findPrefix(LocalElement2->value) != -1 && candidateName == LocalElement2->value.substr(findPrefix(LocalElement2->value) + 1)){
                                noConflicts = false;
                            }
                        }
                        if(noConflicts){
                            for(AST *LocalElement2 : this->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST)->children)
                                LocalElement2->SubstituteIdentifierWithType(LocalElement->value, candidateName, LocalElement->programType);
                            Assert(this->getChildNT(NT_PARSER_LOCAL_ELEMENT_LIST) != NULL);
                            for(AST *State : this->getChildNT(NT_PARSER_STATE_LIST)->children)
                                State->SubstituteIdentifierWithType(LocalElement->value, candidateName, LocalElement->programType);
                            
                            Log::MSG("Renamed \"" + LocalElement->value + "\" to \"" + candidateName + "\" in \"" + this->getChildNT(NT_PARSER_TYPE_DECLARATION)->value + "\"");
                            LocalElement->value = candidateName;
                        }
                    }
                }
            }

            for(AST* state : this->children[2]->children){
                int ulp = findPrefix(state->value);
                if(ulp != -1){
                    const std::string candidateName = state->value.substr(ulp + 1);
                    bool noConflicts = true;
                    for(AST *state2 : this->children[2]->children){
                        if(state2 != state && findPrefix(state2->value) != -1 && candidateName == state2->value.substr(findPrefix(state2->value) + 1)){
                            noConflicts = false;
                        }
                    }
                    if(noConflicts){
                        for(AST *state2 : this->children[2]->children){
                            if(state2->children[1]->nType == NT_NAME_STATE_EXPRESSION && state2->children[1]->value == state->value){
                                state2->children[1]->value = candidateName;
                            }
                            else if(state2->children[1]->nType == NT_SELECT_EXPRESSION){
                                for(AST *selectCase : state2->children[1]->children[1]->children){
                                    if(selectCase->value == state->value)
                                        selectCase->value = candidateName;
                                }
                            }
                        }
                        Log::MSG("Renamed \"" + state->value + "\" to \"" + candidateName + "\" in \"" + this->getChildNT(NT_PARSER_TYPE_DECLARATION)->value + "\"");
                        state->value = candidateName;
                    }
                }
            }
            break;
        case NT_CONTROL_DECLARATION:
            if(this->getChildNT(NT_CONTROL_LOCAL_DEC_LIST) != NULL){
                for(AST* LocalElement : this->getChildNT(NT_CONTROL_LOCAL_DEC_LIST)->children){
                    int ulp = findPrefix(LocalElement->value);
                    if(ulp != -1){
                        const std::string candidateName = LocalElement->value.substr(ulp + 1);
                        bool noConflicts = true;
                        for(AST* LocalElement2 : this->getChildNT(NT_CONTROL_LOCAL_DEC_LIST)->children){
                            if(LocalElement2 != LocalElement && findPrefix(LocalElement2->value) != -1 && candidateName == LocalElement2->value.substr(findPrefix(LocalElement2->value) + 1)){
                                noConflicts = false;
                            }
                        }
                        if(noConflicts){
                            for(AST *LocalElement2 : this->getChildNT(NT_CONTROL_LOCAL_DEC_LIST)->children)
                                LocalElement2->SubstituteIdentifierWithType(LocalElement->value, candidateName, LocalElement->programType);
                            if(this->getChildNT(NT_STAT_OR_DEC_LIST) != NULL)
                                for(AST *SOD : this->getChildNT(NT_STAT_OR_DEC_LIST)->children)
                                    SOD->SubstituteIdentifierWithType(LocalElement->value, candidateName, LocalElement->programType);
                            
                            Log::MSG("Renamed \"" + LocalElement->value + "\" to \"" + candidateName + "\" in \"" + this->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value + "\"");
                            CFG::ChangeVTableName(this->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, LocalElement->value, candidateName);
                            CFG::ChangeVActionName(this->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, LocalElement->value, candidateName);
                            
                            LocalElement->value = candidateName;
                        }
                    }
                }
            }
            break;
        default:
            for(AST *c : this->children){
                if(c != NULL){
                    if(c->nType == NT_HEADER_TYPE_DECLARATION || c->nType == NT_STRUCT_TYPE_DECLARATION){
                        int ulp = findPrefix(c->value);
                        if(ulp != -1){
                            bool noConflicts = true;
                            for(AST *c2 : this->children){
                                if(c2 != NULL && c2 != c && (c2->nType == NT_HEADER_TYPE_DECLARATION || c2->nType == NT_STRUCT_TYPE_DECLARATION) && findPrefix(c2->value) != -1 && c2->value.substr(findPrefix(c2->value)) == c->value.substr(ulp)){
                                    noConflicts = false;
                                }
                            }
                            if(noConflicts){
                                this->RemoveHeaderPrefix(c->value);
                            }
                        }

                        this->RemoveHeaderFieldsPrefix(c);
                    }
                    else if(c->nType == NT_PARSER_DECLARATION){
                        int ulpPN = findPrefix(c->getChildNT(NT_PARSER_TYPE_DECLARATION)->value);
                        if(ulpPN != -1){
                            bool noConflicts = true;
                            for(AST *c2 : this->children){
                                if(c2 != NULL && c2 != c && c2->nType == NT_PARSER_DECLARATION && findPrefix(c2->getChildNT(NT_PARSER_TYPE_DECLARATION)->value) != -1 && c2->getChildNT(NT_PARSER_TYPE_DECLARATION)->value.substr(findPrefix(c2->getChildNT(NT_PARSER_TYPE_DECLARATION)->value)) == c->getChildNT(NT_PARSER_TYPE_DECLARATION)->value.substr(ulpPN)){
                                    noConflicts = false;
                                }
                            }
                            if(noConflicts){
                                std::string newName = c->getChildNT(NT_PARSER_TYPE_DECLARATION)->value.substr(ulpPN + 1);

                                Log::MSG("Renamed \"" + c->getChildNT(NT_PARSER_TYPE_DECLARATION)->value + "\" to \"" + newName + "\"");
                                this->SubstituteTypeName(c->getChildNT(NT_PARSER_TYPE_DECLARATION)->value, newName);
                                c->getChildNT(NT_PARSER_TYPE_DECLARATION)->value = newName;
                            }
                        }
                    }
                    else if(c->nType == NT_CONTROL_DECLARATION){
                        int ulpPN = findPrefix(c->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value);
                        if(ulpPN != -1){
                            bool noConflicts = true;
                            for(AST *c2 : this->children){
                                if(c2 != NULL && c2 != c && c2->nType == NT_CONTROL_DECLARATION && findPrefix(c2->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value) != -1 && c2->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value.substr(findPrefix(c2->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value)) == c->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value.substr(ulpPN)){
                                    noConflicts = false;
                                }
                            }
                            if(noConflicts){
                                std::string newName = c->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value.substr(ulpPN + 1);

                                Log::MSG("Renamed \"" + c->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value + "\" to \"" + newName + "\"");
                                this->SubstituteTypeName(c->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, newName);
                                c->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value = newName;
                            }
                        }
                    }
                    c->RemoveMergePrefixes();
                }
            }
            break;
    }
}

void AST::RemoveUnusedHeaders(){
    std::unordered_set<AST*> childsToDelete;

    for(AST *child : this->children)
        if(child != NULL && (child->nType == NT_HEADER_TYPE_DECLARATION || child->nType == NT_STRUCT_TYPE_DECLARATION) && UnusedTypeName(this, child->value))
            childsToDelete.emplace(child);

    int i = 0;
    while(i < (int) this->children.size()){
        if(childsToDelete.find(this->children[i]) != childsToDelete.end()){
            Log::MSG("Remove unused header/struct \"" + this->children[i]->value + "\"");
            delete this->children[i];
            this->children.erase(this->children.begin() + i);
            i--;
        }
        i++;
    }
}

void AST::RemoveDuplicateHeaders(){
    std::vector<AST*> structHeaderDeclarations;
    std::set<int> childsToDelete;

    for(AST *child : this->children)
        if(child != NULL && (child->nType == NT_HEADER_TYPE_DECLARATION || child->nType == NT_STRUCT_TYPE_DECLARATION))
            structHeaderDeclarations.push_back(child);

    for(int i = 0; i < (int) structHeaderDeclarations.size(); i++){
        if(childsToDelete.find(i) == childsToDelete.end()){
            for(int j = i + 1; j < (int) structHeaderDeclarations.size(); j++){
                if(AST::Equals(structHeaderDeclarations[i], structHeaderDeclarations[j])){
                    this->SubstituteHeaderType(structHeaderDeclarations[j], structHeaderDeclarations[i]);
                    childsToDelete.emplace(j);
                }
            }
        }
    }

    int dc = 0;
    int i = 0;
    while(i < (int) this->children.size()){
        if(childsToDelete.find(i + dc) != childsToDelete.end()){
            delete this->children[i];
            this->children.erase(this->children.begin() + i);
            dc++;
            i--;
        }
        i++;
    }

    //Change VirtTypes numeration
    std::set<int> virtTypesNumbers;
    for(AST *child : this->children)
        if(child != NULL && (child->nType == NT_HEADER_TYPE_DECLARATION || child->nType == NT_STRUCT_TYPE_DECLARATION) && (child->value.size() > strlen(VIRT_TYPE_ID) && child->value.substr(0, strlen(VIRT_TYPE_ID)) == VIRT_TYPE_ID))
            virtTypesNumbers.emplace(IPLBase::intFromStr(child->value.c_str() + strlen(VIRT_TYPE_ID)));

    int vtN = 1;
    for(int n : virtTypesNumbers){
        char buffer1[128];
        char buffer2[128];
        sprintf(buffer1, "%s%d", VIRT_TYPE_ID, n);
        sprintf(buffer2, "%s%d", VIRT_TYPE_ID, vtN);
        this->SubstituteValue(buffer1, buffer2);
        this->RenameProgramTypes(buffer1, buffer2);
        vtN++;
    }
}

AST* AST::MergeStructFieldListByNames(const std::vector<AST*>& headersDec, const std::vector<std::string>& names){
    std::vector<AST*> SFLs;
    for(int i = 0; i < (int) names.size(); i++){
        AST* header = NULL;
        for(int j = 0; j < (int) headersDec.size(); j++)
            if(headersDec[j]->value == names[i])
                header = headersDec[j];

        Assert(header != NULL);

        if(header->getChildNT(NT_STRUCT_FIELD_LIST) != NULL)
            SFLs.push_back(header->getChildNT(NT_STRUCT_FIELD_LIST));
    }

    if(SFLs.size() == 0)
        return NULL;

    AST* result = SFLs[0]->Clone();

    for(int i = 1; i < (int) SFLs.size(); i++){
        const int lastSF = (int) result->children.size();
        std::unordered_set<AST*> matched;
        for(int j = 0; j < (int) SFLs[i]->children.size(); j++){
            int x = 0;
            while(x < lastSF && (!Equals(result->children[x], SFLs[i]->children[j]) || matched.find(result->children[x]) != matched.end()) ){
                x++;
            }
            if(x < lastSF)
                matched.emplace(result->children[x]);
            else
                result->addChild(SFLs[i]->children[j]->Clone());
        }
    }

    return result;
}

void ASTRenameTypeDeclarationsGetRec(AST *a, std::unordered_set<std::string>& s){
    const static std::unordered_set<NodeType> DecTypes = {NT_PARSER_TYPE_DECLARATION, NT_PACKAGE_TYPE_DECLARATION, NT_CONTROL_TYPE_DECLARATION, NT_HEADER_TYPE_DECLARATION, NT_STRUCT_TYPE_DECLARATION};
    for(AST *c : a->getChildren()){
        if(c != NULL){
            if(DecTypes.find(c->getNodeType()) != DecTypes.end())
                if(s.find(c->getValue()) == s.end())
                    s.emplace(c->getValue());
            ASTRenameTypeDeclarationsGetRec(c, s);
        }
    }
}

void ASTRenameTypeDeclarationsRec(AST *a, const std::string& oldName, const std::string& newName){
    for(AST *c : a->getChildren()){
        if(c != NULL){
            if(c->getValue() == oldName && (c->getNodeType() == NT_EXPRESSION || c->getNodeType() == NT_TYPE_NAME || c->getNodeType() == NT_PARSER_TYPE_DECLARATION || c->getNodeType() == NT_PACKAGE_TYPE_DECLARATION || c->getNodeType() == NT_CONTROL_TYPE_DECLARATION || c->getNodeType() == NT_HEADER_TYPE_DECLARATION || c->getNodeType() == NT_STRUCT_TYPE_DECLARATION))
                c->setValue(newName);
            if(c->getProgramType() == oldName)
                c->setProgramType(newName);
            ASTRenameTypeDeclarationsRec(c, oldName, newName);
        }
    }
}

void AST::RenameTypeDeclarations(const std::vector<AST *>& programs){
    char buffer[128];
    std::vector<std::unordered_set<std::string>> PTypeNames(programs.size());

    Log::MSG("Renaming type declarations in programs");
    Log::Push();

    for(int i = 0; i < (int) programs.size(); i++){
        ASTRenameTypeDeclarationsGetRec(programs[i], PTypeNames[i]);
        for(const std::string& t1 : PTypeNames[i]){
            sprintf(buffer, "Ps%d_%s", i+1, t1.c_str());
            ASTRenameTypeDeclarationsRec(programs[i], t1, std::string(buffer));

            Log::MSG("Renamed \"" + t1 + "\" to \"" + std::string(buffer) + "\" in \"" + programs[i]->value + "\"");
        }
    }

    Log::Pop();
}

void AST::AddGlobalActionsToCFG(const std::vector<AST *>& programs){
    int i = 1;
    for(AST* prog : programs){
        for(AST *c : prog->children){
            if(c->nType == NT_ACTION_DECLARATION){
                CFG::Get().AddProgramAction(i - 1, c, c->value, "", "");
            }
        }
        i++;
    }
}

AST* AST::MergeAST(const std::vector<AST*> programs, const std::vector<std::set<int>>& virtualSwitchPorts, const P4Architecture architecture){
    Assert(architecture != P4A_UNDEFINED);

    std::string msg;

    AST *root = new AST("merged.p4", NT_PROGRAM);

    RenameTypeDeclarations(programs);
    AddGlobalActionsToCFG(programs);

    for(AST* p: programs)
        p->InferTypes();

    std::vector<AST*> newExterns = MergeUnifyExterns(programs);
    for(AST * n : newExterns)
        root->addChild(n);

    std::vector<AST*> newHeaders = MergeUnifyHeaders(programs);
    for(AST * n : newHeaders)
        root->addChild(n);

    std::unordered_set<AST*> nodesToRemove;

    //Identify Equal Parsers
    std::vector<AST*> newParsers;
    std::vector<std::vector<std::pair<int, AST*>>> equalParsers;

    Log::MSG("Unifying equal Parsers");
    Log::Push();
    for(int p1 = 0; p1 < (int) programs.size(); p1++){
        for(int c1 = 0; c1 < (int) programs[p1]->children.size(); c1++){
            if(programs[p1]->children[c1]->nType == NT_PARSER_DECLARATION){
                for(int p2 = p1 + 1; p2 < (int) programs.size(); p2++){
                    for(int c2 = 0; c2 < (int) programs[p2]->children.size(); c2++){
                        if(programs[p2]->children[c2]->nType == NT_PARSER_DECLARATION && nodesToRemove.find(programs[p2]->children[c2]) == nodesToRemove.end()){
                            Log::EnablePush(false);
                            if(AST::EqualsParserDeclaration(programs[p1], programs[p2], programs[p1]->children[c1], programs[p2]->children[c2], newHeaders)){
                                Log::EnablePop();
                                msg = "Parsers are equal: \"" + programs[p1]->children[c1]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value  + "\"";
                                msg = msg + " and \"" + programs[p2]->children[c2]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value + "\" are equal";
                                Log::MSG(msg);
                                nodesToRemove.emplace(programs[p2]->children[c2]);
                                if(equalParsers.size() > 0 && equalParsers[equalParsers.size() - 1][0].second == programs[p1]->children[c1]){
                                    equalParsers[equalParsers.size() - 1].push_back(std::make_pair(p2, programs[p2]->children[c2]));
                                }
                                else{
                                    nodesToRemove.emplace(programs[p1]->children[c1]);
                                    equalParsers.push_back({std::make_pair(p1, programs[p1]->children[c1]), std::make_pair(p2, programs[p2]->children[c2])});
                                }
                            }
                            else{
                                Log::EnablePop();
                            }
                        }
                    }
                }
            }
        }
    }
    for(const std::vector<std::pair<int, AST*>>& eqs : equalParsers){
        msg = "Unified parsers: ";

        AST* newParser = eqs[0].second->Clone();
        newParsers.push_back(newParser);
        std::string* pName = &newParser->getChildNT(NT_PARSER_TYPE_DECLARATION)->value;
        for(int i = 1; i < (int) eqs.size(); i++)
            *pName = pName->substr(0, pName->find("_")) + eqs[i].second->getChildNT(NT_PARSER_TYPE_DECLARATION)->value.substr(1, eqs[i].second->getChildNT(NT_PARSER_TYPE_DECLARATION)->value.find("_")) + pName->substr(pName->find("_") + 1);

        for(const std::pair<int, AST*>& pd : eqs){
            msg = msg + "\"" + pd.second->getChildNT(NT_PARSER_TYPE_DECLARATION)->value + "\" ";
            programs[pd.first]->SubstituteTypeName(pd.second->getChildNT(NT_PARSER_TYPE_DECLARATION)->value, *pName);
            pd.second->getChildNT(NT_PARSER_TYPE_DECLARATION)->value = *pName;
        }

        msg = msg + "into \"" + *pName + "\"";
        Log::MSG(msg);
    }
    Log::Pop();

    //Identify Equal Controls
    std::vector<AST*> newControls;
    std::vector<std::vector<std::pair<int, AST*>>> equalControls;

    Log::MSG("Unifying equal Controls");
    Log::Push();
    for(int p1 = 0; p1 < (int) programs.size(); p1++){
        for(int c1 = 0; c1 < (int) programs[p1]->children.size(); c1++){
            if(programs[p1]->children[c1]->nType == NT_CONTROL_DECLARATION){
                for(int p2 = p1 + 1; p2 < (int) programs.size(); p2++){
                    for(int c2 = 0; c2 < (int) programs[p2]->children.size(); c2++){
                        if(programs[p2]->children[c2]->nType == NT_CONTROL_DECLARATION && nodesToRemove.find(programs[p2]->children[c2]) == nodesToRemove.end()){
                            Log::EnablePush(false);
                            if(AST::EqualsControlDeclaration(programs[p1], programs[p2], programs[p1]->children[c1], programs[p2]->children[c2], newHeaders)){
                                Log::EnablePop();
                                msg = "Controls are equal: \"" + programs[p1]->children[c1]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value  + "\"";
                                msg = msg + " and \"" + programs[p2]->children[c2]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value + "\" are equal";
                                Log::MSG(msg);
                                nodesToRemove.emplace(programs[p2]->children[c2]);
                                if(equalControls.size() > 0 && equalControls[equalControls.size() - 1][0].second == programs[p1]->children[c1]){
                                    equalControls[equalControls.size() - 1].push_back(std::make_pair(p2, programs[p2]->children[c2]));
                                }
                                else{
                                    nodesToRemove.emplace(programs[p1]->children[c1]);
                                    equalControls.push_back({std::make_pair(p1, programs[p1]->children[c1]), std::make_pair(p2, programs[p2]->children[c2])});
                                }
                            }
                            else{
                                Log::EnablePop();
                            }
                        }
                    }
                }
            }
        }
    }
    for(const std::vector<std::pair<int, AST*>>& eqs : equalControls){
        msg = "Unified controls: ";

        AST* newControl = eqs[0].second->Clone();
        newControls.push_back(newControl);
        std::string* pName = &newControl->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value;
        for(int i = 1; i < (int) eqs.size(); i++)
            *pName = pName->substr(0, pName->find("_")) + eqs[i].second->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value.substr(1, eqs[i].second->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value.find("_")) + pName->substr(pName->find("_") + 1);

        for(const std::pair<int, AST*>& pd : eqs){
            msg = msg + "\"" + pd.second->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value + "\" ";
            programs[pd.first]->SubstituteTypeName(pd.second->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value, *pName);
            pd.second->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value = *pName;
        }

        msg = msg + "into \"" + *pName + "\"";
        Log::MSG(msg);
    }
    Log::Pop();

    for(AST* np : newParsers)
        programs[programs.size() - 1]->addChild(np);
    for(AST* nc : newControls)
        programs[programs.size() - 1]->addChild(nc);

    //Architecture specific merge
    switch(architecture){
        case P4A_V1MODEL:
            V1Merge::MergeV1(programs, newHeaders, virtualSwitchPorts);
            break;
        case P4A_TNA:
            TofinoMerge::MergeTofino(programs, newHeaders, virtualSwitchPorts);
            break;
        default:
            break;
    }

    for(AST* p : programs){
        for(AST *c : p->children){
            if(nodesToRemove.find(c) == nodesToRemove.end())
                root->addChild(c->Clone());
        }
    }
    
    root->RemoveEmptySwitches();
    root->RemoveEmptyIfElses();

    MergeUnifyErrorMKDeclarations(root);

    //Sort headers and structs to start
    for(int i = 1; i < (int) root->children.size(); i++){
        if(root->children[i]->nType == NT_HEADER_TYPE_DECLARATION || root->children[i]->nType == NT_STRUCT_TYPE_DECLARATION){
            int j = i;
            while(j > 0 && (root->children[j - 1]->nType != NT_HEADER_TYPE_DECLARATION && root->children[j - 1]->nType != NT_STRUCT_TYPE_DECLARATION)){
                AST *a = root->children[j];
                root->children[j] = root->children[j - 1];
                root->children[j - 1] = a;
                j--;
            }
        }
    }

    root->RemoveDuplicateHeaders();

    Log::MSG("Removing unused headers...");
    Log::Push();
    root->RemoveUnusedHeaders();
    Log::Pop();

    Log::MSG("Removing prefixes...");
    Log::Push();
    root->RemoveMergePrefixes();
    Log::Pop();

    return root;
}

void AST::Print(){
    printf("%p (%s) \'%s\': ", this, AST::NodeTypeToString(this->nType).c_str(), this->str());
    for(AST* c : children)
        printf("%p ", c);
    printf("\n");
    for(AST* c : children)
        if(c != NULL)
            c->Print();
}

void ASTGenDotGetADDrs(AST* ast, std::vector<AST*> &addrs){
    addrs.push_back(ast);
    for (AST* c : ast->getChildren())
        if(c != NULL)
            ASTGenDotGetADDrs(c, addrs);
}

void ASTGenDotEdges(AST* ast, FILE *fp, const std::unordered_map<AST*, int>& addrOrder){
    for(AST* c : ast->getChildren()){
        if(c != NULL)
            fprintf(fp, "NODE_%d -> NODE_%d\n", addrOrder.find(ast)->second, addrOrder.find(c)->second);
    }
    for(AST *c : ast->getChildren()){
        if(c != NULL)
            ASTGenDotEdges(c, fp, addrOrder);
    }
}

void AST::GenDot(FILE *fp) {
    std::vector<AST*> addrs;
    std::unordered_map<AST*, int> addrOrder;
    ASTGenDotGetADDrs(this, addrs);
    for(int i = 0; i < (int) addrs.size(); i++)
        addrOrder.emplace(addrs[i], i);

    fprintf(fp, "digraph AST {\n");

    for(AST* a : addrs){
        char buffer[1024];
        sprintf(buffer, "%s\\l%s", AST::NodeTypeToString(a->nType).c_str(), a->str());
        char *pos = strstr(buffer, "\"");
        if(pos == NULL)
            pos = strstr(buffer, "<");
        if(pos == NULL)
            pos = strstr(buffer, ">");
        while(pos != NULL){
            buffer[strlen(buffer) + 1] = '\0';
            for(int i = strlen(buffer); i > (int) (pos - buffer); i--){
                buffer[i] = buffer[i-1];
            }
            *pos = '\\';
            char *ppos = pos + 2;
            pos = strstr(ppos, "\"");
            if(pos == NULL)
                pos = strstr(ppos, "<");
            if(pos == NULL)
                pos = strstr(ppos, ">");
        }
        if(a->programType != ""){
            strcat(buffer, "\\lT: ");
            strcat(buffer, a->programType.c_str());
        }
        strcat(buffer, "\\l");
        fprintf(fp, "NODE_%d [ shape=record, style=\"filled\", fillcolor=cornsilk, label=\"%s\" ];\n", addrOrder.find(a)->second, buffer);
    }

    ASTGenDotEdges(this, fp, addrOrder);

    fprintf(fp, "}\n");
}

std::vector<AST*> AST::getChildren(){
    return this->children;
}

AST* AST::getChildNT(const NodeType nt) const{
    int i = 0;
    bool found = false;
    while(i < (int) this->children.size() && !found){
        if(this->children[i] != NULL && this->children[i]->nType == nt)
            found = true;
        else
            i++;
    }
    if(found)
        return this->children[i];
    return NULL;
}

LexVal AST::getValue() const{
    return this->value;
}

LexVal AST::getValueExpression() const{
    if(this->value == "."){
        if(this->children.size() == 2){
            return this->children[0]->getValueExpression() + "." + this->children[1]->getValueExpression();
        }
    }
    return this->value;
}

std::string AST::getProgramType() const{
    return this->programType;
}

NodeType AST::getNodeType() const{
    return this->nType;
}

void AST::setNodeType(NodeType nt){
    this->nType = nt;
}

void AST::setValue(LexVal value){
    this->value = value;
}

void AST::setProgramType(const std::string& programType){
    this->programType = programType;
}

void AST::addChild(AST* child){
    this->children.push_back(child);
}

void AST::addChildStart(AST* child){
    this->children.push_back(child);
    for(int i = (int) this->children.size() - 1; i > 0; i--)
        this->children[i] = this->children[i - 1];
    this->children[0] = child;
}

int AST::SimplifyRec(const std::unordered_map<NodeType,std::unordered_map<std::string, int>>& DeleteMap){
    auto it = DeleteMap.find(this->nType);
    if(it != DeleteMap.end()){
        auto it2 = it->second.find(this->value);
        if(it2 != it->second.end())
            return it2->second;
    }

    int i = 0;
    while(i < (int) this->children.size()){
        if(this->children[i] != NULL){
            int result = this->children[i]->SimplifyRec(DeleteMap);
            if(result == 0){
                delete this->children[i];
                this->children.erase(this->children.begin() + i);
                i--;
            }
            else if(result > 0){
                return result - 1;
            }
        }
        i++;
    }

    return -1;
}

void AST::Simplify(){
    static const std::unordered_map<NodeType,std::unordered_map<std::string, int>> DeleteMap({
        //{NT_HEADER_TYPE_DECLARATION, {{"ingress_intrinsic_metadata_t", 0}, {"egress_intrinsic_metadata_t", 0}, {"ptp_metadata_t", 0}, {"pktgen_recirc_header_t", 0}, {"pktgen_port_down_header_t", 0}, {"pktgen_timer_header_t", 0}}},
        //{NT_STRUCT_TYPE_DECLARATION, {{"standard_metadata_t", 0}, {"ingress_intrinsic_metadata_for_deparser_t", 0}, {"ingress_intrinsic_metadata_from_parser_t", 0}, {"egress_intrinsic_metadata_for_deparser_t", 0}, {"egress_intrinsic_metadata_from_parser_t", 0}, {"ingress_intrinsic_metadata_for_tm_t", 0}, {"egress_intrinsic_metadata_for_output_port_t", 0}}},
        //{NT_PARSER_TYPE_DECLARATION, {{"TofinoIngressParser", 1}, {"TofinoEgressParser", 1}}},
        {NT_PARSER_TYPE_DECLARATION, {{"Parser", 0}, {"IngressParserT", 0}, {"EgressParserT", 0}}},
        {NT_CONTROL_TYPE_DECLARATION, {{"Deparser", 0}, {"ComputeChecksum", 0}, {"Egress", 0}, {"Ingress", 0}, {"VerifyChecksum", 0}, {"IngressT", 0}, {"EgressT", 0}, {"IngressDeparserT", 0}, {"EgressDeparserT", 0}}},
        {NT_PACKAGE_TYPE_DECLARATION, {{"Pipeline", 0}, {"Switch", 0}, {"V1Switch", 0}, {"MultiParserSwitch", 0}, {"MultiParserPipeline", 0}, {"IngressParsers", 0}, {"EgressParsers", 0}}},
        {NT_ENUM_DECLARATION, {{"HashAlgorithm", 0}, {"CounterType", 0}, {"CloneType", 0}, {"MeterType", 0}, {"CounterType_t", 0}, {"MeterType_t", 0}, {"MeterColor_t", 0}, {"SelectorMode_t", 0}, {"HashAlgorithm_t", 0}, {"MathOp_t", 0},}},
        {NT_EXTERN_DECLARATION, {{"direct_meter", 0}, {"meter", 0}, {"register", 0}, {"action_profile", 0}, {"action_selector", 0}, {"Atcam", 0}, {"Alpm", 0}, {"Digest", 0}, {"Resubmit", 0}, {"Mirror", 0}, {"SelectorAction", 0}, {"ActionProfile", 0}, {"ActionSelector", 0}, {"Register", 0}, {"RegisterParam", 0}, {"DirectRegister", 0}, {"Wred", 0}, {"DirectWred", 0}, {"Lpf", 0}, {"DirectLpf", 0}, {"MathUnit", 0}, {"RegisterAction", 0}, {"DirectRegisterAction", 0},
                                 {"ParserCounter", 0}, {"Checksum", 0}, {"Checksum16", 0}, {"Random", 0}, {"Hash", 0}, {"ParserPriority", 0}, {"CRCPolynomial", 0}, {"packet_in", 0}, {"packet_out", 0}, {"counter", 0}, {"direct_counter", 0}, {"Counter", 0}, {"DirectCounter", 0}, {"Meter", 0}, {"DirectMeter", 0}}},
        {NT_FUNCTION_PROTOTYPE, {{"clone", 1}, {"clone3", 1}, {"log_msg", 1}, {"clone_preserving_field_list", 1}, {"truncate", 1}, {"assert", 1}, {"assume", 1}, {"resubmit", 1}, {"resubmit_preserving_field_list", 1}, {"recirculate", 1}, {"recirculate_preserving_field_list", 1},
                                 {"hash", 1}, {"verify_checksum", 1}, {"update_checksum", 1}, {"verify_checksum_with_payload", 1}, {"update_checksum_with_payload", 1}, {"mark_to_drop", 1}, {"digest", 1}, {"random", 1}, {"static_assert", 1}, {"verify", 1}, {"sizeInBits", 1}, {"port_metadata_unpack", 1}, {"sizeInBytes", 1}, {"is_validated", 1}, {"invalidate", 1}, {"max", 1}, {"min", 1}, {"funnel_shift_right", 1}}},
        //{NT_ACTION_DECLARATION, {{"NoAction", 0}}},
        {NT_CONSTANT_DECLARATION, {{"PARSER_ERROR_FCS", 0}, {"PARSER_ERROR_ARAM_MBE", 0}, {"PARSER_ERROR_MULTIWRITE", 0}, {"PARSER_ERROR_PHV_OWNER", 0}, {"PARSER_ERROR_DST_CONT", 0}, {"PARSER_ERROR_SRC_EXT", 0}, {"PARSER_ERROR_TIMEOUT_HW", 0}, {"PARSER_ERROR_TIMEOUT_USER", 0}, {"PARSER_ERROR_CTR_RANGE", 0}, {"PARSER_ERROR_PARTIAL_HDR", 0}, {"PARSER_ERROR_NO_MATCH", 0}, {"PARSER_ERROR_OK", 0}, {"PORT_METADATA_SIZE", 0}}},
    });

    this->SimplifyRec(DeleteMap);
    this->RemoveConstantDeclarations();
}

void AST::RemoveErrorAndMKDeclarations(){
    int i = 0;
    while(i < (int) this->children.size()){
        if(this->children[i]->nType == NT_ERROR_DECLARATION || this->children[i]->nType == NT_MATCH_KIND_DECLARATION){
            delete this->children[i];
            this->children.erase(this->children.begin() + i);
            i--;
        }
        i++;
    }
}

void AST::RemoveConstantDeclarations(){
    static const std::unordered_set<NodeType> ListTypes = {NT_STAT_OR_DEC_LIST, NT_PARSER_LOCAL_ELEMENT_LIST, NT_CONTROL_LOCAL_DEC_LIST};
    
    int i = 0;
    while(i < (int) this->children.size()){
        AST *child = this->children[i];
        if(child != NULL){
            child->RemoveConstantDeclarations();
            if(ListTypes.find(child->nType) != ListTypes.end() && child->children.size() == 0){
                delete child;
                this->children[i] = NULL;
            }
            else if(child->nType == NT_CONSTANT_DECLARATION){
                delete child;
                this->children.erase(this->children.begin() + i);
                i--;
            }
        }
        i++;
    }
}

void AST::RenameProgramTypes(const std::string& oldPT, const std::string& newPT){
    if(this->programType == oldPT)
        this->programType = newPT;
    for(AST *c : this->children)
        if(c != NULL)
            c->RenameProgramTypes(oldPT, newPT);
}

void AST::InferTypes(){
    std::map<std::string, std::string> nameToTypes;
    std::map<std::string, std::map<std::string, std::string>> nameToList;
    
    Log::MSG("Infering types on \"" + this->value + "\"");
    Log::Push();
    for(AST *c : this->children)
        if(c != NULL)
            c->InferTypesRec(nameToTypes, nameToList);
    Log::Pop();
}

void AST::InferTypesRec(std::map<std::string, std::string>& nameToTypes, std::map<std::string, std::map<std::string, std::string>>& nameToList){
    switch(this->nType){
        case NT_EXPRESSION:
        case NT_NONE:
            {
            auto it = nameToTypes.find(this->value);
            if(it != nameToTypes.end()){
                this->programType = it->second;
            }
            else if(this->value == "."){
                this->children[0]->InferTypesRec(nameToTypes, nameToList);
                if(this->children[1] != NULL && this->children[1]->nType == NT_NONE){
                    auto it = nameToList.find(this->children[0]->programType);
                    if(it != nameToList.end()){
                        auto it2 = it->second.find(this->children[1]->value);
                        if(it2 != it->second.end()){
                            this->programType = it2->second;
                        }
                    }
                    if(this->children[0]->programType != "" && nameToList.find(this->children[0]->programType) != nameToList.end() && this->children.size() == 2)
                        this->children[1]->value = this->children[0]->programType + "_" + this->children[1]->value;
                }
                if(this->children.size() > 2){
                    for(int i = 2; i < (int) this->children.size(); i++){
                        if(this->children[i] != NULL)
                            this->children[i]->InferTypesRec(nameToTypes, nameToList);
                    }
                }
            }
            else{
                for(AST *c : this->children)
                    if(c != NULL)
                        c->InferTypesRec(nameToTypes, nameToList);
            }
            }
            break;
        case NT_FUNCTION_DECLARATION:
        case NT_PARSER_DECLARATION:
        case NT_CONTROL_DECLARATION:
            {
            std::vector<std::pair<std::string, std::string>> shadowTypes;
            std::vector<std::pair<std::string, std::string>> newTypes;

            if(this->children[0]->children[2] != NULL){
                for(AST *parameter : this->children[0]->children[2]->children){
                    if(parameter->children[2]->nType == NT_TYPE_NAME){
                        auto it = nameToTypes.find(parameter->value);
                        if(it != nameToTypes.end()){
                            shadowTypes.push_back(std::make_pair(it->first, it->second));
                            nameToTypes.erase(it);
                        }
                        nameToTypes.emplace(parameter->value, parameter->children[2]->value);
                        newTypes.push_back(std::make_pair(parameter->value, parameter->children[2]->value));
                    }
                }
            }

            for(AST *c : this->children)
                if(c != NULL)
                    c->InferTypesRec(nameToTypes, nameToList);

            for(std::pair<std::string, std::string> p : newTypes)
                nameToTypes.erase(nameToTypes.find(p.first));    
            for(std::pair<std::string, std::string> p : shadowTypes)
                nameToTypes.emplace(p);
            }
            break;
        case NT_HEADER_TYPE_DECLARATION:
        case NT_STRUCT_TYPE_DECLARATION:
            {
            std::map<std::string, std::string> newMap;
            if(this->children[2] != NULL){
                for(int i = 0; i < (int) this->children[2]->children.size(); i++){
                    if(this->children[2]->children[i]->children[1] != NULL){
                        if(this->children[2]->children[i]->children[1]->nType == NT_TYPE_NAME)
                            newMap.emplace(this->children[2]->children[i]->value, this->children[2]->children[i]->children[1]->value);
                        this->children[2]->children[i]->value = this->value + "_" + this->children[2]->children[i]->value;
                    }
                }
            }
            nameToList.emplace(this->value, newMap);
            }
            break;
        default:
            for(AST *c : this->children)
                if(c != NULL)
                    c->InferTypesRec(nameToTypes, nameToList);
            break;
    }
}

void AST::SubstituteIdentifierWithType(const std::string& oldID, const std::string& newID, const std::string& type){
    if((this->nType == NT_NONE || this->nType == NT_EXPRESSION || this->nType == NT_ACTION_REF) && this->value == oldID && this->programType == type)
        this->value = newID;
    for(AST *c : this->children)
        if(c != NULL)
            c->SubstituteIdentifierWithType(oldID, newID, type);
}

void AST::SubstituteTypedef(){
    TypedefTable table;
    this->SubstituteTypedefRec(&table);
    this->RemoveTypedefRec(&table);
}

void AST::SubstituteTypedefRec(TypedefTable *table){
    if(this->nType == NT_TYPEDEF_DECLARATION && this->children.size() == 1){
       TypedefTableEntry entry;
       entry.ast = this->children[0]; 
       table->addEntry(this->value, entry);
    }
    else if(this->nType == NT_TYPE_NAME){
        TypedefTableEntry *entry = table->getEntry(this->value);
        if(entry != NULL){
            this->nType = entry->ast->nType;
            this->value = entry->ast->value;
            for(AST *c : this->children)
                delete c;
            this->children.resize(entry->ast->children.size(), NULL);
            for(int i = 0; i < (int) entry->ast->children.size(); i++){
                this->children[i] = entry->ast->children[i]->Clone();
            }    
        }
    }

    for(AST *c : this->children){
        if(c != NULL){
            c->SubstituteTypedefRec(table);
        }
    }
}

void AST::RemoveTypedefRec(TypedefTable *table){
    int i = 0;
    while(i < (int) this->children.size()){
        AST *c = this->children[i];
        if(c != NULL){
            if(c->nType == NT_TYPEDEF_DECLARATION && c->children.size() == 1){
                delete c;
                this->children.erase(this->children.begin() + i);
                i--;
            }
            else{
                c->RemoveTypedefRec(table);
            }
        }
        i++;
    }
}

void AST::writeCode(FILE *fp){
    static const std::unordered_set<std::string> BinaryOps({".", "-", "+", "*", "/", "&", "^", "|", "==", "!=", "&&", "||", "<", ">", "<=", ">="});
    static const std::unordered_set<std::string> BinaryOpsSpace({"-", "+", "*", "/", "&", "^", "|", "==", "!=", "&&", "||", "<", ">", "<=", ">="});

    switch(this->nType){
        case NT_PROGRAM:
            for(AST *c : this->children){
                if(c != NULL){
                    c->writeCode(fp);
                    if(c->nType == NT_PARSER_TYPE_DECLARATION || c->nType == NT_CONTROL_TYPE_DECLARATION){
                        fprintf(fp, ";\n\n");
                    }
                }
            }
            break;
        case NT_EXTERN_DECLARATION:
            if(this->children[0] != NULL){
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "%sextern ", TAB_SPACES.c_str());
            if(this->children.size() == 3){
                fprintf(fp, "%s", this->value.c_str());
                if(this->children[1] != NULL)
                    this->children[1]->writeCode(fp);
                fprintf(fp, " {\n");
                AST::IncreaseTabSpaces();
                if(this->children[2] != NULL)
                    this->children[2]->writeCode(fp);
                AST::DecreaseTabSpaces();
                fprintf(fp, "}\n\n");
            }
            else{
                if(this->children[1] != NULL)
                    this->children[1]->writeCode(fp);
                fprintf(fp, ";\n\n");
            }
            break;
        case NT_METHOD_PROTOTYPE_LIST:
            for(int i = 0; i < (int) this->children.size(); i++){
                this->children[i]->writeCode(fp);
                fprintf(fp, ";\n");
            }
            break;
        case NT_METHOD_PROTOTYPE:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            break;
        case NT_ABSTRACT_FUNCTION_PROTOTYPE:
            fprintf(fp, "abstract ");
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, " %s", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "(");
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, ")");
            break;
        case NT_TYPEARG_DONTCARE:
            fprintf(fp, "_");
            break;
        case NT_FUNCTION_PROTOTYPE:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, " %s", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "(");
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, ")");
            break;
        case NT_CONSTRUCTOR_METHOD_PROTOTYPE:
            fprintf(fp, "%s(", this->str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ")");
            break;
        case NT_TUPLE_TYPE:
            fprintf(fp, "tuple");
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            break;
        case NT_ACTION_DECLARATION:
            if(this->children[0] != NULL){
                fprintf(fp, "%s", TAB_SPACES.c_str());
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "action %s (", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, ") {\n");
            AST::IncreaseTabSpaces();
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "}\n\n");
            break;
        case NT_ENUM_DECLARATION:
            if(this->children[0] != NULL){
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "%senum ", TAB_SPACES.c_str());
            if(this->children[1] != NULL){
                this->children[1]->writeCode(fp);
                fprintf(fp, " ");
            }
            fprintf(fp, "%s {\n", this->str());
            AST::IncreaseTabSpaces();
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        case NT_TABLE_DECLARATION:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->getChildNT(NT_ANNOTATION_LIST) != NULL){
                this->getChildNT(NT_ANNOTATION_LIST)->writeCode(fp);
                fprintf(fp, "\n%s", TAB_SPACES.c_str());
            }
            fprintf(fp, "table %s {\n", this->str());
            AST::IncreaseTabSpaces();
            if(this->getChildNT(NT_TABLE_PROPERTY_LIST) != NULL)
                this->getChildNT(NT_TABLE_PROPERTY_LIST)->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "}\n\n");
            break;
        /*
        case NT_TABLE_PROPERTY_LIST:
            break;
        */
        case NT_TABLE_PROPERTY:
            if(this->children[0] != NULL){
                switch(this->children[0]->nType){
                    case NT_KEY_ELEMENT_LIST:
                    case NT_ACTION_LIST:
                    case NT_ENTRIES_LIST:
                        this->children[0]->writeCode(fp);
                        break;
                    default:
                        fprintf(fp, "%s", TAB_SPACES.c_str());
                        fprintf(fp, "%s = ", this->str());
                        this->children[0]->writeCode(fp);
                        fprintf(fp, ";\n");
                        break;
                }
            }
            break;
        case NT_KEY_ELEMENT_LIST:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "key = {\n");
            AST::IncreaseTabSpaces();
            for(AST *c : this->children)
                if(c != NULL)
                    c->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "}\n\n");
            break;
        case NT_KEY_ELEMENT:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ": %s;\n", this->str());
            break;
        case NT_ENTRIES_LIST:
            fprintf(fp, "%sconst entries = {\n", TAB_SPACES.c_str());
            if(this->children.size() > 0){
                AST::IncreaseTabSpaces();
                for(AST *c : this->children)
                    if(c != NULL)
                        c->writeCode(fp);
                AST::DecreaseTabSpaces();
            }
            fprintf(fp, "%s}\n", TAB_SPACES.c_str());
            break;
        case NT_ENTRY:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ": ");
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            if(this->children[2] != NULL){
                fprintf(fp, " ");
                this->children[2]->writeCode(fp);
            }
            fprintf(fp, ";\n");
            break;
        case NT_ACTION_LIST:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "actions = {\n");
            AST::IncreaseTabSpaces();
            for(AST *c : this->children){
                if(c != NULL){
                    fprintf(fp, "%s", TAB_SPACES.c_str());
                    c->writeCode(fp);
                    fprintf(fp, ";\n");
                }
            }
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "}\n\n");
            break;
        case NT_ACTION_REF:
            fprintf(fp, "%s", this->str());
            if(this->children[0] != NULL){
                fprintf(fp, "(");
                this->children[0]->writeCode(fp);
                fprintf(fp, ")");
            }
            break;
        case NT_CONSTANT_DECLARATION:
            if(this->children[0] != NULL){
                fprintf(fp, "%s", TAB_SPACES.c_str());
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "%sconst ", TAB_SPACES.c_str());
            this->children[1]->writeCode(fp);
            fprintf(fp, " %s = ", this->str());
            this->children[2]->writeCode(fp);
            fprintf(fp, ";\n\n");
            break;
        case NT_VARIABLE_DECLARATION:
            if(this->children[0] != NULL){
                fprintf(fp, "%s", TAB_SPACES.c_str());
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "%s", TAB_SPACES.c_str());
            this->children[1]->writeCode(fp);
            fprintf(fp, " %s", this->str());
            if(this->children[2] != NULL){
                fprintf(fp, " = ");
                this->children[2]->writeCode(fp);
            }
            fprintf(fp, ";\n\n");
            break;
        case NT_BASE_TYPE:
            fprintf(fp, "%s", this->str());
            if(this->children.size() == 1){
                if(this->children[0]->nType == NT_LITERAL){
                    fprintf(fp, "<");
                    this->children[0]->writeCode(fp);
                    fprintf(fp, ">");
                }
            }
            break;
        case NT_EXPRESSION:
            if(this->children.size() == 0){
                fprintf(fp, "%s", this->str());
            }
            else if(this->children.size() == 1){
                fprintf(fp, "%s(", this->str());
                if(this->children[0] != NULL)
                    this->children[0]->writeCode(fp);
                fprintf(fp, ")");
            }
            else if(this->children.size() == 2){
                if(BinaryOps.find(this->value) != BinaryOps.end()){
                    if(this->children[0] != NULL){
                        if(this->value != "." && this->children[0]->children.size() > 0)
                            fprintf(fp, "(");
                        this->children[0]->writeCode(fp);
                        if(this->value != "." && this->children[0]->children.size() > 0)
                            fprintf(fp, ")");
                    }
                    if(BinaryOpsSpace.find(this->value) != BinaryOpsSpace.end())
                        fprintf(fp, " ");
                    fprintf(fp, "%s", this->str());
                    if(BinaryOpsSpace.find(this->value) != BinaryOpsSpace.end())
                        fprintf(fp, " ");
                    if(this->children[1] != NULL){
                        if(this->value != "." && this->children[1]->children.size() > 0)
                            fprintf(fp, "(");
                        this->children[1]->writeCode(fp);
                        if(this->value != "." && this->children[1]->children.size() > 0)
                            fprintf(fp, ")");
                    }
                }
                else if(this->children[1]->nType == NT_TYPE_ARGUMENT_LIST){
                    fprintf(fp, "%s<", this->str());
                    this->children[1]->writeCode(fp);
                    fprintf(fp, ">(");
                    if(this->children[2] != NULL)
                        this->children[2]->writeCode(fp);
                    fprintf(fp, ")");
                }
            }
            else if(this->children.size() == 3 && this->value == "."){
                if(this->children[0] != NULL)
                    this->children[0]->writeCode(fp);
                fprintf(fp, ".");
                if(this->children[1] != NULL)
                    this->children[1]->writeCode(fp);
                fprintf(fp, "(");
                if(this->children[2] != NULL)
                    this->children[2]->writeCode(fp);
                fprintf(fp, ")");
            }
            else if(this->children.size() == 4 && this->value == "."){
                if(this->children[0] != NULL)
                    this->children[0]->writeCode(fp);
                fprintf(fp, ".");
                if(this->children[1] != NULL)
                    this->children[1]->writeCode(fp);
                if(this->children[2] != NULL)
                    this->children[2]->writeCode(fp);
                fprintf(fp, "(");
                if(this->children[3] != NULL)
                    this->children[3]->writeCode(fp);
                fprintf(fp, ")");
            }
            break;
        case NT_LITERAL:
            fprintf(fp, "%s", this->str());
            break;
        case NT_HEADER_TYPE_DECLARATION:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, "header %s ", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "{\n");
            AST::IncreaseTabSpaces();
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        case NT_HEADER_UNION_DECLARATION:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, "header union %s ", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "{\n");
            AST::IncreaseTabSpaces();
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        case NT_ERROR_DECLARATION:
            fprintf(fp, "error {\n");
            AST::IncreaseTabSpaces();
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        case NT_STRUCT_TYPE_DECLARATION:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, "struct %s ", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "{\n");
            AST::IncreaseTabSpaces();
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        /*
        case NT_STRUCT_FIELD_LIST:
            break;
        */
        case NT_STRUCT_FIELD:
            if(this->children[0] != NULL){
                fprintf(fp, "%s", TAB_SPACES.c_str());
                this->children[0]->writeCode(fp);
            }
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, " %s;\n", this->str());
            break;
        case NT_CONTROL_DECLARATION:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            if(this->children[1] != NULL){
                fprintf(fp, "(");
                this->children[1]->writeCode(fp);
                fprintf(fp, ")");
            }
            fprintf(fp, " {\n\n");
            AST::IncreaseTabSpaces();
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, "%sapply {\n", TAB_SPACES.c_str());
            AST::IncreaseTabSpaces();
            if(this->children[3] != NULL)
                this->children[3]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s}\n\n", TAB_SPACES.c_str());
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        case NT_CONTROL_TYPE_DECLARATION:
            if(this->children[0] != NULL){
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "control %s", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "(");
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, ")");
            break;
        /*
        case NT_ANNOTATION_LIST:
            break;
        */
        /*
        case NT_ANNOTATION_TOKEN_LIST:
            break;
        */
        case NT_ANNOTATION:
            fprintf(fp, "@%s", this->str());
            if(this->children[0] != NULL){
                fprintf(fp, "(");
                this->children[0]->writeCode(fp);
                fprintf(fp, ")");
            }
            fprintf(fp, " ");
            break;
        /*
        case NT_CONTROL_LOCAL_DEC_LIST:
            break;
        */
        /*
        case NT_STAT_OR_DEC_LIST:
            break;
        */
        case NT_FUNCTION_DECLARATION:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, " {\n");
            AST::IncreaseTabSpaces();
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        case NT_RETURN_STATEMENT:
            fprintf(fp, "%sreturn ", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ";\n");
            break;
        case NT_CONDITIONAL_STATEMENT:
            fprintf(fp, "%sif (", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ") {\n");
            AST::IncreaseTabSpaces();
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s}\n", TAB_SPACES.c_str());
            if(this->children[2] != NULL){
                fprintf(fp, "%selse {\n", TAB_SPACES.c_str());
                AST::IncreaseTabSpaces();
                this->children[2]->writeCode(fp);
                AST::DecreaseTabSpaces();
                fprintf(fp, "%s}\n", TAB_SPACES.c_str());
            }
            break;
        case NT_SWITCH_STATEMENT:
            fprintf(fp, "%sswitch (", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ") {\n");
            AST::IncreaseTabSpaces();
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s}\n", TAB_SPACES.c_str());
            break;
        case NT_SWITCH_CASE:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ":");
            if(this->children[1] != NULL){
                fprintf(fp, " {\n");
                AST::IncreaseTabSpaces();
                this->children[1]->writeCode(fp);
                AST::DecreaseTabSpaces();
                fprintf(fp, "%s}", TAB_SPACES.c_str());
            }
            fprintf(fp, "\n");
            break;
        case NT_ASSIGN_STATEMENT:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, " = ");
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, ";\n");
            break;
        case NT_MATCH_KIND_DECLARATION:
            fprintf(fp, "%smatch_kind {\n", TAB_SPACES.c_str());
            AST::IncreaseTabSpaces();
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s}\n\n", TAB_SPACES.c_str());
            break;
        case NT_PARSER_DECLARATION:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, " {\n\n");
            AST::IncreaseTabSpaces();
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "}\n\n");
            break;
        case NT_PARSER_TYPE_DECLARATION:
            if(this->children[0] != NULL){
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "parser %s", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "(");
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, ")");
            break;
        case NT_PACKAGE_TYPE_DECLARATION:
            if(this->children[0] != NULL){
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "package %s", this->str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "(");
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, ");\n\n");
            break;
        /*
        case NT_PARSER_LOCAL_ELEMENT_LIST:
            break;
        */
        /*
        case NT_PARSER_STATE_LIST:
            break;
        */
        case NT_PARSER_STATE:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "state %s {\n", this->str());
            AST::IncreaseTabSpaces();
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "transition ");
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s", TAB_SPACES.c_str());
            fprintf(fp, "}\n\n");
            break;
        case NT_NAME_STATE_EXPRESSION:
            fprintf(fp, "%s;\n", this->str());
            break;
        case NT_SELECT_EXPRESSION:
            fprintf(fp, "%s(", this->str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ") {\n");
            AST::IncreaseTabSpaces();
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            AST::DecreaseTabSpaces();
            fprintf(fp, "%s}\n", TAB_SPACES.c_str());
            break;
        case NT_TUPLE_KEYSET_EXPRESSION:
            fprintf(fp, "(");
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ")");
            break;
        case NT_SIMPLE_EXPRESSION_LIST:
            for(int i = 0; i < (int) this->children.size(); i++){
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ", ");
            }
            break;
        /*
        case NT_SELECT_CASE_LIST:
            break;
        */
        case NT_SELECT_CASE:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, ": %s;\n", this->str());
            break;
        case NT_MASK:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, " &&& ");
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            break;
        case NT_RANGE:
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, " .. ");
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            break;
        case NT_SIMPLE_KEYSET_DEFAULT:
            fprintf(fp, "default");
            break;
        case NT_SIMPLE_KEYSET_DONTCARE:
            fprintf(fp, "_");
            break;
        /*
        case NT_PARSER_STATEMENT_LIST:
            break;
        */
        case NT_SPECIFIED_IDENTIFIER_LIST:
            for(int i = 0; i < (int) this->children.size(); i++){
                fprintf(fp, "%s", TAB_SPACES.c_str());
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ",");
                fprintf(fp, "\n");
            }
            break;
        case NT_SPECIFIED_IDENTIFIER:
            fprintf(fp, "%s", this->str());
            if(this->children[0] != NULL){
                fprintf(fp, " = ");
                this->children[0]->writeCode(fp);
            }
            break;
        case NT_METHOD_CALL_STATEMENT:
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            fprintf(fp, "(");
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, ");\n");
            break;
        case NT_NONE:
            if(this->value == "." && this->children.size() == 2){
                if(this->children[0] != NULL)
                    this->children[0]->writeCode(fp);
                fprintf(fp, ".");
                if(this->children[1] != NULL)
                    this->children[1]->writeCode(fp);
            }
            else{
                fprintf(fp, "%s", this->str());
            }
            break;
        case NT_ARGUMENT_LIST:
            for(int i = 0; i < (int) this->children.size(); i++){
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ", ");
            }
            break;
        case NT_IDENTIFIER_LIST:
            for(int i = 0; i < (int) this->children.size(); i++){
                fprintf(fp, "%s", TAB_SPACES.c_str());
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ",");
                fprintf(fp, "\n");
            }
            break;
        case NT_ARGUMENT:
            fprintf(fp, "%s", this->str());
            if(this->children[0] != NULL){
                if(this->value != "")
                    fprintf(fp, " = ");

                if(this->children[0]->nType == NT_EXPRESSION_LIST)
                    fprintf(fp, "{");
                this->children[0]->writeCode(fp);
                if(this->children[0]->nType == NT_EXPRESSION_LIST)
                    fprintf(fp, "}");
            }
            break;
        case NT_INSTANTIATION:
            if(this->children[0] != NULL){
                fprintf(fp, "%s", TAB_SPACES.c_str());
                this->children[0]->writeCode(fp);
                fprintf(fp, "\n");
            }
            fprintf(fp, "%s", TAB_SPACES.c_str());
            if(this->children[1] != NULL)
                this->children[1]->writeCode(fp);
            fprintf(fp, "(");
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, ") %s;\n", this->str());
            break;
        case NT_PARAMETER_LIST:
            for(int i = 0; i < (int) this->children.size(); i++){
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ", ");
            }
            break;
        case NT_EXPRESSION_LIST:
            for(int i = 0; i < (int) this->children.size(); i++){
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ", ");
            }
            break;
        case NT_PARAMETER:
            if(this->children[0] != NULL){
                this->children[0]->writeCode(fp);
            }
            if(this->children[1] != NULL){
                this->children[1]->writeCode(fp);
                fprintf(fp, " ");
            }
            if(this->children[2] != NULL)
                this->children[2]->writeCode(fp);
            fprintf(fp, " %s", this->str());
            break;
        case NT_TYPE_PARAMETER:
        case NT_TYPEARG_NONTYPENAME:
        case NT_DIRECTION:
        case NT_ANNOTATION_TOKEN:
        case NT_TYPE_OR_VOID:
            fprintf(fp, "%s", this->str());
            break;
        case NT_TYPE_NAME:
            fprintf(fp, "%s", this->str());
            if(this->children.size() > 0 && this->children[0] != NULL)
                this->children[0]->writeCode(fp);
            break;
        case NT_TYPE_PARAMETER_LIST:
            fprintf(fp, "<");
            for(int i = 0; i < (int) this->children.size(); i++){
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ", ");
            }
            fprintf(fp, ">");
            break;
        case NT_TYPE_ARGUMENT_LIST:
            fprintf(fp, "<");
            for(int i = 0; i < (int) this->children.size(); i++){
                this->children[i]->writeCode(fp);
                if(i != (int) this->children.size() - 1)
                    fprintf(fp, ", ");
            }
            fprintf(fp, ">");
            break;
        default:
            for(AST *c : this->children)
                if(c != NULL)
                    c->writeCode(fp);
            break;
    }
}

void AST::SubstituteTypeName(const std::string& oldName, const std::string& newName){
    if(this->value == oldName && (this->nType == NT_TYPE_NAME || this->nType == NT_EXPRESSION))
        this->value = newName;
    if(this->programType == oldName)
        this->programType = newName;
    for(AST *c : this->children)
        if(c != NULL)
            c->SubstituteTypeName(oldName, newName);
}

void AST::IncreaseTabSpaces(){
    for(int i = 0; i < 4; i++)
        AST::TAB_SPACES.push_back(' ');
}

void AST::DecreaseTabSpaces(){
    for(int i = 0; i < 4; i++)
        AST::TAB_SPACES.pop_back();
}