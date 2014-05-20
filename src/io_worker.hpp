/*
  Copyright 2014 DataStax

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef __CASS_IO_WORKER_HPP_INCLUDED__
#define __CASS_IO_WORKER_HPP_INCLUDED__

#include <uv.h>
#include <string>
#include <unordered_map>
#include "async_queue.hpp"
#include "config.hpp"
#include "host.hpp"
#include "ssl_context.hpp"
#include "spsc_queue.hpp"
#include "request_future.hpp"

namespace cass {

class Pool;
struct Session;

struct IOWorker {
    typedef std::shared_ptr<Pool>  PoolPtr;
    typedef std::unordered_map<Host, PoolPtr> PoolCollection;

    struct ReconnectRequest {
        ReconnectRequest(IOWorker* io_worker, Host host)
          : io_worker(io_worker)
          , host(host) { }

        IOWorker* io_worker;
        Host host;
    };

    struct Payload {
        enum Type {
          ADD_POOL,
          REMOVE_POOL,
        };
        Type type;
        Host host;
    };

    Session* session_;
    uv_thread_t thread;
    uv_loop_t* loop;
    SSLContext* ssl_context;
    PoolCollection pools;
    bool is_shutdown_;

    const Config& config_;
    AsyncQueue<SPSCQueue<RequestFuture*>> request_future_queue_;
    AsyncQueue<SPSCQueue<Payload>> event_queue_;

    explicit
    IOWorker(Session* session, const Config& config)
      : session_(session)
      , loop(uv_loop_new())
      , ssl_context(nullptr)
      , is_shutdown_(false)
      , config_(config)
      , request_future_queue_(config.queue_size_io())
      , event_queue_(config.queue_size_event()) { }

    int init() {
      int rc = request_future_queue_.init(loop, this, &IOWorker::on_execute);
      if(rc != 0) {
        return rc;
      }
      return event_queue_.init(loop, this, &IOWorker::on_event);
    }

    void add_pool_q(const Host& host) {
      Payload payload;
      payload.type = Payload::ADD_POOL;
      payload.host = host;
      event_queue_.enqueue(payload);
    }

    void add_pool(Host host);

    void remove_pool_q(const Host& host) {
      Payload payload;
      payload.type = Payload::REMOVE_POOL;
      payload.host = host;
    }

    bool execute(RequestFuture* request_future) {
      return request_future_queue_.enqueue(request_future);
    }

    void try_next_host(RequestFuture* request_future);

    void on_close(Host host);
    static void on_reconnect(Timer* timer);
    static void on_execute(uv_async_t* data, int status);
    static void on_event(uv_async_t* async, int status);

    static void
    run(
        void* data) {
      IOWorker* worker = reinterpret_cast<IOWorker*>(data);
      uv_run(worker->loop, UV_RUN_DEFAULT);
    }

    void
    run() {
      uv_thread_create(&thread, &IOWorker::run, this);
    }

    void
    stop() {
      uv_stop(loop);
    }

    void
    join() {
      uv_thread_join(&thread);
    }

    ~IOWorker() {
      uv_loop_delete(loop);
    }
};

} // namespace cass

#endif
