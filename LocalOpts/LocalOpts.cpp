// 15-745 S13 Assignment 2: LocalOpts.cpp
//
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
// Modified by Ankit Goyal
////////////////////////////////////////////////////////////////////////////////

#define DEBUG_TYPE "my-local-opts"

#include "llvm/Pass.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/AssemblyAnnotationWriter.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/InstVisitor.h"

#include <ostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "generic_methods.h"
//#include "my_arithmatic.h"

STATISTIC(NumXForms, "The # of algebric identies transformations performed");
STATISTIC(NumConstFolds, "The # of constant folding operations");
STATISTIC(NumStrReductions, "The # of strength reductions");

using namespace llvm;

namespace {

    bool replay = false; //in case the first statement is optimized. replay the first statement for optimization.

    template<typename APType, typename ConstantType>
        Value * algIdentityAS(Instruction &i, APType generalZeroOne, int operation){
            //errs() << i << "\n";
            Value *operand1 = i.getOperand(0);
            Value *operand2 = i.getOperand(1);
            /* ConstantInt and ConstantFP. http://llvm.org/docs/doxygen/html/Constants_8h.html */
            /* Constant types */
            ConstantType *cint;


            if(cint = dyn_cast<ConstantType>(operand2)){
                if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                    return operand1;
            }
            else if((cint = dyn_cast<ConstantType>(operand1)) && operation!=1){
                if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                    return operand2;
            }
            return NULL;
        }


    bool isInstEquivalent(const Instruction* inst1, const Instruction* inst2) {
        // Check if the opcodes (operators) are the same
        if (inst1->getOpcode() != inst2->getOpcode()) {
            return false;
        }

        // Check if the number of operands is the same
        if (inst1->getNumOperands() != inst2->getNumOperands()) {
            return false;
        }

        // Check if each operand of inst1 is the same as the corresponding operand of inst2
        for (unsigned i = 0; i < inst1->getNumOperands(); ++i) {
            if (inst1->getOperand(i) != inst2->getOperand(i)) {
                return false;
            }
        }

        // If all criteria pass, the instructions are considered equivalent
        return true;
    }
    //check for X-X and X/X case. Check if the variables are identical.
    Value * varConVar(Instruction &i, ConstantInt* zeroOne){
        if(i.getNumOperands() != 2 ) return NULL;
        Value *op1 = i.getOperand(0);
        Value *op2 = i.getOperand(1);
        //errs() <<  "varConVar"  <<  op1 << " " << op2 << "\n";
        if(i.getOperand(0) == i.getOperand(1)) return zeroOne;
        return NULL;
    }

    //TODO Refactor this thing!
    template<typename APType, typename ConstantType>
        Value * algIdentityMD(Instruction &i, APType generalZeroOne, int operation, int identity){ //0: Multiplication. 1 Division
            Value *operand1 = i.getOperand(0);
            Value *operand2 = i.getOperand(1);

            errs() << "algIdentityMD " << i << "\n";
            /* ConstantInt and ConstantFP. http://llvm.org/docs/doxygen/html/Constants_8h.html */
            /* Constant types */
            ConstantType *cint;
            switch(operation){
                case 0:{ //multiplication
                    if(identity == 0){
                        if(cint = dyn_cast<ConstantType>(operand2)){ //operand 2 is 0 and it's multiplication
                            if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                return ConstantType::get(cint->getContext(), generalZeroOne);
                        }
                        else if(cint = dyn_cast<ConstantType>(operand1)){ //operand 1 is 0 and it's multiplication
                            if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                return ConstantType::get(cint->getContext(), generalZeroOne);
                        }
                    }else if(identity == 1){
                        if(cint = dyn_cast<ConstantType>(operand2)){ //operand 2 is 1 and it's multiplication
                            errs() << "check operand2 is 1 " << id_compare<ConstantType, APType>(*cint, generalZeroOne) << "\n";
                            if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                return operand1;
                        }
                        else if(cint = dyn_cast<ConstantType>(operand1)){ //operand 1 is 1 and it's multiplication
                            errs() << "check operand1 is 1 " << id_compare<ConstantType, APType>(*cint, generalZeroOne) << "\n";
                            if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                return operand2;
                        }

                    }
                    break;
                }
                case 1:{ //division
                    if(identity == 0){
                        if(cint = dyn_cast<ConstantType>(operand1)){ //operand 1 is 0 and it's division 0 / X = 0
                            if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                return ConstantType::get(cint->getContext(), generalZeroOne);
                        }
                    }else if(identity == 1){
                        if(cint = dyn_cast<ConstantType>(operand2)){ //operand 2 is 1 and it's division X / 1 = X
                            if(id_compare<ConstantType, APType>(*cint, generalZeroOne))
                                return dyn_cast<ConstantType>(operand1); //if operand 2 is 1 return X
                        }
                    }
                    break;
                }
            }

            return NULL;
        }


    void makeTheChanges(BasicBlock::iterator &ib, Value* val){
        ib->replaceAllUsesWith(val);
        BasicBlock::iterator j = ib;
        ++ib;
        j->eraseFromParent();
        if(ib != ib->getParent()->begin()){
            --ib;
        }else{
            replay = true;
        }
    }

    void reduce_strength(BasicBlock::iterator &ib, ConstantInt &cint, Value &other, unsigned opcode){
        ++NumStrReductions;
        APInt apint = cint.getValue();
        unsigned reduction_factor = apint.logBase2();
        BinaryOperator* shift_instruction ;
        if(opcode == Instruction::Mul)
            shift_instruction = BinaryOperator::Create( Instruction::Shl, &other, ConstantInt::get(cint.getType(), reduction_factor, false));
        else
            shift_instruction = BinaryOperator::Create( Instruction::AShr, &other, ConstantInt::get(cint.getType(), reduction_factor, false));
        ib->getParent()->getInstList().insertAfter(ib, shift_instruction);
        makeTheChanges(ib, shift_instruction);
        ib++;
    }
    bool can_reduce_strength(ConstantInt& tcint){
        APInt op1 = tcint.getValue();
        if(op1.isPowerOf2()) return true;
        return false;
    }

    ConstantInt* fold_constants(unsigned operation, ConstantInt *op1, ConstantInt *op2){
        switch(operation){
            case Instruction::Add:
                return ConstantInt::get(op1->getContext(), op1->getValue() + op2->getValue());
            case Instruction::Sub:
                return ConstantInt::get(op1->getContext(), op1->getValue() + op2->getValue());
            case Instruction::Mul:
                return ConstantInt::get(op1->getContext(), op1->getValue() * op2->getValue());
            case Instruction::UDiv:
                return ConstantInt::get(op1->getContext(), op1->getValue().udiv(op2->getValue()));
            case Instruction::SDiv:
                return ConstantInt::get(op1->getContext(), op1->getValue().sdiv(op2->getValue()));

        }
        return NULL;
        errs() << "An opportunity to fold constants here.." ;

    }


    class LocalOpts : public FunctionPass{
        public:
            static char ID;
            LocalOpts() : FunctionPass(ID) { } //default constructor
            ~LocalOpts() {  }

            /* Method to print details for a all the functions in a module */
            void printFunctionDetails(Function &F) {
                int arg_size = F.arg_size();
                int num_call_sites = F.getNumUses();
                int num_basic_blocks = F.size(); //defined in value class.
                /*count the number of instructions.*/
                int number_of_instructions = 0;
                for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
                    number_of_instructions += 1;

                errs() << F.getName() <<": arguments=" << arg_size << " call sites=" <<  num_call_sites << " basic blocks=" << num_basic_blocks << " instructions=" << number_of_instructions << "\n\n";
            }


            std::map<Value*, Instruction*> valueMap;

            void runOnBasicBlock(BasicBlock &blk){
                for (BasicBlock::iterator i = blk.begin(), e = blk.end(); i != e; ++i){
                    if(replay){
                        i = blk.begin();
                        replay = false;
                    }

                    Instruction *ii= dyn_cast<Instruction>(i);

                    Value *v;
                    IntegerType *intype;
                    intype = dyn_cast<IntegerType>(i->getType());

                    errs() << "---new inst--" << "\n";

                    switch(ii->getOpcode()){
                        case Instruction::Add:
                            v = algIdentityAS<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0),0);
                            if(v) {
                                makeTheChanges(i, v);
                                ++NumXForms;
                                continue;
                            }

                            break; //X+0 = 0+X = X
                        case Instruction::Sub: {
                            //check for x-x thing
                            ConstantInt * zero = ConstantInt::get(intype->getContext(), getZeroOne<APInt>(intype->getBitWidth(),0));
                            errs() << *ii << " meet sub" << "\n\n";
                            Value *op1 = ii->getOperand(0);
                            Value *op2 = ii->getOperand(1);

                            errs() <<  "compare by value map "  <<  valueMap[op1] << " "<< valueMap[op2] <<
                            " " << isInstEquivalent(valueMap[op1],valueMap[op2]) << "\n";

                            if ((v = varConVar(*ii, zero)) || isInstEquivalent(valueMap[op1],valueMap[op2])) {
                                errs() << *ii << " x-x is zero" << "\n";
                                makeTheChanges(i, zero);
                                ++NumXForms;
                                continue;
                            }

                            else if(v = algIdentityAS<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0), 1)) {
                                errs() << *ii << " x - zero" << "\n";
                                makeTheChanges(i, v);
                                ++NumXForms;
                                continue;
                            } else {
                                errs() << *ii  <<" nothing happend" << "\n";
                            }
                            break; //X-0 = X;
                        }
                        case Instruction::Mul:{
                            //X * 1 = X , 1 * X = X; operation = 0 identity = 1
                            errs() << *ii << " meet mul" << "\n";
                            if(v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),1), 0, 1)) {
                                errs() << *ii  <<" multiply with one" << "\n";
                                makeTheChanges(i, v);
                                ++NumXForms;
                                continue;

                            }
                            else if (v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0), 0, 0)){ //X * 0 = 0, 0 * X = 0 OP=0 ID = 0
                                errs() << *ii  <<" multiply with zero" << "\n";
                                makeTheChanges(i, v);
                                ++NumXForms;
                                continue;
                            } else {
                                errs() << *ii  <<" nothing happend" << "\n";
                            }
                            break; //X*1 = X; 1*X=X
                        }

                        case Instruction::UDiv:
                        case Instruction::SDiv:{
                            ConstantInt * one = ConstantInt::get(intype->getContext(), getZeroOne<APInt>(intype->getBitWidth(),1));
                            Value *op1 = ii->getOperand(0);
                            Value *op2 = ii->getOperand(1);
                            errs() << *ii  <<" divide " << "\n";

                            bool twoOpGenBySameInst = !dyn_cast<ConstantInt>(op1) &&  !dyn_cast<ConstantInt>(op2)
                                                      && isInstEquivalent(valueMap[op1],valueMap[op2]);
                            if((v = varConVar(*ii, one)) || twoOpGenBySameInst) {
                                errs() << *ii << " x/x is 1" << "\n";
                                makeTheChanges(i, one);
                                ++NumXForms;
                                continue;
                            }

                            else if(v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),1),1,1))// X / 1 = X; OP=1 ID=1
                            {
                                errs() << *ii  <<" divide by one" << "\n";
                                makeTheChanges(i, v);
                                ++NumXForms;
                                continue;
                            }
                            else if(v = algIdentityMD<APInt, ConstantInt>(*ii, getZeroOne<APInt>(intype->getBitWidth(),0),1,0)) // X / 1 = X OP=1 ID=0
                            {
                                errs() << *ii  <<" 0 get divided" << "\n";
                                makeTheChanges(i,v);
                                ++NumXForms;
                                continue;
                            }
                            break;
                        }
                        default: break;
                    }

                    //Constant Folding
                    if((ii->getNumOperands() == 2) && isa<Constant>(ii->getOperand(0)) && isa<Constant>(ii->getOperand(1))){
                        Value *v1 = fold_constants(ii->getOpcode(), dyn_cast<ConstantInt>(ii->getOperand(0)), dyn_cast<ConstantInt>(ii->getOperand(1)));
                        if(v1) {
                            makeTheChanges(i,v1);
                            ++NumConstFolds;
                            continue;
                        }
                    }

                    //strength reduction
                    switch(ii->getOpcode()){
                        case Instruction::UDiv:
                        case Instruction::SDiv:
                        case Instruction::Mul:{
                            ConstantInt *tcint;
                            Value *other;
                            if((tcint = dyn_cast<ConstantInt>(ii->getOperand(0))) && can_reduce_strength(*tcint)){
                                other = ii->getOperand(0);
                                reduce_strength(i, *tcint, *other,ii->getOpcode());
                                continue;
                            }
                            else if((tcint = dyn_cast<ConstantInt>(ii->getOperand(1))) && can_reduce_strength(*tcint)){
                                other = ii->getOperand(0);
                                reduce_strength(i, *tcint, *other,ii->getOpcode());
                                continue;
                            }
                            break;
                        }
                        default: break;
                    }

                Value *val = dyn_cast<Value>(ii);
                if (val) {
                    valueMap[val] = ii;
                    // 'inst' is a valid Value, and 'val' now points to it
                }

                }
            }

            virtual bool runOnFunction(Function &func){
                //errs() << "Function: " << func.getName() << "\n";
                for (Function::iterator i = func.begin(), e = func.end(); i != e; ++i){ //iterating over the basic blocks
                    runOnBasicBlock(*i);
                    //print_basic_block_info(*i);
                }
                return true;
            }


            // We don't modify the program, so we preserve all analyses
            virtual void getAnalysisUsage(AnalysisUsage &AU) const {
                AU.setPreservesAll();
            }

        private:
            //helper functions
            void print_basic_block_info(BasicBlock &b){
                errs() << "Basic block (name=" << b.getName() << ") has " << b.size() << " instructions.\n";

            }
    };

    char LocalOpts::ID = 0;

    static void registerMyPass(const PassManagerBuilder &,
            legacy::PassManagerBase &PM) {
        PM.add(new LocalOpts());
    }
    RegisterStandardPasses
        RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                registerMyPass);

    RegisterPass<LocalOpts> X("my-local-opts", "Local Optimizations");

}
