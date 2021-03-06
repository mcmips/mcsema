/*
Copyright (c) 2013, Trail of Bits
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of the {organization} nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <iostream>
#include <string>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Intrinsics.h>

#include <llvm/MC/MCInst.h>

#include "mcsema/Arch/Arch.h"
#include "mcsema/Arch/Dispatch.h"
#include "mcsema/Arch/Register.h"

#include "mcsema/Arch/X86/Util.h"
#include "mcsema/Arch/Mips/Semantics/ADDu.h"

#include "mcsema/BC/Util.h"

using namespace llvm;

static InstTransResult translate_ADDiu(TranslationContext &ctx, 
				      llvm::BasicBlock *&block)
{
	InstTransResult ret;
	auto natM = ctx.natM; 
	//auto F = ctx.F; 
	auto ip = ctx.natI;
	auto &inst = ip->get_inst();
	Function *F = block->getParent();
	Module *M = F->getParent();

	std::cout << "translate_ADDiu -> " << std::hex << ip << ":-" << std::dec << inst.getNumOperands() << "\t-----" ;

	MCOperand op, op0, op1, op2;
	
	for(int i=0; i < inst.getNumOperands(); i++ )
	{

	op = inst.getOperand(i);
	if(op.isValid())
	{
		if(op.isReg())
			std::cout << "isReg " << op.getReg() << "\t";
		if(op.isImm())
			std::cout << "isImm " << op.getImm() << "\t";
		if(op.isFPImm())
			std::cout << "isFPImm " << op.getFPImm() << "\t";
		if(op.isInst())
			std::cout << "isFPImm " << op.getFPImm() << "\t";
		
	}

	}	
	std::cout<<std::endl;

	op0 = inst.getOperand(0); //Reg
	op1 = inst.getOperand(1); //Reg
	op2 = inst.getOperand(2); //Imm
	
	// ADDiu $dst $src value

	Value *srcReg = R_READ<32>(block, op1.getReg());
	
	if( ip->has_external_ref() )
	{
		std::cout<<"has external ref\n";
	}else if( ip->has_mem_reference ) {

		// TODOSH
		std::cout<<"has MEMORY reference\n";

		//Value *data_v = getGlobalFromOriginalAddr(VA, natM, VA, block)

		Value *addr = MEM_REFERENCE(1);
		Value *fromMem = M_READ<32>(ip, block, addr);

		Value *res = BinaryOperator::Create(Instruction::Add, srcReg, fromMem, "", block);
		
		//R_WRITE<32>(block, op0.getReg(), M_READ<32>(ip, block, res)); //res);
		R_WRITE<32>(block, op0.getReg(), res);
		
	}
	else {
		std::cout<<"in else\n";

		Value *srcImm = CONST_V<32>(block, op2.getImm());
		Value *res = BinaryOperator::Create(Instruction::Add, srcReg, srcImm, "", block);
		R_WRITE<32>(block, op0.getReg(), res);

	}
		
	return ContinueBlock;
}

void ADDiu_populateDispatchMap(DispatchMap &m)
{

	m[Mips::ADDiu] = translate_ADDiu;
}
