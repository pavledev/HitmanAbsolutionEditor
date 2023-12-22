#pragma once

#include <istream>
#include <vector>

#include "IO/BufferView.h"
#include "FSB.h"
#include "IO/BinaryReader.h"
#include "IO/BinaryWriter.h"

namespace FSB
{
	class Container
	{
	public:
		Container(BinaryReader& binaryReader);

		const Header& FileHeader() const;
		const std::vector<Sample>& Samples() const;

		// Extracts sample audio data to given stream.
		void ExtractSample(const Sample& sample, BinaryWriter& binaryWriter);

	private:
		Container(const Container& rhs) = delete;
		Container& operator=(const Container& rhs) = delete;

		// Reads file header from a stream.
		void ReadFileHeader(BinaryReader& binaryReader);
		void ReadFileHeader(const std::vector<char>& headerBuffer);

		// Reads sample headers from a stream.
		void ReadSampleHeaders(BinaryReader& binaryReader);
		void ReadSampleHeaders(const std::vector<char>& samplesBuffer);

		// Reads a single sample header from a view.
		// 
		// Position view on a next sample header.
		Sample ReadSampleHeader(IO::BufferView& view);

		// Reads sample names.
		void ReadSampleNames(BinaryReader& binaryReader);
		void ReadSampleNames(const std::vector<char>& namesBuffer);

		static const int headerSize = 60;
		Header header;
		std::vector<Sample> samples;
		std::vector<char> dataBuffer;
	};
}
