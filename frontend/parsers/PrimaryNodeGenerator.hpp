//
// Created by Jerry Chou on 2022/3/25.
//

#ifndef XLANG2_PRIMARYNODEGENERATOR_HPP
#define XLANG2_PRIMARYNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"
#include "TypeSpecifierNodeGenerator.hpp"

class PrimaryNodeGenerator : BaseGenerator {
public:
    explicit PrimaryNodeGenerator(Lexer& L);

    AST Parse() override;
};


#endif //XLANG2_PRIMARYNODEGENERATOR_HPP
