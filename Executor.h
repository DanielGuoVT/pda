//
// Created by xie on 18-5-23.
//

#ifndef PROTEUS_EXECUTOR_H
#define PROTEUS_EXECUTOR_H

#include <z3++.h>
#include <llvm/Support/raw_ostream.h>
#include <set>
#include "llvm/IR/InstVisitor.h"
#include "Utils.h"


using namespace std;
using namespace llvm;
using namespace z3;
class Location;


class Executor : public InstVisitor<Executor> {

    Location* loc;
    map<Instruction*, expr> ECStack; // inst ==> value
    expr_map<expr> ptrVal;

public:
    static map<Instruction *, expr> symVar; // the generated symbolic variables
    static int symIndex; // the symbolic variable index

    Executor(Location* location):loc(location){

    }
    expr getExprFromStack(Value* v);
    bool isConst(expr e);
    void computeIVGeneral();
    bool canBeSummarized();
    bool isIV(expr e1, expr e2, expr& generalForm);
    void run();
    void visitBinaryOperator(BinaryOperator &I);
    void visitPHINode(PHINode &PN){
        errs()<<PN;
        assert(false&&UNHANDLE);
    };
    void visitCallInst(CallInst &I);
    void visitICmpInst(ICmpInst &I);
    void visitBranchInst(BranchInst &I);
    void visitZExtInst(ZExtInst &I);
    void visitReturnInst(ReturnInst &I){};
    void visitSwitchInst(SwitchInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    };
    void visitIndirectBrInst(IndirectBrInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    };
    void visitFCmpInst(FCmpInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    };
    void visitAllocaInst(AllocaInst &I);
    void visitLoadInst(LoadInst &I);
    void visitStoreInst(StoreInst &I);
    void visitGetElementPtrInst(GetElementPtrInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitTruncInst(TruncInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitSExtInst(SExtInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitFPTruncInst(FPTruncInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitFPExtInst(FPExtInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitUIToFPInst(UIToFPInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitSIToFPInst(SIToFPInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitFPToUIInst(FPToUIInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitFPToSIInst(FPToSIInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitPtrToIntInst(PtrToIntInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitIntToPtrInst(IntToPtrInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitBitCastInst(BitCastInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitSelectInst(SelectInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitCallSite(CallSite CS){
        errs()<<"CallSite inst \n";
        assert(false&&UNHANDLE);
    }
    void visitInvokeInst(InvokeInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitUnreachableInst(UnreachableInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitShl(BinaryOperator &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitLShr(BinaryOperator &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitAShr(BinaryOperator &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitVAArgInst(VAArgInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitExtractElementInst(ExtractElementInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitInsertElementInst(InsertElementInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitShuffleVectorInst(ShuffleVectorInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitExtractValueInst(ExtractValueInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitInsertValueInst(InsertValueInst &I){
        errs()<<I;
        assert(false&&UNHANDLE);
    }
    void visitInstruction(Instruction &I) {
        errs()<<I;
        llvm_unreachable("Instruction not interpretable yet!");
    }


    static expr generateSymolicVar(Value *val) {
        if (Instruction *v = dyn_cast<Instruction>(val)) {

            auto it = symVar.find(v);
            if (it != symVar.end())
                return it->second;
            expr e(z3context);
            if (v->hasName()) {
                if (v->getType()->isIntegerTy(1) || isa<ICmpInst>(v))
                    e = z3context.bool_const(v->getName().data());
                else
                    e = z3context.int_const(v->getName().data());

            } else {

                if (v->getType()->isIntegerTy(1) || isa<ICmpInst>(v))
                    e = z3context.bool_const(("x_" + intTostring(symIndex++)).c_str());
                else
                    e = z3context.int_const(("x_" + intTostring(symIndex++)).c_str());
            }
            symVar.insert(make_pair(v, e));
            return e;
        } else
            assert(false);
    }


};


#endif //PROTEUS_EXECUTOR_H
