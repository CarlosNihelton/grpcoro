#include <format>
#include <thread>
#include "client.hpp"
#include "config.hpp"

int main(int argc, char* argv[]) {
  Client client(Endpoint{"127.0.0.1", Config::port});
  std::cout << "Running on main thread ID " << std::this_thread::get_id() << '\n';
  std::string user("I Am");
  std::string reply = client.SayHello(user).get();
  std::cout << "Received: " << reply << std::endl;

  return 0;
}