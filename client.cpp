#include "client.hpp"
#include <grpcpp/grpcpp.h>
#include <thread>
#include <condition_variable>
#include <mutex>

std::string Endpoint::to_string() const {
  return std::format("{}:{}", host, port);
}

Client::Client(Endpoint transport) :
    stub_(helloworld::Greeter::NewStub(grpc::CreateChannel(transport.to_string(), grpc::InsecureChannelCredentials()))) {}

std::string Client::SayHello(std::string const& user) {
  // Data we are sending to the server.
  helloworld::HelloRequest request;
  request.set_name(user);

  // Container for the data we expect from the server.
  helloworld::HelloReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

  std::cout << "Started on thread ID " << std::this_thread::get_id() << '\n';
  // The actual RPC.
  std::mutex              mu;
  std::condition_variable cv;
  bool                    done = false;
  grpc::Status status;
  stub_->async()->SayHello(&context, &request, &reply, [&status, &mu, &done, &cv](grpc::Status st) {
    status = std::move(st);
    std::lock_guard<std::mutex> lock(mu);
    done = true;
    cv.notify_one();
    std::cout << "Called back on thread ID " << std::this_thread::get_id() << '\n';
  });

  std::unique_lock<std::mutex> lock(mu);
  while (!done) {
    cv.wait(lock);
  }
  // Act upon its status.
   if (status.ok()) {
     return reply.message();
   } else {
     std::cout << status.error_code() << ": " << status.error_message() << std::endl;
     return "RPC failed";
   }
}