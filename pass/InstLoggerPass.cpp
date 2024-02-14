#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <llvm/Support/Casting.h>

using namespace llvm;

namespace {
    struct InstLoggerPass : public FunctionPass {
        
        static char ID;

        InstLoggerPass() : FunctionPass(ID) {}

        bool isFuncLogger(StringRef name) {
            return name == "instLogger";
        }

        virtual bool runOnFunction(Function &F) {
            if (isFuncLogger(F.getName())) {
                return false;
            }

            LLVMContext &Ctx = F.getContext();
            IRBuilder<> builder(Ctx);
            Type *retType = Type::getVoidTy(Ctx);

            // Prepare instLogger function
            ArrayRef<Type *> instParamTypes = {builder.getInt8Ty()->getPointerTo()};
            FunctionType *instLogFuncType = FunctionType::get(retType, instParamTypes, false);
            FunctionCallee instLogFunc = F.getParent()->getOrInsertFunction("instLogger", instLogFuncType);

            for (auto &B : F) {
                for (auto &I : B) {
                    if (auto *phi = dyn_cast<PHINode>(&I)) {
                        continue;
                    }
                    Value *valueAddr = ConstantInt::get(builder.getInt64Ty(), (int64_t)(&I));
                    if (auto *call = dyn_cast<CallInst>(&I)) {
                        Function *callee = call->getCalledFunction();
                        if (!(callee && !isFuncLogger(callee->getName()))) {
                            continue; 
                        }
                    }
                    auto *inst = dyn_cast<Instruction>(&I);
                    // Insert before call
                    builder.SetInsertPoint(inst);

                    // Insert a call to instLogFunc function
                    Value *instName = builder.CreateGlobalStringPtr(inst->getOpcodeName());
                    Value *args[] = {instName};
                    builder.CreateCall(instLogFunc, args);
                }
            }
            return true;
        }
    };
}

char InstLoggerPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerMyPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
    PM.add(new InstLoggerPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerMyPass);
