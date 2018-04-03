/*
  Copyright (c) DataStax, Inc.

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

#ifndef __CASS_REQUEST_PROCESSOR_MANAGER_HPP_INCLUDED__
#define __CASS_REQUEST_PROCESSOR_MANAGER_HPP_INCLUDED__

#include "host.hpp"
#include "ref_counted.hpp"
#include "request_processor.hpp"

namespace cass {

/**
 * A manager for one or more request processor that will process requests from
 * the session
 */
class RequestProcessorManager : public RefCounted<RequestProcessorManager> {
public:
  typedef SharedRefPtr<RequestProcessorManager> Ptr;

  /**
   * Constructor; Don't use directly use the initializer
   *
   * Handles initialization and connections for each request processor to the
   * pre-established hosts from the control connection
   *
   * @param num_threads Number of request processors to create for handling the
   *                    processing of client requests
   */
  RequestProcessorManager(size_t num_threads);

  /**
   * Close/Terminate the request processors
   *
   * @param A key to restrict access to the method
   */
  void close();
  /**
   * Closes the request processor(s) handles (thread-safe)
   *
   * @param A key to restrict access to the method
   */
  void close_handles();
  /**
   * Waits for the request processor(s) to exit (thread-safe)
   *
   * @param A key to restrict access to the method
   */
  void join();

  /**
   * Update the current keyspace being used for requests
   *
   * @param keyspace New current keyspace to utilize
   */
  void keyspace_update(const String& keyspace);
  /**
   * Add a new host to the request processors
   *
   * @param host New host to be added
   */
  void notify_host_add_async(const Host::Ptr& host);
  /**
   * Remove a host from the request processors
   *
   * @param host Host to be removed
   */
  void notify_host_remove_async(const Host::Ptr& host);
  /**
   * Update the token map being used for the requests
   *
   * @param token_map Update token map (do not clone)
   */
  void notify_token_map_update_async(const TokenMap* token_map);
  /**
   * Notify one of the request processors that a new request is available
   *
   * NOTE: The request processor selected during the round robin process may or
   *       may not be notified if it is currently flushing requests from the
   *       queue.
   */
  void notify_request_async();

public:
  class Protected {
    friend class RequestProcessorManagerInitializer;
    Protected() { }
    Protected(Protected const&) { }
  };

  /**
   * Add request processor to the manager
   *
   * @param request_processor A request processor to add to the manager
   * @param A key to restrict access to the method
   */
  void add_request_processor(const RequestProcessor::Ptr& request_processor,
                             Protected);

private:
  void internal_add_request_processor(const RequestProcessor::Ptr& request_processor);
  void internal_close();
  void internal_close_handles();
  void internal_join();
  void internal_notify_host_add_async(const Host::Ptr& host);
  void internal_notify_host_remove_async(const Host::Ptr& host);
  void internal_keyspace_update(const String& keyspace);
  void internal_notify_token_map_update_async(const TokenMap* token_map);
  void internal_notify_request_async();

private:
  Atomic<size_t> current_;
  DynamicArray<RequestProcessor::Ptr> threads_;
};

} // namespace cass

#endif // __CASS_REQUEST_PROCESSOR_MANAGER_HPP_INCLUDED__
