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

    /// all stores/loads SVFGNodes
    SVFGNodeSet ldStNodeSet;

    SVFGNodeSet markedNodeSet;
    CallSiteSet deallocCallsite;

    void collectLoadStoreSVFGNodes();

    // collect deallocs
    void collectSinks();

    PTACallGraph *getCallgraph();

    inline bool isSinkLikeFun(const SVFFunction *fun) 
    {
        return SaberCheckerAPI::getCheckerAPI()->isMemDealloc(fun);
    }

    bool isMHPPair(const StmtSVFGNode *n1, const StmtVFGNode *n2, PointerAnalysis *pta);

    void writeLocInfo(raw_ostream &outs, const llvm::DebugLoc& loc);
  public:
    MHPAnalysis(SVFG *g, MHP *m);
    void getMHPInstructions(PointerAnalysis *pta);
    void dump(llvm::StringRef filename);
};

} // namespace SVF

#endif
