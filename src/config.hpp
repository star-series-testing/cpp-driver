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

#ifndef __CASS_CONFIG_HPP_INCLUDED__
#define __CASS_CONFIG_HPP_INCLUDED__

#include <list>
#include <string>

#include "common.hpp"

namespace cass {

struct Config {
  int                    port_;
  std::string            version_;
  int                    compression_;
  size_t                 max_schema_agreement_wait_;
  size_t                 control_connection_timeout_;
  std::list<std::string> contact_points_;
  size_t                 thread_count_io_;
  size_t                 thread_count_callback_;
  size_t                 queue_size_io_;
  size_t                 queue_size_event_;
  size_t                 core_connections_per_host_;
  size_t                 max_connections_per_host_;
  size_t                 reconnect_wait_;
  LogCallback            log_callback_;

 public:
  Config() :
      port_(9042),
      version_("3.0.0"),
      compression_(0),
      max_schema_agreement_wait_(10),
      control_connection_timeout_(10),
      thread_count_io_(1),
      thread_count_callback_(4),
      queue_size_io_(1024),
      queue_size_event_(256),
      core_connections_per_host_(1),
      max_connections_per_host_(2),
      reconnect_wait_(2000),
      log_callback_(nullptr)
  {}

  void log_callback(LogCallback callback) {
    log_callback_ = callback;
  }

  size_t thread_count_io() const {
    return thread_count_io_;
  }

  size_t queue_size_io() const {
    return queue_size_io_;
  }

  size_t queue_size_event() const {
    return queue_size_event_;
  }

  size_t core_connections_per_host() const {
    return core_connections_per_host_;
  }

  size_t max_connections_per_host() const {
    return max_connections_per_host_;
  }

  size_t reconnect_wait() const {
    return reconnect_wait_;
  }

  const std::list<std::string>& contact_points() const {
    return contact_points_;
  }

  int port() const {
    return port_;
  }

  CassError
  option(
      CassOption   option,
      const void* value,
      size_t      size) {
    int int_value = *(reinterpret_cast<const int*>(value));

    switch (option) {
      case CASS_OPTION_THREADS_IO:
        thread_count_io_ = int_value;
        break;

      case CASS_OPTION_THREADS_CALLBACK:
        thread_count_callback_ = int_value;
        break;

      case CASS_OPTION_CONTACT_POINT_ADD:
        contact_points_.push_back(
            std::string(reinterpret_cast<const char*>(value), size));
        break;

      case CASS_OPTION_PORT:
        port_ = int_value;
        break;

      case CASS_OPTION_CQL_VERSION:
        version_.assign(reinterpret_cast<const char*>(value), size);
        break;

      case CASS_OPTION_COMPRESSION:
        compression_ = int_value;
        break;

      case CASS_OPTION_CONTROL_CONNECTION_TIMEOUT:
        control_connection_timeout_ = int_value;
        break;

      case CASS_OPTION_SCHEMA_AGREEMENT_WAIT:
        max_schema_agreement_wait_ = int_value;
        break;
      
      default:
        return CASS_ERROR_LIB_INVALID_OPTION;
    }

    return CASS_OK;
  }
};

} // namespace cass

#endif
