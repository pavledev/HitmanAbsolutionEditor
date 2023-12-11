#pragma once

//extern "C"
//{
//#include "libavformat/avio.h"
//}
//
//#include "IO/BinaryReader.h"
//
//class MemoryInputAVIOContext
//{
//public:
//	MemoryInputAVIOContext(BinaryReader& binaryReader);
//	static int ReadPacket(void* opaque, unsigned char* buffer, int bufferSize);
//	static long long Seek(void* opaque, long long offset, int whence);
//	AVIOContext* GetIOContext();
//
//private:
//	BinaryReader binaryReader;
//	AVIOContext* ioContext;
//};
