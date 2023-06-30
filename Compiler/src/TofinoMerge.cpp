#include "../include/pcheaders.h"
#include "../include/AST.h"
#include "../include/TofinoMerge.h"
#include "../include/Log.h"
#include "../include/CFG.h"

#include "../include/IPLBase.hpp"

#define PROGRAM_ID_TYPE_BITS_STR "8"
#define METADATA_NAME "VirtMetadata"
#define METADATA_TYPE_NAME "VirtMetadataType"
#define METADATA_PROG_ID_FIELD_NAME "VirtProgramID"
#define SINGLE_PIPELINE_NAME "VirtPipe"

#define PI_PARAM_TYPE "packet_in"
#define PI_PARAM_NAME "pkt"
#define PO_PARAM_TYPE "packet_out"
#define PO_PARAM_NAME "pkt"
#define IG_INTRINSIC_MD_PARAM_TYPE "ingress_intrinsic_metadata_t"
#define IG_INTRINSIC_MD_PARAM_NAME "ig_intr_md"
#define IG_INTRINSIC_MD_PARSER_PARAM_TYPE "ingress_intrinsic_metadata_from_parser_t"
#define IG_INTRINSIC_MD_PARSER_PARAM_NAME "ig_intr_prsr_md"
#define IG_INTRINSIC_MD_DEPARSER_PARAM_TYPE "ingress_intrinsic_metadata_for_deparser_t"
#define IG_INTRINSIC_MD_DEPARSER_PARAM_NAME "ig_intr_dprsr_md"
#define IG_INTRINSIC_MD_TM_PARAM_TYPE "ingress_intrinsic_metadata_for_tm_t"
#define IG_INTRINSIC_MD_TM_PARAM_NAME "ig_intr_tm_md"
#define EG_INTRINSIC_MD_PARAM_TYPE "egress_intrinsic_metadata_t"
#define EG_INTRINSIC_MD_PARAM_NAME "eg_intr_md"
#define EG_INTRINSIC_MD_PARSER_PARAM_TYPE "egress_intrinsic_metadata_from_parser_t"
#define EG_INTRINSIC_MD_PARSER_PARAM_NAME "eg_intr_from_prsr"
#define EG_INTRINSIC_MD_DEPARSER_PARAM_TYPE "egress_intrinsic_metadata_for_deparser_t"
#define EG_INTRINSIC_MD_DEPARSER_PARAM_NAME "eg_intr_md_for_dprsr"
#define EG_INTRINSIC_MD_OP_PARAM_TYPE "egress_intrinsic_metadata_for_output_port_t"
#define EG_INTRINSIC_MD_OP_PARAM_NAME "eg_intr_md_for_oport"

void RenameVirtMetadataVariables(AST *ast){
    if(ast->getProgramType() == METADATA_TYPE_NAME)
        ast->setValue(METADATA_NAME);
    for(AST *c : ast->getChildren())
        if(c != NULL)
            RenameVirtMetadataVariables(c);
}

void RemoveTempProgramCode(AST *ast){
    if(ast->getNodeType() == NT_EXPRESSION && ast->getValue() == TEMP_PROGRAM_CODE){
        ast->setValue(".");
        ast->addChild(new AST(METADATA_NAME, NT_EXPRESSION));
        ast->addChild(new AST(METADATA_PROG_ID_FIELD_NAME, NT_NONE));
        ast->getChildren()[1]->setProgramType(METADATA_TYPE_NAME);
    }
    for(AST *c : ast->getChildren())
        if(c != NULL)
            RemoveTempProgramCode(c);
}

bool TofinoMerge::MergeTofino(const std::vector<AST*> programs, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts){
    AST* LastProg = programs[programs.size() - 1];

    std::string msg = "Merging Tofino Programs:";
    for(AST *program : programs)
        msg = msg + " \"" + program->value + "\"";
    Log::MSG(msg);
    Log::Push();

    std::vector<std::vector<AST*>> Pipelines(programs.size());

    int nPipes = -1;
    for(int i = 0; i < (int) programs.size(); i++){
        if(!GetPipelines(programs[i], &Pipelines[i])){
            Log::Pop();
            return false;
        }
        if(nPipes == -1)
            nPipes = (int) Pipelines[i].size();
        else if(nPipes != (int) Pipelines[i].size()){
            fprintf(stderr, "TofinoMerge: Different number of Switch Pipelines\n");
            Log::Pop();
            return false;
        }
        
        TofinoMerge::RemoveSwitchInstantiation(programs[i]);
    }
    
    for(int i = 0; i < (int) Pipelines[0].size(); i++){
        std::vector<AST*> pipelinesToMerge;
        for(int j = 0; j < (int) programs.size(); j++)
            pipelinesToMerge.push_back(Pipelines[j][i]);
        bool result = TofinoMerge::MergePipelines(programs, pipelinesToMerge, headersDec, virtualSwitchPorts);
        if(!result){
            Log::Pop();
            return false;
        }
    }

    //Construct new Switch instantiation
    Assert(Pipelines[0].size() == 1);
    AST* newSwitch = new AST("main", NT_INSTANTIATION);
    newSwitch->addChild(NULL); //annotations
    newSwitch->addChild(new AST("Switch", NT_TYPE_NAME));
    newSwitch->addChild(new AST(NULL, NT_ARGUMENT_LIST));
    newSwitch->getChildNT(NT_ARGUMENT_LIST)->addChild(new AST(NULL, NT_ARGUMENT));
    newSwitch->getChildNT(NT_ARGUMENT_LIST)->children[0]->addChild(new AST(SINGLE_PIPELINE_NAME, NT_EXPRESSION));
    newSwitch->getChildNT(NT_ARGUMENT_LIST)->children[0]->addChild(NULL);
    LastProg->addChild(newSwitch);

    //Change Switch Local Metadata parameters to METADATA_NAME
    const std::unordered_map<std::string, int> VirtPipelineStructNames = {{"VirtIngressParser", 2}, {"VirtEgressParser", 2}, {"VirtIngress", 1}, {"VirtEgress", 1}, {"VirtIngressDeparser", 2}, {"VirtEgressDeparser", 2}};
    for(int i = 0; i < (int) LastProg->children.size(); i++){
        if(LastProg->children[i]->nType == NT_PARSER_DECLARATION && VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value) != VirtPipelineStructNames.end()){
            const std::string vMetadataHeaderName = LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value)->second]->getChildNT(NT_TYPE_NAME)->value;
            LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value)->second]->value = METADATA_NAME;
            int pp = 0;
            while(pp < (int) LastProg->children.size() && ((LastProg->children[pp]->nType != NT_HEADER_TYPE_DECLARATION && LastProg->children[pp]->nType != NT_STRUCT_TYPE_DECLARATION) || LastProg->children[pp]->value != vMetadataHeaderName))
                pp++;
            Assert(pp < (int) LastProg->children.size());
            AST *newField = new AST(METADATA_PROG_ID_FIELD_NAME, NT_STRUCT_FIELD);
            newField->addChild(NULL); //optAnnotations
            newField->addChild(new AST("bit", NT_BASE_TYPE));
            newField->children[1]->addChild(new AST("8", NT_LITERAL));
            LastProg->children[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->addChild(newField);
            LastProg->SubstituteTypeName(LastProg->children[pp]->value, METADATA_TYPE_NAME);
            LastProg->children[pp]->value = METADATA_TYPE_NAME;
        }
        else if(LastProg->children[i]->nType == NT_CONTROL_DECLARATION && VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value) != VirtPipelineStructNames.end()){
            const std::string vMetadataHeaderName = LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value)->second]->getChildNT(NT_TYPE_NAME)->value;
            LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value)->second]->value = METADATA_NAME;
            int pp = 0;
            while(pp < (int) LastProg->children.size() && ((LastProg->children[pp]->nType != NT_HEADER_TYPE_DECLARATION && LastProg->children[pp]->nType != NT_STRUCT_TYPE_DECLARATION) || LastProg->children[pp]->value != vMetadataHeaderName))
                pp++;
            Assert(pp < (int) LastProg->children.size());
            AST *newField = new AST(METADATA_PROG_ID_FIELD_NAME, NT_STRUCT_FIELD);
            newField->addChild(NULL); //optAnnotations
            newField->addChild(new AST("bit", NT_BASE_TYPE));
            newField->children[1]->addChild(new AST("8", NT_LITERAL));
            LastProg->children[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->addChild(newField);
            LastProg->SubstituteTypeName(LastProg->children[pp]->value, METADATA_TYPE_NAME);
            LastProg->children[pp]->value = METADATA_TYPE_NAME;
        }
    }
    RenameVirtMetadataVariables(LastProg);

    //Change generic TEMP_PROGRAM_CODE to METADATA_NAME.METADATA_PROG_ID_FIELD_NAME
    RemoveTempProgramCode(LastProg);

    Log::Pop();
    return true;
}

bool TofinoMerge::MergePipelines(const std::vector<AST*>& programs, const std::vector<AST*>& pipelines, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts){
    const static std::unordered_map<std::string, std::string> ParamNames = {
        {PI_PARAM_TYPE, PI_PARAM_NAME},
        {PO_PARAM_TYPE, PO_PARAM_NAME},
        {IG_INTRINSIC_MD_PARAM_TYPE, IG_INTRINSIC_MD_PARAM_NAME},
        {IG_INTRINSIC_MD_PARSER_PARAM_TYPE, IG_INTRINSIC_MD_PARSER_PARAM_NAME},
        {IG_INTRINSIC_MD_DEPARSER_PARAM_TYPE, IG_INTRINSIC_MD_DEPARSER_PARAM_NAME},
        {IG_INTRINSIC_MD_TM_PARAM_TYPE, IG_INTRINSIC_MD_TM_PARAM_NAME},
        {EG_INTRINSIC_MD_PARAM_TYPE, EG_INTRINSIC_MD_PARAM_NAME},
        {EG_INTRINSIC_MD_PARSER_PARAM_TYPE, EG_INTRINSIC_MD_PARSER_PARAM_NAME},
        {EG_INTRINSIC_MD_DEPARSER_PARAM_TYPE, EG_INTRINSIC_MD_DEPARSER_PARAM_NAME},
        {EG_INTRINSIC_MD_OP_PARAM_TYPE, EG_INTRINSIC_MD_OP_PARAM_NAME},
    };

    std::vector<AST*> nodesToAdd;
    
    //Parsers
    const std::vector<int> parserParams = {0, 3};
    const std::vector<std::string> parserNames = {"VirtIngressParser", "VirtEgressParser"};
    int pCounter = 0;
    for(int parserParam : parserParams){
        std::vector<AST*> elements;
        std::vector<std::string> elementNames;
        for(AST* pipeline : pipelines)
            elementNames.push_back(pipeline->getChildNT(NT_ARGUMENT_LIST)->children[parserParam]->children[0]->value);

        for(int p = 0; p < (int) programs.size(); p++){
            int i = 0;
            while(i < (int) programs[p]->children.size() && (programs[p]->children[i]->nType != NT_PARSER_DECLARATION || programs[p]->children[i]->children[0]->value != elementNames[p]))
                i++;
            if(i == (int) programs[p]->children.size()){
                fprintf(stderr, "TofinoMerge: Could not find \"%s\" Parser declaration\n", elementNames[p].c_str());
                return false;
            }
            elements.push_back(programs[p]->children[i]);
        }

        AST *newExpression = new AST(".", NT_EXPRESSION);
        AST *newExpressionL = new AST(IG_INTRINSIC_MD_PARSER_PARAM_NAME, NT_EXPRESSION);
        AST *newExpressionR = new AST("ingress_port", NT_NONE);
        newExpression->addChild(newExpressionL);
        newExpression->addChild(newExpressionR);
        std::vector<AST*> newNodes = AST::MergeParsers(programs, elements, headersDec, parserNames[pCounter], virtualSwitchPorts, newExpression);
        delete newExpression;
        if(newNodes.size() == 0)
            return false;

        for(AST *param : newNodes[newNodes.size() - 1]->children[0]->children[2]->children){
            auto it = ParamNames.find(param->children[2]->value);
            if(it != ParamNames.end()){
                if(newNodes[newNodes.size() - 1]->children[1] != NULL)
                    newNodes[newNodes.size() - 1]->children[1]->SubstituteIdentifierWithType(param->value, it->second, it->first);
                if(newNodes[newNodes.size() - 1]->children[2] != NULL)
                    newNodes[newNodes.size() - 1]->children[2]->SubstituteIdentifierWithType(param->value, it->second, it->first);
                param->value = it->second;
            }
        }

        for(int p = 0; p < (int) programs.size(); p++){
            int i = 0;
            while(i < (int) programs[p]->children.size()){
                if(programs[p]->children[i] == elements[p]){
                    programs[p]->children.erase(programs[p]->children.begin() + i);
                    i--;
                }
                i++;
            }
            delete elements[p];
        }

        pCounter++;

        for(AST* n: newNodes)
            nodesToAdd.push_back(n);
    }

    //Controls
    const std::vector<int> controlParams = {1, 2, 4, 5};
    const std::vector<std::string> controlNames = {"VirtIngress", "VirtIngressDeparser", "VirtEgress", "VirtEgressDeparser"};
    int controlPipeN = 0;
    for(int controlP : controlParams){
        std::vector<AST*> elements;
        std::vector<std::string> elementNames;
        for(AST* pipeline : pipelines)
            elementNames.push_back(pipeline->getChildNT(NT_ARGUMENT_LIST)->children[controlP]->children[0]->value);

        for(int p = 0; p < (int) programs.size(); p++){
            int i = 0;
            while(i < (int) programs[p]->children.size() && (programs[p]->children[i]->nType != NT_CONTROL_DECLARATION || programs[p]->children[i]->children[0]->value != elementNames[p]))
                i++;
            if(i == (int) programs[p]->children.size()){
                fprintf(stderr, "TofinoMerge: Could not find \"%s\" Control declaration\n", elementNames[p].c_str());
                return false;
            }
            elements.push_back(programs[p]->children[i]);
        }

        std::vector<AST*> newNodes = AST::MergeControls(programs, elements, headersDec, controlNames[controlPipeN], virtualSwitchPorts);
        if(newNodes.size() == 0)
            return false;

        AST* mergedControl = newNodes[newNodes.size() - 1];
        for(AST *param : mergedControl->children[0]->children[2]->children){
            auto it = ParamNames.find(param->children[2]->value);
            if(it != ParamNames.end()){
                if(mergedControl->getChildNT(NT_CONTROL_LOCAL_DEC_LIST) != NULL)
                    mergedControl->getChildNT(NT_CONTROL_LOCAL_DEC_LIST)->SubstituteIdentifierWithType(param->value, it->second, it->first);
                if(mergedControl->getChildNT(NT_STAT_OR_DEC_LIST) != NULL)
                    mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->SubstituteIdentifierWithType(param->value, it->second, it->first);
                param->value = it->second;
            }
        }

        for(int p = 0; p < (int) programs.size(); p++){
            int i = 0;
            while(i < (int) programs[p]->children.size()){
                if(programs[p]->children[i] == elements[p]){
                    programs[p]->children.erase(programs[p]->children.begin() + i);
                    i--;
                }
                i++;
            }
            delete elements[p];
        }

        //Add metadata programID table on the ingress pipeline
        if(controlPipeN == 0){
            if(mergedControl->children[2] == NULL)
                mergedControl->children[2] = new AST(NULL, NT_CONTROL_LOCAL_DEC_LIST);
            if(mergedControl->children[3] == NULL)
                mergedControl->children[3] = new AST(NULL, NT_STAT_OR_DEC_LIST);
            AST *controlLDs = mergedControl->children[2];
            AST *controlBody = mergedControl->children[3];

            AST *actionSetProgramID = new AST("VirtSetProgramID", NT_ACTION_DECLARATION);
            controlLDs->addChild(actionSetProgramID);
            actionSetProgramID->addChild(NULL); //optAnnotations
            actionSetProgramID->addChild(new AST(NULL, NT_PARAMETER_LIST));
            actionSetProgramID->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
            AST *parameter = new AST("programID", NT_PARAMETER);
            actionSetProgramID->children[1]->addChild(parameter);
            parameter->addChild(NULL); //optAnnotations
            parameter->addChild(NULL); //direction
            parameter->addChild(new AST("bit", NT_BASE_TYPE));
            parameter->children[2]->addChild(new AST(PROGRAM_ID_TYPE_BITS_STR, NT_LITERAL));
            AST *assignStatement = new AST("=", NT_ASSIGN_STATEMENT);
            actionSetProgramID->children[2]->addChild(assignStatement);
            assignStatement->addChild(new AST(".", NT_NONE));
            assignStatement->children[0]->addChild(new AST(METADATA_NAME, NT_NONE));
            assignStatement->children[0]->addChild(new AST(METADATA_PROG_ID_FIELD_NAME, NT_NONE));
            assignStatement->addChild(new AST("programID", NT_EXPRESSION));

            AST *actionDropUnmappedPort = new AST("VirtDropUnmappedPort", NT_ACTION_DECLARATION);
            controlLDs->addChild(actionDropUnmappedPort);
            actionDropUnmappedPort->addChild(NULL); //optAnnotations
            actionDropUnmappedPort->addChild(NULL); //parameterList
            actionDropUnmappedPort->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
            assignStatement = assignStatement->Clone();
            actionDropUnmappedPort->children[2]->addChild(assignStatement);
            assignStatement->children[0]->children[0]->value = IG_INTRINSIC_MD_DEPARSER_PARAM_NAME;
            assignStatement->children[0]->children[1]->value = "drop_ctl";
            assignStatement->children[1]->value = "1";

            AST *tableProgramID = new AST("VirtTableSetProgramID", NT_TABLE_DECLARATION);
            controlLDs->addChild(tableProgramID);
            tableProgramID->addChild(NULL); //optAnnotations
            AST *tablePL = new AST(NULL, NT_TABLE_PROPERTY_LIST);
            tableProgramID->addChild(tablePL);
            tablePL->addChild(new AST("key", NT_TABLE_PROPERTY));
            AST *key = new AST("exact", NT_KEY_ELEMENT);
            key->addChild(new AST(".", NT_EXPRESSION));
            key->children[0]->addChild(new AST(IG_INTRINSIC_MD_PARAM_NAME, NT_EXPRESSION));
            key->children[0]->addChild(new AST("ingress_port", NT_NONE));
            tablePL->children[0]->addChild(new AST(NULL, NT_KEY_ELEMENT_LIST));
            tablePL->children[0]->children[0]->addChild(key);
            tablePL->addChild(new AST("actions", NT_TABLE_PROPERTY));
            tablePL->children[1]->addChild(new AST(NULL, NT_ACTION_LIST));
            tablePL->children[1]->children[0]->addChild(new AST(actionSetProgramID->value, NT_ACTION_REF));
            tablePL->children[1]->children[0]->addChild(new AST(actionDropUnmappedPort->value, NT_ACTION_REF));
            tablePL->children[1]->children[0]->children[0]->addChild(NULL);
            tablePL->children[1]->children[0]->children[1]->addChild(NULL);

            tablePL->addChild(new AST("const default_action", NT_TABLE_PROPERTY));
            tablePL->children[2]->addChild(new AST(actionDropUnmappedPort->value, NT_EXPRESSION));

            char buffer[128];
            tablePL->addChild(new AST("const entries", NT_TABLE_PROPERTY));
            tablePL->children[3]->addChild(new AST(NULL, NT_ENTRIES_LIST));
            int tSize = 0;
            for(int vsN = 0; vsN < (int) virtualSwitchPorts.size(); vsN++){
                tSize = tSize + ((int) virtualSwitchPorts[vsN].size());
                for(int port : virtualSwitchPorts[vsN]){
                    AST *entry = new AST(NULL, NT_ENTRY);
                    tablePL->children[3]->children[0]->addChild(entry);
                    sprintf(buffer, "%d", port);
                    entry->addChild(new AST(buffer, NT_EXPRESSION));
                    entry->addChild(new AST(actionSetProgramID->value, NT_ACTION_REF));
                    sprintf(buffer, "%d", vsN + 1);
                    entry->children[1]->addChild(new AST(buffer, NT_EXPRESSION));
                    entry->addChild(NULL); //optAnnotations
                }
            }

            sprintf(buffer, "%d", tSize);
            tablePL->addChild(new AST("size", NT_TABLE_PROPERTY));
            tablePL->children[4]->addChild(new AST(buffer, NT_LITERAL));

            AST* tableApplyStatement = new AST(NULL, NT_METHOD_CALL_STATEMENT);
            controlBody->addChildStart(tableApplyStatement);
            tableApplyStatement->addChild(new AST(".", NT_NONE));
            tableApplyStatement->children[0]->addChild(new AST("VirtTableSetProgramID", NT_NONE));
            tableApplyStatement->children[0]->addChild(new AST("apply", NT_NONE));
            tableApplyStatement->addChild(NULL);
        }
        //Add Egress Port-Check
        else if(controlPipeN == 2){
            if(mergedControl->children[2] == NULL)
                mergedControl->children[2] = new AST(NULL, NT_CONTROL_LOCAL_DEC_LIST);
            if(mergedControl->children[3] == NULL)
                mergedControl->children[3] = new AST(NULL, NT_STAT_OR_DEC_LIST);
            AST *controlLDs = mergedControl->children[2];

            AST *actionOK= new AST("VirtOutPortOK", NT_ACTION_DECLARATION);
            controlLDs->addChild(actionOK);
            actionOK->addChild(NULL); //optAnnotations
            actionOK->addChild(NULL); //parameterList
            actionOK->addChild(NULL);

            AST *actionDropUnmappedPort = new AST("VirtOutPortWrong", NT_ACTION_DECLARATION);
            controlLDs->addChild(actionDropUnmappedPort);
            actionDropUnmappedPort->addChild(NULL); //optAnnotations
            actionDropUnmappedPort->addChild(NULL); //parameterList
            actionDropUnmappedPort->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
            AST *assignStatement = new AST("=", NT_ASSIGN_STATEMENT);
            assignStatement->addChild(new AST(".", NT_NONE));
            assignStatement->children[0]->addChild(new AST(EG_INTRINSIC_MD_DEPARSER_PARAM_NAME, NT_NONE));
            assignStatement->children[0]->addChild(new AST("drop_ctl", NT_NONE));
            assignStatement->addChild(new AST("1", NT_EXPRESSION));
            actionDropUnmappedPort->children[2]->addChild(assignStatement);

            AST *tableProgramID = new AST("VirtTableCheckProgramID", NT_TABLE_DECLARATION);
            controlLDs->addChild(tableProgramID);
            tableProgramID->addChild(NULL); //optAnnotations
            AST *tablePL = new AST(NULL, NT_TABLE_PROPERTY_LIST);
            tableProgramID->addChild(tablePL);
            tablePL->addChild(new AST("key", NT_TABLE_PROPERTY));
            AST *key = new AST("exact", NT_KEY_ELEMENT);
            key->addChild(new AST(".", NT_EXPRESSION));
            key->children[0]->addChild(new AST(EG_INTRINSIC_MD_PARAM_NAME, NT_EXPRESSION));
            key->children[0]->addChild(new AST("egress_port", NT_NONE));
            AST *key2 = new AST("exact", NT_KEY_ELEMENT);
            key2->addChild(new AST(".", NT_EXPRESSION));
            key2->children[0]->addChild(new AST(METADATA_NAME, NT_EXPRESSION));
            key2->children[0]->addChild(new AST(METADATA_PROG_ID_FIELD_NAME, NT_NONE));
            tablePL->children[0]->addChild(new AST(NULL, NT_KEY_ELEMENT_LIST));
            tablePL->children[0]->children[0]->addChild(key);
            tablePL->children[0]->children[0]->addChildStart(key2);
            tablePL->addChild(new AST("actions", NT_TABLE_PROPERTY));
            tablePL->children[1]->addChild(new AST(NULL, NT_ACTION_LIST));
            tablePL->children[1]->children[0]->addChild(new AST(actionOK->value, NT_ACTION_REF));
            tablePL->children[1]->children[0]->addChild(new AST(actionDropUnmappedPort->value, NT_ACTION_REF));
            tablePL->children[1]->children[0]->children[0]->addChild(NULL);
            tablePL->children[1]->children[0]->children[1]->addChild(NULL);

            tablePL->addChild(new AST("const default_action", NT_TABLE_PROPERTY));
            tablePL->children[2]->addChild(new AST(actionDropUnmappedPort->value, NT_EXPRESSION));

            char buffer[128];
            tablePL->addChild(new AST("const entries", NT_TABLE_PROPERTY));
            tablePL->children[3]->addChild(new AST(NULL, NT_ENTRIES_LIST));
            int tSize = 0;
            for(int vsN = 0; vsN < (int) virtualSwitchPorts.size(); vsN++){
                tSize = tSize + ((int) virtualSwitchPorts[vsN].size());
                for(int port : virtualSwitchPorts[vsN]){
                    AST *entry = new AST(NULL, NT_ENTRY);
                    tablePL->children[3]->children[0]->addChild(entry);
                    entry->addChild(new AST(NULL, NT_TUPLE_KEYSET_EXPRESSION));
                    entry->children[0]->addChild(new AST(NULL, NT_SIMPLE_EXPRESSION_LIST));
                    sprintf(buffer, "%d", vsN + 1);
                    entry->children[0]->children[0]->addChild(new AST(buffer, NT_EXPRESSION));
                    sprintf(buffer, "%d", port);
                    entry->children[0]->children[0]->addChild(new AST(buffer, NT_EXPRESSION));
                    entry->addChild(new AST(actionOK->value, NT_ACTION_REF));
                    entry->children[1]->addChild(NULL);
                    entry->addChild(NULL); //optAnnotations
                }
            }

            sprintf(buffer, "%d", tSize);
            tablePL->addChild(new AST("size", NT_TABLE_PROPERTY));
            tablePL->children[4]->addChild(new AST(buffer, NT_LITERAL));

            AST *newCondition = new AST("IF", NT_CONDITIONAL_STATEMENT);
            newCondition->addChild(new AST(".", NT_EXPRESSION));
            newCondition->children[0]->addChild(new AST(".", NT_EXPRESSION));
            newCondition->children[0]->children[0]->addChild(new AST("VirtTableCheckProgramID", NT_NONE));
            newCondition->children[0]->children[0]->addChild(new AST("apply", NT_NONE));
            newCondition->children[0]->children[0]->addChild(NULL);
            newCondition->children[0]->addChild(new AST("hit", NT_NONE));
            newCondition->addChild(mergedControl->children[3]);
            newCondition->addChild(NULL);
            mergedControl->children[3] = new AST(NULL, NT_STAT_OR_DEC_LIST);
            mergedControl->children[3]->addChild(newCondition);
        }

        controlPipeN++;

        for(AST* n: newNodes)
            nodesToAdd.push_back(n);
    }

    //Construct new pipeline
    AST *newPipe = pipelines[pipelines.size() - 1]->Clone();
    newPipe->value = SINGLE_PIPELINE_NAME;
    for(int i = 0; i < (int) parserParams.size(); i++)
        newPipe->getChildNT(NT_ARGUMENT_LIST)->children[parserParams[i]]->children[0]->value = parserNames[i];
    for(int i = 0; i < (int) controlParams.size(); i++)
        newPipe->getChildNT(NT_ARGUMENT_LIST)->children[controlParams[i]]->children[0]->value = controlNames[i];
    nodesToAdd.push_back(newPipe);

    for(int i = 0; i < (int) programs.size(); i++){
        int cc = 0;
        while(cc < (int) programs[i]->children.size()){
            if(programs[i]->children[cc] == pipelines[i]){
                programs[i]->children.erase(programs[i]->children.begin() + cc);
                cc--;
            }
            cc++;
        }
        delete pipelines[i];
    }

    for(AST* n : nodesToAdd)
        programs[programs.size() - 1]->addChild(n);

    return true;
}

bool TofinoMerge::RemoveSwitchInstantiation(AST *ast){
    int i = 0;
    while(i < (int) ast->children.size() && (ast->children[i]->nType != NT_INSTANTIATION || ast->children[i]->value != "main" || (ast->children[i]->getChildNT(NT_TYPE_NAME) == NULL || ast->children[i]->getChildNT(NT_TYPE_NAME)->value != "Switch")))
        i++;
    if(i == (int) ast->children.size()){
        fprintf(stderr, "TofinoMerge: Could not find \"main\" Switch instantiation\n");
        return false;
    }

    delete ast->children[i];
    ast->children.erase(ast->children.begin() + i);
    return true;
}

bool TofinoMerge::GetPipelines(AST *ast, std::vector<AST*> *Pipelines){
    int i = 0;
    while(i < (int) ast->children.size() && (ast->children[i]->nType != NT_INSTANTIATION || ast->children[i]->value != "main" || (ast->children[i]->getChildNT(NT_TYPE_NAME) == NULL || ast->children[i]->getChildNT(NT_TYPE_NAME)->value != "Switch")))
        i++;
    if(i == (int) ast->children.size()){
        fprintf(stderr, "TofinoMerge: Could not find \"main\" Switch instantiation\n");
        return false;
    }

    for(AST *argument : ast->children[i]->getChildNT(NT_ARGUMENT_LIST)->children){
        std::string instName = argument->children[0]->value;
        int j = 0;
        while(j < (int) ast->children.size() && (ast->children[j]->nType != NT_INSTANTIATION || ast->children[j]->value != instName || ast->children[j]->children[1]->value != "Pipeline"))
            j++;
        if(j == (int) ast->children.size()){
            fprintf(stderr, "TofinoMerge: Could not find \"%s\" Pipeline instantiation\n", instName.c_str());
            return false;
        }

        //TODO: Check that each pipeline has 6 arguments
        Pipelines->push_back(ast->children[j]);
    }

    return true;
}