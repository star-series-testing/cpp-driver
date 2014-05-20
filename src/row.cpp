/*
  Copyright (c) 2014 DataStax

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

#include "cassandra.hpp"
#include "row.hpp"

extern "C" {

CassError
cass_row_get_column(
    const CassRow* row,
    cass_size_t index,
    const CassValue** value) {
  const cass::Row* internal_row = row->from();
  if(index >= internal_row->size()) {
    return CASS_ERROR_LIB_BAD_PARAMS; // TODO(mpenick): Figure out error codes
  }
  *value = CassValue::to(&(*internal_row)[index]);
  return CASS_OK;
}

} // extern "C"
