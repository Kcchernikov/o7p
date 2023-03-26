#include "validate.h"
#include "expression.h"
#include "procedure.h"

bool IsComparable(TypeContext* formalType, TypeContext* actualType) {
    if (actualType == formalType) {
        return true;
    } else {
        // Check record type the same
        // актуальный параметр должен быть потомком формального
        TypeRecordContext* actualRec = dynamic_cast<TypeRecordContext*>(actualType);
        TypeRecordContext* formalRec = dynamic_cast<TypeRecordContext*>(formalType);
        if (actualRec && formalRec) {
            if (actualRec->isOneOfParents(formalRec)) {
                return true;
            }
        } else {
            // Check pointer type the same
            TypePointerContext* actualP = dynamic_cast<TypePointerContext*>(actualType);
            TypePointerContext* formalP = dynamic_cast<TypePointerContext*>(formalType);
            if (formalP && dynamic_cast<TypeNilContext*>(actualType)) {
                return true;
            }
            if (actualP && formalP && actualP->isOneOfParents(formalP)) {
                return true;
            } else {
                // Check array type the same
                TypeArrayContext* actualArr = dynamic_cast<TypeArrayContext*>(actualType);
                TypeArrayContext* formalArr = dynamic_cast<TypeArrayContext*>(formalType);
                if (actualArr && formalArr && actualArr->isEqual(formalArr)) {
                    return true;
                }
            }
        }
    }
    return false;
}

TypeContext* ValidateParameters(Designator* des, const std::vector<Expression*>& actualParams) {
    if (!des) {
        assert(false && "Designator is null");
    }
    ProcContext* proc = dynamic_cast<ProcContext*>(des->getType());
    if (!proc) {
        assert(false && "Only procedure is callable");
    }
    // Check params' type
    FormalParameters* formalParams = proc->getFormalParameters();
    size_t i = 0, sz = actualParams.size();
    for (FPSection* sec : formalParams->getSections()) {
        for (size_t j = 0; j < sec->getParameters().size(); ++j) {
            if (i == sz) {
                assert(false && "Formal parameters and actual parameters must be the same length");
            }
            // Запрет передавать в функцию не переменную там, где параметр VAR
            if (sec->getIsVar() && !actualParams[i++]->getIsVar()) {
                assert(false && "Only variable are avaliable for VAR formal type");
            }
            TypeContext* actualType = actualParams[i++]->getResultType();
            TypeContext* formalType = sec->getType();
            if (!IsComparable(formalType, actualType)) {
                assert(false && "Formal parameters and actual parameters must be the same type");
            }
        }
    }
    TypeContext* resultType = proc->getResultType();
    return resultType;
}