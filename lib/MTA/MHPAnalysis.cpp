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
                // ldnodeSet.insert(node);
                nodeSet.insert(node);
            }
        }
        if (SVFUtil::isa<StoreSVFGNode>(snode))
        {
            const StmtSVFGNode *node = SVFUtil::cast<StmtSVFGNode>(snode);
            if (node->getInst())
            {
                // stnodeSet.insert(node);
                nodeSet.insert(node);
            }
        }
    }
}

void MHPAnalysis::getMHPInstructions(PointerAnalysis *pta)
{

    // SVFUtil::outs() << "store: \n";
    // for (SVFGNodeSet::const_iterator it1 = stnodeSet.begin(), eit1 = stnodeSet.end(); it1 != eit1; ++it1)
    // {
    //     const StmtSVFGNode *n1 = SVFUtil::cast<StmtSVFGNode>(*it1);
    //     const Instruction *i1 = n1->getInst();
    //     auto &loc = i1->getDebugLoc();
    //     if (loc) {
    //         loc->print(SVFUtil::outs());
    //         SVFUtil::outs() << "\n";
    //     }
    // }

    // SVFUtil::outs() << "load: \n";
    // for (SVFGNodeSet::const_iterator it1 = ldnodeSet.begin(), eit1 = ldnodeSet.end(); it1 != eit1; ++it1)
    // {
    //     const StmtSVFGNode *n1 = SVFUtil::cast<StmtSVFGNode>(*it1);
    //     const Instruction *i1 = n1->getInst();
    //     auto &loc = i1->getDebugLoc();
    //     if (loc) {
    //         loc->print(SVFUtil::outs());
    //         SVFUtil::outs() << "\n";
    //     }
    // }

    for (auto it1 = nodeSet.begin(); it1 != nodeSet.end(); it1++) {
        const StmtSVFGNode *node1 = SVFUtil::cast<StmtVFGNode>(*it1);
        const Instruction *inst1 = node1->getInst();
        auto &loc1 = inst1->getDebugLoc();
        if (!loc1) {
            continue;
        }
        for (auto it2 = std::next(it1); it2 != nodeSet.end(); it2++) {
            const StmtSVFGNode *node2 = SVFUtil::cast<StmtVFGNode>(*it2);
            const Instruction *inst2 = node2->getInst();
            auto &loc2 = inst2->getDebugLoc();
            if (!loc2) {
                continue;
            }
            handleInstPair(node1, node2, pta);
        }
    }
}

void MHPAnalysis::handleInstPair(const StmtSVFGNode *n1, const StmtVFGNode *n2, PointerAnalysis *pta) {
    const Instruction *i1 = n1->getInst();
    const Instruction *i2 = n2->getInst();

    /// Alias check
    NodeID id1(0), id2(0);
    bool isLoad1 = SVFUtil::isa<LoadSVFGNode>(n1);
    bool isLoad2 = SVFUtil::isa<LoadSVFGNode>(n2);

    // for global pointer
    if (isLoad1) {
        id1 = n1->getPAGSrcNodeID();
    } else {
        id1 = n1->getPAGDstNodeID();
    }
    if (isLoad2) {
        id2 = n2->getPAGSrcNodeID();
    } else {
        id2 = n2->getPAGDstNodeID();
    }

    NodeID svfgNodeId1 = n1->getId();
    NodeID svfgNodeId2 = n2->getId();

    if (!pta->alias(id1, id2)) {
        // for pointers passed via arg
        if (isLoad1 && isLoad2) {
            id1 = n1->getPAGDstNodeID();
            id2 = n2->getPAGDstNodeID();
            if (!pta->alias(id1, id2))
            {
                return;
            }
        }
        else {
            return;
        }
    }

    if (!mhp->mayHappenInParallel(i1, i2))
        return;

    auto &loc1 = i1->getDebugLoc();
    auto &loc2 = i2->getDebugLoc();
    if (loc1 && loc2)
    {
        loc1.print(SVFUtil::outs());
        SVFUtil::outs() << " " << "svfg id: " << n1->getId() << "\n";
        loc2.print(SVFUtil::outs());
        SVFUtil::outs() << " " << "svfg id: " << n2->getId() << "\n";
        SVFUtil::outs() << "\n";
    }
}
