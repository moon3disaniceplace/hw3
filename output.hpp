
#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <vector>
#include <string>
#include <sstream>
#include "visitor.hpp"
#include "nodes.hpp"

namespace output {
    std::string toString(ast::BuiltInType type);
    std::string toStringUpper(ast::BuiltInType type);
    void errorLex(int lineno);

    void errorSyn(int lineno);

    void errorUndef(int lineno, const std::string &id);

    void errorDefAsFunc(int lineno, const std::string &id);

    void errorUndefFunc(int lineno, const std::string &id);

    void errorDefAsVar(int lineno, const std::string &id);

    void errorDef(int lineno, const std::string &id);

    void errorPrototypeMismatch(int lineno, const std::string &id, std::vector<std::string> &paramTypes);

    void errorMismatch(int lineno);

    void errorUnexpectedBreak(int lineno);

    void errorUnexpectedContinue(int lineno);

    void errorMainMissing();

    void errorByteTooLarge(int lineno, int value);

    class ScopePrinter {
    private:
        std::stringstream globalsBuffer;
        std::stringstream buffer;
        int indentLevel;

        std::string indent() const;

    public:
        ScopePrinter();

        void beginScope();

        void endScope();

        void emitVar(const std::string &id, const ast::BuiltInType &type, int offset);

        void emitFunc(const std::string &id, const ast::BuiltInType &returnType,
                      const std::vector<ast::BuiltInType> &paramTypes);

        friend std::ostream &operator<<(std::ostream &os, const ScopePrinter &printer);
    };
}

#endif //OUTPUT_HPP
