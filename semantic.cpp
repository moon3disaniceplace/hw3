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
        if(!table.isDefined(node.value,node.line)){
            output::errorUndef(node.line, node.value);
        }
        node.type = table.findVar(node.value);
    } 

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
             output::errorMismatch(node.line);
        }
    }

    void SemanticVisitor::visit(ast::RelOp &node){
        node.left->accept(*this);
        node.right->accept(*this);
        if((node.left->type != ast::BuiltInType::INT && node.left->type != ast::BuiltInType::BYTE) && (node.right->type != ast::BuiltInType::INT && node.right->type != ast::BuiltInType::BYTE)){
            output::errorMismatch(node.line);
        }
        node.type = ast::BuiltInType::BOOL; //the result of the relational operation is always a boolean
    }

    void SemanticVisitor::visit(ast::Not &node){
        node.exp->accept(*this);
        if(node.exp->type != ast::BuiltInType::BOOL){ //do we need to check for casting?
            output::errorMismatch(node.line);
        }
        node.type = ast::BuiltInType::BOOL;
    }

    void SemanticVisitor::visit(ast::And &node){
        node.left->accept(*this);
        node.right->accept(*this);
        if(node.left->type != ast::BuiltInType::BOOL || node.right->type != ast::BuiltInType::BOOL){ //do we need to check for casting?
            output::errorMismatch(node.line);
        }
        node.type = ast::BuiltInType::BOOL;
    }

    void SemanticVisitor::visit(ast::Or &node){
        node.left->accept(*this);
        node.right->accept(*this);
        if(node.left->type != ast::BuiltInType::BOOL || node.right->type != ast::BuiltInType::BOOL){ //do we need to check for casting?
            output::errorMismatch(node.line);
        }
        node.type = ast::BuiltInType::BOOL;
    }

    void SemanticVisitor::visit(ast::Type &node){
        //node.type = node.type; useless
    }

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
            output::errorMismatch(node.line);
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
        node.type = table.findFunction(node)->ret_type;//the return type of the function
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
        if(returnType==ast::BuiltInType::VOID){
            if(node.exp){
                output::errorMismatch(node.line);
            }
            return;
        }
        if (!node.exp) {
            output::errorMismatch(node.line);
        }
        node.exp->accept(*this);
        if(node.exp->type == ast::BuiltInType::BYTE && returnType == ast::BuiltInType::INT){ //auto casting from byte to int
            node.exp->type = ast::BuiltInType::INT;
        }
        else if(node.exp->type != returnType){
            output::errorMismatch(node.line);
        }
    }

    void SemanticVisitor::visit(ast::If &node){
        table.beginScope();
        printer.beginScope();
        node.condition->accept(*this);
        if(node.condition->type != ast::BuiltInType::BOOL){
            output::errorMismatch(node.line);
        }
        table.beginScope();
        printer.beginScope();
        node.then->accept(*this);
        table.endScope();
        printer.endScope();
        table.endScope();
        printer.endScope();
        if (node.otherwise) {
            table.beginScope();
            printer.beginScope();
            table.beginScope();
            printer.beginScope();
            node.otherwise->accept(*this);
            table.endScope();
            printer.endScope();
            table.endScope();
            printer.endScope();
        }
    }

    void SemanticVisitor::visit(ast::While &node){
        insideWhile++;
        table.beginScope();
        printer.beginScope();
        node.condition->accept(*this);
        if(node.condition->type != ast::BuiltInType::BOOL){
            output::errorMismatch(node.line);
        }
        table.beginScope();
        printer.beginScope();
        node.body->accept(*this);
        table.endScope();
        printer.endScope();
        table.endScope();
        printer.endScope();
        insideWhile--;
    }

    void SemanticVisitor::visit(ast::VarDecl &node){
        table.declareVariable(node.id->value, node.type->type, node.line);
        printer.emitVar(node.id->value, node.type->type, table.currentOffset);
        if (node.init_exp) {
            node.init_exp->accept(*this);
            if(node.init_exp->type == ast::BuiltInType::BYTE && node.type->type == ast::BuiltInType::INT){ //auto casting from byte to int
                node.init_exp->type = ast::BuiltInType::INT;
            }
            else if(node.init_exp->type != node.type->type){
                output::errorMismatch(node.line);
            }
        }
    }

    void SemanticVisitor::visit(ast::Assign &node){
        node.id->accept(*this);
        node.exp->accept(*this);
        if(node.id->type == ast::BuiltInType::INT && node.exp->type == ast::BuiltInType::BYTE){ //auto casting from byte to int
            node.exp->type = ast::BuiltInType::INT;
        }
        else if((node.id->type != node.exp->type))
            output::errorMismatch(node.line);
    }

    void SemanticVisitor::visit(ast::Formal &node){
    } //dont sure why to use

    void SemanticVisitor::visit(ast::Formals &node) {
        for (auto i = node.formals.size(); i >= 1; i--)
        {
            table.declareVariableOfFunction(node.formals[i-1]->id->value, node.formals[i-1]->type->type, i, node.line);
        }
        for (int i = 0; i < node.formals.size(); i++)
        {
            printer.emitVar(node.formals[i]->id->value, node.formals[i]->type->type, table.currentOffset-i-1);
        }
        
    }

    void SemanticVisitor::visit(ast::FuncDecl &node){
        table.beginScope();
        printer.beginScope();
        //node.id->accept(*this); surly we dont need to get inside the id
        node.formals->accept(*this); //need!
        returnType = node.return_type->type;
        node.body->accept(*this); //need!
        table.endScope();
        printer.endScope();
    }

    void SemanticVisitor::visit(ast::Funcs &node) {
        //declare all functions in the table
        printer.emitFunc("print", ast::BuiltInType::VOID, {ast::BuiltInType::STRING});
        printer.emitFunc("printi", ast::BuiltInType::VOID, {ast::BuiltInType::INT});
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
