#include <format>
#include "client.hpp"
#include "config.hpp"

int main(int argc, char* argv[]) {
  Client      client(Endpoint{"127.0.0.1", Config::port});
  std::string user("I Am");
  std::string reply = client.SayHello(user);
  std::cout << "Received: " << reply << std::endl;

  return 0;
}