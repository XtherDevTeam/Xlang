//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_ARGUMENTSNODEGENERATOR_HPP
#define XLANG2_ARGUMENTSNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

#include "ExpressionNodeGenerator.hpp"

class ArgumentsNodeGenerator : BaseGenerator {
public:

    explicit ArgumentsNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_ARGUMENTSNODEGENERATOR_HPP
