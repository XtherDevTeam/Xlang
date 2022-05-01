//
// Created by Jerry Chou on 2022/5/1.
//

#ifndef XLANG2_WHILESTATEMENTNODEGENERATOR_HPP
#define XLANG2_WHILESTATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"
#include "CodeBlockNodeGenerator.hpp"

class WhileStatementNodeGenerator : BaseGenerator {
public:
    explicit WhileStatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_WHILESTATEMENTNODEGENERATOR_HPP
