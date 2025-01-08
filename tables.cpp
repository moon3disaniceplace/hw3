#include <unordered_map>
#include <stack>
#include <string>
#include "tables.hpp"
//writing symbol table + stack of scopes
//the identifior is only by name and not including the type

//remember to first emit print and printi functions
Symbol::Symbol(ast::BuiltInType type, int offset) : type(type), offset(offset) {}
//create constructor for function
Function::Function(ast::BuiltInType return_type, std::vector<ast::BuiltInType> parameters) : ret_type(return_type), parameters(parameters) {}
SymbolTable::SymbolTable() : currentOffset(0) { 
    scopes.push_back({});
    offsets.push(0); 
    //add print and printi functions
    std::vector<ast::BuiltInType> parameters1;
    parameters1.push_back(ast::BuiltInType::STRING);
    functions["print"] = std::make_shared<Function>(ast::BuiltInType::VOID, parameters1);
    std::vector<ast::BuiltInType> parameters2;
    parameters2.push_back(ast::BuiltInType::INT);
    functions["printi"] = std::make_shared<Function>(ast::BuiltInType::VOID, parameters2);
}

void SymbolTable::beginScope() {
    scopes.push_back({});
    offsets.push(currentOffset);
}

void SymbolTable::endScope() {
    scopes.pop_back();
    offsets.pop();
    currentOffset = offsets.top();
}

bool SymbolTable::declareVariable(const std::string &name, const ast::BuiltInType &type, int line) {
    if(isDefined(name,line)){
        output::errorDef(line, name);
    } //return false; // Redefinition
    auto &currentScope = *scopes.rbegin();
    currentScope[name] = std::make_shared<Symbol>(type, currentOffset); //Assuming all variables have size 1 (change later when converting to asmmbly)
    offsets.top() = ++currentOffset;
    return true;
}

ast::BuiltInType SymbolTable::findVar(const std::string &name) {
    //start by searching the current scope and then go down the stack
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) return it->find(name)->second->type;
    }
    return ast::BuiltInType::NO_TYPE;
    //error

}


bool SymbolTable::isDefined(const std::string &name, int line) {
    //start by check its not a function
    if (functions.find(name) != functions.end()) output::errorDefAsFunc(line, name);
    //start by searching the current scope and then go down the stack
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) return 1;
    }
    return 0;
    //error

}

void SymbolTable::declareFunction(const ast::FuncDecl &node){ //function can only be declared in the global scope
    if (functions.find(node.id->value) != functions.end()){
        output::errorDef(node.line, node.id->value);
    } //return error; // Redefinition
    std::vector<ast::BuiltInType> parameters;
    for (auto formal : node.formals->formals){
        parameters.push_back(formal->type->type);
    }
    functions[node.id->value] = std::make_shared<Function>(node.return_type->type, parameters);
}

//function can be declared in the global scope only
std::shared_ptr<Function> SymbolTable::findFunction(const ast::Call &node){
    //check if the function isnt defined as variable
    if (functions.find(node.func_id->value) == functions.end()){
        if (isDefined(node.func_id->value, node.line)) {
            output::errorDefAsVar(node.line, node.func_id->value);
        }
        output::errorUndefFunc(node.line, node.func_id->value);
    } 
    auto function = functions[node.func_id->value];

    std::vector<std::string> req_types;
    for (auto parameter : function->parameters)
    {
        req_types.push_back(output::toString(parameter));
    }
    if(function->parameters.size() != node.args->exps.size()){
        output::errorPrototypeMismatch(node.line, node.func_id->value,req_types);
    } //return false; //number of arguments mismatch
    for (int i = 0; i < function->parameters.size(); i++)
    {
        if(function->parameters[i] == node.args->exps[i]->type){
            continue;
        }
        else if(function->parameters[i] == ast::BuiltInType::BYTE && node.args->exps[i]->type == ast::BuiltInType::INT){
            node.args->exps[i]->type = ast::BuiltInType::BYTE; //auto casting from int to byte
        }
        output::errorPrototypeMismatch(node.line, node.func_id->value,req_types);
    }
    return function;
}


void SymbolTable::declareVariableOfFunction(const std::string &name, const ast::BuiltInType &type, int offset_function,int line){
    if(isDefined(name,line)){
        output::errorDef(line, name);
    } // Redefinition
    auto &currentScope = *scopes.rbegin();
    //emit
    currentScope[name] = std::make_shared<Symbol>(type, currentOffset-offset_function); //Assuming all variables have size 1 (change later when converting to asmmbly)
}
