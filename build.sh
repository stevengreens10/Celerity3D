#!/bin/bash
cmake --build cmake-build-debug
# Issue with building glew, building again solves it
cmake --build cmake-build-debug
