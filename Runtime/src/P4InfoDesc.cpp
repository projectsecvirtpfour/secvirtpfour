#include "../include/pcheaders.h"
#include "../include/IPLBase.hpp"
#include "../include/PropertyList.h"
#include "../include/P4InfoDesc.h"

P4InfoDesc::P4InfoDesc(){

}

P4InfoDesc::P4InfoDesc(const std::string& filePath){
    this->LoadFromFile(filePath);
}

void P4InfoDesc::LoadFromFile(const std::string& filePath){
    this->m_propertyList.LoadFromFile(filePath);
}

void P4InfoDesc::Print() const{
    this->m_propertyList.Print();
}

PropertyList* P4InfoDesc::getTablePropertyList(const std::string& tableName){
    std::vector<Property>* tablesP = this->m_propertyList.getProperty("tables");
    Assert(tablesP != NULL);

    int i = 0;
    while(i < (int) tablesP->size()){
        const std::vector<Property>* preambleP = tablesP->at(i).propList->getProperty("preamble");
        Assert(preambleP != NULL && preambleP->size() == 1);

        const std::vector<Property>* nameP = preambleP->at(0).propList->getProperty("name");
        Assert(nameP != NULL && nameP->size() == 1);

        if(("\"" + tableName + "\"") == nameP->at(0).propString.c_str())
            return tablesP->at(i).propList;
        i++;
    }
    return NULL;
}

PropertyList* P4InfoDesc::getTablePropertyList(const int id){
    std::vector<Property>* tablesP = this->m_propertyList.getProperty("tables");
    Assert(tablesP != NULL);

    int i = 0;
    while(i < (int) tablesP->size()){
        const std::vector<Property>* preambleP = tablesP->at(i).propList->getProperty("preamble");
        Assert(preambleP != NULL && preambleP->size() == 1);

        const std::vector<Property>* idP = preambleP->at(0).propList->getProperty("id");
        Assert(idP != NULL && idP->size() == 1);

        if(id == IPLBase::intFromStr(idP->at(0).propString.c_str()))
            return tablesP->at(i).propList;
        i++;
    }
    return NULL;
}

std::string P4InfoDesc::getTableNameFromID(const int id){
    PropertyList *tpl = this->getTablePropertyList(id);
    if(tpl == NULL)
        return "";
    const std::vector<Property>* preambleP = tpl->getProperty("preamble");
    Assert(preambleP != NULL);
    const std::vector<Property>* nameP = preambleP->at(0).propList->getProperty("name");
    Assert(nameP != NULL);
    std::string tableName = nameP->at(0).propString;
    if(tableName.size() > 1){
        if(tableName[tableName.size() - 1] == '"')
            tableName.pop_back();
        if(tableName[0] == '"')
            tableName.erase(tableName.begin());
    }
    return tableName;
}

int P4InfoDesc::getTableIDFromName(const std::string& name){
    PropertyList *tpl = this->getTablePropertyList(name);
    if(tpl == NULL)
        return -1;
    const std::vector<Property>* preambleP = tpl->getProperty("preamble");
    Assert(preambleP != NULL);
    const std::vector<Property>* idP = preambleP->at(0).propList->getProperty("id");
    Assert(idP != NULL);
    int id = IPLBase::intFromStr(idP->at(0).propString.c_str());
    Assert(id > 0);
    return id;
}

PropertyList* P4InfoDesc::getActionPropertyList(const std::string& actionName){
    const std::vector<Property>* actionsP = this->m_propertyList.getProperty("actions");
    Assert(actionsP != NULL);

    int i = 0;
    while(i < (int) actionsP->size()){
        const std::vector<Property>* preambleP = actionsP->at(i).propList->getProperty("preamble");
        Assert(preambleP != NULL && preambleP->size() == 1);

        const std::vector<Property>* nameP = preambleP->at(0).propList->getProperty("name");
        Assert(nameP != NULL && nameP->size() == 1);

        if(("\"" + actionName + "\"") == nameP->at(0).propString.c_str())
            return actionsP->at(i).propList;
        i++;
    }
    return NULL;
}

PropertyList* P4InfoDesc::getActionPropertyList(const int id){
    const std::vector<Property>* actionsP = this->m_propertyList.getProperty("actions");
    Assert(actionsP != NULL);

    int i = 0;
    while(i < (int) actionsP->size()){
        const std::vector<Property>* preambleP = actionsP->at(i).propList->getProperty("preamble");
        Assert(preambleP != NULL && preambleP->size() == 1);

        const std::vector<Property>* idP = preambleP->at(0).propList->getProperty("id");
        Assert(idP != NULL && idP->size() == 1);

        if(id == IPLBase::intFromStr(idP->at(0).propString.c_str()))
            return actionsP->at(i).propList;
        i++;
    }
    return NULL;
}

std::string P4InfoDesc::getActionNameFromID(const int id){
    PropertyList *apl = this->getActionPropertyList(id);
    if(apl == NULL)
        return "";
    const std::vector<Property>* preambleP = apl->getProperty("preamble");
    Assert(preambleP != NULL);
    const std::vector<Property>* nameP = preambleP->at(0).propList->getProperty("name");
    Assert(nameP != NULL);
    std::string actionName = nameP->at(0).propString;
    if(actionName.size() > 1){
        if(actionName[actionName.size() - 1] == '"')
            actionName.pop_back();
        if(actionName[0] == '"')
            actionName.erase(actionName.begin());
    }
    return actionName;
}

int P4InfoDesc::getActionIDFromName(const std::string& name){
    PropertyList *apl = this->getActionPropertyList(name);
    if(apl == NULL)
        return -1;
    const std::vector<Property>* preambleP = apl->getProperty("preamble");
    Assert(preambleP != NULL);
    const std::vector<Property>* idP = preambleP->at(0).propList->getProperty("id");
    Assert(idP != NULL);
    int id = IPLBase::intFromStr(idP->at(0).propString.c_str());
    Assert(id > 0);
    return id;
}
