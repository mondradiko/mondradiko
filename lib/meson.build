# Copyright (c) 2020 Marceline Cramer
# SPDX-License-Identifier: LGPL-3.0-or-later

third_party_src = [
    'library_implementation.cc',
    'tracy/TracyClient.cpp'
]

third_party_deps = [
    vulkan,
    cpp.find_library('pthread', required: true),
    cpp.find_library('dl', required: true)
]

third_party_lib = library(
    'third-party-deps',
    third_party_src,
    dependencies: third_party_deps
)
