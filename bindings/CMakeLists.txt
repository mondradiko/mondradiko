# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

# Create components/ binary directory
add_subdirectory(components)

api_bindings(SCRIPT_LINKERS
  components/Transform.toml
)

add_library(mondradiko-api-components OBJECT ${SCRIPT_LINKERS})
# TODO(marceline-cramer): Don't expose third-party dependencies to the public interface
target_link_libraries(mondradiko-api-components mondradiko-lib)
