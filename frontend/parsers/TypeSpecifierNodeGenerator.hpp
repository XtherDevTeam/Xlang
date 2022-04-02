//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_TYPESPECIFIERNODEGENERATOR_HPP
#define XLANG2_TYPESPECIFIERNODEGENERATOR_HPP

#include "../../share/config.hpp"
#include "BaseGenerator.hpp"

class TypeSpecifierNodeGenerator : BaseGenerator {
public:
    explicit TypeSpecifierNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_TYPESPECIFIERNODEGENERATOR_HPP
