#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>  // Add this line
#include "hello.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerWriter;
using hello::Greeter;
using hello::HelloRequest;
using hello::HelloReply;

// Logic and data behind the server's behavior
class GreeterServiceImpl final : public Greeter::Service {
    Status SayHello(ServerContext* context, const HelloRequest* request,
                   HelloReply* reply) override {
        std::string prefix("Hello ");
        reply->set_message(prefix + request->name());
        reply->set_count(1);
        return Status::OK;
    }

    Status SayHelloStream(ServerContext* context, const HelloRequest* request,
                         ServerWriter<HelloReply>* writer) override {
        std::string prefix("Hello ");
        for (int i = 0; i < 5; ++i) {
            HelloReply reply;
            reply.set_message(prefix + request->name() + " #" + std::to_string(i));
            reply.set_count(i);
            writer->Write(reply);

            // Simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    // Enable reflection
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate
    builder.RegisterService(&service);

    // Finally assemble the server
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
