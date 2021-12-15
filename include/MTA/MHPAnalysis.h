#ifndef MHPANALYSIS_H_
#define MHPANALYSIS_H_

#include "MTA/FSMPTA.h"
#include "SABER/SaberCheckerAPI.h"

namespace SVF
{

class MHPAnalysis
{
    typedef PointerAnalysis::CallSiteSet CallSiteSet;
    typedef PointerAnalysis::CallEdgeMap CallEdgeMap;
    typedef PointerAnalysis::FunctionSet FunctionSet;
    typedef Set<const SVFGNode *> SVFGNodeSet;
    typedef std::vector<const SVFGNode *> SVFGNodeVec;
    typedef NodeBS SVFGNodeIDSet;
    typedef Set<const Instruction *> InstSet;
    typedef std::pair<NodeID, NodeID> NodeIDPair;
    typedef Map<SVFGNodeLockSpan, bool> PairToBoolMap;

  private:
    SVFG *svfg;
    MHP *mhp;
    PointerAnalysis *pta;

    /// all stores/loads SVFGNodes
    SVFGNodeSet ldStNodeSet;

    SVFGNodeSet markedNodeSet;
    CallSiteSet deallocCallsite;
    SVFGNodeSet nullPointerSet;

    std::vector<NodeID> mhpPointers;
    
    void collectLoadStoreSVFGNodes();

    PTACallGraph *getCallgraph();

    inline bool isSinkLikeFun(const SVFFunction *fun) 
    {
        return SaberCheckerAPI::getCheckerAPI()->isMemDealloc(fun);
    }

    bool isMHPPair(const StmtSVFGNode *n1, const StmtVFGNode *n2, PointerAnalysis *pta);

    void writeLocInfo(raw_ostream &outs, const llvm::DebugLoc& loc);

    bool isPointToMHP(NodeID id);

    void addToPointerSet(const StmtSVFGNode *node);

    void dumpInst(llvm::raw_ostream &outs, const Instruction *inst);
  public:
    MHPAnalysis(SVFG *g, MHP *m, PointerAnalysis *p);

    void getMHPInstructions();

    void dump(llvm::StringRef filename);

    // collect deallocs, should call after *getMHPInstructions*
    void collectSinks();

    // collect null pointers, should call after *getMHPInstructions*
    void collectNullPointers();
};

} // namespace SVF

#endif
