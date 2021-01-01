# Copyright (c) 2020 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

def print_usage(arg1):
  print("Usage:")
  print(" ", arg1, "[out_cpp_header.h]", "[in_component_definition.toml ... ]")

def main():
  import sys

  if len(sys.argv) < 3:
    print_usage(sys.argv[0])
    sys.exit(1)
  
  src_path = sys.argv[1]
  # component_paths = sys.argv[3:]

  src = open(src_path, "w")
  # TODO(marceline-cramer) Actually generate link definitions
  src.close()

if __name__ == "__main__":
  main()
