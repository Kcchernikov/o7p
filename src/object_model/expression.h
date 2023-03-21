#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <cstddef>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include "../smodel/type.h"
#include "declaration.h"
#include "operator.h"

class Designator;
class Expression;

class Set {
public:
    Set(TypeContext* type);
    void addElement(Expression* exp1, Expression* exp2);
    TypeContext* getResultType();
    void debugOut(size_t tabcnt = 0);

private:
    std::vector<std::pair<Expression*, Expression*>> elements;
    TypeContext* resultType;
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
public:
    Factor(ConstFactor* factor);
    Factor(Set* st);
    Factor(DesignatorWrapper* wr);
    Factor(Expression* exp);
    Factor(Factor* factor);
    TypeContext* getResultType();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);

private:
    // factor = number | string | NIL | TRUE | FALSE |
    //       set | designator [ActualParameters] | "(" expression ")" | "~" factor.
    std::variant<ConstFactor*, Set*, DesignatorWrapper*, Expression*, Factor*> value;
    TypeContext* resultType;
    bool isVar;
};

class Term {
public:
    void setStartFactor(Factor* factor);
    void addFactor(BinaryOperator op, Factor* factor);
    TypeContext* getResultType();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);

private:
    Factor* startFactor;
    std::vector<std::pair<BinaryOperator, Factor*>> factors;
    TypeContext* resultType;
    bool isVar;
};

class SimpleExpression {
public:
    void setStartTerm(UnaryOperator op, Term* term);
    void addTerm(BinaryOperator op, Term* term);
    TypeContext* getResultType();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);

private:
    UnaryOperator unaryOperator;
    Term* startTerm;
    std::vector<std::pair<BinaryOperator, Term*>> terms;
    TypeContext* resultType;
    bool isVar;
};

class Expression {
public:
    void setFirstSimpleExpression(SimpleExpression* exp);
    void setSecondSimpleExpression(Relation rel, SimpleExpression* exp, TypeContext* result);
    TypeContext* getResultType();
    bool getIsVar();
    void debugOut(size_t tabcnt = 0);
private:
    SimpleExpression* firstSimpleExpression;
    std::optional<std::pair<Relation, SimpleExpression*>> secondSimpleExpression;
    TypeContext* resultType;
    bool isVar;
};

#endif // EXPRESSION_H
