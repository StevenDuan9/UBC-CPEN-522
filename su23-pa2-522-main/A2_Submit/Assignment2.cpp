#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <cxxabi.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <string>

using namespace llvm;
using namespace std;



// get the taint line
int getSourceLine(Instruction *I)
{
	llvm::DebugLoc debugInfo = I->getDebugLoc();
	int line = -1;

	if (debugInfo)
		line = debugInfo.getLine();

	return line;
}


// get the block of llvm.dbg.declare callInst for a given value
BasicBlock* getCallBlock(Value *value, Function &F)
{
	for(auto &bb: F)
	{
		for (BasicBlock::const_iterator It = bb.begin(); It != bb.end(); ++It)
		{
			Instruction *inst = const_cast<llvm::Instruction *>(&*It);

			if(isa<CallInst>(inst))
			{
				CallInst* callInst = dyn_cast<CallInst>(inst);
				
				if (callInst->getCalledFunction()->getName() == "llvm.dbg.declare")
				{
					Metadata *Meta = cast<MetadataAsValue> (callInst->getOperand(0))->getMetadata();

					if (isa<ValueAsMetadata>(Meta)) {

						Value *v = cast<ValueAsMetadata>(Meta)->getValue();

						if (v == value)
						{
							return &bb;
						}
					}
				}
			}
		}
	}

	return NULL;
}



namespace {
    struct Assignment2 : public FunctionPass {
        static char ID;

        Assignment2() : FunctionPass(ID) {}

        bool runOnFunction(Function& F) override {

			// get the main function
			if (F.getName() == "main") 
			{    
				// maintain entryset and exitset
				map<BasicBlock*, set<Value*>> taintSet;
				map<BasicBlock*, set<Value*>> updatedTaintSet;

				for (auto &bb: F)
				{
					set<Value*> taint;
					taintSet[&bb] = taint;
				}



				bool hasChange = true;
				int counter = 0;

				// record the taint line
				map<Value*, int> taintTracker;
				map<Value*, int> untaintTracker;


				// stop if there are no change in entryset and exitset or force to stop after 100 iteration
				while(hasChange && (counter < 100))
				{
					hasChange = false;

					// iterate all blocks to start taint analysis
					for(auto &bb: F)
					{
						// record previous entryset and exitset
						set<Value*> previousEntrySet = taintSet[&bb];
						set<Value*> previousExitSet = updatedTaintSet[&bb];

						// initialize entryset by union
						for (auto Pred = pred_begin(&bb), End = pred_end(&bb); Pred != End; ++Pred)
						{
							BasicBlock *predecessor = *Pred;
							taintSet[&bb].insert(updatedTaintSet[predecessor].begin(), updatedTaintSet[predecessor].end());
						}

						// check if there are changes of entryset
						if(previousEntrySet != taintSet[&bb])
						{
							hasChange = true;
						}

						updatedTaintSet[&bb] = taintSet[&bb];



						// for all instruction in each block
						for (BasicBlock::const_iterator It = bb.begin(); It != bb.end(); ++It)
						{
							Instruction *inst = const_cast<llvm::Instruction *>(&*It);


							// check if it is a callInst
							if (isa<llvm::CallInst>(inst))
							{
								CallInst *callinst = dyn_cast<CallInst>(inst);
								Function *calledFunction = callinst->getCalledFunction();


								// check if it is cin input
								if (calledFunction->getReturnType()->isPointerTy()
										&& calledFunction->getReturnType()->getPointerElementType()->isStructTy()
										&& calledFunction->getReturnType()->getPointerElementType()->getStructName() == "class.std::basic_istream")
								{
									updatedTaintSet[&bb].insert(callinst->getOperand(1));
									taintTracker.insert(make_pair(callinst->getOperand(1), getSourceLine(inst)));
								}
								// check if it is a primitive-type(integer, floatingpoint) function
								else if (calledFunction->getReturnType()->isIntegerTy() || calledFunction->getReturnType()->isFloatingPointTy())
								{
									bool hasTaintArgs = false;

									// check if there are any taint input
									for (int i = 0; i < int(callinst->getNumArgOperands()); i++) 
									{
										Value *arg = callinst->getArgOperand(i);

										if (updatedTaintSet[&bb].count(arg))
										{
											hasTaintArgs = true;
										}
									}

									// if there are taint input, the output will be taint
									if (hasTaintArgs)
									{
										updatedTaintSet[&bb].insert(callinst);
										taintTracker.insert(make_pair(callinst, getSourceLine(inst)));
									}
								}
							}


							// check if it is a storeInst
							if (isa<llvm::StoreInst>(inst))
							{
								StoreInst *storeInst = dyn_cast<StoreInst>(inst);

								Value *storePointer = storeInst->getPointerOperand();
								Value *storeValue = storeInst->getValueOperand();

								if (updatedTaintSet[&bb].count(storeValue))
								{
									updatedTaintSet[&bb].insert(storePointer);
									taintTracker.insert(make_pair(storePointer, getSourceLine(inst)));
								}
								else if (updatedTaintSet[&bb].count(storePointer))
								{
									updatedTaintSet[&bb].erase(storePointer);
									untaintTracker.insert(make_pair(storePointer, getSourceLine(inst)));
								}
							}

							// check if it is a loadInst
							if (isa<llvm::LoadInst>(inst))
							{
								LoadInst *loadInst = dyn_cast<LoadInst>(inst);
								Value *pointeroperand = loadInst->getPointerOperand();

								if (updatedTaintSet[&bb].count(pointeroperand))
								{
									updatedTaintSet[&bb].insert(loadInst);
									taintTracker.insert(make_pair(loadInst, getSourceLine(inst)));
								}
							}

							// check if it is a binaryoperation
							if (isa<llvm::BinaryOperator>(inst))
							{
								BinaryOperator *binaryOperator = dyn_cast<BinaryOperator>(inst);
								Value *value1 = binaryOperator->getOperand(0);
								Value *value2 = binaryOperator->getOperand(1);

								if (updatedTaintSet[&bb].count(value1) || updatedTaintSet[&bb].count(value2))
								{
									updatedTaintSet[&bb].insert(binaryOperator);
									taintTracker.insert(make_pair(binaryOperator, getSourceLine(inst)));
								}
							}
						} // for each instruction

						// check if there are changes of exitset
						if (previousExitSet != updatedTaintSet[&bb])
						{
							hasChange = true;
						}

					} // for each block

					counter = counter + 1;
				} // while


				// print the taint analysis result
				printTracker(taintTracker, untaintTracker, updatedTaintSet, F);
			} // if
					
			return false;
		} // run on function


		// import dominator tree
		void getAnalysisUsage(AnalysisUsage& AU) const
		{
			AU.addRequired<DominatorTreeWrapperPass>();
			AU.addRequired<PostDominatorTreeWrapperPass>();
			AU.setPreservesAll();
		}
 

		void printTracker(map<Value*, int> taintTracker, map<Value*, int> untaintTracker, map<BasicBlock*, set<Value*>> taintSet, Function &F)
		{
			DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
			int maxline = 0;

			for(auto &bb: F)
			{
				if (isa<llvm::ReturnInst>(bb.getTerminator()))
				{
					for (map<Value*, int>::iterator it=taintTracker.begin(); it!=taintTracker.end(); ++it)
					{
						if (it->second > maxline)
							maxline = it->second;
					}

					for (map<Value*, int>::iterator it=untaintTracker.begin(); it!=untaintTracker.end(); ++it)
					{
						if (it->second > maxline)
							maxline = it->second;
					}


					for (int i = 1; i <= maxline; i++)
					{
						for (map<Value*, int>::iterator it=taintTracker.begin(); it!=taintTracker.end(); ++it)
						{
							if (i == it->second)
							{
								if (AllocaInst* allocaInst = dyn_cast<AllocaInst>(it->first))
								{
									if (DT.dominates(getCallBlock(it->first, F), &bb))
									{
										errs() << "Line " << it->second << ": " << allocaInst->getName() << " is tainted" << "\n";
									}
								}
							}
						}

						for (map<Value*, int>::iterator it=untaintTracker.begin(); it!=untaintTracker.end(); ++it)
						{
							if (i == it->second)
							{
								if (AllocaInst* allocaInst = dyn_cast<AllocaInst>(it->first))
								{
									if (DT.dominates(getCallBlock(it->first, F), &bb))
									{
										errs() << "Line " << it->second << ": " << allocaInst->getName() << " is now untainted" << "\n";
									}
								}
							}
						}
					}


					errs() << "Tainted: {";

					auto it = taintSet[&bb].begin();
					Value *value = *it;

					if (it != taintSet[&bb].end())
					{
						if (AllocaInst* allocaInst = dyn_cast<AllocaInst>(value))
						{
							if (DT.dominates(getCallBlock(value, F), &bb))
							{
								errs() << allocaInst->getName();
							}
                        			}

						++it;
                    			}


					while (it!=taintSet[&bb].end())
					{
						value = *it;

						if (AllocaInst* allocaInst = dyn_cast<AllocaInst>(value))
						{
							if (DT.dominates(getCallBlock(value, F), &bb))
							{
                                				errs() << "," << allocaInst->getName();
							}
						}

						++it;
					}

					errs() << "}" << "\n";
				} // if
			} // for
		} // printTracker 
	}; // struct
}// namespace

char Assignment2::ID = 0;

static RegisterPass<Assignment2> X("taintanalysis", "Pass to find tainted variables");
