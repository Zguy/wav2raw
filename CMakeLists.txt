cmake_minimum_required (VERSION 2.6)

project (wav2raw)

set (SRCROOT "${PROJECT_SOURCE_DIR}/src")

set (SRC
	"${SRCROOT}/main.cpp"
	"${SRCROOT}/WaveFile.cpp"
	"${SRCROOT}/WaveFile.h"
)

add_executable (wav2raw ${SRC})
