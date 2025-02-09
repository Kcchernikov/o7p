#ifndef PROCEDURE_CONTEXT_H
#define PROCEDURE_CONTEXT_H

#include "context.h"
#include "module.h"
#include "type_context.h"
#include "variable.h"
#include "../object_model/declaration.h"

#include <string>
#include <vector>
#include <unordered_set>

class FPSection {
    friend class GeneratorC;
public:
    FPSection(): isVar(false), type(nullptr) {
    }
    void debugOut(size_t tabcnt = 0);
    void setIsVar(bool isVariable);
    bool getIsVar() const;
    void setType(TypeContext* t);
    TypeContext* getType();
    void addParameter(const std::string& ident);
    std::vector<std::string> getParameters();
private:
    bool isVar = false;
    TypeContext* type;
    std::vector<std::string> parameters;
};

class FormalParameters {
    friend class GeneratorC;
public:
    void addFPSection(FPSection* fps);
    void setResultType(TypeContext* ctx);
    TypeContext* getResultType();
    std::vector<FPSection*> getSections();
    void debugOut(size_t tabcnt = 0);

private:
    std::vector<FPSection*> parameters;
    TypeContext* resultType = nullptr;
};

// Класс, задающий контекст для процедуры
class ProcContext: public TypeContext, CommonData {
    friend class GeneratorC;
public:
    // Конструктор для создания переменной заданного типа
    ProcContext(Module& m);
    ProcContext();
    ProcContext(DeclarationSequence* prev);

    // Получение информации о типе переменой
    virtual TypeContext* getType() {
        return this;
    }
    // Получение информации о типе переменой
    TypeContext* getResultType() { 
        if (heading) {
            return heading->getResultType(); 
        }
        return nullptr;
    }
    // Получение формальных параметров
    FormalParameters* getFormalParameters() const { return heading; }
    // Установка типа переменной
    void setResultType(TypeContext* type) { heading->setResultType(type); }
    // Установка формальных параметров
    void setFormalParameters(FormalParameters* fp);

    DeclarationSequence* getDeclaration();

    // Вывод отладочной информации о базовом типе
    virtual void debugOut(size_t tabcnt = 0);
    virtual void generate(class Generator* generator, std::stringstream& cur, const std::string& name);

    virtual std::string getTypeName() const {
        return "ProcContext";
    }
protected:
    DeclarationSequence* declaration;
    FormalParameters* heading = nullptr;
    std::unordered_set<NamedArtefact*> refs;
    // TypeContext* resultType;   // Тип результата
};


#endif // PROCEDURE_CONTEXT_H
