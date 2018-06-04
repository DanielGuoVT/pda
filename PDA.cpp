//
// Created by xie on 18-5-21.
//

#include <llvm/Analysis/CFG.h>
#include <stack>
#include <llvm/Support/raw_ostream.h>
#include "PDA.h"
#include "Executor.h"


void Location::dump() {
    outs()<<"==============Path "<<index<<"================\n";
//    for(auto it = path.begin(), ed = path.end(); it != ed; it++){
//        outs()<<**it;
//    }
    cout<<"Path Condition: "<<pathCondition<<"\n";


    outs()<<"IV Variable Change \n";

    for(auto it = IVGeneral.begin(), ed = IVGeneral.end(); it != ed; it ++){
        cout<<it->first<<":  "<<it->second<<"\n";
    }
    outs()<<"NIV Variable \n";

    for(auto it = nonIV.begin(), ed = nonIV.end(); it != ed; it ++){
        cout<<*it<<"\n";
    }
    outs()<<"\n";

}

bool Location::canTransit(Location *loc) {
    if(path.back() == *loc->getPath().begin()){
        expr k = z3context.int_const("k");
        expr cond =  k > 0;

        expr pc = pathCondition;
        expr_map <expr> sub1;
        sub1.insert(make_pair(k,k-1));
        pc = substitute(pc, IVGeneral);// IVGeneral: x+1<100 ==> x+k <100
        cond = cond && substitute(pc, sub1); // k ==> k-1

        pc = loc->pathCondition;
        pc = substitute(pc, IVGeneral);
        cond = cond && pc;

        return isSat(cond);

    }else
        return false;
}
bool Location::isFeasible(){
    //this->dump();
    Executor executor(this);
    executor.run();
    expr pc = pathCondition;
    if(isSat(pc)){
        if(*path.begin() == path.back()){
            pc = substitute(pc, valChange );
            IterPath = isSat(pc);
        }else
            IterPath = false;
        executor.computeIVGeneral();// x=x+1 ==> x=x+k
        canBeSummarized = executor.canBeSummarized() && nonIV.empty();
        return true;
    }else
        return false;
}
PDA::PDA(Function* f){
    //Get all head BBs
    SmallPtrSet<const BasicBlock*, 8> headBBs;
    SmallPtrSet<BasicBlock*, 8> visited;
    SmallVector<std::pair<const BasicBlock *, const BasicBlock *>,8 > backEdges;
    FindFunctionBackedges(*f,backEdges);
    for (auto it = backEdges.begin(); it != backEdges.end(); it++)
        headBBs.insert(it->second);

    //Get all pahts
    BasicBlock* BB = &f->getEntryBlock();
    stack<BasicBlock*> BBStack;
    BBStack.push(BB);
    stack<vector<BasicBlock*>> pathStack;
    pathStack.push(vector<BasicBlock *> {BB});
    int num = 0;

    while(!BBStack.empty()){
        BasicBlock* CurBB = BBStack.top();
        vector<BasicBlock*> curPath = pathStack.top();

        BBStack.pop();
        pathStack.pop();

        if (succ_empty(CurBB)){
            Location* loc = new Location(curPath);
            if(loc->isFeasible()){
                loc->setIndex(num++);
                paths.push_back(loc);
            }

        }else{
            for( succ_iterator I = succ_begin(CurBB); I != succ_end(CurBB); I++){
                BasicBlock* sucBB = *I;
                vector<BasicBlock *> newPath = curPath;
                newPath.push_back(sucBB);
                if(headBBs.count(sucBB)){

                    Location* loc = new Location(newPath);
                    if(loc->isFeasible()){
                        loc->setIndex(num++);
                        paths.push_back(loc);
                    }
                    if(!visited.count(sucBB)){
                        BBStack.push(sucBB);
                        visited.insert(sucBB);
                        pathStack.push(vector<BasicBlock *> {sucBB});
                    }
                }else{
                    BBStack.push(sucBB);
                    pathStack.push(newPath);
                }


            }
        }
    }

    for(auto it = paths.begin(), ed = paths.end(); it != ed; it++){
        Location* loc1 = *it;
        for(auto it1 = paths.begin(), ed1 = paths.end(); it1 != ed1; it1++){
            Location* loc2 = *it1;
            if(loc1 != loc2 && loc1->canTransit(loc2))
                transitions.insert(make_pair(loc1, loc2));
        }
    }
}
