//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_GLOBALENVIRONMENT_HPP
#define XLANG2_GLOBALENVIRONMENT_HPP

#include "LocalEnvironment.hpp"
#include "XlangFunction.hpp"
#include "XlangClass.hpp"
#include "EmulateStack.hpp"

class GlobalEnvironment {
public:
    std::vector<LocalEnvironment> Environments;
    std::vector<XIndexType> RemovedEnvironmentsList;
    EmulateStack EmuStack;
    std::vector<XlangFunction> FunctionPool;
    std::vector<XlangClass> ClassPool;

    void LeaveEnvironment(XIndexType LocalIndex);

    XIndexType CreateEnvironment(XString CreateBy, XIndexType Parent);

    bool PushSymbolItem(XIndexType Environment, XString Key, const TypenameDerive &Value);

    /**
     * @biref An O(n) searching function to search a function in function pool.
     * @param FunctionName Name to search
     */
    std::vector<XlangFunction>::iterator SearchFunction(const XString &FunctionName);

    /**
     * @biref Push a function to pool.
     * @param Function The function to push.
     */
    void PushFunction(const XlangFunction &Function);

    /**
     * @biref Search a class.
     * @param ClassName The class name.
     * @return Return a iterator from ClassPool.
     */
    std::vector<XlangClass>::iterator SearchClass(const XString &ClassName);

    /**
     * @biref Push a class to pool.
     * @param Class The class to push.
     */
    void PushClass(const XlangClass &Class);

    /**
     * Search a symbol from Environment.
     * @param Environment To searching from.
     * @param SymbolName Symbol name to search.
     * @return The index of this symbol, if it is -1, then it cannot find a symbol with this name.
     */
    std::pair<XIndexType, XIndexType> SearchSymbol(XIndexType Environment, const XString &SymbolName);
};


#endif //XLANG2_GLOBALENVIRONMENT_HPP
