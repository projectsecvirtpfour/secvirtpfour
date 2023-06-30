#include "../include/pcheaders.h"
#include "../include/IPLBase.hpp"
#include "../include/PreProcessor.h"

PreProcessor::PreProcessor(const std::string& filePath, const std::vector<std::string>& p4includePaths){
    this->m_p4architecture = P4A_UNDEFINED;
    this->m_filePath = filePath;
    this->m_p4includePaths = p4includePaths;
    this->m_fileLines = PreProcessor::readFileLines(filePath);
}

void PreProcessor::Process(){
    std::vector<std::pair<int, bool>> ifstack;
    std::vector<std::string> currentFilePathStack = {this->m_filePath};

    this->ProcessStripComments();
    this->m_defineMacros.clear();
    int i = 0;
    while(i < (int) this->m_fileLines.size()){
        std::vector<std::string> stringSplit = IPLBase::split(this->m_fileLines[i]);
        if(stringSplit.size() > 0){
            if(stringSplit[0][0] == '#' && stringSplit[0].size() > 1){
                stringSplit.emplace(stringSplit.begin() + 1, stringSplit[0].substr(1));
                stringSplit[0].erase(stringSplit[0].begin() + 1, stringSplit[0].end());
            }
            if(stringSplit[0] == "#" && stringSplit.size() > 1){
                if(ifstack.size() > 0){
                    if(stringSplit[1] == "else" && stringSplit.size() == 2){
                        this->m_fileLines[i].clear();
                        ifstack[ifstack.size() - 1].second = !ifstack[ifstack.size() - 1].second;
                    }
                    else if(stringSplit[1] == "endif" && stringSplit.size() == 2){
                        this->m_fileLines[ifstack[ifstack.size() - 1].first].clear();
                        this->m_fileLines[i].clear();
                        ifstack.pop_back();
                    }
                    else if(!ifstack[ifstack.size() - 1].second){
                        this->m_fileLines[i].clear();
                    }
                }

                if(stringSplit[1] == "include" && stringSplit.size() == 3)
                    this->ProcessInclude(i, stringSplit[2], &currentFilePathStack);
                else if(stringSplit[1] == "define" && stringSplit.size() > 2){
                    this->ProcessDefine(i, stringSplit);
                }
                else if(stringSplit.size() == 3 && (stringSplit[1] == "ifdef" || stringSplit[1] == "ifndef")){
                    bool isDefined = (this->m_defineMacros.find(stringSplit[2]) != this->m_defineMacros.end());
                    ifstack.push_back(std::make_pair(i, ((stringSplit[1] == "ifdef" && isDefined) || (stringSplit[1] == "ifndef" && !isDefined)) && (ifstack.size() == 0 || ifstack[ifstack.size() - 1].second))); 
                }
                else if(stringSplit.size() >= 3 && stringSplit[1] == "if"){
                    bool isTrue = PreProcessor::EvalIfStringSplit(stringSplit);
                    ifstack.push_back(std::make_pair(i, isTrue && (ifstack.size() == 0 || ifstack[ifstack.size() - 1].second)));
                }
            }
            else if(stringSplit[0] == "__POP_INCLUDE__")
                currentFilePathStack.pop_back();
            else if(ifstack.size() > 0 && !ifstack[ifstack.size() - 1].second)
                this->m_fileLines[i].clear();
        }
        i++;
    }
}

void PreProcessor::ProcessStripComments(){
    bool isComment = false;
    size_t lc, lc2;
    for(int i = 0; i < (int) this->m_fileLines.size(); i++){
        lc = m_fileLines[i].find("//");
        if(lc != std::string::npos)
            m_fileLines[i].erase(m_fileLines[i].begin() + lc, m_fileLines[i].end());
    }
    for(int i = 0; i < (int) this->m_fileLines.size(); i++){
        if(isComment){
            lc = m_fileLines[i].find("*/");
            if(lc == std::string::npos)
                m_fileLines[i].clear();
            else{
                isComment = false;
                for(size_t x = 0; x < lc + 2; x++)
                    m_fileLines[i][x] = ' ';
                i--;
            }
        }
        else{
            lc = m_fileLines[i].find("/*");
            if(lc != std::string::npos){
                lc2 = m_fileLines[i].find("*/");
                if(lc2 != std::string::npos && lc2 > lc + 1){
                    for(size_t x = lc; x < lc2 + 2; x++)
                        m_fileLines[i][x] = ' ';
                    i--;
                }
                else{
                    isComment = true;
                    m_fileLines[i].erase(m_fileLines[i].begin() + lc, m_fileLines[i].end());
                }
            }
        }
    }
}

void PreProcessor::Print(FILE *out) const{
    for(const std::string& line : this->m_fileLines)
        fprintf(out, "%s\n", line.c_str());
}

void PreProcessor::ProcessInclude(const int line, const std::string& includeFile, std::vector<std::string>* currentFilePathStack){
    std::string fileP;
    if(includeFile.size() < 2){
        fprintf(stderr, "Error: Malformed include on \"%s\" line %d:\n%s\n", currentFilePathStack->at(currentFilePathStack->size() - 1).c_str(), line + 1, m_fileLines[line].c_str());
        exit(1);
    }
    if(includeFile[0] == '"' && includeFile[includeFile.size() - 1] == '"'){
        std::vector<std::string> filePathDirChain = IPLBase::split(currentFilePathStack->at(currentFilePathStack->size() - 1), '/');
        filePathDirChain.pop_back();
        std::vector<std::string> includeFilePathDirChain = IPLBase::split(includeFile.substr(1, includeFile.size() - 2), '/');
        for(const std::string& s : includeFilePathDirChain){
            if(s == ".." && filePathDirChain.size() > 0)
                filePathDirChain.pop_back();
            else
                filePathDirChain.push_back(s);
        }
        fileP = filePathDirChain[0];
        for(int i = 1; i < (int) filePathDirChain.size(); i++)
            fileP = fileP + "/" + filePathDirChain[i];
    }
    else if(includeFile[0] == '<' && includeFile[includeFile.size() - 1] == '>'){
        FILE *fp = NULL;
        int dirIDX = 0;
        while(dirIDX < (int) this->m_p4includePaths.size() && fp == NULL){
            fileP = this->m_p4includePaths[dirIDX] + includeFile.substr(1, includeFile.size() - 2);
            fp = fopen(fileP.c_str(), "r");
            dirIDX++;
        }
        if(!fp){
            fprintf(stderr, "Error: Could not find include file: \"%s\" (Line %d - \"%s\")\n", includeFile.substr(1, includeFile.size() - 2).c_str(), line + 1, m_filePath.c_str());
            exit(1);
        }
        fclose(fp);
    }
    else{
        fprintf(stderr, "Error: Malformed include on \"%s\" line %d:\n%s\n", currentFilePathStack->at(currentFilePathStack->size() - 1).c_str(), line + 1, m_fileLines[line].c_str());
        exit(1);
    }

    std::vector<std::string> includeFileLines = readFileLines(fileP);
    this->m_fileLines[line] = "__PUSH_INCLUDE__ \"" + fileP + "\"";
    this->m_fileLines.emplace(this->m_fileLines.begin() + line + 1, "__POP_INCLUDE__ \"" + fileP + "\"");

    currentFilePathStack->push_back(fileP);

    this->m_fileLines.resize(this->m_fileLines.size() + includeFileLines.size());
    for(int i = (int) this->m_fileLines.size() - 1; i >= (int) (line + includeFileLines.size()); i--)
        this->m_fileLines[i] = this->m_fileLines[i - includeFileLines.size()];
    for(int i = 0; i < (int) includeFileLines.size(); i++)
        this->m_fileLines[line + i + 1] = includeFileLines[i];

    this->ProcessStripComments();

    //Updates Infered Architecture
    if(fileP.find("v1model.p4") != std::string::npos)
        this->m_p4architecture = (this->m_p4architecture == P4A_UNDEFINED || this->m_p4architecture == P4A_V1MODEL) ? P4A_V1MODEL : P4A_UNDEFINED;
    else if(fileP.find("tna.p4") != std::string::npos)
        this->m_p4architecture = (this->m_p4architecture == P4A_UNDEFINED || this->m_p4architecture == P4A_TNA) ? P4A_TNA : P4A_UNDEFINED;
}

void PreProcessor::ProcessDefine(const int line, const std::vector<std::string>& stringSplit){
    size_t parenOpen = 0, parenClose = 0;
    while(parenOpen < stringSplit[2].size() && stringSplit[2][parenOpen] != '(')
        parenOpen++;
    while(parenClose < stringSplit[2].size() && stringSplit[2][parenClose] != ')')
        parenClose++;
    if((parenClose < parenOpen) || (parenOpen != stringSplit[2].size() && parenOpen == stringSplit[2].size()) || (parenOpen == stringSplit[2].size() && parenOpen != stringSplit[2].size())){
        fprintf(stderr, "Error: Malformed define on \"%s\" line %d:\n%s\n", m_filePath.c_str(), line + 1, m_fileLines[line].c_str());
        exit(1);
    }

    PPDefineMacro newM;
    newM.name = stringSplit[2].substr(0, parenOpen);
    size_t x = 3;
    while(x < stringSplit.size()){
        newM.body = newM.body + stringSplit[x];
        if(x < stringSplit.size() - 1)
            newM.body = newM.body + " ";
        x++;
    }

    if(parenOpen < stringSplit[2].size()){
        newM.argNames = IPLBase::split(stringSplit[2].substr(parenOpen + 1, parenClose - parenOpen - 1), ',');
        for(size_t i = 0; i < newM.argNames.size(); i++){
            while(newM.argNames[i].size() > 0 && newM.argNames[i][newM.argNames[i].size() - 1] == ' ')
                newM.argNames[i].pop_back();
            while(newM.argNames[i].size() > 0 && newM.argNames[i][0] == ' ')
                newM.argNames[i].erase(newM.argNames[i].begin());
            if(newM.argNames[i].size() == 0){
                fprintf(stderr, "Error: Malformed define on \"%s\" line %d:\n%s\n", m_filePath.c_str(), line + 1, m_fileLines[line].c_str());
                exit(1);
            }
        }
    }

    this->m_defineMacros.emplace(newM.name, newM);
    this->m_fileLines[line].clear();

    this->ReplaceDefine(line + 1, newM);
}

void PreProcessor::ReplaceDefine(const int startLine, const PPDefineMacro& ppdm){
    for(int line = startLine; line < (int) this->m_fileLines.size(); line++){
        int position = this->m_fileLines[line].find(ppdm.name);
        int cur = position + ppdm.name.size() + 1;
        if(position != (int) std::string::npos){
            std::string argsList;
            if(position + ppdm.name.size() < this->m_fileLines[line].size() && this->m_fileLines[line][position + ppdm.name.size()] == '('){
                int ps = 1;
                while(ps > 0 && cur < (int) this->m_fileLines[line].size()){
                    argsList.push_back(this->m_fileLines[line][cur]);
                    if(this->m_fileLines[line][cur] == '(')
                        ps++;
                    if(this->m_fileLines[line][cur] == ')')
                        ps--;
                    cur++;
                }
                cur++;
                if(ps != 0)
                    argsList.clear();
            }
            std::vector<std::string> argListSplit;
            if(argsList.size() > 0){
                argsList.pop_back();
                argListSplit = IPLBase::split(argsList, ',');
            }
            std::string tempBody = ppdm.body;
            if(argListSplit.size() == ppdm.argNames.size()){
                int additionalPos = 0;
                for(int i = 0; i < (int) ppdm.argNames.size(); i++){
                    int argPos = ppdm.body.find(ppdm.argNames[i]);
                    while(argPos != (int) std::string::npos){
                        tempBody = tempBody.substr(0, argPos + additionalPos) + argListSplit[i] + tempBody.substr(argPos + additionalPos + ppdm.argNames[i].size());
                        additionalPos = additionalPos + argListSplit[i].size() - ppdm.argNames[i].size();
                        argPos = ppdm.body.find(ppdm.argNames[i], argPos + 1);
                    }
                }
            }
            this->m_fileLines[line] = this->m_fileLines[line].substr(0, position) + tempBody + this->m_fileLines[line].substr(cur-1);
        }
    }
}

bool PreProcessor::EvalIfStringSplit(const std::vector<std::string>& stringSplit){
    if(stringSplit.size() == 5){
        if(IPLBase::isInt(stringSplit[2].c_str()) && IPLBase::isInt(stringSplit[4].c_str())){
            int a = IPLBase::intFromStr(stringSplit[2].c_str());
            int b = IPLBase::intFromStr(stringSplit[4].c_str());
            if(stringSplit[3] == "==")
                return a == b;
            else if(stringSplit[3] == "!=")
                return a != b;
            else if(stringSplit[3] == ">=")
                return a >= b;
            else if(stringSplit[3] == "<=")
                return a <= b;
            else if(stringSplit[3] == ">")
                return a > b;
            else if(stringSplit[3] == "<")
                return a < b;
        }
    }

    return false;
}

void PreProcessor::PrintDefineMacro(const PPDefineMacro& ppdm){
    printf("Name: %s\nArgs: (", ppdm.name.c_str());
    for(int i = 0; i < (int) ppdm.argNames.size(); i++){
        printf("%s", ppdm.argNames[i].c_str());
        if(i < (int) ppdm.argNames.size() - 1)
            printf(", ");
    }
    printf(")\nBody: %s\n\n", ppdm.body.c_str());
}

std::vector<std::string> PreProcessor::readFileLines(const std::string& filePath){
    std::vector<std::string> result;
    std::string aux;
    FILE *fp = fopen(filePath.c_str(), "r");

    if(!fp){
        fprintf(stderr, "Could not open file: %s\n", filePath.c_str());
        exit(1);
    }

    char c = fgetc(fp);
    while(!feof(fp)){
        while(c != '\n' && !feof(fp)){
            aux.push_back(c);
            c = fgetc(fp);
        }
        result.push_back(aux);
        aux.clear();
        c = fgetc(fp);
    }

    fclose(fp);
    return result;
}
