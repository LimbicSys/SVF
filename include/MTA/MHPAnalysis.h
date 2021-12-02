#ifndef MHPANALYSIS_H_
#define MHPANALYSIS_H_

#include "MTA/FSMPTA.h"

namespace SVF
{
// TODO: collect free, refer to `LeakChecker::initSnks()`
// check the arg of free is in the MHP pairs

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

    void collectLoadStoreSVFGNodes();
    bool isMHPPair(const StmtSVFGNode *n1, const StmtVFGNode *n2, PointerAnalysis *pta);

  public:
    MHPAnalysis(SVFG *g, MHP *m) : svfg(g), mhp(m)
    {
        collectLoadStoreSVFGNodes();
    }

    void getMHPInstructions(PointerAnalysis *pta);
    void dump(llvm::StringRef filename);
};

} // namespace SVF

#endif
