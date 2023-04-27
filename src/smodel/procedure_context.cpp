#include "procedure_context.h"
#include "context.h"
#include "type_context.h"
#include "../generator/generator.h"

#include "variable.h"
#include <cstddef>
#include <string>

void FormalParameters::addFPSection(FPSection* fps) {
    parameters.push_back(fps);
}

void FormalParameters::setResultType(TypeContext *ctx) {
    resultType = ctx;
}

TypeContext* FormalParameters::getResultType() {
    return resultType;
}

std::vector<FPSection*> FormalParameters::getSections() {
    return parameters;
}

void FPSection::debugOut(size_t tabcnt) {
    std::cout << "FPSection: ";
    if (isVar) {
        std::cout << "VAR ";
    }
    for (auto id : parameters) {
        std::cout << id << ", ";
    }
    if (type) {
        std::cout << ": ";
        type->debugOut(tabcnt);
    }
}

void FPSection::setIsVar(bool isVariable) {
    isVar = isVariable;
}

bool FPSection::getIsVar() const {
    return isVar;
}

void FPSection::setType(TypeContext* t) {
    type = t;
}

TypeContext* FPSection::getType() {
    return type;
}

void FPSection::addParameter(const std::string& ident) {
    parameters.push_back(ident);
}

std::vector<std::string> FPSection::getParameters() {
    return parameters;
}

void FormalParameters::debugOut(size_t tabcnt) {
    for (auto p : parameters) {
        p->debugOut(tabcnt);
        std::cout << " ";
    }
    // if (resultType) {
    //     std::cout << ": ";
    //     resultType->debugOut(tabcnt);
    // }
    std::cout << "\n";
}

// Конструктор для создания первоначального процедурного контекста
ProcContext::ProcContext(Module &m) {}
ProcContext::ProcContext() {}

ProcContext::ProcContext(DeclarationSequence* prev)
        : declaration(new DeclarationSequence(prev)) {
}

// Установка формальных параметров
void ProcContext::setFormalParameters(FormalParameters* fp) {
    heading = fp;
    for (FPSection* section : fp->getSections()) {
        for (auto& name : section->getParameters()) {
            NamedArtefact* art = new NamedArtefact(name, new VarContext(section->getType()), false);
            declaration->addNamedArtefact(art);
            declaration->addHideArtefact(name);
            // 'The result type of a procedure can be neither a record nor an array'
            // Поскольку массив не может быть результатом выражения или процедуры,
            // а также для него нет оператора присваивания, то передаваться в функцию
            // он может только по ссылке
            if (section->getIsVar() || section->getType()->getTypeName() == "TypeArrayContext") {
                refs.insert(art);
            }
        }
    }
}

DeclarationSequence* ProcContext::getDeclaration() {
    return declaration;
}

// Вывод общей отладочной информации о переменной
void ProcContext::debugOut(size_t tabcnt) {
    std::cout << "PROCEDURE ";
    if (heading) {
        std::cout << "FORMAL PARAMETERS ";
        heading->debugOut(tabcnt);

        // Добавление информации о типе переменной
        std::cout << "RESULT ";
        if (getResultType()) {
            getResultType()->debugOut(tabcnt);
        }
    }
}

void ProcContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateProcedureContext(*this, cur, name);
}

