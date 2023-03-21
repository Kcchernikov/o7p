#include "statement.h"
#include "declaration.h"
#include "expression.h"

#include <iostream>

void Statement::debugOut(size_t tabcnt) {
    std::cout << "Statement ";
}

Assigment::Assigment(Designator* des, Expression* exp): designator(des), expression(exp) {
}

void Assigment::debugOut(size_t tabcnt) {
    std::cout << "Assignment ";
    designator->debugOut(tabcnt);
    std::cout << " := ";
    expression->debugOut();
}

ProcedureCall::ProcedureCall(Designator* des, const std::vector<Expression*> params): designator(des), params(params) {
    // TODO validate params
}

void ProcedureCall::debugOut(size_t tabcnt) {
    std::cout << "ProcedureCall ";
    designator->debugOut(0);
    std::cout << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        params[i]->debugOut(0);
        if (i + 1 < params.size()) {
            std::cout << ", ";
        }
    }
    std::cout << ")";
}

IfStatement::IfStatement(): els(nullptr) {
}

void IfStatement::setMainExpression(Expression *exp, StatementSequence *st) {
    main = std::pair<Expression*, StatementSequence*>(exp, st);
}

void IfStatement::addElsifExpression(Expression *exp, StatementSequence *st) {
    elsif.push_back(std::pair<Expression*, StatementSequence*>(exp, st));
}

void IfStatement::setElseExpression(StatementSequence *st) {
    els = st;
}

void IfStatement::debugOut(size_t tabcnt) {
    std::cout << "IfStatement ";
    main.first->debugOut(tabcnt);
    std::cout << "\n";
    if (main.second) {
        main.second->debugOut(tabcnt + 1);
    }
    for (auto el : elsif) {
        std::cout << "ELSE IF ";
        el.first->debugOut(tabcnt);
        std::cout << "\n";
        if (el.second) {
            el.second->debugOut(tabcnt + 1);
        }
    }
    if (els) {
        std::cout << "ELSE \n";
        els->debugOut(tabcnt + 1);
    }
}

void CaseStatement::setExpression(Expression *exp) {
    expression = exp;
}

void CaseStatement::addCase(CaseLabelList* labelList, StatementSequence* st) {
    cases.push_back(std::pair<CaseLabelList*, StatementSequence*>(labelList, st));
}

void CaseStatement::debugOut(size_t tabcnt) {
    std::cout << "CaseStatement ";
}

void WhileStatement::setMainExpression(Expression *exp, StatementSequence *st) {
    main = std::pair<Expression*, StatementSequence*>(exp, st);
}

void WhileStatement::addElsifExpression(Expression *exp, StatementSequence *st) {
    elsif.push_back(std::pair<Expression*, StatementSequence*>(exp, st));
}

void WhileStatement::debugOut(size_t tabcnt) {
    std::cout << "WhileStatement ";
}

RepeatStatement::RepeatStatement(Expression* exp, StatementSequence* st): expression(exp), stSeq(st) {
}

void RepeatStatement::debugOut(size_t tabcnt) {
    std::cout << "RepeatStatement ";
}

ForStatement::ForStatement(
    VarContext* id,
    Expression* start,
    Expression* stop,
    ConstFactor* by,
    StatementSequence* st
): ident(id), start(start), stop(stop), by(by), stSeq(st) {
}    

void ForStatement::debugOut(size_t tabcnt) {
    std::cout << "ForStatement ";
}

void StatementSequence::addStatement(Statement *st) {
    statements.push_back(st);
}

void StatementSequence::debugOut(size_t tabcnt) {
    std::cout << "Statement Sequence:" << std::endl;
    for (Statement* st : statements) {
        if (!st) {
            std::cout << "\e[1;31m" << "Statement is null" << "\e[0m" << std::endl;
        } else {
            st->debugOut(tabcnt);
            std::cout << std::endl;
        }
    }
    std::cout << "End Statement Sequence\n";
    std::cout << "---------" << std::endl;
}