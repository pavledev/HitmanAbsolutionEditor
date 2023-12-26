#pragma once

#include <Windows.h>
#include <D3Dcompiler.h>

#include <HLSLDecompiler/DecompileHLSL.h>
#include <log.h>
#include <version.h>

FILE* LogFile = stderr; // Log to stderr by default
bool gLogDebug = false;

// Old version directly using D3DDisassemble, suffers from precision issues due
// to bug in MS's disassembler that always prints floats with %f, which does
// not have sufficient precision to reproduce a 32bit floating point value
// exactly. Might still be useful for comparison:
static HRESULT DisassembleMS(const void* pShaderBytecode, size_t BytecodeLength, std::string* asmText)
{
	ID3DBlob* disassembly = nullptr;
	UINT flags = D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS;
	std::string comments = "//   using 3Dmigoto command line v" + std::string(VER_FILE_VERSION_STR) + " on " + LogTime() + "//\n";

	HRESULT hr = D3DDisassemble(pShaderBytecode, BytecodeLength, flags, comments.c_str(), &disassembly);
	if (FAILED(hr))
	{
		LogInfo("  disassembly failed. Error: %x\n", hr);
		return hr;
	}

	// Successfully disassembled into a Blob.  Let's turn it into a C++ std::string
	// so that we don't have a null byte as a terminator.  If written to a file,
	// the null bytes otherwise cause Git diffs to fail.
	*asmText = std::string(static_cast<char*>(disassembly->GetBufferPointer()));

	disassembly->Release();
	return S_OK;
}

static HRESULT Decompile(const void* pShaderBytecode, size_t BytecodeLength, string* hlslText, string* shaderModel)
{
	// Set all to zero, so we only init the ones we are using here:
	ParseParameters p = { 0 };
	DecompilerSettings d;
	bool patched = false;
	bool errorOccurred = false;
	std::string disassembly;
	HRESULT hret;

	hret = DisassembleMS(pShaderBytecode, BytecodeLength, &disassembly);
	if (FAILED(hret))
		return E_FAIL;

	LogInfo("    creating HLSL representation\n");

	p.bytecode = pShaderBytecode;
	p.decompiled = disassembly.c_str(); // XXX: Why do we call this "decompiled" when it's actually disassembled?
	p.decompiledSize = disassembly.size();
	p.G = &d;

	// Disable IniParams and StereoParams registers. This avoids inserting
	// these in a shader that already has them, such as some of our test
	// cases. Also, while cmd_Decompiler is part of 3DMigoto, it is NOT
	// 3DMigoto so it doesn't really make sense that it should add 3DMigoto
	// registers, and if someone wants these registers there is nothing
	// stopping them from adding them by hand. May break scripts that use
	// cmd_Decompiler and expect these to be here, but those scripts can be
	// updated to add them or they can keep using an old version.
	d.IniParamsReg = -1;
	d.StereoParamsReg = -1;

	*hlslText = DecompileBinaryHLSL(p, patched, *shaderModel, errorOccurred);
	if (!hlslText->size() || errorOccurred)
	{
		LogInfo("    error while decompiling\n");
		return E_FAIL;
	}

	return S_OK;
}
