cmake_minimum_required(VERSION 3.8)

add_library(imgui-color-text-edit STATIC
	source/TextEditor.cpp
	source/TextEditor.h
)

target_include_directories(imgui-color-text-edit PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(imgui-color-text-edit PUBLIC
	imgui-docking-layout
)
