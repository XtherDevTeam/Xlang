//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_IDENTIFIERNODEGENERATOR_HPP
#define XLANG2_IDENTIFIERNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class IdentifierNodeGenerator : BaseGenerator {
public:
    explicit IdentifierNodeGenerator(Lexer &L);

    AST Parse( ) override;
};


#endif //XLANG2_IDENTIFIERNODEGENERATOR_HPP
