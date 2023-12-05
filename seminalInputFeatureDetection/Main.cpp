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
#include <vector>
#include <map>
#include <string.h>
using namespace llvm;
using namespace std;

namespace {


struct SIFDPass : public PassInfoMixin<SIFDPass> {
    vector<Value *> Users;
    vector<vector<Value *> > UserList;

    map <Value *, bool> visited;
    map <Value *, int> var2id;
    map <int, Value *> id2var;
    map <int, Value *> id2inst;
    map <Value *, StringRef> varNames;
    map <Value *, int> varIsFile;
    map <Value *, int> varLine;
    
    int var_id = 0;

    void findDefUseRelation(Value *V) {
        if (visited.find(V) != visited.end() && visited[V] == true) 
            return;

        visited[V] = true;

        //outs() << "Def Use Relation for : " << *V << "\n";

        // Tranverse the intructions of this var
        for (auto *user : V->users()) {
            Users.push_back(user);
            //outs() << "Used by : " << *user << "\n";
            findDefUseRelation(user);
            if (StoreInst *storeInst = dyn_cast<StoreInst>(user)) {
                // Check if it's a store instruction
                Value *storePointer = storeInst->getPointerOperand();
                Users.push_back(storePointer);
                findDefUseRelation(storePointer);
        }
        }
    }

    void findInputVar(Value *V) {
        // outs() << "Zrf, current value is " << V->getName() << "\n";

        for (int i = 0; i < var_id; i ++) {
            // outs() << "Zrf, this is the input value " << (id2var[i])->getName() << "\n";

            auto currentUsers = UserList[i];

            for (auto it = currentUsers.begin(); it != currentUsers.end(); it ++) {
                // outs() << "User list: " << (**it) << "\n";

                if (*it == V) {
                    Value* seminalInputVar = (id2var[i]);
                    outs() << "**************Seminal Input**************" << "\n";
                    if (varIsFile[seminalInputVar]) {
                         outs() << "Line " << varLine[seminalInputVar] << ": size of file " << varNames[seminalInputVar] << "\n"; 
                    } else {
                        outs() << "Line " << varLine[seminalInputVar] << ": " << varNames[seminalInputVar] << "\n"; 
                    }
                    outs() << "*****************************************" << "\n";
                    // outs() << "!!!!!! this is the input value" << *(id2var[i]) << "\n";

                    // outs() << " the " << i + 1 << "th input" << "\n"; 


                    /*
                    if (DILocation *Loc = *(id2instr[i])) {
                        outs() << "Line " << Loc->getLine() << ":" << "\n";
                    }
                    */

                    break;
                }
            }

        }
    }

    void DocInputVar(Value* inputVar, Instruction* I, int isFile)
    {
        Type *inputType = inputVar -> getType();

        // outs() << "Operand " << i << " has type: " << *inputType << "\n";

        //Type *elementType = inputType -> getArrayElementType();

        // outs() << "Argument " << i << " corresponds to C variable: " << inputVar->getName() << "\n";
        Users.clear();
        visited.clear();
        findDefUseRelation(inputVar);

        id2inst[var_id] = I;
        id2var[var_id] = inputVar;
        var2id[inputVar] = var_id++;
        varNames[inputVar] = inputVar->getName();
        UserList.push_back(Users);
        varLine[inputVar] = -1;
        if (DILocation *Loc = I->getDebugLoc()) 
        { 
            varLine[inputVar] = Loc->getLine();
        }
        varIsFile[inputVar] = isFile;
    }

    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        //Branch Trace directory file
        std::fstream dictFile("branch_dictionary.txt", std::ios::in | std::ios::out | std::ios::trunc);

        int branchID =1;

        for (auto &F : M.functions()) {
            //errs()<<F.getName()<<"\n";

            //get the LLVM context
            LLVMContext &Ctx = F.getContext();
            //define external function's parameter types, for function pointer should be getInt8PtrTy
            std::vector<Type*> paramTypesfunc = {Type::getInt8PtrTy(Ctx)};
            std::vector<Type*> paramTypesbr = {Type::getInt32Ty(Ctx)};
            //define external function's return value type
            Type *retTypefunc = Type::getVoidTy(Ctx);
            //create function type, false means this function is not variadic
            FunctionType *logFuncType = FunctionType::get(retTypefunc, paramTypesfunc, false);
            FunctionType *logBrType = FunctionType::get(retTypefunc, paramTypesbr, false);

            FunctionCallee logFunc =F.getParent()->getOrInsertFunction("logfunction", logFuncType);
            FunctionCallee logBr = F.getParent()->getOrInsertFunction("logbranch", logBrType);
            Value* fopenRetVal = NULL;
            // Basic block
            for (auto &B : F) {
                // Instruction
                for (auto &I : B) {
                    if (fopenRetVal)
                        if (StoreInst *storeInst = dyn_cast<StoreInst>(&I)) {
                                    // Check if it's a store instruction
                                    Value *storedValue = storeInst->getValueOperand();
                                    if (storedValue == fopenRetVal) {
                                        Value *storePointer = storeInst->getPointerOperand();

                                        // errs() << "Store Instruction: " << *storeInst << "\n";
                                        // errs() << "Stored Value: " << *storedValue << "\n";
                                        // errs() << "Store Pointer: " << storePointer->getName() << "\n";
                                        DocInputVar(storePointer, &I, 1);
                                        fopenRetVal = NULL;
                                    }
                            }

                    // Find all scanf functions and file read functions
                    if (auto *callInst = dyn_cast<CallInst>(&I)) {
                        if (auto *calledFunction = callInst -> getCalledFunction()) {
                    
                            if (calledFunction -> getName() == "fopen") {
                                // Get file descriptor
                                fopenRetVal = callInst;                              
                            }
                            if (calledFunction -> getName() == "__isoc99_scanf") {

                                // Get scanf parameter list ([2, numArgs])
                                unsigned numArgs = callInst->getNumOperands() - 1;
                                outs() << "total number of var : " << numArgs << "\n";

                                for (unsigned i = 1; i < numArgs; i ++) {
                                    Value *inputVar = callInst -> getArgOperand(i);


                                    // outs() << "scanf Input Variable: " << *inputVar << "\n";
                                    // StringRef name = "Not found"; 
                                    // llvm::outs() << "Argument [] " << i << " has name: " << inputVar->getName() << "\n";

                                    DocInputVar(inputVar, &I, 0);
                                    
                                }
                            }
                        }
                    }

                    if (auto *brinst = dyn_cast<BranchInst>(&I)) {
                            //check if instruction is conditional
                            if(brinst->isConditional()){

                                if (DILocation *Loc = I.getDebugLoc()) { 
                                    errs() << "branch id " << branchID << "\n";
                                    brinst->print(llvm::outs());
                                    errs() << "\n";
                                    // Get var
                                    Value *var = brinst->getOperand(0);
                                    findInputVar(var);

                                    for (unsigned int i = 0; i < brinst->getNumSuccessors(); ++i) {

                                        BasicBlock *succ = brinst->getSuccessor(i);

                                        if (!succ->empty()){

                                            DILocation *succLoc = succ->front().getDebugLoc();
                                            errs() << "br_" << branchID << ": " << Loc->getFilename().str() << ", " << Loc->getLine() << ", " << succLoc->getLine() << "\n";
                                            dictFile << "br_" << branchID << ": " << Loc->getFilename().str() << ", " << Loc->getLine() << ", " << succLoc->getLine() << "\n";
                                            branchID++;

                                            IRBuilder<> Builder(Ctx);
                                            Builder.SetInsertPoint(&(succ->front()));
                                            
                                            Builder.CreateCall(logBr, {ConstantInt::get(Type::getInt32Ty(Ctx), branchID)});
                                        }                   
                                    }
                                }                           
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
                    //errs()<<I<<"\n";
            }
        }
    };
    dictFile.close();
    return PreservedAnalyses::none();
    }
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
                    MPM.addPass(SIFDPass());
                });
        }
    };
}
