cmake_minimum_required(VERSION 3.8)

add_library(hlsl-decompiler STATIC
	source/BinaryDecompiler/include/hlslcc.h
	source/BinaryDecompiler/internal_includes/debug.h
	source/BinaryDecompiler/internal_includes/decode.h
	source/BinaryDecompiler/internal_includes/languages.h
	source/BinaryDecompiler/internal_includes/reflect.h
	source/BinaryDecompiler/internal_includes/shaderLimits.h
	source/BinaryDecompiler/internal_includes/structs.h
	source/BinaryDecompiler/internal_includes/tokens.h
	source/BinaryDecompiler/internal_includes/tokensDX9.h
	source/BinaryDecompiler/decode.cpp
	source/BinaryDecompiler/decodeDX9.cpp
	source/BinaryDecompiler/reflect.cpp
	source/HLSLDecompiler/DecompileHLSL.cpp
	source/HLSLDecompiler/DecompileHLSL.h
	source/log.h
	source/version.h
)

target_include_directories(hlsl-decompiler PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/source
)
