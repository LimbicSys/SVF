#include "MTA/MHPAnalysis.h"

using namespace SVF;

void MHPAnalysis::collectLoadStoreSVFGNodes()
{
    for (SVFG::const_iterator it = svfg->begin(), eit = svfg->end(); it != eit; ++it)
    {
        const SVFGNode *snode = it->second;
        if (SVFUtil::isa<LoadSVFGNode>(snode))
        {
            const StmtSVFGNode *node = SVFUtil::cast<StmtSVFGNode>(snode);
            if (node->getInst())
            {
                ldnodeSet.insert(node);
            }
        }
        if (SVFUtil::isa<StoreSVFGNode>(snode))
        {
            const StmtSVFGNode *node = SVFUtil::cast<StmtSVFGNode>(snode);
            if (node->getInst())
            {
                stnodeSet.insert(node);
            }
        }
    }
}

void MHPAnalysis::getMHPInstructions(PointerAnalysis *pta)
{
    for (SVFGNodeSet::const_iterator it1 = stnodeSet.begin(), eit1 = stnodeSet.end(); it1 != eit1; ++it1)
    {
        const StmtSVFGNode *n1 = SVFUtil::cast<StmtSVFGNode>(*it1);
        const Instruction *i1 = n1->getInst();

        for (SVFGNodeSet::const_iterator it2 = ldnodeSet.begin(), eit2 = ldnodeSet.end(); it2 != eit2; ++it2)
        {
            const StmtSVFGNode *n2 = SVFUtil::cast<StmtSVFGNode>(*it2);
            const Instruction *i2 = n2->getInst();
            handleStoreLoad(n1, n2, pta);
        }

        for (SVFGNodeSet::const_iterator it2 = std::next(it1), eit2 = stnodeSet.end(); it2 != eit2; ++it2)
        {
            const StmtSVFGNode *n2 = SVFUtil::cast<StmtSVFGNode>(*it2);
            const Instruction *i2 = n2->getInst();
            handleStoreStore(n1, n2, pta);
        }
    }
}

void MHPAnalysis::handleStoreLoad(const StmtSVFGNode *n1, const StmtSVFGNode *n2, PointerAnalysis *pta)
{
    const Instruction *i1 = n1->getInst();
    const Instruction *i2 = n2->getInst();

    if (!mhp->mayHappenInParallel(i1, i2))
        return;
    /// Alias
    if (!pta->alias(n1->getPAGDstNodeID(), n2->getPAGSrcNodeID()))
        return;
    auto &loc1 = i1->getDebugLoc();
    auto &loc2 = i2->getDebugLoc();
    if (loc1 && loc2)
    {
        loc1.print(SVFUtil::outs());
        SVFUtil::outs() << "\n";
        loc2.print(SVFUtil::outs());
        SVFUtil::outs() << "\n";
        SVFUtil::outs() << "\n";
    }
}

void MHPAnalysis::handleStoreStore(const StmtSVFGNode *n1, const StmtSVFGNode *n2, PointerAnalysis *pta)
{
    const Instruction *i1 = n1->getInst();
    const Instruction *i2 = n2->getInst();

    if (!mhp->mayHappenInParallel(i1, i2))
        return;
    /// Alias
    if (!pta->alias(n1->getPAGDstNodeID(), n2->getPAGSrcNodeID()))
        return;
    auto &loc1 = i1->getDebugLoc();
    auto &loc2 = i2->getDebugLoc();
    if (loc1 && loc2)
    {
        loc1.print(SVFUtil::outs());
        SVFUtil::outs() << "\n";
        loc2.print(SVFUtil::outs());
        SVFUtil::outs() << "\n";
        SVFUtil::outs() << "\n";
    }
}
