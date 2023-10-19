// ECE 5984 S20 Assignment 3: DeadCodeElimination.cpp
// Group: Sunny Wadkar and Ramprasath Pitta Sekar

////////////////////////////////////////////////////////////////////////////////

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IntrinsicInst.h"
#include <stack>
#include "dataflow.h"

using namespace llvm;
using namespace std;

namespace {

  class DeadCodeElimination : public FunctionPass {
  public:
    static char ID;
    DeadCodeElimination() : FunctionPass(ID) { }
    virtual bool runOnFunction(Function& F) {
      backorder.clear();
      bbSets.clear();
      domainMap.clear();
      revDomainMap.clear();
      std::vector<Instruction*> instructionSet;
      for (BasicBlock &BB : F)
      {
        for (Instruction &II : BB)
        {
          Instruction *I = &II;
          if(!isLive(I) && (dyn_cast<PHINode>(I) == NULL))
          {
            instructionSet.push_back(I);
          }
	      }
      }
      po_iterator<BasicBlock*> start = po_begin(&F.getEntryBlock());
      po_iterator<BasicBlock*> end = po_end(&F.getEntryBlock());
      while(start != end)
      {
        backorder.push_back(*start);
        ++start;
      }

      BitVector boundaryCond(instructionSet.size(), true);
      BitVector initCond(instructionSet.size(), true);
      dataFlow* df = new dataFlow(instructionSet.size(),INTERSECTION,BACKWARD,boundaryCond,initCond);
      genTFDependencies(instructionSet);
      df->executeDataFlowPass(F,bbSets);
      // df->printMapping();
      stack<Instruction*> delList = getDeleteSet(df->dataFlowHash,instructionSet.size());
      if(!delList.empty())
      {
        deleteDeadInstructions(delList);
        return true;
      }
      return false;
    }

    virtual void getAnalysisUsage(AnalysisUsage& AU) const {
      AU.setPreservesAll();
    }

  private:
    std::vector<BasicBlock*> backorder;
    std::map<BasicBlock*,basicBlockDeps*> bbSets;
    std::map<Instruction*,int> domainMap;
    std::map<int,Instruction*> revDomainMap;
    void genTFDependencies(std::vector<Instruction*> domain)
    {
      BitVector empty((int)domain.size(), false);
      BitVector phiValues = empty;
      int vectorIdx = 0;
      for(Instruction* v : domain)
      {
        domainMap[v] = vectorIdx;
        revDomainMap[vectorIdx] = v;
        ++vectorIdx;
      }
      for(BasicBlock* BB : backorder)
      {
        struct basicBlockDeps* bbSet = new basicBlockDeps();
        std::map<int,std::vector<int>> checkLHS;
        bbSet->blockRef = BB;
        bbSet->genSet = empty;
        bbSet->killSet = empty;
        for(BasicBlock::reverse_iterator II = BB->rbegin(); II != BB->rend(); ++II)
        {
          Instruction *I = &*II;
          if(std::find(domain.begin(),domain.end(),I) != domain.end())
          {
            bbSet->genSet.set(domainMap[I]);
            if(bbSet->killSet[domainMap[I]])
            {
              bbSet->genSet.reset(domainMap[I]);
            }
          }
          for(int opc = 0; opc < (int)I->getNumOperands(); ++opc)
          {
            Value* op = I->getOperand(opc);
            if(PHINode* phi = dyn_cast<PHINode>(op))
            {
              for(int z = 0; z < (int)phi->getNumIncomingValues();++z)
              {
                Value* phi_val = phi->getIncomingValue(z);
                if(std::find(domain.begin(),domain.end(),phi_val) != domain.end())
                {
                  phiValues.set(domainMap[(Instruction*)phi_val]);
                  bbSet->killSet.set(domainMap[(Instruction*)phi_val]);
                }
              }
            }
            else if(std::find(domain.begin(),domain.end(),op) != domain.end())
            {
              bbSet->killSet.set(domainMap[(Instruction*)op]);
              if(dyn_cast<BinaryOperator>(I))
              {
                checkLHS[domainMap[(Instruction*)op]].push_back(domainMap[I]);
              }
            }
          }
        }
        bbSet->NonSeparableDFASet = checkLHS;
        bbSets[BB] = bbSet;
      }
    }

    bool isLive(Instruction* I)
    {
      return(I->isTerminator() || isa<DbgInfoIntrinsic>(I) || isa<LandingPadInst>(I) || I->mayHaveSideEffects());
    }

    stack<Instruction*> getDeleteSet(std::map<BasicBlock*,basicBlockProps*> dFData, int domainSize)
    {
      stack<Instruction*> delList;
      BitVector del(domainSize, true);
      std::map<BasicBlock*,basicBlockProps*>::iterator it = dFData.begin();
      while(it != dFData.end())
      {
        struct basicBlockProps* temp = dFData[it->first];
        del &= temp->bbInput;
        it++;
      }
      for(int i = 0 ; i < (int)del.size(); i++)
      {
        if(del[i])
        {
          delList.push(revDomainMap[i]);
        }
      }
      return delList;
    }

    void deleteDeadInstructions(stack<Instruction*> delList)
    {
      while(!delList.empty())
      {
          outs() << "Removing Instruction " <<*delList.top() << "\n";
          delList.top()->eraseFromParent();
          delList.pop();
      }
    }
  };

  char DeadCodeElimination::ID = 0;
  RegisterPass<DeadCodeElimination> X("dead-code-elimination", "ECE 5984 DCE");
}
