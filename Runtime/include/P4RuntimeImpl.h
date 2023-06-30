#ifndef P4RUNTIME_IMPL_H
#define P4RUNTIME_IMPL_H

class P4RuntimeImpl final : public p4::v1::P4Runtime::Service {
    public:
        P4RuntimeImpl(RuntimeCFG *rtcfg, std::shared_ptr<p4::v1::P4Runtime::Stub> stub);

        // Update one or more P4 entities on the target.
        grpc::Status Write(grpc::ServerContext* context, const p4::v1::WriteRequest* request, p4::v1::WriteResponse* response) override;
        // Read one or more P4 entities from the target.
        grpc::Status Read(grpc::ServerContext* context, const p4::v1::ReadRequest* request, grpc::ServerWriter< p4::v1::ReadResponse>* writer) override;
        // Sets the P4 forwarding-pipeline config.
        grpc::Status SetForwardingPipelineConfig(grpc::ServerContext* context, const p4::v1::SetForwardingPipelineConfigRequest* request, p4::v1::SetForwardingPipelineConfigResponse* response) override;
        // Gets the current P4 forwarding-pipeline config.
        grpc::Status GetForwardingPipelineConfig(grpc::ServerContext* context, const p4::v1::GetForwardingPipelineConfigRequest* request, p4::v1::GetForwardingPipelineConfigResponse* response) override;
        // Represents the bidirectional stream between the controller and the
        // switch (initiated by the controller), and is managed for the following
        // purposes:
        // - connection initiation through client arbitration
        // - indicating switch session liveness: the session is live when switch
        //   sends a positive client arbitration update to the controller, and is
        //   considered dead when either the stream breaks or the switch sends a
        //   negative update for client arbitration
        // - the controller sending/receiving packets to/from the switch
        // - streaming of notifications from the switch
        grpc::Status StreamChannel(grpc::ServerContext* context, grpc::ServerReaderWriter< p4::v1::StreamMessageResponse, p4::v1::StreamMessageRequest>* stream) override;
        grpc::Status Capabilities(grpc::ServerContext* context, const p4::v1::CapabilitiesRequest* request, p4::v1::CapabilitiesResponse* response) override;

    private:
        RuntimeCFG *m_runtimeCFG;
        std::shared_ptr<p4::v1::P4Runtime::Stub> m_stub;
};

#endif