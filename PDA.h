//
// Created by xie on 18-5-21.
//

#ifndef PROTEUS_PDA_H
#define PROTEUS_PDA_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "Executor.h"

#include <vector>
#include <z3++.h>

using namespace llvm;
using namespace std;
using namespace z3;
class Location {

    int index;
    vector<BasicBlock *> path; // the BBs in the corresponding path

    vector<expr> atomicConditions; // atomic conditions of the path
    expr pathCondition; //path condition
    vector<expr> property; // the assert property in the loc

    expr_map<expr> valChange; // the value change after one execution of the path
    expr_map<expr> IVGeneral; // the general form for IV
    expr_set nonIV; // the non-induction variables

    bool IterPath;// whether the path is iterative path or one time path
    bool canBeSummarized; //whether all the atomicConditions are IV conditions and all the variables are IVs



public:
    Location(vector<BasicBlock *> blks): path(blks), pathCondition(z3context.bool_val(true)){

    }
    vector<BasicBlock*> getPath(){return path;}
    bool isFeasible();
    bool isIterPath(){return IterPath;}
    bool canBeSumm(){return canBeSummarized;}

    bool canTransit(Location* loc);
    expr getPathCondition(){ return  pathCondition;}
    expr_map<expr> getValChange(){return valChange;}

    void dump();
    void addPeroperty(expr e){property.push_back(e);};
    void addIVGeneral(expr e1, expr e2){IVGeneral.insert(make_pair(e1,e2));}
    void setIVGeneral(expr_map<expr> emap) {IVGeneral = emap;}
    void setIndex(int id){index = id;}
    int getIndex(){return index;}

    void setValChange(expr_map<expr> valCh) {valChange = valCh;}
    void addPathCond(expr e){
        atomicConditions.push_back(e);
        pathCondition = pathCondition && e;
    };

    void addNonIV(expr e){nonIV.insert(e);}

    BasicBlock* getNextNode(BasicBlock* bb){
        auto it = std::find(path.begin(), path.end()-1, bb);
        it++;
        return *it;
    }

};
class PDA {
private:
    Function* func;
    vector<Location*> paths;
    map<Location*, Location*> transitions;
    Location* initPaths;
public:
    PDA(Function* f);
    int size(){return paths.size();}
    vector<Location*> getPaths() {return paths;}
    map<Location*, Location*> getTransitions(){return transitions;};
    void dump(){
        for(auto it = paths.begin(), ed = paths.end(); it != ed; it++)
            (*it)->dump();

        for(auto it = transitions.begin(), ed = transitions.end(); it != ed; it++){
            outs()<<"Path "<<it->first->getIndex()<<"   --->  Path "<<it->second->getIndex()<<"\n";
        }
    }

};


#endif //PROTEUS_PDA_H
