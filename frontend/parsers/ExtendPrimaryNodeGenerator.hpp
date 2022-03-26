//
// Created by Jerry Chou on 2022/3/25.
//

#ifndef XLANG2_EXTENDPRIMARYNODEGENERATOR_HPP
#define XLANG2_EXTENDPRIMARYNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "PrimaryNodeGenerator.hpp"

class ExtendPrimaryNodeGenerator : BaseGenerator {
public:
    explicit ExtendPrimaryNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_EXTENDPRIMARYNODEGENERATOR_HPP
