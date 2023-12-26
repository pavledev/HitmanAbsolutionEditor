#ifndef DECODE_H
#define DECODE_H

#include "structs.h"

BinaryDecompiler::Shader* DecodeDXBC(uint32_t* data);

//You don't need to call this directly because DecodeDXBC
//will call DecodeDX9BC if the shader looks
//like it is SM1/2/3.
BinaryDecompiler::Shader* DecodeDX9BC(const uint32_t* pui32Tokens);

void UpdateOperandReferences(BinaryDecompiler::Shader* psShader, Instruction* psInst);

#endif
