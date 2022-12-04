cmake_minimum_required(VERSION 2.9)
project(wav2raw)

add_executable(wav2raw "main.cpp")
install(TARGETS wav2raw)

if(CMAKE_COMPILER_IS_GNUCXX)
	set_target_properties(wav2raw PROPERTIES COMPILE_FLAGS "-std=c++11 -Wno-multichar")
endif()
