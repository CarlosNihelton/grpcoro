#include "client.hpp"

#include <coroutine>
#include <thread>
#include <type_traits>

#include <grpcpp/grpcpp.h>

#include "grpcoro.hpp"

std::string Endpoint::to_string() const {
  return std::format("{}:{}", host, port);
}

struct AsyncStub {
  using async_type = class helloworld::Greeter::Stub::async;
  async_type* me;
  auto        sayHello(grpc::ClientContext* context, helloworld::HelloRequest const* request, helloworld::HelloReply* response) const {
    return grpcoro::GrpcUnaryAwaitable<helloworld::HelloRequest, helloworld::HelloReply, async_type>{
               .context = context, .request = request, .response = response, .async = me, .rpc = &async_type::SayHello};
  }
};

concurrency::task<std::string> Client::SayHello(std::string const& user) {
  // Data we are sending to the server.
  helloworld::HelloRequest request;
  request.set_name(user);

  // Container for the data we expect from the server.
  helloworld::HelloReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

  std::cout << "Task started on thread ID " << std::this_thread::get_id() << '\n';
  // The actual RPC.
  const AsyncStub asyncStub{stub_->async()};
  grpc::Status    status = co_await asyncStub.sayHello(&context, &request, &reply);

  // Act upon its status.
  if (status.ok()) {
    co_return reply.message();
  } else {
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    co_return "RPC failed";
  }
}

Client::Client(Endpoint transport) :
    stub_(helloworld::Greeter::NewStub(grpc::CreateChannel(transport.to_string(), grpc::InsecureChannelCredentials()))) {}