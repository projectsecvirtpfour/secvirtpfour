#include "../include/pcheaders.h"
#include "../proto/p4runtime.grpc.pb.h"
#include "../include/PropertyList.h"
#include "../include/P4InfoDesc.h"
#include "../include/RuntimeCFG.h"
#include "../include/P4RuntimeImpl.h"

typedef struct _certs{
    std::string key;
    std::string cert;
    std::string root;
} Certificates;

Certificates getCertificates(){
    const std::vector<std::string> Paths = {"certificates/server.key", "certificates/server.crt", "certificates/VirtP4.crt"};
    std::string fileReads[3];

    for(int i = 0; i < 3; i++){
        FILE *fp = fopen(Paths[i].c_str(), "r");
        if(!fp){
            fprintf(stderr, "Error: Could not open file \"%s\"\n", Paths[i].c_str());
            exit(1);
        }
        while(!feof(fp)){
            char c = fgetc(fp);
            if(!feof(fp))
                fileReads[i].push_back(c);
        }
        fclose(fp);
    }

    return Certificates({fileReads[0], fileReads[1], fileReads[2]});
}

void parseArgs(int argc, char *argv[], bool *help, std::string* clientFilePath, std::string* virtMapFilePath, std::string* virtP4InfoPath){
    int i = 1;
    while(i < argc){
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
            *help = true;
        }
        else if(!strcmp(argv[i], "--clientdata")){
            i++;
            if(i < argc)
                *clientFilePath = std::string(argv[i]);
        }
        else if(!strcmp(argv[i], "--virtmap")){
            i++;
            if(i < argc)
                *virtMapFilePath = std::string(argv[i]);
        }
        else if(!strcmp(argv[i], "--virtp4info")){
            i++;
            if(i < argc)
                *virtP4InfoPath = std::string(argv[i]);
        }
        else{
            fprintf(stderr, "Warning: Unrecognized option \"%s\"\n", argv[i]);
        }
        i++;
    }
}

void printHelp(char progName[]){
    printf("Usage: %s [OPTS] --virtp4info Virt.p4info.txt --virtmap VirtMap.cfg --clientdata ClientFile.cfg\n", progName);
    printf("\nOPTS:\n");
    printf("(--help | -h) : Displays help message\n");
}

void RunServerThread(const std::string serverAddr, Certificates certificates, P4RuntimeImpl* service){ 
    grpc::ServerBuilder builder;

	grpc::SslServerCredentialsOptions::PemKeyCertPair keycert =
	{
		certificates.key,
		certificates.cert
	};

	grpc::SslServerCredentialsOptions sslOps;
	sslOps.pem_root_certs = certificates.root;
	sslOps.pem_key_cert_pairs.push_back ( keycert );
    sslOps.client_certificate_request = GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;
    sslOps.force_client_auth = true;

	builder.AddListeningPort(serverAddr, grpc::SslServerCredentials(sslOps));
	builder.RegisterService(service);

	std::unique_ptr < grpc::Server > server ( builder.BuildAndStart() );
	std::cout << "Server listening on " << serverAddr << std::endl;

	server->Wait();
}

int main(int argc, char *argv[]){
    const std::string serverAddr = "localhost:9559";
    
    std::string clientFilePath = "";
    std::string virtMapFilePath = "";
    std::string virtP4InfoFilePath = "";
    bool help = false;

    parseArgs(argc, argv, &help, &clientFilePath, &virtMapFilePath, &virtP4InfoFilePath);

    if(help || clientFilePath == "" || virtMapFilePath == "" || virtP4InfoFilePath == ""){
        printHelp(argv[0]);
        return 0;
    }

    Certificates certificates = getCertificates();
    RuntimeCFG runtimeCFG(clientFilePath, virtMapFilePath, virtP4InfoFilePath);

    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    printf("Waiting for connection...\n");
    channel->WaitForConnected(gpr_time_from_seconds(3600, gpr_clock_type::GPR_TIMESPAN));
    if(channel->GetState(false) != GRPC_CHANNEL_READY){
        fprintf(stderr, "Could not stabilish connection to %s\n", serverAddr.c_str());
        return 1;
    }
    std::shared_ptr<p4::v1::P4Runtime::Stub> stub = p4::v1::P4Runtime::NewStub(channel);
    
    grpc::ClientContext cContext;
    std::shared_ptr<grpc::ClientReaderWriter<p4::v1::StreamMessageRequest, p4::v1::StreamMessageResponse>> stream(stub->StreamChannel(&cContext));
    p4::v1::StreamMessageRequest smr;
    smr.mutable_arbitration()->mutable_election_id()->set_low(1);
    stream->Write(smr);

    P4RuntimeImpl service(&runtimeCFG, stub);

    std::thread serverThread(RunServerThread, serverAddr, certificates, &service);

    serverThread.join();

    return 0;
}