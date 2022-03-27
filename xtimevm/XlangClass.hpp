//
// Created by Jerry Chou on 2022/3/27.
//

#ifndef XLANG2_XLANGCLASS_HPP
#define XLANG2_XLANGCLASS_HPP

#include "../share/config.hpp"

class XlangClass {
public:
    XIndexType HashName;
    /**
     * VTable of parents and this class
     * [ ClassHashName -> [VariableNameHash -> HeapElementIndex] ]
     */
    XMap<XIndexType, XMap<XIndexType, XIndexType> > VTables;

    /**
     *  SubclassesFilepath : Filepath to ClassHashName
     *  For loading subclasses
     */
    XMap<XString, XIndexType> SubclassesFilepath;
    /**
     * Subclasses in this class
     */
    XMap<XIndexType, XlangClass> Subclasses;

    XBoolean instanceOf(XIndexType Parent) const;
};


#endif //XLANG2_XLANGCLASS_HPP
