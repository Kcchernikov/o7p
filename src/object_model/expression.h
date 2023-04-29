#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <cstddef>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include "../smodel/type.h"
#include "../smodel/variable.h"
#include "declaration.h"
#include "operator.h"

class Designator;
class Expression;

class Set {
    friend class GeneratorC;
public:
    Set(TypeContext* type);
    void addElement(Expression* exp1, Expression* exp2);
    TypeContext* getResultType();
    void debugOut(size_t tabcnt = 0);

private:
    std::vector<std::pair<Expression*, Expression*>> elements;
    TypeContext* resultType;
    VarContext* var;
    bool isVar;
};

struct ActualParameters {
    ActualParameters(const std::vector<Expression*>& p);
    std::vector<Expression*> params;
};

struct DesignatorWrapper {
    Designator* designator;
    ActualParameters* params;
};

class Factor {
    friend class GeneratorC;
public:
    Factor(ConstFactor* factor);
    Factor(Set* st);
    Factor(DesignatorWrapper* wr);
    Factor(Expression* exp);
    Factor(Factor* factor);
    TypeContext* getResultType();
    VarContext* getVar();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);

private:
    // factor = number | string | NIL | TRUE | FALSE |
    //       set | designator [ActualParameters] | "(" expression ")" | "~" factor.
    std::variant<ConstFactor*, Set*, DesignatorWrapper*, Expression*, Factor*> value;
    TypeContext* resultType;
    VarContext* var;
    bool isVar;
};

class Term {
    friend class GeneratorC;
public:
    void setStartFactor(Factor* factor);
    void addFactor(BinaryOperator op, Factor* factor);
    TypeContext* getResultType();
    VarContext* getVar();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);

private:
    Factor* startFactor;
    std::vector<std::pair<BinaryOperator, Factor*>> factors;
    TypeContext* resultType;
    VarContext* var;
    bool isVar;
};

class SimpleExpression {
    friend class GeneratorC;
public:
    void setStartTerm(UnaryOperator op, Term* term);
    void addTerm(BinaryOperator op, Term* term);
    TypeContext* getResultType();
    VarContext* getVar();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);

private:
    UnaryOperator unaryOperator;
    Term* startTerm;
    std::vector<std::pair<BinaryOperator, Term*>> terms;
    TypeContext* resultType;
    VarContext* var;
    bool isVar;
};

class Expression {
    friend class GeneratorC;
public:
    void setFirstSimpleExpression(SimpleExpression* exp);
    void setSecondSimpleExpression(Relation rel, SimpleExpression* exp, TypeContext* result);
    TypeContext* getResultType();
    VarContext* getVar();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);
private:
    SimpleExpression* firstSimpleExpression;
    std::optional<std::pair<Relation, SimpleExpression*>> secondSimpleExpression;
    TypeContext* resultType;
    VarContext* var;
    bool isVar;
};

#endif // EXPRESSION_H
