#include <format>
#include <thread>
#include "client.hpp"
#include "config.hpp"

int main(int argc, char* argv[]) {
  Client client(Endpoint{"127.0.0.1", Config::port});
  std::cout << "Running on main thread ID " << std::this_thread::get_id() << '\n';
  std::string user("I Am");

  static constexpr size_t                     calls = 10000;
  std::vector<concurrency::task<std::string>> queue;
  queue.reserve(calls);
  for (int i = 0; i < calls; i++) {
    queue.emplace_back(client.SayHello(std::format("{} {}", user, i)));
  }

  auto joint = concurrency::when_all(std ::begin(queue), std::end(queue));
  joint.wait();

  std::string reply = client.SayHello(user).get();
  std::cout << "Received: " << reply << std::endl;

  return 0;
}