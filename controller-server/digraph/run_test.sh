#!/bin/bash

make clean

tput reset

set -e

make

tput reset

valgrind ./digraph-test