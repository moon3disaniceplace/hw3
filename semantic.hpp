#include "nodes.hpp"
#include "tables.hpp"
#include "visitor.hpp"
#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

//need to add scopes + semantic analysis
class SemanticVisitor : public Visitor {
output::ScopePrinter printer;
SymbolTable table;
int insideFunction;
int insideWhile; //remember them
public:
    void visit(ast::Num &node);

    void visit(ast::NumB &node);

    void visit(ast::String &node);

    void visit(ast::Bool &node);

    void visit(ast::ID &node);

    void visit(ast::BinOp &node);

    void visit(ast::RelOp &node);

    void visit(ast::Not &node);

    void visit(ast::And &node);

    void visit(ast::Or &node);
    void visit(ast::Type &node);

    void visit(ast::Cast &node);

    void visit(ast::ExpList &node);

    void visit(ast::Call &node);

    void visit(ast::Statements &node);

    void visit(ast::Break &node);

    void visit(ast::Continue &node);

    void visit(ast::Return &node);

    void visit(ast::If &node);

    void visit(ast::While &node);

    void visit(ast::VarDecl &node);

    void visit(ast::Assign &node);

    void visit(ast::Formal &node);

    void visit(ast::Formals &node);

    void visit(ast::FuncDecl &node);

    void visit(ast::Funcs &node);
};

#endif
