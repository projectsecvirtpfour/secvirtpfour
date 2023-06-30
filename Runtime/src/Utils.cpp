#include "../include/pcheaders.h"
#include "../proto/p4runtime.grpc.pb.h"
#include "../include/Utils.h"

bool Utils::EnablePrints = true;
std::string Utils::TabSpaces = "";
int Utils::TabIncrease = 4;

void UtilsIncreaseTabSpaces(){
    for(int i = 0; i < Utils::TabIncrease; i++)
        Utils::TabSpaces.push_back(' ');
}

void UtilsDecreaseTabSpaces(){
    for(int i = 0; i < Utils::TabIncrease; i++)
        Utils::TabSpaces.pop_back();
}

std::string Utils::StringBytes(const std::string& str){
    const int slen = ((int) str.length()) * 4 + 1;
    char* buffer = (char*) malloc(sizeof(char) * slen);
    buffer[slen - 1] = '\0';

    for(int i = 0; i < (int) str.length(); i++){
        buffer[i*4] = '\\';
        buffer[1 + i*4] = (str[i] / 64) + '0';
        buffer[2 + i*4] = ((str[i] % 64) / 8) + '0';
        buffer[3 + i*4] = (str[i] % 8) + '0';
    }

    std::string result(buffer);
    free(buffer);
    return result;
}

std::string Utils::UpdateTypeToString(const p4::v1::Update_Type update){
    switch(update){
        case p4::v1::Update_Type::Update_Type_INSERT:
            return "INSERT";
        case p4::v1::Update_Type::Update_Type_MODIFY:
            return "MODIFY";
        case p4::v1::Update_Type::Update_Type_DELETE:
            return "DELETE";
        case p4::v1::Update_Type::Update_Type_UNSPECIFIED:
            return "UNSPECIFIED";
        default:
            break;
    }
    return "";
}

std::string Utils::WriteRequestAtomicityToString(const p4::v1::WriteRequest_Atomicity atomicity){
    switch(atomicity){
        case p4::v1::WriteRequest_Atomicity::WriteRequest_Atomicity_CONTINUE_ON_ERROR:
            return "CONTINUE_ON_ERROR";
        case p4::v1::WriteRequest_Atomicity::WriteRequest_Atomicity_DATAPLANE_ATOMIC:
            return "DATAPLANE_ATOMIC";
        case p4::v1::WriteRequest_Atomicity::WriteRequest_Atomicity_ROLLBACK_ON_ERROR:
            return "ROLLBACK_ON_ERROR";
        default:
            break;
    }
    return "";
}

void Utils::PrintFieldMatch(const p4::v1::FieldMatch& match){
    printf("%sfield_id: %d\n", Utils::TabSpaces.c_str(), match.field_id());
    if(match.has_exact()){
        printf("%sexact {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        printf("%svalue: %s\n", Utils::TabSpaces.c_str(), Utils::StringBytes(match.exact().value()).c_str());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(match.has_ternary()){
        printf("%sternary {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        printf("%svalue: %s\n", Utils::TabSpaces.c_str(), Utils::StringBytes(match.ternary().value()).c_str());
        printf("%smask: %s\n", Utils::TabSpaces.c_str(), match.ternary().mask().c_str());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(match.has_lpm()){
        printf("%slpm {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        printf("%svalue: %s\n", Utils::TabSpaces.c_str(), Utils::StringBytes(match.lpm().value()).c_str());
        printf("%sprefix_len: %d\n", Utils::TabSpaces.c_str(), match.lpm().prefix_len());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(match.has_range()){
        printf("%srange {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        printf("%slow: %s\n", Utils::TabSpaces.c_str(), match.range().low().c_str());
        printf("%shigh: %s\n", Utils::TabSpaces.c_str(), match.range().high().c_str());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(match.has_optional()){
        printf("%soptional {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        printf("%svalue: %s\n", Utils::TabSpaces.c_str(), Utils::StringBytes(match.optional().value()).c_str());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
}

void Utils::PrintAction(const p4::v1::Action& action){
    printf("%saction_id: %d\n", Utils::TabSpaces.c_str(), action.action_id());
    printf("%sparams {\n", Utils::TabSpaces.c_str());
    UtilsIncreaseTabSpaces();
    for(p4::v1::Action_Param param : action.params()){
        printf("%sparam {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        printf("%sparam_id: %d\n", Utils::TabSpaces.c_str(), param.param_id());
        printf("%svalue: %s\n", Utils::TabSpaces.c_str(), Utils::StringBytes(param.value()).c_str());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    UtilsDecreaseTabSpaces();
    printf("%s}\n", Utils::TabSpaces.c_str());
}

void Utils::PrintTableAction(const p4::v1::TableAction& table_action){
    if(table_action.has_action()){
        printf("%saction {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintAction(table_action.action());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(table_action.has_action_profile_member_id())
        printf("%saction_profile_member_id: %d\n", Utils::TabSpaces.c_str(), table_action.action_profile_member_id());
    else if(table_action.has_action_profile_group_id())
        printf("%saction_profile_group_id: %d\n", Utils::TabSpaces.c_str(), table_action.action_profile_group_id());
}

void Utils::PrintExternEntry(const p4::v1::ExternEntry& extern_entry){
    //TODO
}

void Utils::PrintTableEntry(const p4::v1::TableEntry& table_entry){
    printf("%stable_id: %d\n", Utils::TabSpaces.c_str(), table_entry.table_id());
    printf("%saction {\n", Utils::TabSpaces.c_str());
    UtilsIncreaseTabSpaces();
    Utils::PrintTableAction(table_entry.action());
    UtilsDecreaseTabSpaces();
    printf("%s}\n", Utils::TabSpaces.c_str());
    printf("%sMatches: {\n", Utils::TabSpaces.c_str());
    UtilsIncreaseTabSpaces();
    for(p4::v1::FieldMatch fieldMatch : table_entry.match()){
        printf("%smatch {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintFieldMatch(fieldMatch);
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    UtilsDecreaseTabSpaces();
    printf("%s}\n", Utils::TabSpaces.c_str());
}

void Utils::PrintActionProfileMember(const p4::v1::ActionProfileMember& action_profile_member){
    //TODO
}

void Utils::PrintActionProfileGroup(const p4::v1::ActionProfileGroup& action_profile_group){
    //TODO
}

void Utils::PrintMeterEntry(const p4::v1::MeterEntry& meter_entry){
    //TODO
}

void Utils::PrintDirectMeterEntry(const p4::v1::DirectMeterEntry& direct_meter_entry){
    //TODO
}

void Utils::PrintCounterEntry(const p4::v1::CounterEntry& counter_entry){
    //TODO
}

void Utils::PrintDirectCounterEntry(const p4::v1::DirectCounterEntry& direct_counter_entry){
    //TODO
}

void Utils::PrintPacketReplicationEngineEntry(const p4::v1::PacketReplicationEngineEntry& packet_replication_engine_entry){
    //TODO
}

void Utils::PrintValueSetEntry(const p4::v1::ValueSetEntry& value_set_entry){
    //TODO
}

void Utils::PrintRegisterEntry(const p4::v1::RegisterEntry& register_entry){
    //TODO
}

void Utils::PrintDigestEntry(const p4::v1::DigestEntry& digest_entry){
    //TODO
}

void Utils::PrintEntity(const p4::v1::Entity& entity){
    if(entity.has_extern_entry()){
        printf("%sextern_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintExternEntry(entity.extern_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_table_entry()){
        printf("%stable_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintTableEntry(entity.table_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_action_profile_member()){
        printf("%saction_profile_member {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintActionProfileMember(entity.action_profile_member());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_action_profile_group()){
        printf("%saction_profile_group {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintActionProfileGroup(entity.action_profile_group());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_meter_entry()){
        printf("%smeter_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintMeterEntry(entity.meter_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_direct_meter_entry()){
        printf("%sdirect_meter_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintDirectMeterEntry(entity.direct_meter_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_counter_entry()){
        printf("%scounter_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintCounterEntry(entity.counter_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_direct_counter_entry()){
        printf("%sdirect_counter_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintDirectCounterEntry(entity.direct_counter_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_packet_replication_engine_entry()){
        printf("%spacket_replication_engine_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintPacketReplicationEngineEntry(entity.packet_replication_engine_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_value_set_entry()){
        printf("%svalue_set_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintValueSetEntry(entity.value_set_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_register_entry()){
        printf("%sregister_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintRegisterEntry(entity.register_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    else if(entity.has_digest_entry()){
        printf("%sdigest_entry {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintDigestEntry(entity.digest_entry());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
}

void Utils::PrintUpdate(const p4::v1::Update& update){
    printf("%stype: %s\n", Utils::TabSpaces.c_str(), Utils::UpdateTypeToString(update.type()).c_str());
    printf("%sentity {\n", Utils::TabSpaces.c_str());
    UtilsIncreaseTabSpaces();
    Utils::PrintEntity(update.entity());
    UtilsDecreaseTabSpaces();
    printf("%s}\n", Utils::TabSpaces.c_str());
}

void Utils::PrintWriteRequest(const p4::v1::WriteRequest* request){
    if(!Utils::EnablePrints)
        return;

    if(request->has_election_id()){
        printf("%selection_id {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        printf("%slow: %lu\n%shigh: %lu\n", Utils::TabSpaces.c_str(), request->election_id().low(), Utils::TabSpaces.c_str(), request->election_id().high());
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    printf("%sUpdates: {\n", Utils::TabSpaces.c_str());
    UtilsIncreaseTabSpaces();
    for(p4::v1::Update update : request->updates()){
        printf("%supdate {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintUpdate(update);
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    UtilsDecreaseTabSpaces();
    printf("%s}\n%sAtomicity: %s\n", Utils::TabSpaces.c_str(), Utils::TabSpaces.c_str(), Utils::WriteRequestAtomicityToString(request->atomicity()).c_str());
}

void Utils::PrintReadRequest(const p4::v1::ReadRequest* request){
    if(!Utils::EnablePrints)
        return;

    printf("%sEntities: {\n", Utils::TabSpaces.c_str());
    UtilsIncreaseTabSpaces();
    for(p4::v1::Entity entity : request->entities()){
        printf("%sentity {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintEntity(entity);
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    UtilsDecreaseTabSpaces();
    printf("%s}\n", Utils::TabSpaces.c_str());
}

void Utils::PrintReadResponse(const p4::v1::ReadResponse* response){
    if(!Utils::EnablePrints)
        return;

    printf("%sEntities: {\n", Utils::TabSpaces.c_str());
    UtilsIncreaseTabSpaces();
    for(p4::v1::Entity entity : response->entities()){
        printf("%sentity {\n", Utils::TabSpaces.c_str());
        UtilsIncreaseTabSpaces();
        Utils::PrintEntity(entity);
        UtilsDecreaseTabSpaces();
        printf("%s}\n", Utils::TabSpaces.c_str());
    }
    UtilsDecreaseTabSpaces();
    printf("%s}\n", Utils::TabSpaces.c_str());
}