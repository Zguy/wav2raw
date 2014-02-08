cmake_minimum_required (VERSION 2.8.12)

project (wav2raw)

set (SRCROOT "${PROJECT_SOURCE_DIR}/src")

set (SRC
	"${SRCROOT}/main.cpp"
	"${SRCROOT}/WaveFile.cpp"
	"${SRCROOT}/WaveFile.h"
)

add_compile_options (
	$<$<CXX_COMPILER_ID:GNU>:-std=c++11>
	$<$<CXX_COMPILER_ID:GNU>:-Wno-multichar>
)

add_executable (wav2raw ${SRC})
