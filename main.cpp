#include "output.hpp"
#include "nodes.hpp"
#include "semantic.hpp"
#include <iostream>
// Extern from the bison-generated parser
extern int yyparse();

extern std::shared_ptr<ast::Node> program;

int main() {
    // Parse the input. The result is stored in the global variable `program`
    yyparse();

    // Print the AST using the PrintVisitor
    //output::PrintVisitor printVisitor;
    //program->accept(printVisitor);

    // Semantic analysis
    SemanticVisitor semanticVisitor;
    program->accept(semanticVisitor);
    std::cout << semanticVisitor.printer;
}
