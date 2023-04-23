#ifndef STATEMENT_H
#define STATEMENT_H

#include "declaration.h"
#include "designator.h"
#include "expression.h"
#include "../smodel/context.h"
#include "../smodel/variable.h"

#include <vector>
#include <variant>

class StatementSequence;

class Statement {
public:
    virtual void debugOut(size_t tabcnt = 0);
    virtual void generate(Generator* generator, std::stringstream& cur) = 0;
    virtual ~Statement() {}
private:
    // std::variant<Assigment, ProcedureCall, IfStatement,
    //  CaseStatement, WhileStatement, RepeatStatement, ForStatement> value;
};

class Assigment: public Statement {
    friend class GeneratorC;
public:
    Assigment(Designator* d, Expression* exp);
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
private:
    Designator* designator;
    Expression* expression;
};

class ProcedureCall: public Statement {
    friend class GeneratorC;
public:
    ProcedureCall(Designator* des, const std::vector<Expression*>& params);
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
private:
    Designator* designator;
    std::vector<Expression*> params;
};

class IfStatement: public Statement {
    friend class GeneratorC;
public:
    IfStatement();
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
    void setMainExpression(Expression* exp, StatementSequence* st);
    void addElsifExpression(Expression* exp, StatementSequence* st);
    void setElseExpression(StatementSequence* st);
private:
    std::pair<Expression*, StatementSequence*> main;
    std::vector<std::pair<Expression*, StatementSequence*> > elsif;
    StatementSequence* els;
};

struct CaseLabelList {
    std::vector<std::pair<Context*, Context*> > list;
};

class CaseStatement: public Statement {
    friend class GeneratorC;
public:
    void setExpression(Expression* exp);
    void addCase(CaseLabelList* labelList, StatementSequence* st);
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
private:
    Expression* expression;
    std::vector<std::pair<CaseLabelList*, StatementSequence*> > cases;
};

class WhileStatement: public Statement {
    friend class GeneratorC;
public:
    void setMainExpression(Expression* exp, StatementSequence* st);
    void addElsifExpression(Expression* exp, StatementSequence* st);
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
private:
    std::pair<Expression*, StatementSequence*> main;
    std::vector<std::pair<Expression*, StatementSequence*> > elsif;
};

class RepeatStatement: public Statement {
    friend class GeneratorC;
public:
    RepeatStatement(Expression* exp, StatementSequence* st);
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
private:
    Expression* expression;
    StatementSequence* stSeq;
};

class ForStatement: public Statement {
    friend class GeneratorC;
public:
    ForStatement(
        VarContext* id,
        Expression* start,
        Expression* stop,
        ConstFactor* by,
        StatementSequence* st
    );
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
private:
    VarContext* ident;
    Expression* start;
    Expression* stop;
    ConstFactor* by;
    StatementSequence* stSeq;
};

class StatementSequence {
    friend class GeneratorC;
public:
    void addStatement(Statement* st);
    void debugOut(size_t tabcnt = 0);
private:
    std::vector<Statement*> statements;
};


#endif // STATEMENT_H
