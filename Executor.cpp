//
// Created by xie on 18-5-23.
//

#include "Executor.h"
#include "PDA.h"

int Executor::symIndex = 0;
map<Instruction *, expr> Executor::symVar;


bool Executor::isConst(expr e) {

    if (e.is_const()) {
        if (e.is_numeral())
            return true;
        auto it = ptrVal.find(e);
        return it == ptrVal.end();
    }
    else {
        unsigned num = e.num_args();
        if(e.decl().decl_kind()==Z3_OP_UNINTERPRETED)
            return false;
        for (unsigned i = 0; i < num; i++) {
            if (!isConst(e.arg(i)))
                return false;
        }
        return true;
    }

}

//Check whether e1 is IV in the assignment e1 := e2
//If is IV, then return the generalform
bool Executor::isIV(expr e1, expr e2, expr& generalForm){
    if(!loc->isIterPath()){
        generalForm = e2;
        return true;
    }else{
        // Assign to constant
        if(isConst(e2)){
            generalForm = e2;
            return true;
        }
        else{
            // ArithSeq Check
            expr d = (e2 - e1).simplify();
            if(isConst(d)){
                generalForm = e1 + z3context.int_const("k") * d; // x_n = x_0 + n*d;
                return true;
            }

                // TODO other IV check
        }
    }
    return false;


}
bool Executor::canBeSummarized() {
    vector<expr> atomicConds;
    if(!loc->isIterPath())
        return true;
    for(auto I = atomicConds.begin(), E = atomicConds.end(); I != E; I++){
        expr e = *I;
        expr left =e.arg(0);
        cout<< left;
        expr temp = left;
        temp = substitute(temp, ptrVal);

        expr gf(z3context);
        if(!isIV(left, temp, gf))
            return false;

    }
    return true;
}
void Executor::computeIVGeneral(){
    if(!loc->isIterPath())
        loc->setIVGeneral(ptrVal);
    else{
        for(auto it = ptrVal.begin(), ed = ptrVal.end(); it != ed; it++){

            expr gf(z3context);
            bool iv = isIV(it->first, it->second, gf);
            if(iv)
                loc->addIVGeneral(it->first, gf);
            else
                loc->addNonIV(it->first);
        }
    }

}

// interprete all instructions along a path
void Executor::run() {
    vector<BasicBlock *> path = loc->getPath();
    int size = path.size() -1;
    assert(size>0);
    for(int i = 0; i < size; i++){
        BasicBlock *BB = path[i];
        for (BasicBlock::iterator it = BB->begin(), end = BB->end(); it != end; it++) {
            Instruction *ins = &*it;
            visit(ins);
        }
    }
    loc->setValChange(ptrVal);

}

void Executor::visitAllocaInst(AllocaInst &I) {
    expr sym = generateSymolicVar(&I);
    ECStack.insert(make_pair(&I, sym));
}

expr Executor::getExprFromStack(Value *v) {
    if (Instruction *ins = dyn_cast<Instruction>(v)) {
        auto it = ECStack.find(ins);
        if (it == ECStack.end()) {
            return generateSymolicVar(v);
        } else
            return it->second;

    } else if (ConstantInt *cons = dyn_cast<ConstantInt>(v)) {
        int num = cons->getZExtValue(); // not consider the width and sign
        return z3context.int_val(num);
    } else {
        outs() << *v->getType() << *v;
        assert(false && UNHANDLE);
    }

}

void Executor::visitStoreInst(StoreInst &I) {
    Value *val = I.getOperand(0);
    Value *src = I.getOperand(1);
    expr e1 = getExprFromStack(val);
    expr e2 = getExprFromStack(src);
    ptrVal.insert(make_pair(e2, e1));

}

void Executor::visitLoadInst(LoadInst &I) {
    Value *ptr = I.getOperand(0);
    expr e1 = getExprFromStack(ptr);
    auto it = ptrVal.find(e1);
    if (it ==
        ptrVal.end()) { //new path and the val is from the unknown context, we use the symbolic variable as the result
        ECStack.insert(make_pair(&I, e1));
    } else
        ECStack.insert(make_pair(&I, it->second));
}

void Executor::visitICmpInst(ICmpInst &I) {
    expr e1 = getExprFromStack(I.getOperand(0));
    expr e2 = getExprFromStack(I.getOperand(1));
    expr result(z3context);
    switch (I.getPredicate()) {
        case ICmpInst::ICMP_EQ:
            result = (e1 - e2 == 0);
            break;
        case ICmpInst::ICMP_NE:
            result = (e1 - e2 != 0);
            break;
        case ICmpInst::ICMP_UGE:
        case ICmpInst::ICMP_SGE:
            result = (e1 - e2 >= 0);
            break;
        case ICmpInst::ICMP_ULT:
        case ICmpInst::ICMP_SLT:
            result = (e1 - e2 < 0);
            break;
        case ICmpInst::ICMP_ULE:
        case ICmpInst::ICMP_SLE:
            result = (e1 - e2 <= 0);
            break;
        case ICmpInst::ICMP_UGT:
        case ICmpInst::ICMP_SGT:
            result = (e1 - e2 > 0);
            break;
        default:
            assert(false);
    }
    ECStack.insert(make_pair(&I, result));
    //atomicConditions.push_back(result);
}

expr bv_op(expr e1, expr e2, Type *t, BinaryOperator::BinaryOps op, bool sign = true) {

    if (t->isIntegerTy()) {
        unsigned int bitsize = t->getIntegerBitWidth();
        expr bv_e1 = expr(z3context, Z3_mk_int2bv(z3context, bitsize, e1));
        expr bv_e2 = expr(z3context, Z3_mk_int2bv(z3context, bitsize, e2));
        expr result(z3context);
        switch (op) {
            case Instruction::And:
                result = bv_e1 & bv_e2;
                break;
            case Instruction::Or:
                result = bv_e1 | bv_e2;
                break;
            case Instruction::Xor:
                result = bv_e1 ^ bv_e2;
                break;
            default:
                assert(false);
        }
        result = expr(z3context, Z3_mk_bv2int(z3context, result, sign));
        return result;

    } else
        assert(false);


}

void Executor::visitBinaryOperator(BinaryOperator &I) {
    expr e1 = getExprFromStack(I.getOperand(0));
    expr e2 = getExprFromStack(I.getOperand(1));
    expr result(z3context);
    switch (I.getOpcode()) {
        case Instruction::Add:
        case Instruction::FAdd:
            result = e1 + e2;
            break;
        case Instruction::Sub:
        case Instruction::FSub:
            result = e1 - e2;
            break;
        case Instruction::Mul:
        case Instruction::FMul:
            result = e1 * e2;
            break;
        case Instruction::SDiv:
        case Instruction::UDiv:
        case Instruction::FDiv:
            result = e1 / e2;
            break;
        case Instruction::SRem:
        case Instruction::URem:
            result = expr(z3context, Z3_mk_rem(z3context, e1, e2));
            break;
        case Instruction::Xor:
            result = I.getType()->isIntegerTy(1) ? expr(z3context, Z3_mk_xor(z3context, e1, e2)) : bv_op(e1, e2,
                                                                                                         I.getType(),
                                                                                                         Instruction::Xor);
            break;
        case Instruction::And:
            result = I.getType()->isIntegerTy(1) ? e1 && e2 : bv_op(e1, e2, I.getType(), Instruction::And);
            break;
        case Instruction::Or:
            result = I.getType()->isIntegerTy(1) ? e1 || e2 : bv_op(e1, e2, I.getType(), Instruction::Or);
            break;
        default:
            errs() << I;
            assert(false);

    }
    ECStack.insert(make_pair(&I, result));
}

void Executor::visitZExtInst(ZExtInst &I) {
    Value *v = I.getOperand(0);
    if (v->getType()->isIntegerTy(1)) {  //i1 traverse to integer, others not support now
        expr e = getExprFromStack(I.getOperand(0));
        ECStack.insert(make_pair(&I, ite(e, z3context.int_val(1), z3context.int_val(0))));
    } else
        assert(false); // not support others
}

bool is_func(CallInst *var, string funcname) {
    CallSite call = var;
    Function *fc = call.getCalledFunction();
    if (fc == nullptr)
        return 0;
    string st = fc->getName();
    return st.find(funcname) != string::npos;
}

void Executor::visitCallInst(CallInst &I) {
    if (is_func(&I, "__VERIFIER_nondet_int")) {
        expr sym = generateSymolicVar(&I);
        ECStack.insert(make_pair(&I, sym));
    } else if (is_func(&I, "__VERIFIER_assume")) {
        CallSite ci = &I;
        Value *var = *ci.arg_begin();
        // add assumption
    } else if (is_func(&I, "__VERIFIER_assert")) {
        CallSite ci = &I;
        Value *var = *ci.arg_begin();
        expr e = getExprFromStack(var);
        loc->addPeroperty(e==1);
    }else
        assert(false);
}

void Executor::visitBranchInst(BranchInst &I){
    if(I.isUnconditional())
        return;
    BasicBlock* next = loc->getNextNode(I.getParent());

    expr e = getExprFromStack(I.getCondition());
    if(I.getSuccessor(0) == next)
        loc->addPathCond(e);
    else
        loc->addPathCond(!e);


}
