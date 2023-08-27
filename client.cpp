#include "client.hpp"
#include <grpcpp/grpcpp.h>
#include <thread>

std::string Endpoint::to_string() const {
  return std::format("{}:{}", host, port);
}

Client::Client(Endpoint transport) :
    stub_(helloworld::Greeter::NewStub(grpc::CreateChannel(transport.to_string(), grpc::InsecureChannelCredentials()))) {}

concurrency::task<std::string> Client::SayHello(std::string const& user) {
  // Data we are sending to the server.
  helloworld::HelloRequest request;
  request.set_name(user);

  // Container for the data we expect from the server.
  helloworld::HelloReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

  // The actual RPC.
  grpc::Status status;
  stub_->SayHello(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    co_return reply.message();
  } else {
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    co_return "RPC failed";
  }
}