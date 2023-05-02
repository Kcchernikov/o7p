#include "validate.h"
#include "expression.h"
#include "procedure.h"

#include <cassert>

bool IsComparable(TypeContext* formalType, TypeContext* actualType) {
    if (actualType == formalType) {
        return true;
    } else {
        // Check record type the same
        // актуальный параметр должен быть потомком формального
        TypeRecordContext* actualRec = (actualType->getTypeName() == "TypeRecordContext"
            ? dynamic_cast<TypeRecordContext*>(actualType)
            : nullptr
        );
        TypeRecordContext* formalRec = (formalType->getTypeName() == "TypeRecordContext"
            ? dynamic_cast<TypeRecordContext*>(formalType)
            : nullptr
        );
        if (actualRec && formalRec) {
            if (actualRec->isOneOfParents(formalRec)) {
                return true;
            }
        } else {
            // Check pointer type the same
            TypePointerContext* actualP = (actualType->getTypeName() == "TypePointerContext"
                ? dynamic_cast<TypePointerContext*>(actualType)
                : nullptr
            );
            TypePointerContext* formalP = (formalType->getTypeName() == "TypePointerContext"
                ? dynamic_cast<TypePointerContext*>(formalType)
                : nullptr
            );
            if (formalP && actualType->getTypeName() == "TypeNilContext") {
                return true;
            }
            if (actualP && formalP && actualP->isOneOfParents(formalP)) {
                return true;
            } else {
                // Check array type the same
                TypeArrayContext* actualArr = (actualType->getTypeName() == "TypeArrayContext"
                    ? dynamic_cast<TypeArrayContext*>(actualType)
                    : nullptr
                );
                TypeArrayContext* formalArr = (formalType->getTypeName() == "TypeArrayContext"
                    ? dynamic_cast<TypeArrayContext*>(formalType)
                    : nullptr
                );
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
    ProcContext* proc = (des->getType()->getTypeName() == "ProcContext" || des->getType()->getTypeName() == "Procedure"
        ? dynamic_cast<ProcContext*>(des->getType())
        : nullptr
    );
    if (!proc) {
        assert(false && "Only procedure is callable");
    }
    // Check params' type
    FormalParameters* formalParams = proc->getFormalParameters();
    size_t i = 0, sz = actualParams.size();
    // Функция new проверяется отдельно и подставляется в момент генерации кода
    if (des->getQualident().idents.size() == 1 && des->getQualident().idents[0] == "NEW") {
        if (sz != 1) {
            assert(false && "Actual params size must be one for 'NEW'");
        }
        TypeContext* actualType = actualParams[0]->getResultType();
        if (actualType->getTypeName() != "TypePointerContext") {
            assert(false && "Actual param for 'NEW' must be a pointer");
        }
        return nullptr;
    }
    if (formalParams == nullptr) {
        if (sz != 0) {
            assert(false && "Formal parameters and actual parameters must be the same length");
        } else {
            TypeContext* resultType = proc->getResultType();
            return resultType;
        }
    }
    for (FPSection* sec : formalParams->getSections()) {
        for (size_t j = 0; j < sec->getParameters().size(); ++j) {
            if (i == sz) {
                assert(false && "Formal parameters and actual parameters must be the same length");
            }
            // Запрет передавать в функцию не переменную там, где параметр VAR
            if (sec->getIsVar() && !actualParams[i]->getIsVar()) {
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