// Assignment 1 Template
// Assignment1_template.cpp

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

using namespace llvm;
using namespace std;

// Comment out the line below to turn off all debug statements.
// **Note** For final submission of this assignment,
//          please comment out the line below.
//#define __DEBUG__

// Output strings for debugging
std::string debug_str;
raw_string_ostream debug(debug_str);

// Strings for output
std::string output_str;
raw_string_ostream output(output_str);

// Demangles the function name.
std::string demangle(const char *name) {
  int status = -1;

  std::unique_ptr<char, void (*)(void *)> res{
      abi::__cxa_demangle(name, NULL, NULL, &status), std::free};
  return (status == 0) ? res.get() : std::string(name);
}

// Function to attach debug Metadata to an instruction
void addDebugMetaData(Instruction *I, char *debugInfo) {
  LLVMContext &C = I->getContext();
  MDNode *N = MDNode::get(C, MDString::get(C, debugInfo));
  char DebugMetadata[100];
  strcpy(DebugMetadata, "cpen400Debug.");
  strcat(DebugMetadata, debugInfo);
  I->setMetadata(DebugMetadata, N);
}

// Returns the source code line number cooresponding to the LLVM instruction.
// Returns -1 if the instruction has no associated Metadata.
int getSourceCodeLine(Instruction *I) {
  // Get debugInfo associated with every instruction.
  llvm::DebugLoc debugInfo = I->getDebugLoc();

  int line = -1;
  if (debugInfo)
    line = debugInfo.getLine();

  return line;
}

// Topologically sort all the basic blocks in a function.
// Handle cycles in the directed graph using Tarjan's algorithm
// of Strongly Connected Components (SCCs).
vector<BasicBlock *> topoSortBBs(Function &F) {
  vector<BasicBlock *> tempBB;
  for (scc_iterator<Function *> I = scc_begin(&F), IE = scc_end(&F); I != IE;
       ++I) {

    // Obtain the vector of BBs in this SCC and print it out.
    const std::vector<BasicBlock *> &SCCBBs = *I;

    for (std::vector<BasicBlock *>::const_iterator BBI = SCCBBs.begin(),
                                                   BBIE = SCCBBs.end();
         BBI != BBIE; ++BBI) {

      BasicBlock *b = const_cast<llvm::BasicBlock *>(*BBI);
      tempBB.push_back(b);
    }
  }

  reverse(tempBB.begin(), tempBB.end());
  return tempBB;
}

namespace {
struct Assignment1 : public FunctionPass {
  static char ID;

  // Vector to store the line numbers at which undefined
  // variable(s) is(are) used.
  vector<int> BuggyLines;

  // Keep track of all the functions we have encountered so far.
  unordered_map<string, bool> funcNames;

  // Reset all global variables when a new function is called.
  void cleanGlobalVariables() {

    BuggyLines.clear();
    output_str = "";
    debug_str = "";
  }

  Assignment1() : FunctionPass(ID) {}

  map<BasicBlock*, set<Value*>> EntrySet;
  map<BasicBlock*, set<Value*>> ExitSet;
  
  // Complete this function.
  // The function should insert the buggy line numbers
  // in the "BuggyLines" vector.
  void checkUseBeforeDef(Instruction *I, BasicBlock *b) {

    bool isBug = false;



    // 3.a
    if (isa<llvm::AllocaInst>(I)){
       
       debug_str = debug_str + "Case a: It is AllocaInst\n";


       AllocaInst *Alloca = dyn_cast<AllocaInst>(I);

       //if (!isa<llvm::PointerType>(Alloca->getType())){
       EntrySet[b].insert(Alloca);
       //}
    }


    // 3.b
    if (isa<llvm::StoreInst>(I)){

       debug_str = debug_str + "Case b: It is StoreInst\n";

       StoreInst *storeInst = dyn_cast<StoreInst>(I);

       // Get the pointer to the memory location where the value is stored and its name
       Value* storePointer = storeInst->getPointerOperand();
       


       // Get the value being stored and its name
       Value* storedValue = storeInst->getValueOperand();
       //StringRef valueVariableName = storedValue->getName();


      
       //debug_str = debug_str + " The value has the name: " + valueVariableName.str() + "\n";
       // debug_str = debug_str + " The pointer has the name: " + pointerVariableName.str() + "\n";


           
       if (EntrySet[b].count(storedValue)){
   	   
         EntrySet[b].insert(storePointer);	    
	 isBug = true;
       
       	 debug_str = debug_str + " Store Inst bug!!!!!!!!!!\n";
	 //debug_str = debug_str + " " + valueVariableName.str() + " add into EntrySet\n"; 

       }
       else if(EntrySet[b].count(storePointer)) {
		   
         EntrySet[b].erase(storePointer);
     	// debug_str = debug_str + " "  + pointerVariableName.str() + " remove from Entryset(case 3.bb) \n";

       }    
    }

    // 3.c
    if (isa<llvm::LoadInst>(I)){

       debug_str = debug_str + "Case c: It is LoadInst\n";


       LoadInst *Load = dyn_cast<LoadInst>(I);

       Value *PointerOperand = Load->getPointerOperand();
      // StringRef VariableName = PointerOperand->getName();


      // debug_str = debug_str + " " + VariableName.str() + " load\n";
      // debug_str = debug_str + " " + Load->getName().str() + "loaded\n";


       if (EntrySet[b].count(PointerOperand)) {
	 // Value *LoadedValue = Load->
         // StringRef LoadedValueName = LoadedValue->getName();


	 
	 debug_str = debug_str + " Load Inst bug!!!!!!!!!!\n";


	 isBug = true;
	 //addDebugMetaData(I, strdup("Load inst bug"));
	 
	 EntrySet[b].insert(Load);


       }

       //debug_str = debug_str + " " + VariableName.str() + " add into EntrySet\n";
    }



    // Add code here...




    if (isBug) {
      int line = getSourceCodeLine(I);
      if (line > 0)
        BuggyLines.push_back(line);
    }

    return;
  }

  // Function to return the line numbers that uses an undefined variable.
  bool runOnFunction(Function &F) override {

    std::string funcName = demangle(F.getName().str().c_str());

    // Remove all non user-defined functions and functions
    // that starts with '_' or has 'std'.
    if (F.isDeclaration() || funcName[0] == '_' ||
        funcName.find("std") != std::string::npos)
      return false;

    // Remove all functions that we have previously encountered.
    if (funcNames.find(funcName) != funcNames.end())
      return false;

    funcNames.insert(make_pair(funcName, true));

    // Demangle function name and print it.
    debug << "\n\n---------New Function---------"
          << "\n";
    debug << funcName << "\n";
    debug << "--------------------------"
          << "\n\n";

    // Iterate through basic blocks of the function.
    for (auto b : topoSortBBs(F)) {


      set<Value*> entry;
      set<Value*> exit;
      EntrySet[b] = entry;
      ExitSet[b] = exit;

      for (auto Pred = pred_begin(b), End = pred_end(b); Pred != End; ++Pred) {
        BasicBlock *predecessor = *Pred;
        EntrySet[b].insert(ExitSet[predecessor].begin(), ExitSet[predecessor].end());
      }



      // Iterate over all the instructions within a basic block.
      for (BasicBlock::const_iterator It = b->begin(); It != b->end(); ++It) {

        Instruction *ins = const_cast<llvm::Instruction *>(&*It); 
        checkUseBeforeDef(ins, b);
      }


      ExitSet[b] = EntrySet[b];

    }   

    // Export data from Set to Vector
    vector<int> temp;
    for (auto line : BuggyLines) {
      temp.push_back(line);
    }

    // Sort vector
    temp.erase(unique(temp.begin(), temp.end()), temp.end());
    std::sort(temp.begin(), temp.end());

    // Print the source code line number(s).
    for (auto line : temp) {
      output << funcName << " : " << line << "\n";
    }

// Print debug string if __DEBUG__ is enabled.
#ifdef __DEBUG__
    errs() << debug.str();
#endif
    debug.flush();

    // Print output
    errs() << output.str();
    output.flush();

    cleanGlobalVariables();
    return false;
  }
};
} // namespace

char Assignment1::ID = 0;
static RegisterPass<Assignment1> X("undeclvar",
                                   "Pass to find undeclared variables");
