#include "statement.h"
#include "declaration.h"
#include "expression.h"
#include "validate.h"

#include <iostream>

void Statement::debugOut(size_t tabcnt) {
    std::cout << "Statement ";
}

Assigment::Assigment(Designator* des, Expression* exp): designator(des), expression(exp) {
    if (!des || !des->getIsVar()) {
        assert(false && "Designator in assigment must be a variable");  
    }
    if (!exp) {
        assert(false && "Expression must not be null");  
    }
    if (!IsComparable(des->getType(), exp->getResultType())) {
        assert(false && "Not comparable types in expression");  
    }
}

void Assigment::debugOut(size_t tabcnt) {
    std::cout << "Assignment ";
    designator->debugOut(tabcnt);
    std::cout << " := ";
    expression->debugOut();
}

ProcedureCall::ProcedureCall(Designator* des, const std::vector<Expression*>& params): designator(des), params(params) {
    ValidateParameters(des, params);
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
    if (!exp || !dynamic_cast<TypeBoolContext*>(exp->getResultType())) {
        assert(false && "Сonditional expressions must be boolean");  
    }
    main = std::pair<Expression*, StatementSequence*>(exp, st);
}

void IfStatement::addElsifExpression(Expression *exp, StatementSequence *st) {
    if (!exp || !dynamic_cast<TypeBoolContext*>(exp->getResultType())) {
        assert(false && "Сonditional expressions must be boolean");  
    }
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
    if (!exp) {
        assert(false && "Сonditional expressions must not be null");  
    }
    expression = exp;
}

void CaseStatement::addCase(CaseLabelList* labelList, StatementSequence* st) {
    if (!labelList) {
        assert(false && "Label list must not be empty");  
    }
    for (auto p : labelList->list) {
        TypeContext* type = p.first->getType();
        if (p.second) {
            // В таком случае типы должны быть одинаковыми и быть либо integer, либо byte, либо string
            if (!IsComparable(type, p.second->getType())) {
                assert(false && "Label list's types must be the same");  
            }
            if (!dynamic_cast<TypeIntegerContext*>(type) && !!dynamic_cast<TypeByteContext*>(type)) {
                // TODO понять, допускаются ли здесь строки
                assert(false && "Label list's types must be integer or byte");  
            }
        }
        if (!IsComparable(expression->getResultType(), type)) {
            assert(false && "Expression and label types must be comparable");
        }
    }
    cases.push_back(std::pair<CaseLabelList*, StatementSequence*>(labelList, st));
}

void CaseStatement::debugOut(size_t tabcnt) {
    std::cout << "CaseStatement ";
}

void WhileStatement::setMainExpression(Expression *exp, StatementSequence *st) {
    if (!exp || !dynamic_cast<TypeBoolContext*>(exp->getResultType())) {
        assert(false && "Сonditional expressions must be boolean");  
    }
    main = std::pair<Expression*, StatementSequence*>(exp, st);
}

void WhileStatement::addElsifExpression(Expression *exp, StatementSequence *st) {
    if (!exp || !dynamic_cast<TypeBoolContext*>(exp->getResultType())) {
        assert(false && "Сonditional expressions must be boolean");  
    }
    elsif.push_back(std::pair<Expression*, StatementSequence*>(exp, st));
}

void WhileStatement::debugOut(size_t tabcnt) {
    std::cout << "WhileStatement ";
}

RepeatStatement::RepeatStatement(Expression* exp, StatementSequence* st): expression(exp), stSeq(st) {
    if (!exp || !dynamic_cast<TypeBoolContext*>(exp->getResultType())) {
        assert(false && "Сonditional expressions must be boolean");  
    }
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
    if (!dynamic_cast<TypeIntegerContext*>(id->getType())) {
        assert(false && "Variable must be integer");
    }
    if (!dynamic_cast<TypeIntegerContext*>(start->getResultType()) ||
        !dynamic_cast<TypeIntegerContext*>(stop->getResultType())  ||
        !dynamic_cast<TypeIntegerContext*>(by->getResultType())) {
        assert(false && "For expressions must be integer");
    }
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