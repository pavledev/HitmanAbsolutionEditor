//extern "C"
//{
//#include "libavutil/mem.h"
//}
//
//#include "IO/MemoryInputAVIOContext.h"
//
//MemoryInputAVIOContext::MemoryInputAVIOContext(BinaryReader& binaryReader)
//{
//	this->binaryReader = binaryReader;
//
//	unsigned char* buffer = static_cast<unsigned char*>(av_malloc(binaryReader.GetSize()));
//
//	ioContext = avio_alloc_context(buffer, binaryReader.GetSize(), 0, this, &MemoryInputAVIOContext::ReadPacket, nullptr, &MemoryInputAVIOContext::Seek);
//}
//
//int MemoryInputAVIOContext::ReadPacket(void* opaque, unsigned char* buffer, int bufferSize)
//{
//	MemoryInputAVIOContext* memoryInputAVIOContext = static_cast<MemoryInputAVIOContext*>(opaque);
//
//	memoryInputAVIOContext->binaryReader.Read(buffer, bufferSize);
//
//	return bufferSize;
//}
//
//long long MemoryInputAVIOContext::Seek(void* opaque, long long offset, int whence)
//{
//	MemoryInputAVIOContext* memoryInputAVIOContext = static_cast<MemoryInputAVIOContext*>(opaque);
//
//	switch (whence)
//	{
//		case SEEK_SET:
//			memoryInputAVIOContext->binaryReader.Seek(static_cast<size_t>(offset), SeekOrigin::Begin);
//			break;
//		case SEEK_CUR:
//			memoryInputAVIOContext->binaryReader.Seek(static_cast<size_t>(offset), SeekOrigin::Current);
//			break;
//		case SEEK_END:
//			memoryInputAVIOContext->binaryReader.Seek(static_cast<size_t>(offset), SeekOrigin::End);
//			break;
//		case AVSEEK_SIZE:
//			return memoryInputAVIOContext->binaryReader.GetSize();
//	}
//
//	size_t newPosition = memoryInputAVIOContext->binaryReader.GetPosition();
//
//	if (newPosition < 0 || newPosition > memoryInputAVIOContext->binaryReader.GetSize())
//	{
//		return -1;
//	}
//
//	return newPosition;
//}
//
//AVIOContext* MemoryInputAVIOContext::GetIOContext()
//{
//	return ioContext;
//}
