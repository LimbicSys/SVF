#ifndef CXT_THREAD_STMT_MAP_H_
#define CXT_THREAD_STMT_MAP_H_

#include "MTA/TCT.h"
#include "Util/SVFUtil.h"

#include "llvm/ADT/IndexedMap.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/ValueMap.h"

using std::vector;
using std::string;


namespace SVF
{

template <typename V>
class CxtThreadStmtMap
{
private:
    typedef llvm::StringMap<V> CxtMap;
    typedef llvm::StringMapIterator<V> iterator;
    typedef llvm::StringMapConstIterator<V> const_iterator;

    CxtMap internalMap;

    std::string convert_u32_arr_to_u8(string &arr, const CallStrCxt &callCxt) const
    {
        for (const u32_t val : callCxt)
        {
            arr.push_back((val >> 24) & 0xff);
            arr.push_back((val >> 16) & 0xff);
            arr.push_back((val >> 8) & 0xff);
            arr.push_back(val & 0xff);
        }
        return arr;
    }

    void push_u32(std::string& arr, const u32_t val) const
    {
        arr.push_back((val >> 24) & 0xff);
        arr.push_back((val >> 16) & 0xff);
        arr.push_back((val >> 8) & 0xff);
        arr.push_back(val & 0xff);
    }

    void push_u64(std::string& arr, const u64_t val) const
    {
        arr.push_back((val >> 56) & 0xff);
        arr.push_back((val >> 48) & 0xff);
        arr.push_back((val >> 40) & 0xff);
        arr.push_back((val >> 32) & 0xff);
        arr.push_back((val >> 24) & 0xff);
        arr.push_back((val >> 16) & 0xff);
        arr.push_back((val >> 8) & 0xff);
        arr.push_back(val & 0xff);
    }

    string convert_cxt_to_str(const CxtThreadStmt &cxt) const
    {
        NodeID tid = cxt.getTid();
        const Instruction *inst = cxt.getStmt();
        CallStrCxt callStrCxt = cxt.getContext();

        string cxtStr;
        push_u32(cxtStr, tid);
        push_u64(cxtStr, (u64_t)inst);
        convert_u32_arr_to_u8(cxtStr, callStrCxt);
        return cxtStr;
    }

    void insert_or_assign(const CxtThreadStmt &cxt, const V &v)
    {
        string byteArr = convert_cxt_to_str(cxt);
        internalMap[byteArr] = v;
    }

public:

    const_iterator begin() const { return internalMap.begin(); }
    iterator begin() { return internalMap.begin(); }
    const_iterator end() const { return internalMap.end(); }
    iterator end() { return internalMap.end(); }

    const_iterator find(const CxtThreadStmt &cxt) const
    {
        string byteArr = convert_cxt_to_str(cxt);
        return internalMap.find(byteArr);
    }

    iterator find(const CxtThreadStmt &cxt)
    {
        string byteArr = convert_cxt_to_str(cxt);
        return internalMap.find(byteArr);
    }

    V &operator[](const CxtThreadStmt &cxt)
    {
        iterator it = find(cxt);
        if (it != end())
        {
            return it->second;
        }

        insert_or_assign(cxt, V());
        return find(cxt)->second;
    }

};

}
#endif // !CXT_THREAD_STMT_MAP_H_
