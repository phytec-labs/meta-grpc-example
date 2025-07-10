#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;
using hello::Greeter;
using hello::HelloRequest;
using hello::HelloReply;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response
    std::string SayHello(const std::string& user) {
        // Data we are sending to the server
        HelloRequest request;
        request.set_name(user);

        // Container for the data we expect from the server
        HelloReply reply;

        // Context for the client
        ClientContext context;

        // The actual RPC
        Status status = stub_->SayHello(&context, request, &reply);

        // Act upon its status
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    // Example of client streaming RPC
    void SayHelloStream(const std::string& user) {
        HelloRequest request;
        request.set_name(user);

        ClientContext context;
        HelloReply reply;

        std::unique_ptr<ClientReader<HelloReply>> reader(
            stub_->SayHelloStream(&context, request));

        while (reader->Read(&reply)) {
            std::cout << "Received: " << reply.message()
                      << " (count: " << reply.count() << ")" << std::endl;
        }

        Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "SayHelloStream rpc failed." << std::endl;
        }
    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
    // Server address - change this to your embedded device IP
    std::string server_address("localhost:50051");

    if (argc > 1) {
        server_address = argv[1];
    }

    // Create a channel to the server
    GreeterClient greeter(grpc::CreateChannel(
        server_address, grpc::InsecureChannelCredentials()));

    std::string user("Embedded User");
    if (argc > 2) {
        user = argv[2];
    }

    // Simple RPC call
    std::string reply = greeter.SayHello(user);
    std::cout << "Greeter received: " << reply << std::endl;

    // Streaming RPC call
    std::cout << "\nStreaming example:" << std::endl;
    greeter.SayHelloStream(user);

    return 0;
}
