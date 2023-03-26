#include "expression.h"
#include "designator.h"
#include "operator.h"
#include "procedure.h"
#include "validate.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <vector>

Set::Set(TypeContext* type): resultType(type), isVar(false) { 
}

void Set::addElement(Expression* exp1, Expression* exp2) {
    if (!dynamic_cast<TypeIntegerContext*>(exp1->getResultType())) {
        assert(false && "Set elements must be integer");
    }
    if (exp2) {
        if (!dynamic_cast<TypeIntegerContext*>(exp2->getResultType())) {
            assert(false && "Set elements must be integer");
        }
    }
    elements.push_back({exp1, exp2});
}

TypeContext* Set::getResultType() {
    return resultType;
}

void Set::debugOut(size_t tabcnt) {
    std::cout << "SET ";
    for (auto p : elements) {
        p.first->debugOut(0);
        if (p.second) {
            std::cout << " .. ";
            p.second->debugOut(0);
        }
    }
}

ActualParameters::ActualParameters(const std::vector<Expression*>& p): params(p) {
}

Factor::Factor(ConstFactor* factor): value(factor), isVar(false) {
    resultType = factor->getResultType();
}

Factor::Factor(Set* st): value(st), isVar(false) {
    resultType = st->getResultType();
}

Factor::Factor(DesignatorWrapper* wr): value(wr), isVar(wr->designator->getIsVar() && wr->params == nullptr) {
    if (wr->params) {
        resultType = ValidateParameters(wr->designator, wr->params->params);
        if (!resultType) {
            assert(false && "Procedure without result type are banned in expressions");
        }
    } else {
        resultType = wr->designator->getType();
    }
}

Factor::Factor(Expression* exp): value(exp), isVar(exp->getIsVar()) {
    resultType = exp->getResultType();
}

Factor::Factor(Factor* factor): value(factor), isVar(false) {
    if (!dynamic_cast<TypeBoolContext*>(factor->getResultType())) {
        assert(false && "Negotiation is enable only for boolean");
    }
    resultType = factor->getResultType();
}

TypeContext* Factor::getResultType() {
    return resultType;
}

bool Factor::getIsVar() {
    return isVar;
}

void Factor::debugOut(size_t tabcnt) {
    switch (value.index()) {
    case 0:
        std::get<0>(value)->debugOut(0);
        break;
    case 1:
        std::get<1>(value)->debugOut(0);
        break;
    case 2:
        { // Чтобы инициализоровать локальные переменные
        DesignatorWrapper* wr = std::get<2>(value);
        wr->designator->debugOut(0);
        ActualParameters* params = wr->params;
        if (params) {
            std::cout << "(";
            for (size_t i = 0; i < params->params.size(); ++i) {
                params->params[i]->debugOut(0);
                if (i + 1 < params->params.size()) {
                    std::cout << ", ";
                }
            }
            std::cout << ")";
        }
        }
        break;
    case 3:
        std::cout << "(";
        std::get<3>(value)->debugOut(0);
        std::cout << ")";
        break;
    case 4:
        std::cout << "~";
        std::get<4>(value)->debugOut(0);
        break;
    }
}

void Term::setStartFactor(Factor* factor) {
    startFactor = factor;
    resultType = startFactor->getResultType();
    isVar = factor->getIsVar();
}

void CheckBinaryOperator(TypeContext* type, BinaryOperator op) {
    if (dynamic_cast<TypeNilContext*>(type)) {
        assert(false && "Binary operators ara not avaliable for NIL");
    }
    if (dynamic_cast<TypeRecordContext*>(type)) {
        assert(false && "Binary operators ara not avaliable for records");
    }
    if (dynamic_cast<TypePointerContext*>(type)) {
        assert(false && "Binary operators ara not avaliable for pointers");
    }
    if (dynamic_cast<TypeArrayContext*>(type)) {
        assert(false && "Binary operators ara not avaliable for arrays");
    }
    // оставшиеся типы : integer, real, boolean, char, set, string
    if (op == BinaryOperator::MINUS || op == BinaryOperator::PLUS
        || op == BinaryOperator::PRODUCT || op == BinaryOperator::QUOTIENT) { // определен для integer, real, set
        if (dynamic_cast<TypeBoolContext*>(type) 
            || dynamic_cast<TypeCharContext*>(type) || dynamic_cast<TypeStringContext*>(type)) {
            assert(false && "Binary operators 'minus', 'plus', 'product' and 'quotient' is not defined for this type"); 
        }
    } else if (op == BinaryOperator::OR || op == BinaryOperator::LOGICAL_CONJUNCTION) { // определен для boolean
        if (!dynamic_cast<TypeBoolContext*>(type)) {
            assert(false && "Binary operators 'or' ans 'logical conjuction' is defined only for bool"); 
        }
    } else if (op == BinaryOperator::INTEGER_QUOTIENT || op == BinaryOperator::MODULUS) { // определен для integer
         if (!dynamic_cast<TypeIntegerContext*>(type)) {
            assert(false && "Binary operators 'integer quotient' and 'modulus', is defined only for bool"); 
        }
    }
}

void Term::addFactor(BinaryOperator op, Factor* factor) {
    factors.push_back({op, factor});
    if (resultType != factor->getResultType()) {
        assert(false && "Binary operators ara avaliable only for the same types");
    }
    CheckBinaryOperator(resultType, op);
    isVar = false;  
}

TypeContext* Term::getResultType() {
    return resultType;
}

bool Term::getIsVar() {
    return isVar;
}

void Term::debugOut(size_t tabcnt) {
    startFactor->debugOut(tabcnt);
    for (auto p : factors) {
        std::cout << " " << p.first << " ";
        p.second->debugOut(0); 
    }
}

void CheckUnaryOperator(TypeContext* type, UnaryOperator op) {
    switch (op) {
        // определен для boolean
        case UnaryOperator::NEGATION:
            if (!dynamic_cast<TypeBoolContext*>(type)) {
                assert(false && "Negotiation operator is defined only for bool"); 
            }
            break;
        // определен для integer, real
        case UnaryOperator::UN_MINUS:
            if (!dynamic_cast<TypeIntegerContext*>(type) || !dynamic_cast<TypeRealContext*>(type)) {
                assert(false && "Unary minus operator is defined only for integer and real"); 
            }
            break;
        case UnaryOperator::UN_PLUS:
            if (!dynamic_cast<TypeIntegerContext*>(type) || !dynamic_cast<TypeRealContext*>(type)) {
                assert(false && "Unary plus operator is defined only for integer and real"); 
            }
            break;
        case UnaryOperator::NONE:
            break;
    }
}

void SimpleExpression::setStartTerm(UnaryOperator op, Term* term) {
    unaryOperator = op;
    startTerm = term;
    resultType = term->getResultType();
    if (op != UnaryOperator::NONE) {
        isVar = false;
    } else {
        isVar = term->getIsVar();
    }
    CheckUnaryOperator(resultType, op);
}

void SimpleExpression::addTerm(BinaryOperator op, Term* term) {
    isVar = false;
    terms.push_back({op, term});
    if (resultType != term->getResultType()) {
        assert(false && "Binary operators ara avaliable only for the same types");
    }
    CheckBinaryOperator(resultType, op);
}

TypeContext* SimpleExpression::getResultType() {
    return resultType;
}

bool SimpleExpression::getIsVar() {
    return isVar;
}

void SimpleExpression::debugOut(size_t tabcnt) {
    if (unaryOperator != UnaryOperator::NONE) {
        std::cout << unaryOperator;
    }
    startTerm->debugOut(0);
    for (auto p : terms) {
        std::cout << " " << p.first << " ";
        p.second->debugOut(0); 
    }
}

void Expression::setFirstSimpleExpression(SimpleExpression* exp) {
    firstSimpleExpression = exp;
    resultType = exp->getResultType();
    secondSimpleExpression = std::nullopt;
    isVar = exp->getIsVar();
}

void CheckRelation(TypeContext* type1, Relation rel, TypeContext* type2) {
    if (rel == Relation::TYPE_TEST) { // type1 IS type2
        if (type1 == type2) return;
        TypeRecordContext* rec1 = dynamic_cast<TypeRecordContext*>(type1);
        TypeRecordContext* rec2 = dynamic_cast<TypeRecordContext*>(type2);
        if ((rec1 && !rec2) || (!rec1 && rec2)) {
            assert(false && "Incorrect usage of type test"); 
        }
        if (rec1 && rec2) {
            if (rec2->isOneOfParents(rec1)) {
                return;
            }
            assert(false && "Incorrect usage of type test"); 
        }
        TypePointerContext* p1 = dynamic_cast<TypePointerContext*>(type1);
        TypePointerContext* p2 = dynamic_cast<TypePointerContext*>(type2);
        if (p1 && p2) {
            if (p1->isOneOfParents(p2)) {
                return;
            }
        }
        assert(false && "Incorrect usage of type test"); 
    }
    if (rel == Relation::SET_MEMBERSHIP) {
        if (!dynamic_cast<TypeIntegerContext*>(type1) || !dynamic_cast<TypeSetContext*>(type2)) {
            assert(false && "Set membership relation is defined only for integer and set");
        }
    }
    TypeArrayContext* arr1 = dynamic_cast<TypeArrayContext*>(type1);
    TypeArrayContext* arr2 = dynamic_cast<TypeArrayContext*>(type2);
    bool isCharArray = false;
    if (arr1 && arr2) {
        if (dynamic_cast<TypeCharContext*>(arr1->getElementType()) && dynamic_cast<TypeCharContext*>(arr2->getElementType())) {
            isCharArray = true;
        } else {
            assert(false && "Relation is not avaliable for not char arrays");
        }
    }
    if (type1 != type2 && !isCharArray) {
        assert(false && "Relation is defined only for same types"); 
    }
    // остались типы: set, string, record, pointer, procedure, integer, array of char, bool, real, char
    if (rel == Relation::EQUAL || rel == Relation::UNEQUAL) {
        // определено для integer, real, char, array of char, boolean, set, string, pointer, procedure
        if (dynamic_cast<TypeRecordContext*>(type1)) {
            assert(false && "Relation 'equal' is not defined for records"); 
        }
    } else if (rel == Relation::LESS || rel == Relation::LESS_OR_EQUAL 
            || rel == Relation::GREATER || rel == Relation::GREATER_OR_EQUAL) {
        if (dynamic_cast<TypeRecordContext*>(type1) 
            || dynamic_cast<TypeSetContext*>(type1) || dynamic_cast<TypeBoolContext*>(type1)    
            || dynamic_cast<TypePointerContext*>(type1) || dynamic_cast<ProcContext*>(type1)) {
            // определено для integer, real, char, array of char, string
            assert(false && "Relation 'less' is defined only for numbers, char and strings"); 
        }
    }
}

void Expression::setSecondSimpleExpression(Relation rel, SimpleExpression* exp, TypeContext* result) {
    CheckRelation(resultType, rel, exp->getResultType());
    secondSimpleExpression = {rel, exp};
    resultType = result;
    isVar = false;
}

TypeContext* Expression::getResultType() {
    return resultType;
}

bool Expression::getIsVar() {
    return isVar;
}

void Expression::debugOut(size_t tabcnt) {
    std::cout << "Expression ";
    firstSimpleExpression->debugOut(0);
    if (secondSimpleExpression != std::nullopt) {
        std::cout << secondSimpleExpression->first << " ";
        secondSimpleExpression->second->debugOut(0);
    }
    std::cout << " -> ";
    resultType->debugOut(0);
}
