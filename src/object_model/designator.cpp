#include "designator.h"

#include <iostream>

TypeContext* Selector::getType() {
    return type;
}

void Selector::debugOut(size_t tabcnt) {
}

RecordSelector::RecordSelector(const std::string& id, TypeRecordContext* prev): ident(id) {
    if (!prev) {
        assert(false && "Incorrect Selector");
    }
    NamedArtefact* art = prev->getArtefactByName(id);
    if (!art) {
        assert(false && "Incorrect ident in Record Selector");
    }
    Context* ctx = art->getContext();
    if (!ctx) {
        assert(false && "Ident's context is null in Record Selector");
    }
    type = ctx->getType();
    if (!type) {
        assert(false && "Ident's context's type is null in Record Selector");
    }
}

std::string RecordSelector::getIdent() {
    return ident;
}

void RecordSelector::debugOut(size_t tabcnt) {
    std::cout << "RECORD SELECTOR ." << ident;
}

IndexSelector::IndexSelector(const std::vector<Expression*>& exps, TypeContext* prev) {
    if (!prev || exps.empty()) {
        assert(false && "Incorrect Selector");
    }
    type = prev;
    for (auto exp : exps) {
        TypeArrayContext* cur = dynamic_cast<TypeArrayContext*>(type);
        if (!cur) {
            assert(false && "It is unable to get index not from array");
        }
        type = cur->getElementType();
    }
}

void IndexSelector::debugOut(size_t tabcnt) {
    std::cout << "INDEX SELECTOR [ ";
    for (Expression* exp : expList) {
        exp->debugOut(tabcnt);
    }
}

AssertSelector::AssertSelector(const Qualident& qual, TypeContext* prev): idents(qual.idents) {
    TypeContext* newType = qual.type;
    if (newType != prev) {
        TypeRecordContext* rec = dynamic_cast<TypeRecordContext*>(prev);
        if (!rec->isOneOfParents(newType)) {
            assert(false && "Types in assert are not relative");
        }
    }
    type = newType;
}

void AssertSelector::debugOut(size_t tabcnt) {
    std::cout << "ASSERT SELECTOR (";
    type->debugOut();
    std::cout << ")";
}

PoinerSelector::PoinerSelector(TypeContext* prev) {
    TypePointerContext* p = dynamic_cast<TypePointerContext*>(prev);
    if (!p) {
        assert(false && "^ is avaliable only for pointers");
    }
    type = p->getRecord();
}

void PoinerSelector::debugOut(size_t tabcnt) {
    std::cout << "POINTER SELECTOR ^";
}

std::vector<Expression*> IndexSelector::getExpressionList() {
    return expList;
}

void Designator::addQualident(Qualident qual) {
    qualident = qual;
    type = qual.type;
    isVar = qual.isVariable;
}

void Designator::addRecordSelector(std::string ident) {
    if (!isVar) {
        assert(false && "Selectors are avaliable only for variables");
    }
    if (!type) {
        assert(false && "Empty Designator's type");
    }
    TypeRecordContext* rec = dynamic_cast<TypeRecordContext*>(type);
    if (rec) {
        selectors.push_back(new RecordSelector(ident, rec));
    } else {
        TypePointerContext* p = dynamic_cast<TypePointerContext*>(type);
        if (p) {
            selectors.push_back(new RecordSelector(ident, p->getRecord()));
        } else {
            assert(false && "Record Selector is enable only for record or pointer");
        }
    }
    type = selectors.back()->getType();
}

void Designator::addIndexSelector(const std::vector<Expression*> exps) {
    if (!isVar) {
        assert(false && "Selectors are avaliable only for variables");
    }
    selectors.push_back(new IndexSelector(exps, type));
    type = selectors.back()->getType();
}

void Designator::addAssertSelector(const Qualident& qual) {
    if (!isVar) {
        assert(false && "Selectors are avaliable only for variables");
    }
    selectors.push_back(new AssertSelector(qual, type));
    type = selectors.back()->getType();
}

void Designator::addPointerSelector() {
    if (!isVar) {
        assert(false && "Selectors are avaliable only for variables");
    }
    selectors.push_back(new PoinerSelector(type));
    type = selectors.back()->getType();
}

void Designator::debugOut(size_t tabcnt) {
    std::cout << "DESIGNATOR ";
    std::cout << "Qualident " << qualident.idents[0];
    if (qualident.idents.size() > 1) {
        std::cout << "." << qualident.idents[1] << " : ";
    } else {
        std::cout << " ";
    }
    for (auto sel : selectors) {
        sel->debugOut(tabcnt);
        std::cout << " : ";
    }
    std::cout << "Result Type: ";
    type->debugOut(tabcnt);
}

TypeContext* Designator::getType() {
    return type;
}

bool Designator::getIsVar() {
    return isVar;
}

