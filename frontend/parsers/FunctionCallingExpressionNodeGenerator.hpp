//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_FUNCTIONCALLINGEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_FUNCTIONCALLINGEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "IdentifierNodeGenerator.hpp"
#include "IndexExpressionNodeGenerator.hpp"
#include "ArgumentsNodeGenerator.hpp"

class FunctionCallingExpressionNodeGenerator : BaseGenerator {
public:
    explicit FunctionCallingExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_FUNCTIONCALLINGEXPRESSIONNODEGENERATOR_HPP
