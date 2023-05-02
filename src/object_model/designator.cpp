#include "designator.h"
#include "../generator/generator.h"

#include <cassert>
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

void RecordSelector::generate(Generator* generator, std::stringstream& cur) {
    generator->GenerateRecordSelector(*this, cur);
}

IndexSelector::IndexSelector(const std::vector<Expression*>& exps, TypeContext* prev): expList(exps) {
    if (!prev || exps.empty()) {
        assert(false && "Incorrect Selector");
    }
    type = prev;
    for (auto exp : exps) {
        TypeArrayContext* cur = (type->getTypeName() == "TypeArrayContext"
            ? dynamic_cast<TypeArrayContext*>(type)
            : nullptr
        );
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

void IndexSelector::generate(Generator* generator, std::stringstream& cur) {
    generator->GenerateIndexSelector(*this, cur);
}

AssertSelector::AssertSelector(const Qualident& qual, TypeContext* prev): qualident(qual), prevType(prev) {
    TypeContext* newType = qual.type;
    if (newType != prev) {
        TypeRecordContext* rec = (prev->getTypeName() == "TypeRecordContext"
            ? dynamic_cast<TypeRecordContext*>(prev)
            : nullptr
        );
        if (!rec->isOneOfParents(newType)) {
            assert(false && "Types in assert are not relative");
        }
    } else {
        isAlwaysTrue = true;
    }
    type = newType;
}

void AssertSelector::debugOut(size_t tabcnt) {
    std::cout << "ASSERT SELECTOR (";
    type->debugOut();
    std::cout << ")";
}

void AssertSelector::generate(Generator* generator, std::stringstream& cur) {
    generator->GenerateAssertSelector(*this, cur);
}

PointerSelector::PointerSelector(TypeContext* prev) {
    TypePointerContext* p = (prev->getTypeName() == "TypePointerContext"
        ? dynamic_cast<TypePointerContext*>(prev)
        : nullptr
    );
    if (!p) {
        assert(false && "^ is avaliable only for pointers");
    }
    type = p->getRecord();
}

void PointerSelector::debugOut(size_t tabcnt) {
    std::cout << "POINTER SELECTOR ^";
}

void PointerSelector::generate(Generator* generator, std::stringstream& cur) {
    generator->GeneratePointerSelector(*this, cur);
}

std::vector<Expression*> IndexSelector::getExpressionList() {
    return expList;
}

void Designator::addQualident(Qualident qual) {
    qualident = qual;
    type = qual.type;
    isVar = qual.isVariable;
    var = nullptr;
    if (qual.varArtefact != nullptr && qual.varArtefact->getContext() != nullptr
        && qual.varArtefact->getContext()->getTypeName() == "VarContext") {
        var = dynamic_cast<VarContext*>(qual.varArtefact->getContext());
    }
}

void Designator::addRecordSelector(std::string ident) {
    if (!isVar) {
        assert(false && "Selectors are avaliable only for variables");
    }
    if (!type) {
        assert(false && "Empty Designator's type");
    }
    TypeRecordContext* rec = (type->getTypeName() == "TypeRecordContext"
        ? dynamic_cast<TypeRecordContext*>(type)
        : nullptr
    );
    if (rec) {
        selectors.push_back(new RecordSelector(ident, rec));
    } else {
        TypePointerContext* p = (type->getTypeName() == "TypePointerContext"
            ? dynamic_cast<TypePointerContext*>(type)
            : nullptr
        );
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
    selectors.push_back(new PointerSelector(type));
    type = selectors.back()->getType();
}

Qualident Designator::getQualident() const {
    return qualident;
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

VarContext* Designator::getVar() {
    return var;
}

bool Designator::getIsVar() {
    return isVar;
}

