#!/bin/bash

cpplint --recursive --filter=-build/c++11 --quiet assets bundler client core log
