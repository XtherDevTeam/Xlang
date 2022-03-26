//
// Created by Jerry Chou on 2022/3/25.
//

#include "ExtendPrimaryNodeGenerator.hpp"

ExtendPrimaryNodeGenerator::ExtendPrimaryNodeGenerator(Lexer &L) : BaseGenerator(L) {

}

AST ExtendPrimaryNodeGenerator::Parse() {
    AST Result{};
    Result = PrimaryNodeGenerator(L).Parse();
    if (Result.Type != AST::TreeType::NotMatch) {
        return Result;
    }
    Rollback();
}
