# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

add_library(mondradiko-log STATIC log.cc)

set_target_properties(mondradiko-log PROPERTIES FOLDER "components")
