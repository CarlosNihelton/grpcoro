#pragma once

namespace grpcoro {
  template <typename Req, typename Resp, typename Async>
  struct GrpcUnaryAwaitable : std::suspend_always {
    grpc::ClientContext* context;
    Req const*           request;
    Resp*                response;
    // The async member of the client stub;
    Async*               async;
    // The member function of the async stub - i.e the Callback API version of the RPC - we want to call
    using Rpc = void (Async::*)(grpc::ClientContext* context, Req const* request, Resp* response, std::function<void(::grpc::Status)>);
    Rpc rpc;

    // The result of the RPC.
    grpc::Status result;

    void await_suspend(std::coroutine_handle<> h) {
      (async->*rpc)(context, request, response, [this, h](grpc::Status status) {
        result = status;
        std::cout << "Resumed on thread ID " << std::this_thread::get_id() << '\n';
        // From now on is UB to access the this pointer.
        h.resume();
      });
    }
    grpc::Status await_resume() { return result; }
  };
}