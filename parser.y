%{

#include "nodes.hpp"
#include "output.hpp"

// bison declarations
extern int yylineno;
extern int yylex();

void yyerror(const char*);

// root of the AST, set by the parser and used by other parts of the compiler
std::shared_ptr<ast::Node> program;

using namespace std;

// TODO: Place any additional declarations here
%}
// TODO: Define tokens here 
%token VOID INT BYTE BOOL AND OR NOT TRUE FALSE RETURN IF ELSE WHILE BREAK CONTINUE SC COMMA LPAREN RPAREN LBRACE RBRACE ASSIGN
%token ID NUM NUM_B STRING PLUS MINUS MULT DIV LESS GREATER LEQ GEQ EQUAL NEQUAL


// TODO: Define precedence and associativity here

%left OR
%left AND
%nonassoc EQUAL NEQUAL //need to be nonassoc?
%nonassoc LESS GREATER LEQ GEQ //need to be nonassoc?
%left PLUS MINUS
%left MULT DIV
%right NOT
%left LPAREN RPAREN
%precedence IFX
%precedence ELSE    
%%

// While reducing the start variable, set the root of the AST
//need to check push_Back and push_front
Program: Funcs {program = $1; }
;
// TODO: Define grammar here
Funcs:
    /* Empty */ { $$ = std::make_shared<ast::Funcs>(); }
    | FuncDecl Funcs { $$ = $2; std::dynamic_pointer_cast<ast::Funcs>($$)->push_front(std::dynamic_pointer_cast<ast::FuncDecl>($1)); }
;

FuncDecl:
    RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE {
        $$ = std::make_shared<ast::FuncDecl>(
            std::dynamic_pointer_cast<ast::ID>($2),
            std::dynamic_pointer_cast<ast::Type>($1),
            std::dynamic_pointer_cast<ast::Formals>($4),
            std::dynamic_pointer_cast<ast::Statements>($7)
        );
    }
;

RetType:
    Type { $$ = $1; }
    | VOID { $$ = std::make_shared<ast::Type>(ast::BuiltInType::VOID); }
;

Formals:
    /* Empty */ { $$ = std::make_shared<ast::Formals>(); }
    | FormalsList { $$ = $1; }
;
//not sure why push_front
FormalsList:
    FormalDecl { $$ = std::make_shared<ast::Formals>(); std::dynamic_pointer_cast<ast::Formals>($$)->push_back(std::dynamic_pointer_cast<ast::Formal>($1)); }
    | FormalDecl COMMA FormalsList { $$ = $3; std::dynamic_pointer_cast<ast::Formals>($$)->push_front(std::dynamic_pointer_cast<ast::Formal>($1)); }
;

FormalDecl:
    Type ID {
        $$ = std::make_shared<ast::Formal>(
            std::dynamic_pointer_cast<ast::ID>($2),
            std::dynamic_pointer_cast<ast::Type>($1)
        );
    }
;

Statements:
    Statement { $$ = std::make_shared<ast::Statements>(std::dynamic_pointer_cast<ast::Statement>($1)); }
    | Statements Statement { $$ = $1; std::dynamic_pointer_cast<ast::Statements>($$)->push_back(std::dynamic_pointer_cast<ast::Statement>($2)); }
;

Statement:
    LBRACE Statements RBRACE { $$ = $2; }
    | Type ID SC {
        $$ = std::make_shared<ast::VarDecl>(
            std::dynamic_pointer_cast<ast::ID>($2),
            std::dynamic_pointer_cast<ast::Type>($1),
            nullptr
        );
    }
    | Type ID ASSIGN Exp SC {
        $$ = std::make_shared<ast::VarDecl>(
            std::dynamic_pointer_cast<ast::ID>($2),
            std::dynamic_pointer_cast<ast::Type>($1),
            std::dynamic_pointer_cast<ast::Exp>($4)
        );
    }
    | ID ASSIGN Exp SC {
        $$ = std::make_shared<ast::Assign>(
            std::dynamic_pointer_cast<ast::ID>($1),
            std::dynamic_pointer_cast<ast::Exp>($3)
        );
    }
    | Call SC { $$ = $1; }
    | RETURN SC { $$ = std::make_shared<ast::Return>(nullptr); }
    | RETURN Exp SC { $$ = std::make_shared<ast::Return>(std::dynamic_pointer_cast<ast::Exp>($2)); }
    | IF LPAREN Exp RPAREN Statement %prec IFX {
        $$ = std::make_shared<ast::If>(
            std::dynamic_pointer_cast<ast::Exp>($3),
            std::dynamic_pointer_cast<ast::Statement>($5),
            nullptr
        );
    }
    | IF LPAREN Exp RPAREN Statement ELSE Statement {
        $$ = std::make_shared<ast::If>(
            std::dynamic_pointer_cast<ast::Exp>($3),
            std::dynamic_pointer_cast<ast::Statement>($5),
            std::dynamic_pointer_cast<ast::Statement>($7)
        );
    }
    | WHILE LPAREN Exp RPAREN Statement {
        $$ = std::make_shared<ast::While>(
            std::dynamic_pointer_cast<ast::Exp>($3),
            std::dynamic_pointer_cast<ast::Statement>($5)
        );
    }
    | BREAK SC { $$ = std::make_shared<ast::Break>(); }
    | CONTINUE SC { $$ = std::make_shared<ast::Continue>(); }
;

Call:
    ID LPAREN Explist RPAREN {
        $$ = std::make_shared<ast::Call>(
            std::dynamic_pointer_cast<ast::ID>($1),
            std::dynamic_pointer_cast<ast::ExpList>($3)
        );
    }
    | ID LPAREN RPAREN {
        $$ = std::make_shared<ast::Call>(
            std::dynamic_pointer_cast<ast::ID>($1),
            std::make_shared<ast::ExpList>()
        );
    }
;

Explist:
    Exp { $$ = std::make_shared<ast::ExpList>(std::dynamic_pointer_cast<ast::Exp>($1)); }
    | Exp COMMA Explist { $$ = std::dynamic_pointer_cast<ast::ExpList>($3); std::dynamic_pointer_cast<ast::ExpList>($$)->push_front(std::dynamic_pointer_cast<ast::Exp>($1)); }
;

Type:
    INT { $$ = std::make_shared<ast::Type>(ast::BuiltInType::INT); }
    | BYTE { $$ = std::make_shared<ast::Type>(ast::BuiltInType::BYTE); }
    | BOOL { $$ = std::make_shared<ast::Type>(ast::BuiltInType::BOOL); }
;

Exp:
    LPAREN Exp RPAREN { $$ = $2; }
    | Exp PLUS Exp {
        $$ = std::make_shared<ast::BinOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::ADD
        );
    }
    | Exp MINUS Exp {
        $$ = std::make_shared<ast::BinOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::SUB
        );
    }
    | Exp MULT Exp {
        $$ = std::make_shared<ast::BinOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::MUL
        );
    }
    | Exp DIV Exp {
        $$ = std::make_shared<ast::BinOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::BinOpType::DIV
        );
    }
    | ID {
        $$ = $1;
    }
    | Call { $$ = $1; }
    | NUM {
        $$ = $1;
    }
    | NUM_B {
        $$ = $1;
    }
    | STRING {
        $$ = $1;
    }
    | TRUE {
        $$ = std::make_shared<ast::Bool>(true);
    }
    | FALSE {
        $$ = std::make_shared<ast::Bool>(false);
    }
    | NOT Exp {
        $$ = std::make_shared<ast::Not>(std::dynamic_pointer_cast<ast::Exp>($2));
    }
    | Exp AND Exp {
        $$ = std::make_shared<ast::And>(std::dynamic_pointer_cast<ast::Exp>($1), std::dynamic_pointer_cast<ast::Exp>($3));
    }
    | Exp OR Exp {
        $$ = std::make_shared<ast::Or>(std::dynamic_pointer_cast<ast::Exp>($1), std::dynamic_pointer_cast<ast::Exp>($3));
    }


    | Exp EQUAL Exp {
        $$ = std::make_shared<ast::RelOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::EQ
        );    }
    | Exp NEQUAL Exp {
        $$ = std::make_shared<ast::RelOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::NE
        );    }


    | Exp LESS Exp {
        $$ = std::make_shared<ast::RelOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::LT
        );    }

    | Exp GREATER Exp {
        $$ = std::make_shared<ast::RelOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::GT
        );    }

    | Exp LEQ Exp {
        $$ = std::make_shared<ast::RelOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::LE
        );    }

    | Exp GEQ Exp {
        $$ = std::make_shared<ast::RelOp>(
            std::dynamic_pointer_cast<ast::Exp>($1),
            std::dynamic_pointer_cast<ast::Exp>($3),
            ast::RelOpType::GE
        );    }

    | LPAREN Type RPAREN Exp {
    $$ = std::make_shared<ast::Cast>(
        std::dynamic_pointer_cast<ast::Exp>($4),     
        std::dynamic_pointer_cast<ast::Type>($2)
    );
    }
    ;

%%

void yyerror(const char*){
    output::errorSyn(yylineno); //check later
}
