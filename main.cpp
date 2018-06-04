#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_os_ostream.h>
#include "FuncSummaryPass.h"
using namespace llvm;


int main(int argc, const char **argv)
{
    llvm::cl::opt<std::string> input("i",
                                     llvm::cl::desc("The input module.\n"),
                                     llvm::cl::value_desc("input file"), llvm::cl::init(""),cl::Required);
    llvm::cl::opt<std::string> output("o",
                                     llvm::cl::desc("The output dot file.\n"),
                                     llvm::cl::value_desc("filename"), llvm::cl::init(""));

    llvm::cl::ParseCommandLineOptions(argc, argv);

    LLVMContext context;
    SMDiagnostic error;
    //clang test.c -emit-llvm -S -o test.ll
    std::unique_ptr<Module> m = parseIRFile(input.c_str(), error, context);
    if(m){
        FuncSummaryPass funcPass(output);
        Function* func = m.get()->getFunction("main");
        funcPass.runOnFunction(*func);
    }else{
        errs()<<"Cannot load the input module\n";
    }

    return 0;
}