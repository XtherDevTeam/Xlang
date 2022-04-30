//
// Created by Jerry Chou on 2022/4/30.
//

#ifndef XLANG2_IFSTATEMENTNODEGENERATOR_HPP
#define XLANG2_IFSTATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"
#include "CodeBlockNodeGenerator.hpp"

class IfStatementNodeGenerator : BaseGenerator {
public:
    explicit IfStatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_IFSTATEMENTNODEGENERATOR_HPP
