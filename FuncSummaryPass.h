//
// Created by xie on 18-5-21.
//

#ifndef PROTEUS_LOOPSUMMARYPASS_H
#define PROTEUS_LOOPSUMMARYPASS_H
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/CFG.h"
#include "PDA.h"
#include "Executor.h"
#include "PDA2Dot.h"
using namespace llvm;
class FuncSummaryPass : public FunctionPass {
public:
    static char ID;
    string outfile;
    FuncSummaryPass(): FunctionPass(ID),outfile(nullptr){}
    FuncSummaryPass(string output): FunctionPass(ID),outfile(output){}
    virtual void getAnalysisUsage(AnalysisUsage &AU) const override
    {
        AU.setPreservesCFG();
        AU.addRequired<LoopInfoWrapperPass>();
    }
    bool runOnFunction(Function& f) {
        PDA pda(&f);
        PDA2Dot dumper(&pda);
        if(outfile == "")
            dumper.dump(nullptr);
        else
            dumper.dump(outfile.c_str());
    }


};


#endif //PROTEUS_LOOPSUMMARYPASS_H
