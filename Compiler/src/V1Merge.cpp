#include "../include/pcheaders.h"
#include "../include/AST.h"
#include "../include/V1Merge.h"
#include "../include/Log.h"
#include "../include/CFG.h"

#include "../include/IPLBase.hpp"

#define PROGRAM_ID_TYPE_BITS_STR "8"
#define METADATA_NAME "VirtMetadata"
#define METADATA_TYPE_NAME "VirtMetadataType"
#define METADATA_PROG_ID_FIELD_NAME "VirtProgramID"

#define PI_PARAM_TYPE "packet_in"
#define PI_PARAM_NAME "pkt"
#define PO_PARAM_TYPE "packet_out"
#define PO_PARAM_NAME "pkt"
#define STD_METADATA_PARAM_TYPE "standard_metadata_t"
#define STD_METADATA_PARAM_NAME "standard_metadata"

void V1RenameVirtMetadataVariables(AST *ast){
    if(ast->getProgramType() == METADATA_TYPE_NAME)
        ast->setValue(METADATA_NAME);
    for(AST *c : ast->getChildren())
        if(c != NULL)
            V1RenameVirtMetadataVariables(c);
}

void V1RemoveTempProgramCode(AST *ast){
    if(ast->getNodeType() == NT_EXPRESSION && ast->getValue() == TEMP_PROGRAM_CODE){
        ast->setValue(".");
        ast->addChild(new AST(METADATA_NAME, NT_EXPRESSION));
        ast->addChild(new AST(METADATA_PROG_ID_FIELD_NAME, NT_NONE));
        ast->getChildren()[1]->setProgramType(METADATA_TYPE_NAME);
    }
    for(AST *c : ast->getChildren())
        if(c != NULL)
            V1RemoveTempProgramCode(c);
}

bool V1Merge::MergeV1(const std::vector<AST*> programs, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts){
    CFG::SetMetadataProgIDName(std::string(METADATA_NAME "." METADATA_PROG_ID_FIELD_NAME));
    
    std::string msg = "Merging V1 Programs:";
    for(AST *program : programs)
        msg = msg + " \"" + program->value + "\"";
    Log::MSG(msg);
    Log::Push();

    std::vector<std::vector<AST*>> Pipelines(programs.size());

    for(int i = 0; i < (int) programs.size(); i++){
        if(!V1Merge::GetPipeline(programs[i], &Pipelines[i])){
            Log::Pop();
            return false;
        }
    }
    
    bool result = V1Merge::MergePipelines(programs, Pipelines, headersDec, virtualSwitchPorts);
    if(!result){
        Log::Pop();
        return false;
    }

    AST* LastProg = programs[programs.size() - 1];

    //Change Switch Local Metadata parameters to METADATA_NAME
    const std::unordered_map<std::string, int> VirtPipelineStructNames = {{"VirtParser", 2}, {"VirtVerifyChecksum", 1}, {"VirtIngress", 1}, {"VirtEgress", 1}, {"VirtComputeChecksum", 1}};
    for(int i = 0; i < (int) LastProg->children.size(); i++){
        if(LastProg->children[i]->nType == NT_PARSER_DECLARATION && VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value) != VirtPipelineStructNames.end()){
            const std::string vMetadataHeaderName = LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value)->second]->getChildNT(NT_TYPE_NAME)->value;
            LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_PARSER_TYPE_DECLARATION)->value)->second]->value = METADATA_NAME;
            
            AST *newField = new AST(METADATA_PROG_ID_FIELD_NAME, NT_STRUCT_FIELD);
            newField->addChild(NULL); //optAnnotations
            newField->addChild(new AST("bit", NT_BASE_TYPE));
            newField->children[1]->addChild(new AST("8", NT_LITERAL));
            
            int pp = 0;
            while(pp < (int) LastProg->children.size() && ((LastProg->children[pp]->nType != NT_HEADER_TYPE_DECLARATION && LastProg->children[pp]->nType != NT_STRUCT_TYPE_DECLARATION) || LastProg->children[pp]->value != vMetadataHeaderName))
                pp++;
            if(pp == (int) LastProg->children.size()){
                pp = 0;
                while(pp < (int) headersDec.size() && ((headersDec[pp]->nType != NT_HEADER_TYPE_DECLARATION && headersDec[pp]->nType != NT_STRUCT_TYPE_DECLARATION) || headersDec[pp]->value != vMetadataHeaderName))
                    pp++;
                Assert(pp < (int) headersDec.size());
                if(headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST) != NULL){
                    bool alreadyHas = false;
                    for(int sf = 0; sf < (int) headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->children.size(); sf++){
                        if(headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->children[sf]->value == METADATA_PROG_ID_FIELD_NAME)
                            alreadyHas = true;
                    }
                    if(!alreadyHas)
                        headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->addChild(newField);
                }
                else{
                    AST *newSFL = new AST(NULL, NT_STRUCT_FIELD_LIST);
                    newSFL->addChild(newField);
                    headersDec[pp]->children[headersDec[pp]->children.size() - 1] = newSFL;
                }
                LastProg->SubstituteTypeName(headersDec[pp]->value, METADATA_TYPE_NAME);
                headersDec[pp]->value = METADATA_TYPE_NAME;
            }
            else{
                LastProg->children[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->addChild(newField);
                LastProg->SubstituteTypeName(LastProg->children[pp]->value, METADATA_TYPE_NAME);
                LastProg->children[pp]->value = METADATA_TYPE_NAME;
            }
        }
        else if(LastProg->children[i]->nType == NT_CONTROL_DECLARATION && VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value) != VirtPipelineStructNames.end()){
            const std::string vMetadataHeaderName = LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value)->second]->getChildNT(NT_TYPE_NAME)->value;
            LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[VirtPipelineStructNames.find(LastProg->children[i]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value)->second]->value = METADATA_NAME;
            
            AST *newField = new AST(METADATA_PROG_ID_FIELD_NAME, NT_STRUCT_FIELD);
            newField->addChild(NULL); //optAnnotations
            newField->addChild(new AST("bit", NT_BASE_TYPE));
            newField->children[1]->addChild(new AST("8", NT_LITERAL));
            
            int pp = 0;
            while(pp < (int) LastProg->children.size() && ((LastProg->children[pp]->nType != NT_HEADER_TYPE_DECLARATION && LastProg->children[pp]->nType != NT_STRUCT_TYPE_DECLARATION) || LastProg->children[pp]->value != vMetadataHeaderName))
                pp++;
            if(pp == (int) LastProg->children.size()){
                pp = 0;
                while(pp < (int) headersDec.size() && ((headersDec[pp]->nType != NT_HEADER_TYPE_DECLARATION && headersDec[pp]->nType != NT_STRUCT_TYPE_DECLARATION) || headersDec[pp]->value != vMetadataHeaderName))
                    pp++;
                Assert(pp < (int) headersDec.size());
                if(headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST) != NULL){
                    bool alreadyHas = false;
                    for(int sf = 0; sf < (int) headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->children.size(); sf++){
                        if(headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->children[sf]->value == METADATA_PROG_ID_FIELD_NAME)
                            alreadyHas = true;
                    }
                    if(!alreadyHas)
                        headersDec[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->addChild(newField);
                    else
                        delete newField;
                }
                else{
                    AST *newSFL = new AST(NULL, NT_STRUCT_FIELD_LIST);
                    newSFL->addChild(newField);
                    headersDec[pp]->children[headersDec[pp]->children.size() - 1] = newSFL;
                }
                LastProg->SubstituteTypeName(headersDec[pp]->value, METADATA_TYPE_NAME);
                headersDec[pp]->value = METADATA_TYPE_NAME;
            }
            else{
                LastProg->children[pp]->getChildNT(NT_STRUCT_FIELD_LIST)->addChild(newField);
                LastProg->SubstituteTypeName(LastProg->children[pp]->value, METADATA_TYPE_NAME);
                LastProg->children[pp]->value = METADATA_TYPE_NAME;
            }
        }
    }
    V1RenameVirtMetadataVariables(LastProg);
    //Change generic TEMP_PROGRAM_CODE to METADATA_NAME.METADATA_PROG_ID_FIELD_NAME
    V1RemoveTempProgramCode(LastProg);

    Log::Pop();
    return true;
}

bool V1Merge::MergePipelines(const std::vector<AST*>& programs, const std::vector<std::vector<AST*>>& pipelines, const std::vector<AST*>& headersDec, const std::vector<std::set<int>>& virtualSwitchPorts){
    const static std::unordered_map<std::string, std::string> ParamNames = {
        {PI_PARAM_TYPE, PI_PARAM_NAME},
        {PO_PARAM_TYPE, PO_PARAM_NAME},
        {STD_METADATA_PARAM_TYPE, STD_METADATA_PARAM_NAME}
    };

    std::vector<AST*> nodesToAdd;

    const std::vector<std::string> VirtPipeArgNames = {"VirtParser", "VirtVerifyChecksum", "VirtIngress", "VirtEgress", "VirtComputeChecksum", "VirtDeparser"};

    //Parser
    AST *newExpression = new AST(".", NT_EXPRESSION);
    AST *newExpressionL = new AST(STD_METADATA_PARAM_NAME, NT_EXPRESSION);
    AST *newExpressionR = new AST("ingress_port", NT_NONE);
    newExpression->addChild(newExpressionL);
    newExpression->addChild(newExpressionR);

    std::vector<AST*> elements;
    for(int pc = 0; pc < (int) pipelines.size(); pc++)
        elements.push_back(pipelines[pc][0]);
    std::vector<AST*> newNodes = AST::MergeParsers(programs, elements, headersDec, VirtPipeArgNames[0], virtualSwitchPorts, newExpression);
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
    for(AST* n: newNodes)
        nodesToAdd.push_back(n);

    //Controls
    for(int elementC = 1; elementC < (int) pipelines[0].size(); elementC++){
        elements.clear();
        for(int pc = 0; pc < (int) pipelines.size(); pc++)
            elements.push_back(pipelines[pc][elementC]);
        newNodes =  AST::MergeControls(programs, elements, headersDec, VirtPipeArgNames[elementC], virtualSwitchPorts);
        
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

        //Modify the checksum calculation / verification to comply with v1model restrictions
        if(elementC == 1 || elementC == 4){
            if(mergedControl->getChildNT(NT_STAT_OR_DEC_LIST) != NULL){
                if(mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->getChildNT(NT_SWITCH_STATEMENT) != NULL){
                    AST *SCL = mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->getChildNT(NT_SWITCH_STATEMENT)->getChildNT(NT_SWITCH_CASE_LIST);
                    int pc = 0;
                    while(pc < (int) SCL->children.size() && SCL->children[pc]->getChildNT(NT_STAT_OR_DEC_LIST) == NULL){
                        pc++;
                    }
                    Assert(pc != (int) SCL->children.size());
                    AST *newStatList = SCL->children[pc]->getChildNT(NT_STAT_OR_DEC_LIST)->Clone();
                    pc = 0;
                    while(mergedControl->children[pc]== NULL || mergedControl->children[pc]->nType != NT_STAT_OR_DEC_LIST)
                        pc++;
                    delete mergedControl->children[pc];
                    mergedControl->children[pc] = newStatList;
                }
            }
        }
        //Add metadata programID table on the ingress pipeline
        else if(elementC == 2){
            if(mergedControl->children[2] == NULL)
                mergedControl->children[2] = new AST(NULL, NT_CONTROL_LOCAL_DEC_LIST);
            if(mergedControl->children[3] == NULL)
                mergedControl->children[3] = new AST(NULL, NT_STAT_OR_DEC_LIST);
            AST *controlLDs = mergedControl->children[2];
            AST *controlBody = mergedControl->children[3];

            AST *actionSetProgramID = new AST("VirtSetProgramID", NT_ACTION_DECLARATION);
            controlLDs->addChild(actionSetProgramID);
            actionSetProgramID->addChild(new AST(NULL, NT_ANNOTATION_LIST));
            actionSetProgramID->children[0]->addChild(new AST("hidden", NT_ANNOTATION));
            actionSetProgramID->children[0]->children[0]->addChild(NULL);
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
            actionDropUnmappedPort->addChild(new AST(NULL, NT_ANNOTATION_LIST));
            actionDropUnmappedPort->children[0]->addChild(new AST("hidden", NT_ANNOTATION));
            actionDropUnmappedPort->children[0]->children[0]->addChild(NULL);
            actionDropUnmappedPort->addChild(NULL); //parameterList
            actionDropUnmappedPort->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
            
            AST* callDrop = new AST(NULL, NT_METHOD_CALL_STATEMENT);
            actionDropUnmappedPort->children[2]->addChild(callDrop);
            callDrop->addChild(new AST("mark_to_drop", NT_NONE));
            callDrop->addChild(new AST(NULL, NT_ARGUMENT_LIST));
            callDrop->children[1]->addChild(new AST(NULL, NT_ARGUMENT));
            callDrop->children[1]->children[0]->addChild(new AST(STD_METADATA_PARAM_NAME, NT_EXPRESSION));

            AST *tableProgramID = new AST("VirtTableSetProgramID", NT_TABLE_DECLARATION);
            controlLDs->addChild(tableProgramID);
            AST *annotationList = new AST(NULL, NT_ANNOTATION_LIST);
            annotationList->addChild(new AST("noWarn", NT_ANNOTATION));
            annotationList->children[0]->addChild(new AST(NULL, NT_ANNOTATION_TOKEN_LIST));
            annotationList->children[0]->children[0]->addChild(new AST("\"mismatch\"", NT_ANNOTATION_TOKEN));
            annotationList->addChild(new AST("hidden", NT_ANNOTATION));
            annotationList->children[1]->addChild(NULL);
            tableProgramID->addChild(annotationList);
            AST *tablePL = new AST(NULL, NT_TABLE_PROPERTY_LIST);
            tableProgramID->addChild(tablePL);
            tablePL->addChild(new AST("key", NT_TABLE_PROPERTY));
            AST *key = new AST("exact", NT_KEY_ELEMENT);
            key->addChild(new AST(".", NT_EXPRESSION));
            key->children[0]->addChild(new AST(STD_METADATA_PARAM_NAME, NT_EXPRESSION));
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
        else if(elementC == 3){
            if(mergedControl->children[2] == NULL)
                mergedControl->children[2] = new AST(NULL, NT_CONTROL_LOCAL_DEC_LIST);
            if(mergedControl->children[3] == NULL)
                mergedControl->children[3] = new AST(NULL, NT_STAT_OR_DEC_LIST);
            AST *controlLDs = mergedControl->children[2];

            AST *actionOK = new AST("VirtOutPortOK", NT_ACTION_DECLARATION);
            controlLDs->addChild(actionOK);
            actionOK->addChild(new AST(NULL, NT_ANNOTATION_LIST));
            actionOK->children[0]->addChild(new AST("hidden", NT_ANNOTATION));
            actionOK->children[0]->children[0]->addChild(NULL);
            actionOK->addChild(NULL); //parameterList
            actionOK->addChild(NULL);

            AST *actionDropUnmappedPort = new AST("VirtOutPortWrong", NT_ACTION_DECLARATION);
            controlLDs->addChild(actionDropUnmappedPort);
            actionDropUnmappedPort->addChild(new AST(NULL, NT_ANNOTATION_LIST));
            actionDropUnmappedPort->children[0]->addChild(new AST("hidden", NT_ANNOTATION));
            actionDropUnmappedPort->children[0]->children[0]->addChild(NULL);
            actionDropUnmappedPort->addChild(NULL); //parameterList
            actionDropUnmappedPort->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
            AST* callDrop = new AST(NULL, NT_METHOD_CALL_STATEMENT);
            callDrop->addChild(new AST("mark_to_drop", NT_NONE));
            callDrop->addChild(new AST(NULL, NT_ARGUMENT_LIST));
            callDrop->children[1]->addChild(new AST(NULL, NT_ARGUMENT));
            callDrop->children[1]->children[0]->addChild(new AST(STD_METADATA_PARAM_NAME, NT_EXPRESSION));
            actionDropUnmappedPort->children[2]->addChild(callDrop);

            AST *tableProgramID = new AST("VirtTableCheckProgramID", NT_TABLE_DECLARATION);
            controlLDs->addChild(tableProgramID);
            AST *annotationList = new AST(NULL, NT_ANNOTATION_LIST);
            annotationList->addChild(new AST("noWarn", NT_ANNOTATION));
            annotationList->children[0]->addChild(new AST(NULL, NT_ANNOTATION_TOKEN_LIST));
            annotationList->children[0]->children[0]->addChild(new AST("\"mismatch\"", NT_ANNOTATION_TOKEN));
            annotationList->addChild(new AST("hidden", NT_ANNOTATION));
            annotationList->children[1]->addChild(NULL);
            tableProgramID->addChild(annotationList);
            AST *tablePL = new AST(NULL, NT_TABLE_PROPERTY_LIST);
            tableProgramID->addChild(tablePL);
            tablePL->addChild(new AST("key", NT_TABLE_PROPERTY));
            AST *key = new AST("exact", NT_KEY_ELEMENT);
            key->addChild(new AST(".", NT_EXPRESSION));
            key->children[0]->addChild(new AST(STD_METADATA_PARAM_NAME, NT_EXPRESSION));
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

            AST *newControlVar = new AST("VirtEgressOk", NT_VARIABLE_DECLARATION);
            newControlVar->addChild(NULL); //annotations
            newControlVar->addChild(new AST("bool", NT_BASE_TYPE));
            newControlVar->addChild(new AST(".", NT_EXPRESSION));
            newControlVar->children[2]->addChild(new AST(".", NT_EXPRESSION));
            newControlVar->children[2]->children[0]->addChild(new AST("VirtTableCheckProgramID", NT_NONE));
            newControlVar->children[2]->children[0]->addChild(new AST("apply", NT_NONE));
            newControlVar->children[2]->children[0]->addChild(NULL);
            newControlVar->children[2]->addChild(new AST("hit", NT_NONE));
            AST *newCondition = new AST("IF", NT_CONDITIONAL_STATEMENT);
            newCondition->addChild(new AST("VirtEgressOk", NT_EXPRESSION));
            newCondition->addChild(mergedControl->children[3]);
            newCondition->addChild(NULL);
            mergedControl->children[3] = new AST(NULL, NT_STAT_OR_DEC_LIST);
            mergedControl->children[3]->addChild(newControlVar);
            mergedControl->children[3]->addChild(newCondition);
        }
        //Deparser can only contain emit, calls.
        //We add invalidation statements to egress to mantain program logic
        else if(elementC == 5 && mergedControl->getChildNT(NT_STAT_OR_DEC_LIST) != NULL){
            //If there is no switch statement then all deparsers are equal and we are good
            //TODO: Check if emit "order" is the same in every program (i.e. no such thing as "emit(h1); emit(h2)" in p1 and "emit(h2); emit(h1)" in p2)
            if(mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->getChildNT(NT_SWITCH_STATEMENT) != NULL){
                AST *SCL = mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->getChildNT(NT_SWITCH_STATEMENT)->getChildNT(NT_SWITCH_CASE_LIST);
                std::vector<AST*> uniqueStatements;
                std::vector<std::set<AST*>> programStatements(programs.size());
                for(int sc = 0; sc < (int) SCL->children.size(); sc++){
                    int prog = IPLBase::intFromStr(SCL->children[sc]->getChildNT(NT_EXPRESSION)->value.c_str()) - 1;
                    Assert(prog >= 0);
                    int effSC = sc;
                    while(effSC < (int) SCL->children.size() && SCL->children[effSC]->getChildNT(NT_STAT_OR_DEC_LIST) == NULL)
                        effSC++;
                    if(effSC < (int) SCL->children.size()){
                        for(AST *statement : SCL->children[effSC]->getChildNT(NT_STAT_OR_DEC_LIST)->children){
                            bool unique = true;
                            for(AST *uniqueS : uniqueStatements){
                                if(AST::Equals(statement, uniqueS)){
                                    unique = false;
                                    programStatements[sc].emplace(uniqueS);
                                }
                            }
                            if(unique){
                                Assert(statement->nType == NT_METHOD_CALL_STATEMENT);
                                uniqueStatements.push_back(statement->Clone());
                                programStatements[sc].emplace(uniqueStatements[uniqueStatements.size() - 1]);
                            }
                        }
                    }
                }

                int dps = 0;
                while(dps < (int) nodesToAdd.size() && (nodesToAdd[dps]->nType != NT_CONTROL_DECLARATION || nodesToAdd[dps]->getChildNT(NT_CONTROL_TYPE_DECLARATION)->value != VirtPipeArgNames[3]))
                    dps++;
                Assert(dps < (int) nodesToAdd.size());
                AST *EgressControl = nodesToAdd[dps];

                AST* newEgressSwitchStatement = new AST(NULL, NT_SWITCH_STATEMENT);
                newEgressSwitchStatement->addChild(new AST(".", NT_EXPRESSION));
                newEgressSwitchStatement->children[0]->addChild(new AST(METADATA_NAME, NT_NONE));
                newEgressSwitchStatement->children[0]->addChild(new AST(METADATA_PROG_ID_FIELD_NAME, NT_NONE));
                newEgressSwitchStatement->addChild(new AST(NULL, NT_SWITCH_CASE_LIST));
                for(int p = 0; p < (int) programs.size(); p++){
                    AST *SC = new AST(NULL, NT_SWITCH_CASE);
                    newEgressSwitchStatement->children[1]->addChild(SC);
                    char buffer[32];
                    sprintf(buffer, "%d", p+1);
                    SC->addChild(new AST(std::string(buffer), NT_EXPRESSION));
                    SC->addChild(new AST(NULL, NT_STAT_OR_DEC_LIST));
                    for(AST *us : uniqueStatements){
                        if(programStatements[p].find(us) == programStatements[p].end()){
                            AST* invalidateStatement = new AST(NULL, NT_METHOD_CALL_STATEMENT);
                            invalidateStatement->addChild(new AST(".", NT_NONE));
                            invalidateStatement->addChild(NULL); //setInvalid arguments
                            invalidateStatement->children[0]->addChild(us->getChildNT(NT_ARGUMENT_LIST)->getChildNT(NT_ARGUMENT)->getChildNT(NT_EXPRESSION)->Clone());
                            invalidateStatement->children[0]->children[0]->children[0]->value = EgressControl->getChildNT(NT_CONTROL_TYPE_DECLARATION)->getChildNT(NT_PARAMETER_LIST)->children[0]->value;
                            invalidateStatement->children[0]->addChild(new AST("setInvalid", NT_NONE));
                            SC->children[1]->addChild(invalidateStatement);
                        }
                    }
                }
                
                EgressControl->getChildNT(NT_STAT_OR_DEC_LIST)->getChildNT(NT_CONDITIONAL_STATEMENT)->children[1]->addChild(newEgressSwitchStatement);
                delete mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->children[0];
                mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->children.clear();
                for(AST *us : uniqueStatements)
                    mergedControl->getChildNT(NT_STAT_OR_DEC_LIST)->addChild(us);
            }
        }

        for(AST* n: newNodes)
            nodesToAdd.push_back(n);
    }

    //Construct new pipeline
    AST *newPipe = new AST("main", NT_INSTANTIATION);
    newPipe->addChild(NULL); //annotations
    newPipe->addChild(new AST("V1Switch", NT_TYPE_NAME));
    newPipe->addChild(new AST(NULL, NT_ARGUMENT_LIST));
    for(int i = 0; i < (int) VirtPipeArgNames.size(); i++){
        newPipe->children[2]->addChild(new AST(NULL, NT_ARGUMENT));
        newPipe->children[2]->children[i]->addChild(new AST(VirtPipeArgNames[i], NT_EXPRESSION));
        newPipe->children[2]->children[i]->children[0]->addChild(NULL);
    }
    nodesToAdd.push_back(newPipe);

    for(AST* n : nodesToAdd)
        programs[programs.size() - 1]->addChild(n);

    return true;
}

bool V1Merge::GetPipeline(AST *ast, std::vector<AST*> *Pipeline){
    int i = 0;
    while(i < (int) ast->children.size() && (ast->children[i]->nType != NT_INSTANTIATION || ast->children[i]->value != "main"))
        i++;
    if(i == (int) ast->children.size()){
        ast->writeCode(stdout);
        fprintf(stderr, "V1Merge: Could not find \"main\" V1Switch instantiation\n");
        return false;
    }

    for(AST *argument : ast->children[i]->getChildNT(NT_ARGUMENT_LIST)->children){
        std::string instName = argument->children[0]->value;
        int j = 0;
        while(j < (int) ast->children.size() && ((ast->children[j]->nType != NT_PARSER_DECLARATION && ast->children[j]->nType != NT_CONTROL_DECLARATION) || ast->children[j]->children[0]->value != instName))
            j++;
        if(j == (int) ast->children.size()){
            fprintf(stderr, "V1Merge: Could not find \"%s\" instantiation\n", instName.c_str());
            return false;
        }

        //TODO: Check that each pipeline has 6 arguments
        Pipeline->push_back(ast->children[j]);
    }

    delete ast->children[i];
    ast->children.erase(ast->children.begin() + i);

    return true;
}