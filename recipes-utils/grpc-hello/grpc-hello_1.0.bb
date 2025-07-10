SUMMARY = "gRPC Hello World Application"
DESCRIPTION = "Example gRPC client/server application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# Version and source
PV = "1.0"
SRC_URI = "file://grpc_server.cpp \
           file://grpc_client.cpp \
           file://hello.proto \
           file://CMakeLists.txt"

S = "${WORKDIR}"

# Dependencies - need both target libs and native build tools
DEPENDS = "grpc protobuf-native grpc-native pkgconfig-native"

# Use CMake
inherit cmake pkgconfig

# C++17 requirement
EXTRA_OECMAKE += "-DCMAKE_CXX_STANDARD=17"

# Simple single package with both executables
FILES:${PN} = "${bindir}/grpc_server ${bindir}/grpc_client"

# Runtime dependencies
RDEPENDS:${PN} = "grpc"
