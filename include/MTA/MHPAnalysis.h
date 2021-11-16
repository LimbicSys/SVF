#ifndef MHPANALYSIS_H_
#define MHPANALYSIS_H_

#include "MTA/FSMPTA.h"

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
    SVFGNodeSet stnodeSet;
    SVFGNodeSet ldnodeSet;

    void collectLoadStoreSVFGNodes();
    void handleStoreLoad(const StmtSVFGNode *n1, const StmtSVFGNode *n2, PointerAnalysis *pta);
    void handleStoreStore(const StmtSVFGNode *n1, const StmtSVFGNode *n2, PointerAnalysis *pta);

  public:
    MHPAnalysis(SVFG *g, MHP *m) : svfg(g), mhp(m)
    {
        collectLoadStoreSVFGNodes();
    }

    void getMHPInstructions(PointerAnalysis *pta);
};

} // namespace SVF

#endif
