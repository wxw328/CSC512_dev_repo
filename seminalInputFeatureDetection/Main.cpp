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

void ParseDefUse(Value* V, Value* ori);
struct MyVar {
    Value* V;
    int key;
    MyVar(Value* v, int val): V(v), key(val) {}
};

struct SIFDPass : public PassInfoMixin<SIFDPass> {
    vector<Value *> Users;
    vector<vector<Value *> > UserList;
    vector<MyVar*> myVars;
    map <Value *, bool> visited;
    map <Value *, bool> visited_temp;
    map <Value *, bool> sem_visited;
    map <Value *, int> var2id;
    map <int, Value *> id2var;
    map <int, Value *> id2inst;
    map <Value *, StringRef> varNames;
    map <Value *, int> varIsFile;
    map <Value *, int> varLine;
    Module* thisModule;
    map <Value *, vector<Value*>> Def2User;
    map <Value*, vector<Value*>> User2Def;
    map <MyVar*, MyVar*> pointer2pointer;
    
    int var_id = 0;
    int cmd_input_line = -1;
    // Decide whether a function is a user created function. Didn't find a general approach so this is specific to the target program.
    bool isUserFunction(Function *F) {
        return F->getName().startswith("MAIN_") || F->getParent()->getName().startswith("LBM_");
    }

    // This function links pointers in seperate functions. So that the def-use chain can expand on pointers
    void checkPointer() {
        for (auto &F : thisModule->functions()) {
            for (auto &BB : F) {
                // Go through every command
                for (auto &I : BB) {
                    if (auto *callInst = dyn_cast<CallInst>(&I)) {
                        if ((!callInst->getCalledFunction()) || (!isUserFunction(callInst->getCalledFunction())))
                            continue;
                        int operand_number = callInst->getNumOperands() - 1;
                        for (unsigned i = 0; i < operand_number; ++i) {
                            Value *arg = callInst->getArgOperand(i);
                            // If the called function uses pointer as argument
                            if (arg->getType()->isPointerTy()) {
                                if (arg->getType()->getPointerElementType()->isStructTy()) {
                                    // Get the variable in the target function
                                    visited_temp.clear();
                                    FindRealDef(&I, arg, 1);
                                    int o_idx = myVars.size();
                                    // Get real defs for this struct in the current function
                                    visited_temp.clear();
                                    FindRealDef(arg, arg, 0);
                                    int i_idx = myVars.size();
                                    // Get real def for this struct in the target function
                                    visited_temp.clear();
                                    FindRealDef(myVars[o_idx-1]->V, arg, 0);
                                    // Link pointers to struct member variables from outside and inside
                                    for (int i = o_idx; i < i_idx; i++) {
                                        for (int j = i_idx; j < myVars.size(); j++)
                                        {
                                            if (myVars[i]->key == myVars[j]->key)
                                            {
                                                pointer2pointer[myVars[j]] = myVars[i];
                                            }
                                        }
                                    }
                                } else {
                                    // Simply link the two pointers from outside and inside (May not always be correct)
                                    visited_temp.clear();
                                    FindRealDef(&I, arg, 1);
                                    myVars.push_back(new MyVar(arg, -1));
                                    var2id[arg] = myVars.size();
                                    pointer2pointer[myVars[myVars.size()-2]] = myVars[myVars.size()-1];
                                }
                                
                            }
                        }
                    }
                }
            }
        }
    }

    // Print the seminal input.
    void findInputVar(Value *V) {
        auto it = User2Def.find(V);
        // Look for the input related to this variable, if any
        if (it != User2Def.end())
        {
            for (auto rit = it->second.rbegin(); rit != it->second.rend(); ++rit)
            {
                Value* def_var = *rit;
                if (sem_visited.find(def_var) != sem_visited.end())
                {
                    continue;
                }
                sem_visited[def_var] = true;
                MyVar* this_var = myVars[var2id[def_var]];
                outs() << "**************Seminal Input**************" << "\n";
                // cmdline inputs
                if (this_var->key == -1)
                {
                    outs() << "Line " << cmd_input_line << ": " << "Command line input: " << myVars[var2id[def_var] - 1]->key << "\n"; 
                }
                // file
                else if (this_var->key == -2)
                {
                    outs() << "Line " << varLine[this_var->V] << ": size of file [" << this_var->V->getName() << "]" << "\n";
                }
                // normal var
                else if (this_var->key == -3)
                {
                    outs() << "Line " << varLine[this_var->V] << ": " << this_var->V->getName() << "\n";
                }
                // number of cmdline inputs
                else if (this_var->key == -4)
                {
                    outs() << "Line " << cmd_input_line << ": Number of comman line inputs." << "\n";
                }
                outs() << "*****************************************" << "\n";
                break;
            }
        }
    }

    // Find the "real" definition of a variable. For struct, it means a.xxx; For function, it's the corresponding variable in the called function; For cmdline, it's the input's index, i.e. arg[1]
    // V is the argument, ori has different meanings
    // type: 0 for struct, 1 for function, 2 for cmdline input
    void FindRealDef(Value* V, Value* ori, int type)
    {
        if (visited_temp.find(V) != visited_temp.end() && visited_temp[V] == true) {
            return;
        }
        visited_temp[V] = true;
        if (type == 0)
        {
            for (auto *user : V->users()) {
                if (StoreInst *storeInst = dyn_cast<StoreInst>(user)) {
                    // Check if it's a store instruction
                    Value *storePointer = storeInst->getPointerOperand();
                    FindRealDef(storePointer, ori, type); 
                }
                // This indicates an instruction extracts a certain member of the struct, so we make the extracted value the "real" definition
                if (auto getInArray = dyn_cast<GetElementPtrInst>(user)) {
                    Value* index = getInArray->getOperand(1);
                    if (ConstantInt *constint = dyn_cast<ConstantInt>(index)) {
                        myVars.push_back(new MyVar(ori, constint->getSExtValue()));
                        var2id[user] = myVars.size();
                        myVars.push_back(new MyVar(user, -1));
                        continue;
                    }
                }
                FindRealDef(user, ori, type);
            }
        }
        else if (type == 1)
        {
            if (auto *callInst = dyn_cast<CallInst>(V)) {
                if (auto *calledFunction = callInst -> getCalledFunction()) {
                    unsigned numArgs = callInst->getNumOperands() - 1;
                    int idx = -1;
                    vector<int> pointer_idx;
                    // Decide which argument is the variable
                    for (int i = 0; i < numArgs; i ++) {
                        if (ori == callInst -> getArgOperand(i)) {
                            idx = i;
                        }
                        else if (callInst -> getArgOperand(i)->getType()->isPointerTy()) {
                            pointer_idx.push_back(i);
                        }
                    }
                    if (idx < 0)
                    {
                        outs() << "Something wrong. \n";
                        return;
                    }
                    StringRef func_name = calledFunction -> getName();
                    // Look for the corresponding variable in the callee
                    for (auto &F : thisModule->functions()) {
                        if (F.getName() == func_name) {
                            myVars.push_back(new MyVar(F.getArg(idx), -1));
                        }
                    }
                }
            }
        }
        else if (type == 2)
        {
            for (auto *user : V->users()) {
                if (auto *function = dyn_cast<CallInst>(user)) {
                    FindRealDef(user, V, 1);
                    // In the called function
                    Value* new_def = myVars[myVars.size()-1]->V;
                    FindRealDef(new_def, V, 2);
                }
                if (StoreInst *storeInst = dyn_cast<StoreInst>(user)) {
                    // Check if it's a store instruction
                    Value *storePointer = storeInst->getPointerOperand();
                    FindRealDef(storePointer, V, 2);
                    continue;
                }
                // This indicates an instruction extracts a certain member of the cmdline input, so we make the extracted value the "real" definition
                if (auto *getInArray = dyn_cast<GetElementPtrInst>(user)) {
                    Value* index = getInArray->getOperand(1);
                    if (ConstantInt *constint = dyn_cast<ConstantInt>(index)) {
                        myVars.push_back(new MyVar(ori, constint->getSExtValue()));
                        var2id[user] = myVars.size();
                        myVars.push_back(new MyVar(user, -1));
                        visited.clear();
                        ParseDefUse(myVars[myVars.size()-1]->V, myVars[myVars.size()-1]->V);
                        continue;
                    }
                }
                FindRealDef(user, V, 2);
            }
        }
        
    }

    // Build def-use chain, ori is the original real input variable
    void ParseDefUse(Value* V, Value* ori)
    {
        if (visited.find(V) != visited.end() && visited[V] == true) {
            return;
        }
        visited[V] = true;
        // Is a struct, find its real definition
        Type *argType = V->getType();
        if (V->getType()->isStructTy()) {
            int real_def_begin = myVars.size();
            visited_temp.clear();
            FindRealDef(V, V, 0);
            for (int i = real_def_begin + 1; i < myVars.size(); i += 2) {
                ParseDefUse(myVars[i]->V, ori);
            }
        }
        // Is a struct pointer, find its real definition
        if (V->getType()->isPointerTy()) {
            Type *pointedType = V->getType()->getPointerElementType();
            if (pointedType->isStructTy()) {
                int real_def_begin = myVars.size();
                visited_temp.clear();
                FindRealDef(V, V, 0);
                for (int i = real_def_begin + 1; i < myVars.size(); i += 2) {
                    ParseDefUse(myVars[i]->V, ori);
                }              
            }
        }
        // Is a store, then the target address should also be included
        if (StoreInst *storeInst = dyn_cast<StoreInst>(V)) {
            // Check if it's a store instruction
            Value *storePointer = storeInst->getPointerOperand();
            Def2User[ori].push_back(storePointer);
            User2Def[storePointer].push_back(ori);
            ParseDefUse(storePointer, ori);
            auto my_var_it = var2id.find(storePointer);
            if (my_var_it != var2id.end())
            {
                auto it = pointer2pointer.find(myVars[my_var_it->second]);
                if (it != pointer2pointer.end())
                {
                    ParseDefUse(it->second->V, ori);
                }
            }
            return;
        }

        // Tranverse the users of this var
        for (auto *user : V->users()) {
            // If the user is a function call and the current variable is one of its parameters, then 
            // everything in the callee that's related to this parameter should be included
            if (auto *function = dyn_cast<CallInst>(user)) {
                if (function->getCalledFunction() && isUserFunction(function->getCalledFunction()))
                {
                    visited_temp.clear();
                    FindRealDef(user, V, 1);
                    ParseDefUse(myVars[myVars.size()-1]->V, ori);
                }
            }
            Def2User[ori].push_back(user);
            User2Def[user].push_back(ori);
            ParseDefUse(user, ori); 
        }
    }

    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        //Branch Trace directory file
        std::fstream dictFile("branch_dictionary.txt", std::ios::in | std::ios::out | std::ios::trunc);

        int branchID =1;

        thisModule = &M;

        checkPointer();

        for (auto &F : M.functions()) {
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
            // If this is main function with arguments, we need to process command line inputs
            if (F.getName() == "main") {
                cmd_input_line = F.getSubprogram()->getLine();
                // Has cmdline inputs
                if (F.arg_size() > 0) {
                    // This is not always the case, but representitive
                    Value *nArgs = F.getArg(0);
                    Value *args = F.getArg(1);
                    myVars.push_back(new MyVar(nArgs, -4));
                    var2id[nArgs] = myVars.size() - 1;
                    visited.clear();
                    ParseDefUse(nArgs, nArgs);
                    visited_temp.clear();
                    FindRealDef(args, args, 2);
                }
                
            }
            // Basic block
            for (auto &B : F) {
                // Instruction
                for (auto &I : B) {
                    if (auto *allocaInst = dyn_cast<AllocaInst>(&I)) {
                        // Check if this is the definition of function ptr
                        Type *allocaType = allocaInst->getAllocatedType();

                        if (isa<PointerType>(allocaType)) {
                            Type *elementType = allocaType->getPointerElementType();
                            // Is function ptr
                            // if (elementType->isFunctionTy()) {
                            //     myVars.push_back(new MyVar(allocaInst, -3));
                            //     var2id[allocaInst] = myVars.size() - 1;
                            //     visited.clear();
                            //     ParseDefUse(allocaInst, allocaInst);
                            //     varLine[allocaInst] = 8;
                            //     // For alloc instruction it seems not working
                            //     if (DILocation *Loc = I.getDebugLoc()) 
                            //     { 
                            //         varLine[allocaInst] = Loc->getLine();
                            //     }
                            // }
                        }
                    }
                    // After fopen, we catch the stored variable. This is for the purpose of getting input file's name
                    if (fopenRetVal)
                        if (StoreInst *storeInst = dyn_cast<StoreInst>(&I)) {
                                // Check if it's a store instruction
                                Value *storedValue = storeInst->getValueOperand();
                                if (storedValue == fopenRetVal) {
                                    Value *storePointer = storeInst->getPointerOperand();
                                    myVars.push_back(new MyVar(storePointer, -2));
                                    var2id[storePointer] = myVars.size() - 1;
                                    visited.clear();
                                    ParseDefUse(storePointer, storePointer);
                                    varLine[storePointer] = -1;
                                    if (DILocation *Loc = I.getDebugLoc()) 
                                    { 
                                        varLine[storePointer] = Loc->getLine();
                                    }
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

                                for (unsigned i = 1; i < numArgs; i ++) {
                                    Value *inputVar = callInst -> getArgOperand(i);
                                    varLine[inputVar] = -1;
                                    if (DILocation *Loc = I.getDebugLoc()) 
                                    { 
                                        varLine[inputVar] = Loc->getLine();
                                    }
                                    myVars.push_back(new MyVar(inputVar, -3));
                                    var2id[inputVar] = myVars.size() - 1;
                                    visited.clear();
                                    ParseDefUse(inputVar, inputVar);                                    
                                }
                            }
                        }
                    }

                    if (auto *brinst = dyn_cast<BranchInst>(&I)) {
                            //check if instruction is conditional
                            if(brinst->isConditional()){

                                if (DILocation *Loc = I.getDebugLoc()) { 
                                    errs() << "branch id " << branchID << "\n";
                                    brinst->print(llvm::errs());
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
                                           

                                            IRBuilder<> Builder(Ctx);
                                            Builder.SetInsertPoint(&(succ->front()));
                                            
                                            Builder.CreateCall(logBr, {ConstantInt::get(Type::getInt32Ty(Ctx), branchID)});
                                            branchID++;
                                        }                   
                                    }
                                }                           
                            }
                    }

                    if(auto *funcp = dyn_cast<CallInst>(&I)){
                        
                        if(!funcp->getCalledFunction()){
                            findInputVar(funcp);
                            errs()<<*funcp<<"\n";
                            IRBuilder<> Builder(funcp);

                            Value *callpointer = funcp->getCalledOperand();
                            Builder.CreateCall(logFunc, callpointer);
                        }
                        
                    }
                    // errs()<<I<<"\n";
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
