#pragma once
#include <cstdint>
#include <memory>
#include <string>

#include "helloworld.grpc.pb.h"

struct Endpoint {
  std::string  host;
  std::int16_t port;

  std::string to_string() const;
};

class Client {
 private:
  std::unique_ptr<helloworld::Greeter::Stub> stub_;

 public:
  explicit Client(Endpoint transport);

  std::string SayHello(std::string const& user);
};