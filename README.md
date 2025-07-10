# gRPC Hello World Example

A simple gRPC client/server application demonstrating basic RPC patterns in a Yocto/embedded Linux environment.

It includes:

- **Server**: Listens on port 50051 and responds to greeting requests
- **Client**: Connects to the server and demonstrates both simple and streaming calls
- **Protocol**: Defined using Protocol Buffers with a simple greeting service

## Project Structure

```
meta-grpc-example/
├── conf/
│   └── layer.conf                    # Yocto layer configuration
├── recipes-utils/
│   └── grpc-hello/
│       ├── files/
│       │   ├── CMakeLists.txt        # Build configuration
│       │   ├── hello.proto           # Protocol buffer definition
│       │   ├── grpc_server.cpp       # Server implementation
│       │   └── grpc_client.cpp       # Client implementation
│       └── grpc-hello_1.0.bb         # BitBake recipe
├── COPYING.MIT                       # MIT license
└── README.md                         # This file
```

## Building

### Yocto/BitBake

1. Add the layer to your build:
   ```bash
   bitbake-layers add-layer meta-grpc-example
   ```

2. Build the package:
   ```bash
   bitbake grpc-hello
   ```

### Running the Server

```bash
# Start the gRPC server (listens on all interfaces, port 50051)
./grpc_server
```

The server will output:
```
Server listening on 0.0.0.0:50051
```

### Running the Client

```bash
# Connect to local server with default settings
./grpc_client

# Connect to remote server
./grpc_client <server_address:port>

# Connect with custom name
./grpc_client <server_address:port> "Your Name"
```

Example output:
```
Greeter received: Hello Embedded User

Streaming example:
Received: Hello Embedded User #0 (count: 0)
Received: Hello Embedded User #1 (count: 1)
Received: Hello Embedded User #2 (count: 2)
Received: Hello Embedded User #3 (count: 3)
Received: Hello Embedded User #4 (count: 4)
```

## Protocol Definition

The service is defined in `hello.proto`:

```protobuf
service Greeter {
  rpc SayHello (HelloRequest) returns (HelloReply) {}
  rpc SayHelloStream (HelloRequest) returns (stream HelloReply) {}
}
```

- **SayHello**: Simple unary call returning a greeting
- **SayHelloStream**: Returns 5 streaming responses with a 500ms delay between each

### Testing with grpcurl

Since reflection is enabled, you can test the server using `grpcurl`:

```bash
# List available services
grpcurl -plaintext localhost:50051 list

# Call the SayHello method
grpcurl -plaintext -d '{"name":"Test"}' localhost:50051 hello.Greeter/SayHello

# Test streaming
grpcurl -plaintext -d '{"name":"Stream Test"}' localhost:50051 hello.Greeter/SayHelloStream
```
## License

This project is licensed under the MIT License - see the [COPYING.MIT](COPYING.MIT) file for details.
