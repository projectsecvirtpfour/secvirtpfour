#ifndef VIRT_P4RT_UTILS_H
#define VIRT_P4RT_UTILS_H

namespace Utils {

extern bool EnablePrints;
extern std::string TabSpaces;
extern int TabIncrease;

//Returns a string representing the bytes of the given string
//Such as {8,0,0,0,1,2} -> "\010\000\000\000\001\002"
std::string StringBytes(const std::string& str);

std::string UpdateTypeToString(const p4::v1::Update_Type update);
std::string WriteRequestAtomicityToString(const p4::v1::WriteRequest_Atomicity atomicity);

void PrintFieldMatch(const p4::v1::FieldMatch& match);
void PrintAction(const p4::v1::Action& action);
void PrintTableAction(const p4::v1::TableAction& table_action);
void PrintExternEntry(const p4::v1::ExternEntry& extern_entry);
void PrintTableEntry(const p4::v1::TableEntry& table_entry);
void PrintActionProfileMember(const p4::v1::ActionProfileMember& action_profile_member);
void PrintActionProfileGroup(const p4::v1::ActionProfileGroup& action_profile_group);
void PrintMeterEntry(const p4::v1::MeterEntry& meter_entry);
void PrintDirectMeterEntry(const p4::v1::DirectMeterEntry& direct_meter_entry);
void PrintCounterEntry(const p4::v1::CounterEntry& counter_entry);
void PrintDirectCounterEntry(const p4::v1::DirectCounterEntry& direct_counter_entry);
void PrintPacketReplicationEngineEntry(const p4::v1::PacketReplicationEngineEntry& packet_replication_engine_entry);
void PrintValueSetEntry(const p4::v1::ValueSetEntry& value_set_entry);
void PrintRegisterEntry(const p4::v1::RegisterEntry& register_entry);
void PrintDigestEntry(const p4::v1::DigestEntry& digest_entry);
void PrintEntity(const p4::v1::Entity& entity);
void PrintUpdate(const p4::v1::Update& update);
void PrintWriteRequest(const p4::v1::WriteRequest* request);
void PrintReadRequest(const p4::v1::ReadRequest* request);

void PrintReadResponse(const p4::v1::ReadResponse* response);

} //namespace Utils

#endif