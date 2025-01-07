#include "semantic.hpp"
#ifndef SEMANTIC_CPP
#define SEMANTIC_CPP

//need to add scopes + semantic analysis
    void SemanticVisitor::visit(ast::Num &node){
        node.type = ast::BuiltInType::INT;
    } //good

    void SemanticVisitor::visit(ast::NumB &node){
        node.type = ast::BuiltInType::BYTE;
        if(node.value > 255){
            output::errorByteTooLarge(node.line, node.value);
        }
    } //good

    void SemanticVisitor::visit(ast::String &node){
        node.type = ast::BuiltInType::STRING;
    } //good

    void SemanticVisitor::visit(ast::Bool &node){
        node.type = ast::BuiltInType::BOOL;
    } //good

    void SemanticVisitor::visit(ast::ID &node){
        node.type = table.isDefined(node.value);
        if(node.type == ast::BuiltInType::NO_TYPE){
            output::errorUndef(node.line, node.value);
        }
    } 
    //we need to use this function to check if the variable is defined or not

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

    void SemanticVisitor::visit(ast::Type &node){};   //dont need to do anything

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
        auto findFunction = table.findFunction(node);
        if(findFunction == nullptr){
            output::errorUndefFunc(node.line, node.func_id->value);
        }
        if(findFunction->parameters.size() != node.args->exps.size()){
            output::errorPrototypeMismatch(node.line, node.func_id->value, findFunction->parameters);
        }
        for (int i = 0; i < table.; i++)
        {
            /* code */
        }
        
        node.type = findFunction->type; //the return type of the function

    }

    void SemanticVisitor::visit(ast::Statements &node){
        for (const auto &statement : node.statements) {
            statement->accept(*this);
        }
    }

    void SemanticVisitor::visit(ast::Break &node){
        if (insideWhile == 0){
            output::errorUnexpectedBreak(node.line); 
        }
        //check if we are in a while loop
    } //good

    void SemanticVisitor::visit(ast::Continue &node){
        if (insideWhile == 0){
            output::errorUnexpectedContinue(node.line); 
        }
        //check if we are in a while loop
    } //good

    void SemanticVisitor::visit(ast::Return &node){
        if (node.exp) {
            node.exp->accept(*this);
        }
    }

    void SemanticVisitor::visit(ast::If &node){
        table.beginScope();
        node.condition->accept(*this);
        table.beginScope();
        node.then->accept(*this);
        table.endScope();
        table.endScope();
        if (node.otherwise) {
            table.beginScope();
            table.beginScope();
            node.otherwise->accept(*this);
            table.endScope();
            table.endScope();
        }
    }

    void SemanticVisitor::visit(ast::While &node){
        insideWhile++;
        table.beginScope();
        node.condition->accept(*this);
        if(node.condition->type != ast::BuiltInType::BOOL){
            output::errorMismatch(node.line);
        }
        table.beginScope();
        node.body->accept(*this);
        table.endScope();
        table.endScope();
        insideWhile--;
    }

    void SemanticVisitor::visit(ast::VarDecl &node){
        node.id->accept(*this);
        node.type->accept(*this);
        if (node.init_exp) {
            node.init_exp->accept(*this);
            
            if(node.init_exp->type != node.type->type && !(node.init_exp->type == ast::BuiltInType::BYTE && node.type == ast::BuiltInType::INT)){ //auto casting from byte to int
                output::errorMismatch(node.line);
            }
        }
    }

    void SemanticVisitor::visit(ast::Assign &node){
        //node.id->accept(*this);
        node.exp->accept(*this);
    }

    void SemanticVisitor::visit(ast::Formal &node){
        table.declareVariable(node.id->value, node.type->type);
        //node.id->accept(*this);
        //node.type->accept(*this);
    }

    void SemanticVisitor::visit(ast::Formals &node) {
        for (auto i = node.formals.size(); i >= 1; i++)
        {
            table.declareVariableOfFunction(node.formals[i-1]->id->value, node.formals[i-1]->type->type, i);
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
        //check if there is a main function
        if (table.functions.find("main") == table.functions.end()) {
            output::errorMainMissing();
        }
        
        for (auto func : node.funcs) {
            func->accept(*this);
        }
    }

#endif //SEMANTIC_CPP
