#include "MTA/MHPAnalysis.h"

using namespace SVF;

MHPAnalysis::MHPAnalysis(SVFG *g, MHP *m, PointerAnalysis *p) : svfg(g), mhp(m), pta(p) {
    collectLoadStoreSVFGNodes();
}

void MHPAnalysis::collectLoadStoreSVFGNodes() {
    for (SVFG::const_iterator it = svfg->begin(), eit = svfg->end(); it != eit; ++it) {
        const SVFGNode *snode = it->second;
        if (SVFUtil::isa<LoadSVFGNode>(snode)) {
            const StmtSVFGNode *node = SVFUtil::cast<StmtSVFGNode>(snode);
            if (node->getInst()) {
                ldStNodeSet.insert(node);
            }
        }
        if (SVFUtil::isa<StoreSVFGNode>(snode)) {
            const StmtSVFGNode *node = SVFUtil::cast<StmtSVFGNode>(snode);
            if (const Instruction* inst = node->getInst()) {
                ldStNodeSet.insert(node);
           }
        }
    }
}

void MHPAnalysis::addToPointerSet(const StmtSVFGNode *node) {
    NodeID id(0);
    bool isLoad = SVFUtil::isa<LoadSVFGNode>(node);

    // for global pointer
    if (isLoad) {
        id = node->getPAGSrcNodeID();
    } else {
        id = node->getPAGDstNodeID();
    }

    mhpPointers.push_back(id);
}

void MHPAnalysis::getMHPInstructions() {
    for (auto it1 = ldStNodeSet.begin(); it1 != ldStNodeSet.end(); it1++) {
        const StmtSVFGNode *node1 = SVFUtil::cast<StmtVFGNode>(*it1);
        const Instruction *inst1 = node1->getInst();
        auto &loc1 = inst1->getDebugLoc();
        if (!loc1) {
            continue;
        }
        for (auto it2 = std::next(it1); it2 != ldStNodeSet.end(); it2++) {
            const StmtSVFGNode *node2 = SVFUtil::cast<StmtVFGNode>(*it2);
            const Instruction *inst2 = node2->getInst();
            auto &loc2 = inst2->getDebugLoc();
            if (!loc2) {
                continue;
            }
            if (isMHPPair(node1, node2, pta)) {
                addToPointerSet(node1);
                addToPointerSet(node2);
                markedNodeSet.insert(node1);
                markedNodeSet.insert(node2);
            }
        }
    }

    std::sort(mhpPointers.begin(), mhpPointers.end());
    mhpPointers.erase(std::unique(mhpPointers.begin(), mhpPointers.end()), mhpPointers.end());
}

bool MHPAnalysis::isMHPPair(const StmtSVFGNode *n1, const StmtVFGNode *n2, PointerAnalysis *pta) {
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
        // for pointers passed via arg and point to the same obj
        if (isLoad1 && isLoad2) {
            id1 = n1->getPAGDstNodeID();
            id2 = n2->getPAGDstNodeID();
            if (!pta->alias(id1, id2))
            {
                return false;
            }
        }
        else {
            return false;
        }
    }

    if (!mhp->mayHappenInParallel(i1, i2))
        return false;

    return true;
}

void MHPAnalysis::dump(llvm::StringRef filename) {
    std::error_code ec;
    llvm::raw_fd_ostream file(filename, ec);
    if (ec) {
        SVFUtil::errs() << ec.message() << "\n";
        return;
    }
    SVFUtil::outs() << "Write MHP pairs to " << filename << "\n";
    for (const SVFGNode *node : markedNodeSet) {
        if (const Instruction *inst =  SVFUtil::cast<StmtVFGNode>(node)->getInst()) {
            dumpInst(file, inst);
        }
    }
    for (const CallBlockNode *node : deallocCallsite) {
        if (const Instruction *cs = node->getCallSite()) {
            dumpInst(file, cs);
        }
    }
    for (const SVFGNode *node : nullPointerSet) {
        if (const Instruction *inst =  SVFUtil::cast<StmtVFGNode>(node)->getInst()) {
            dumpInst(file, inst);
        }
    }
    file.close();
}

PTACallGraph* MHPAnalysis::getCallgraph() {
    return svfg->getPTA()->getPTACallGraph();
}

void MHPAnalysis::collectNullPointers() {
    for (SVFG::const_iterator it = svfg->begin(), eit = svfg->end(); it != eit; ++it) {
        const SVFGNode *snode = it->second;
        if (SVFUtil::isa<StoreSVFGNode>(snode)) {
            const StmtSVFGNode *node = SVFUtil::cast<StmtSVFGNode>(snode);
            if (const Instruction* inst = node->getInst()) {
                if (const StoreInst* st = SVFUtil::dyn_cast<StoreInst>(inst)) {
                    if (SymbolTableInfo::isNullPtrSym(st->getValueOperand())) {
                        NodeID id = node->getPAGDstNodeID();
                        if (isPointToMHP(id)) {
                            nullPointerSet.insert(snode);
                        }
                    }
                }
            }
        }
    }
}

void MHPAnalysis::collectSinks() {
    PAG* pag = svfg->getPAG();
    for(PAG::CSToArgsListMap::iterator it = pag->getCallSiteArgsMap().begin(),
            eit = pag->getCallSiteArgsMap().end(); it != eit; ++it) {

        PTACallGraph::FunctionSet callees;
        getCallgraph()->getCallees(it->first, callees);
        for(PTACallGraph::FunctionSet::const_iterator cit = callees.begin(), ecit = callees.end(); cit!=ecit; cit++) {
            const SVFFunction* fun = *cit;
			if (isSinkLikeFun(fun)) {
				PAG::PAGNodeList &arglist = it->second;
				assert(!arglist.empty()	&& "no actual parameter at deallocation site?");
				/// we only choose pointer parameters among all the actual parameters
				for (PAG::PAGNodeList::const_iterator ait = arglist.begin(),
						aeit = arglist.end(); ait != aeit; ++ait) {
					const PAGNode *pagNode = *ait;
					if (pagNode->isPointer()) {
                        if (isPointToMHP(pagNode->getId())) {
                            deallocCallsite.insert(it->first);
                        }
					}
				}
			}
        }
    }
}

void MHPAnalysis::writeLocInfo(raw_ostream &outs, const llvm::DebugLoc& loc) {
    llvm::StringRef filename = loc->getFilename();
    llvm::SmallVector<char> path(filename.begin(), filename.end());
    llvm::sys::fs::make_absolute(path);
    std::string fullname(path.begin(), path.end());
    outs << fullname << ":" << loc->getLine() << ":" << loc->getColumn() << "\n";
}

bool MHPAnalysis::isPointToMHP(NodeID id) {
    for (NodeID pointerId : mhpPointers) {
        if (pta->alias(id, pointerId)) {
            return true;
        }
    }
    return false;
}

void MHPAnalysis::dumpInst(llvm::raw_ostream &outs, const Instruction *inst) {
    if (auto &loc = inst->getDebugLoc()) {
        writeLocInfo(outs, loc);
    }
}
