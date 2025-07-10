cmake_minimum_required(VERSION 3.16)
project(grpc_hello)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages using pkg-config (works better with Yocto)
find_package(PkgConfig REQUIRED)

# Find protobuf
pkg_check_modules(PROTOBUF REQUIRED protobuf)

# Find gRPC
pkg_check_modules(GRPC REQUIRED grpc++ grpc)

# Find protoc and grpc_cpp_plugin (should be in PATH from native packages)
find_program(PROTOC_EXECUTABLE protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

if(NOT PROTOC_EXECUTABLE)
    message(FATAL_ERROR "protoc not found - make sure protobuf-native is in DEPENDS")
endif()

if(NOT GRPC_CPP_PLUGIN)
    message(FATAL_ERROR "grpc_cpp_plugin not found - make sure grpc-native is in DEPENDS")
endif()

# Proto file
set(PROTO_FILES hello.proto)

# Generate protobuf and gRPC files
foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)

    # Generate protobuf files
    add_custom_command(
        OUTPUT ${proto_name}.pb.cc ${proto_name}.pb.h
        COMMAND ${PROTOC_EXECUTABLE}
        ARGS --cpp_out=. --proto_path=${CMAKE_CURRENT_SOURCE_DIR} ${proto_file}
        DEPENDS ${proto_file}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Generate gRPC files
    add_custom_command(
        OUTPUT ${proto_name}.grpc.pb.cc ${proto_name}.grpc.pb.h
        COMMAND ${PROTOC_EXECUTABLE}
        ARGS --grpc_out=. --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
             --proto_path=${CMAKE_CURRENT_SOURCE_DIR} ${proto_file}
        DEPENDS ${proto_file}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    list(APPEND PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc)
    list(APPEND PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h)
    list(APPEND GRPC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc)
    list(APPEND GRPC_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h)
endforeach()

# Include generated files directory
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Server executable
add_executable(grpc_server grpc_server.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_include_directories(grpc_server PRIVATE ${PROTOBUF_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})
target_compile_options(grpc_server PRIVATE ${PROTOBUF_CFLAGS_OTHER} ${GRPC_CFLAGS_OTHER})
target_link_libraries(grpc_server ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

# For reflection support, we need to find and link grpc++_reflection
find_library(GRPC_REFLECTION_LIB grpc++_reflection)
if(GRPC_REFLECTION_LIB)
    target_link_libraries(grpc_server ${GRPC_REFLECTION_LIB})
else()
    message(WARNING "grpc++_reflection library not found - reflection will not work")
endif()

# Client executable
add_executable(grpc_client grpc_client.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_include_directories(grpc_client PRIVATE ${PROTOBUF_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})
target_compile_options(grpc_client PRIVATE ${PROTOBUF_CFLAGS_OTHER} ${GRPC_CFLAGS_OTHER})
target_link_libraries(grpc_client ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

# Install
install(TARGETS grpc_server grpc_client
    RUNTIME DESTINATION bin
)
