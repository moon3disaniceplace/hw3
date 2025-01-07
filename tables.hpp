#ifndef TABLES_HPP
#define TABLES_HPP
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include "output.hpp"
//writing symbol table + stack of scopes
//the identifior is only by name and not including the type
class Symbol {
    public:
        ast::BuiltInType type; // Type of the symbol / return type of the function
        int offset; // Offset in the stack frame
        Symbol(ast::BuiltInType type, int offset);
};

class Function{
    public:
        ast::BuiltInType ret_type; // return type of the function
        std::vector<ast::BuiltInType> parameters; // List of parameter types
        Function(ast::BuiltInType return_type, std::vector<ast::BuiltInType> parameters);
};

class SymbolTable {
    std::stack<int> offsets; // Stack of offsets (one for each scope)
    std::vector<std::unordered_map<std::string, std::shared_ptr<Symbol>>> scopes; // Stack of scopes (each scope is a map from variable names to symbols)
    std::unordered_map<std::string, std::shared_ptr<Function>> functions; // Map from function names to function symbols
    int currentOffset;

public:
    SymbolTable();
    void beginScope();
    void endScope();
    bool declareParameter(const std::string &name, const ast::BuiltInType &type, int offset); //need?
    bool declareVariable(const std::string &name, const ast::BuiltInType &type);
    ast::BuiltInType isDefined(const std::string &name);
    void declareFunction(const ast::FuncDecl &node);
    std::shared_ptr<Function> findFunction(const ast::Call &node);
};


#endif