# Copyright (c) 2020 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""This is a hacky tool to run the bundler, extract the asset hashes, and
insert them directly into the World::testInitialize() code.  I expect
this utility to break with future updates, but it's been very helpful
so far for quickly testing changes to Wasm scripts.  -- Turtle1331"""

import os
import sys
import subprocess

while '.git' not in os.listdir():
    os.chdir('..')

os.chdir('builddir')

bundler = subprocess.run([f"./bundler/mondradiko-bundler"], capture_output=True)
bundle = bundler.stderr
if bundler.returncode != 0:
    print("Error from bundler:")
    sys.stderr.write(bundle.decode("utf-8"))
    exit()

s = b"Created material asset with ID 0x"
i = bundle.find(s) + len(s)
first_hash = bundle[i:i+8]

s = b"Added ScriptAsset: 0x"
i = bundle.find(s) + len(s)
second_hash = bundle[i:i+8]

os.chdir('../core/world')

with open(f"World.cc", "rb") as f:
    buf = bytearray(f.read())

i = 1721
buf[i:i+8] = first_hash

i = 2057
buf[i:i+8] = second_hash

with open(f"World.cc", "wb") as f:
    f.write(buf)