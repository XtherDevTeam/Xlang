//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_GLOBALENVIRONMENT_HPP
#define XLANG2_GLOBALENVIRONMENT_HPP

#include "LocalEnvironment.hpp"
#include "XlangFunction.hpp"
#include "XlangClass.hpp"
#include "EmulateStack.hpp"
#include "ConstantPoolItem.hpp"

/**
 * A Xlang2 package for 1 environment
 */
class GlobalEnvironment {
public:
    XArray<LocalEnvironment> Environments;
    XArray<XIndexType> RemovedEnvironmentsList;
    EmulateStack EmuStack;
    XArray<XlangFunction> FunctionPool;
    XArray<XlangClass> ClassPool;
    XArray<ConstantPoolItem> ConstantPool;
    

    void LeaveEnvironment(XIndexType LocalIndex);

    XIndexType CreateEnvironment(XString CreateBy, XIndexType Parent);

    bool PushSymbolItem(XIndexType Environment, XString Key, const TypenameDerive &Value);

    /**
     * @biref An O(n) searching function to search a function in function pool.
     * @param FunctionName Name to search
     */
    XArray<XlangFunction>::iterator SearchFunction(const XString &FunctionName);

    /**
     * @biref Push a function to pool.
     * @param Function The function to push.
     */
    XIndexType PushFunction(const XlangFunction &Function);

    /**
     * @biref Search a class.
     * @param ClassName The class name.
     * @return Return a iterator from ClassPool.
     */
    XArray<XlangClass>::iterator SearchClass(const XString &ClassName);

    /**
     * @biref Push a class to pool.
     * @param Class The class to push.
     */
    void PushClass(const XlangClass &Class);

    /**
     * @biref Search a symbol from Environment.
     * @param Environment To searching from.
     * @param SymbolName Symbol name to search.
     * @return The index of this symbol, if it is -1, then it cannot find a symbol with this name.
     */
    std::pair<XIndexType, XIndexType> SearchSymbol(XIndexType Environment, const XString &SymbolName);

    /**
     * @biref Push a constant value to constant pool.
     * @param Item Item to push.
     * @return The index of this constant.
     */
    XIndexType PushConstantItem(const ConstantPoolItem &Item);

    /**
     * @biref Find a constant that has a same value with param.
     * @param Item Item to find.
     * @return The iterator of this constant if it's end of vector, then it means it isn't exist.
     */
    XArray<ConstantPoolItem>::iterator SearchConstantPoolItem(const ConstantPoolItem &Item);


    void LeaveInnerBlockFrame(XIndexType EnvIndex);

    void CreateInnerBlockFrame(XIndexType EnvIndex);
};


#endif //XLANG2_GLOBALENVIRONMENT_HPP
