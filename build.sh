#! /bin/bash

OUTPUT_DIR="bin/"
mkdir -p "$OUTPUT_DIR"
OUTPUT_FILE="pack8xp"
SOURCE_FILES="pack8xp.cpp"
WARNINGS="-Wall -Wno-unused-function"
LIBRARIES="" #-lm"
LIB_DIR=""
INCLUDE_DIR=""
COMPILE_OPTIONS="-lstdc++ -std=c++11"
MACRO_SWITCHES=""
DEBUG_SYMBOLS="" #-ggdb"
DEBUG_SYMBOLS=""
OPTIMIZATION_LEVEL="-O2" # 0 - none
                         # 1 - moderate
                         # 2 - extensive
                         # 3 - aggressive to the point of being able to degrade code again, needs performance testing.
gcc $SOURCE_FILES $INCLUDE_DIR $MACRO_SWITCHES $COMPILE_OPTIONS $OPTIMIZATION_LEVEL -o $OUTPUT_DIR$OUTPUT_FILE $DEBUG_SYMBOLS $WARNINGS $LIB_DIR $LIBRARIES
