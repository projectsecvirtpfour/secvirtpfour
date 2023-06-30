#include "../include/pcheaders.h"
#include "../proto/p4runtime.grpc.pb.h"
#include "../include/PropertyList.h"
#include "../include/P4InfoDesc.h"
#include "../include/RuntimeCFG.h"
#include "../include/P4RuntimeImpl.h"
#include "../include/Utils.h"


P4RuntimeImpl::P4RuntimeImpl(RuntimeCFG *rtcfg, std::shared_ptr<p4::v1::P4Runtime::Stub> stub){
  this->m_runtimeCFG = rtcfg;
  this->m_stub = stub;
}

grpc::Status P4RuntimeImpl::Write(grpc::ServerContext* context, const p4::v1::WriteRequest* request, p4::v1::WriteResponse* response) {
  const std::string identity = std::string(context->auth_context().get()->GetPeerIdentity().begin()->begin(), context->auth_context().get()->GetPeerIdentity().begin()->end());
  
  printf("Write from: %s\n", identity.c_str());
  Utils::PrintWriteRequest(request);
  
  grpc::Status status = grpc::Status(grpc::StatusCode::OK, "");

  try{
    p4::v1::WriteRequest r = this->m_runtimeCFG->Translate(identity, request);
    printf("Translated:\n");
    Utils::PrintWriteRequest(&r);

    p4::v1::WriteResponse result;
    grpc::ClientContext scontext;
    status = this->m_stub->Write(&scontext, r, &result);
    printf("Status Code: %d\nMSG: %s\n", (int) status.error_code(), status.error_message().c_str());

  }
  catch (const char* err){
    if(!strcmp(err, RUNTIME_EXCEPT_CLIENT_CFG_NOT_FOUND_MSG)){
      fprintf(stderr, "Client %s not found in RuntimeCFG! Ignoring request...\n", identity.c_str());
      return grpc::Status(grpc::StatusCode::UNKNOWN, "Credentials not found, please contact the administrator.");
    }
    else if(!strcmp(err, RUNTIME_EXCEPT_TABLE_FULL)){
      fprintf(stderr, "Resource exhausted on table insert!\n");
      return grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "Table is full.");
    }
    else if(!strcmp(err, RUNTIME_EXCEPT_MALFORMED_REQUEST)){
      fprintf(stderr, "Malformed write request!\n");
      return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid request.");
    }
    else{
      fprintf(stderr, "Unknown exception during request translation\n");
      return grpc::Status(grpc::StatusCode::UNKNOWN, "");
    }
  }

  if(status.error_code() == grpc::StatusCode::UNAVAILABLE)
    return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Manager not connected to the switch server. Please try again later or contact the system administrator.");
  return status;
}

grpc::Status P4RuntimeImpl::Read(grpc::ServerContext* context, const p4::v1::ReadRequest* request, grpc::ServerWriter< p4::v1::ReadResponse>* writer) {
  const std::string identity = std::string(context->auth_context().get()->GetPeerIdentity().begin()->begin(), context->auth_context().get()->GetPeerIdentity().begin()->end());
  
  printf("Read from: %s\n", identity.c_str());
  Utils::PrintReadRequest(request);
  
  grpc::Status status = grpc::Status(grpc::StatusCode::OK, "");

  try{
    p4::v1::ReadRequest r = this->m_runtimeCFG->Translate(identity, request);
    printf("Translated:\n");
    Utils::PrintReadRequest(&r);

    grpc::ClientContext scontext;
    std::unique_ptr<grpc::ClientReader<p4::v1::ReadResponse>> reader(m_stub->Read(&scontext, r));

    p4::v1::ReadResponse response;
    int i = 1;
    while (reader->Read(&response)) {
      printf("Response %d:\n", i);
      i++;
      Utils::PrintReadResponse(&response);
      p4::v1::ReadResponse translated = this->m_runtimeCFG->Translate(identity, &response);
      writer->Write(translated);
    }

    status = reader->Finish();
    printf("Status Code: %d\nMSG: %s\n", (int) status.error_code(), status.error_message().c_str());
  }
  catch (const char* err){
    if(!strcmp(err, RUNTIME_EXCEPT_CLIENT_CFG_NOT_FOUND_MSG)){
      fprintf(stderr, "Client %s not found in RuntimeCFG! Ignoring request...\n", identity.c_str());
      return grpc::Status(grpc::StatusCode::UNKNOWN, "Credentials not found, please contact the administrator.");
    }
    else if(!strcmp(err, RUNTIME_EXCEPT_TABLE_FULL)){
      fprintf(stderr, "Resource exhausted on table insert!\n");
      return grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "Table is full.");
    }
    else if(!strcmp(err, RUNTIME_EXCEPT_MALFORMED_REQUEST)){
      fprintf(stderr, "Malformed read request!\n");
      return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid request.");
    }
    else{
      fprintf(stderr, "Unknown exception during request translation\n");
      return grpc::Status(grpc::StatusCode::UNKNOWN, "");
    }
  }
  
  if(status.error_code() == grpc::StatusCode::UNAVAILABLE)
    return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Manager not connected to the switch server. Please try again later or contact the system administrator.");
  return status;
}

grpc::Status P4RuntimeImpl::SetForwardingPipelineConfig(grpc::ServerContext* context, const p4::v1::SetForwardingPipelineConfigRequest* request, p4::v1::SetForwardingPipelineConfigResponse* response) {
  grpc::Status status = grpc::Status(grpc::StatusCode::OK, "");

  if(status.error_code() == grpc::StatusCode::UNAVAILABLE)
    return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Manager not connected to the switch server. Please try again later or contact the system administrator.");
  return status;
}

grpc::Status P4RuntimeImpl::GetForwardingPipelineConfig(grpc::ServerContext* context, const p4::v1::GetForwardingPipelineConfigRequest* request, p4::v1::GetForwardingPipelineConfigResponse* response) {
  grpc::Status status = grpc::Status(grpc::StatusCode::OK, "");
  
  if(status.error_code() == grpc::StatusCode::UNAVAILABLE)
    return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Manager not connected to the switch server. Please try again later or contact the system administrator.");
  return status;
}

grpc::Status P4RuntimeImpl::StreamChannel(grpc::ServerContext* context, grpc::ServerReaderWriter< p4::v1::StreamMessageResponse, p4::v1::StreamMessageRequest>* stream) {
  //TODO: Require MasterArbitrationUpdate from each client before allowing write / read RPCs
  grpc::Status status = grpc::Status(grpc::StatusCode::OK, "");
  
  if(status.error_code() == grpc::StatusCode::UNAVAILABLE)
    return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Manager not connected to the switch server. Please try again later or contact the system administrator.");
  return status;
}

grpc::Status P4RuntimeImpl::Capabilities(grpc::ServerContext* context, const p4::v1::CapabilitiesRequest* request, p4::v1::CapabilitiesResponse* response) {
  grpc::Status status = grpc::Status(grpc::StatusCode::OK, "");

  if(status.error_code() == grpc::StatusCode::UNAVAILABLE)
    return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Manager not connected to the switch server. Please try again later or contact the system administrator.");
  return status;
}
