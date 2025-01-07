#include "semantic.hpp"
#ifndef SEMANTIC_CPP
#define SEMANTIC_CPP

//need to add scopes + semantic analysis
    void SemanticVisitor::visit(ast::Num &node){
        node.type = ast::BuiltInType::INT;
    }

    void SemanticVisitor::visit(ast::NumB &node){
        node.type = ast::BuiltInType::BYTE;
    }

    void SemanticVisitor::visit(ast::String &node){
        node.type = ast::BuiltInType::STRING;
    }

    void SemanticVisitor::visit(ast::Bool &node){
        node.type = ast::BuiltInType::BOOL;
    }
    void SemanticVisitor::visit(ast::ID &node){} //just the name of the variable

    void SemanticVisitor::SemanticVisitor::visit(ast::BinOp &node){
        node.left->accept(*this);
        node.right->accept(*this);
        if(node.left->type == ast::BuiltInType::INT && node.right->type == ast::BuiltInType::BYTE){ 
            node.type = ast::BuiltInType::INT;
        }
        else if(node.left->type == ast::BuiltInType::BYTE && node.right->type == ast::BuiltInType::INT){ 
            node.type = ast::BuiltInType::INT;
        }
        else if(node.left->type == ast::BuiltInType::INT && node.right->type == ast::BuiltInType::INT){ 
            node.type = ast::BuiltInType::INT;
        }
        else if(node.left->type == ast::BuiltInType::BYTE && node.right->type == ast::BuiltInType::BYTE){ 
            node.type = ast::BuiltInType::BYTE;
        }
        else{
             //throw std::runtime_error("Type mismatch"); //ERROR
        }
    }

    void SemanticVisitor::visit(ast::RelOp &node){
        node.left->accept(*this);
        node.right->accept(*this);
        node.type = ast::BuiltInType::BOOL; //the result of the relational operation is always a boolean
        if((node.left->type != ast::BuiltInType::INT && node.left->type != ast::BuiltInType::BYTE) && (node.right->type != ast::BuiltInType::INT && node.right->type != ast::BuiltInType::BYTE)){
            throw std::runtime_error("Type mismatch");
        }
    }

    void SemanticVisitor::visit(ast::Not &node){
        node.exp->accept(*this);
        if(node.exp->type != ast::BuiltInType::BOOL){ //do we need to check for casting?
            throw std::runtime_error("Type mismatch");
        }
    }

    void SemanticVisitor::visit(ast::And &node){
        node.left->accept(*this);
        node.right->accept(*this);
        if(node.left->type != ast::BuiltInType::BOOL || node.right->type != ast::BuiltInType::BOOL){ //do we need to check for casting?
            throw std::runtime_error("Type mismatch");
        }
    }

    void SemanticVisitor::visit(ast::Or &node){
        node.left->accept(*this);
        node.right->accept(*this);
        if(node.left->type != ast::BuiltInType::BOOL || node.right->type != ast::BuiltInType::BOOL){ //do we need to check for casting?
            throw std::runtime_error("Type mismatch");
        }
    }

    void SemanticVisitor::visit(ast::Type &node){};    


    void SemanticVisitor::visit(ast::Cast &node){
        node.exp->accept(*this);
        //only int to byte or vice versa is allowed
        if(node.target_type->type == ast::BuiltInType::INT &&  node.exp->type == ast::BuiltInType::BYTE){ //casting from byte to int
            node.exp->type = ast::BuiltInType::INT;
        }
        if(node.target_type->type == ast::BuiltInType::BYTE &&  node.exp->type == ast::BuiltInType::INT){ //casting from int to byte
            node.exp->type = ast::BuiltInType::BYTE;
        }
        if(node.target_type->type != node.exp->type){ //problem with casting
            throw std::runtime_error("Type mismatch");
        }
    }

    void SemanticVisitor::visit(ast::ExpList &node){
        for (const auto &exp : node.exps) {
            exp->accept(*this);
        }
    }

    void SemanticVisitor::visit(ast::Call &node){
        node.args->accept(*this);
        //check in the table if exist function with the same name and the same arguments
        node.type = findFunction->type; //the return type of the function

    }

    void SemanticVisitor::visit(ast::Statements &node){
        for (const auto &statement : node.statements) {
            statement->accept(*this);
        }
    }

    void SemanticVisitor::visit(ast::Break &node){
        //check if we are in a while loop

    }

    void SemanticVisitor::visit(ast::Continue &node){
        //check if we are in a while loop
    }

    void SemanticVisitor::visit(ast::Return &node){
        if (node.exp) {
            node.exp->accept(*this);
        }
    }

    void SemanticVisitor::visit(ast::If &node){
        node.condition->accept(*this);
        node.then->accept(*this);
        if (node.otherwise) {
            node.otherwise->accept(*this);
        }
    }

    void SemanticVisitor::visit(ast::While &node){
        node.condition->accept(*this);
        node.body->accept(*this);
    }

    void SemanticVisitor::visit(ast::VarDecl &node){
        node.id->accept(*this);
        node.type->accept(*this);
        if (node.init_exp) {
            node.init_exp->accept(*this);
            
            if(node.init_exp->type != node.type->type && !(node.init_exp->type == ast::BuiltInType::BYTE && node.type == ast::BuiltInType::INT)){ //auto casting from byte to int
                //throw std::runtime_error("Type mismatch"); //should print right error
            }
        }
    }

    void SemanticVisitor::visit(ast::Assign &node){
        node.id->accept(*this);
        node.exp->accept(*this);
    }

    void SemanticVisitor::visit(ast::Formal &node){
        node.id->accept(*this);
        node.type->accept(*this);
    }

    void SemanticVisitor::visit(ast::Formals &node) {
        for (const auto &formal : node.formals) {
            formal->accept(*this);
        }
    }

    void SemanticVisitor::visit(ast::FuncDecl &node){
        printer.beginScope();
        table.beginScope();
        //node.id->accept(*this); //need?
        //node.return_type->accept(*this); //need?
        node.formals->accept(*this); //need!
        node.body->accept(*this); //need!
        table.endScope();
        printer.endScope();
    }

    void SemanticVisitor::visit(ast::Funcs &node) {
        //declare all functions in the table
        for (auto func : node.funcs) {
            table.declareFunction(*func);
            std::vector<ast::BuiltInType> paramTypes;
            for (auto formal : func->formals->formals) {
                paramTypes.push_back(formal->type->type);
            }
            printer.emitFunc(func->id->value, func->return_type->type, paramTypes);
        }
        
        for (auto func : node.funcs) {
            func->accept(*this);
        }
    }

#endif //SEMANTIC_CPP
