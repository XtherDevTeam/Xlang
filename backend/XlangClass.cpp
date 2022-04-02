//
// Created by Jerry Chou on 2022/4/2.
//

#include "XlangClass.hpp"

#include <utility>

XlangClass::XlangClass() : Extends() {

}

XlangClass::XlangClass(XString ClassName) : ClassName(std::move(ClassName)) {

}
