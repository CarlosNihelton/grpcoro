#include <format>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "helloworld.grpc.pb.h"

#include "config.hpp"

class GreeterService final : public helloworld::Greeter::Service {
  grpc::Status SayHello(grpc::ServerContext* ctx, helloworld::HelloRequest const* req, helloworld::HelloReply* reply) {
    std::cout << "Received request from name: " << req->name() << '\n';
    auto msg = std::format("Hello my wonderful and exceptional {}", req->name());
    reply->set_message(msg);
    return grpc::Status::OK;
  }
};

void RunServer(uint16_t port) {
  std::string    server_address = std::format("0.0.0.0:{}", port);
  GreeterService service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char* argv[]) {
  RunServer(Config::port);
  return 0;
}