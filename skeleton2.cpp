#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include <fstream>
using namespace llvm;

namespace {

struct SkeletonPass : public PassInfoMixin<SkeletonPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        //Branch Trace directory file
        std::fstream dictFile("branch_dictionary.txt", std::ios::in | std::ios::out | std::ios::trunc);
        int branchID =1;
        std::set<BranchInst*> processedBranches;

        //get the LLVM context
        LLVMContext &Ctx = M.getContext();
        //define external function's parameter types, for function pointer should be getInt8PtrTy
        std::vector<Type*> paramTypesfunc = {Type::getInt8PtrTy(Ctx)};
        std::vector<Type*> paramTypesbr = {Type::getInt32Ty(Ctx)};
        //define external function's return value type
        Type *retTypefunc = Type::getVoidTy(Ctx);
        //create function type, false means this function is not variadic
        FunctionType *logFuncType = FunctionType::get(retTypefunc, paramTypesfunc, false);
        FunctionType *logBrType = FunctionType::get(retTypefunc, paramTypesbr, false);

        FunctionCallee logFunc =M.getOrInsertFunction("logfunction", logFuncType);
        FunctionCallee logBr = M.getOrInsertFunction("logbranch", logBrType);

        for (auto &F : M.functions()) {
            //errs()<<F.getName()<<"\n";

            for (auto &B : F) {
                for (auto &I : B) {
                    if (auto *brinst = dyn_cast<BranchInst>(&I)) {
                            //check if instruction is conditional
                            if(brinst->isConditional()){
                                processedBranches.insert(brinst);                                             
                            }
                    }

                    if(auto *funcp = dyn_cast<CallInst>(&I)){
                        
                        if(!funcp->getCalledFunction()){
                            //errs()<<*funcp<<"\n";
                            IRBuilder<> Builder(funcp);

                            Value *callpointer = funcp->getCalledOperand();
                            Builder.CreateCall(logFunc, callpointer);
                        }
                        
                    }
                    
            }
        }
    };
    for(auto brinst: processedBranches){
        int trueID = branchID;
        int falseID = branchID + 1;
        if (DILocation *Loc = brinst->getDebugLoc()) { 
            for (unsigned int i = 0; i < brinst->getNumSuccessors(); ++i) {
                BasicBlock *succ = brinst->getSuccessor(i);
                if (!succ->empty()){
                    DILocation *succLoc = succ->front().getDebugLoc();
                    dictFile << "br_" << branchID << ": " << Loc->getFilename().str() << ", " << Loc->getLine() << ", " << succLoc->getLine() << "\n";
                    branchID++; 
                }                   
            }
        } 

   
    }
    dictFile.close();
    return PreservedAnalyses::none();
};

};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "Skeleton pass",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(SkeletonPass());
                });
        }
    };
}