#ifndef PROCEDURE_CONTEXT_H
#define PROCEDURE_CONTEXT_H

#include "context.h"
#include "module.h"
#include "type_context.h"
#include "variable.h"
#include "../object_model/declaration.h"

#include <vector>

class FPSection {
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
    FormalParameters* getFormalParameters() { return heading; }
    // Установка типа переменной
    void setResultType(TypeContext* type) { heading->setResultType(type); }
    // Установка формальных параметров
    void setFormalParameters(FormalParameters* fp);

    DeclarationSequence* getDeclaration();

    // Вывод отладочной информации о базовом типе
    virtual void debugOut(size_t tabcnt = 0);
protected:
    DeclarationSequence* declaration;
    // DeclarationSequence* value;
    FormalParameters* heading = nullptr;
    // TypeContext* resultType;   // Тип результата
};


#endif // PROCEDURE_CONTEXT_H
