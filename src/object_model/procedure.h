#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "../smodel/procedure_context.h"
#include "declaration.h"
#include "statement.h"

#include <cstddef>
#include <vector>

struct ProcedureBody {
    // DeclarationSequence* declaration; декларация сливается с декларацией в формальных параметрах
    StatementSequence* statement;
    Expression* result;
};

// Класс, процедуры
class Procedure: public ProcContext {
public:
    Procedure(DeclarationSequence* ds, ProcedureBody body): ProcContext(ds), body(body) {}
    Procedure(DeclarationSequence* ds): ProcContext(ds) {}
// public:
//     // Конструктор для создания переменной заданного типа
//     ProcContext(Module& m);

//     // Получение информации о типе переменой
//     TypeContext* getResultType() { return resultType; }
//     // Установка типа переменной
//     void setResultType(TypeContext* type) { resultType = type; }
    void setIdent(const Identdef& id);

    Identdef getIdent();

    void setBody(ProcedureBody body);

//     // Вывод отладочной информации о базовом типе
    virtual void debugOut(size_t tabcnt = 0);
private:
    // heading уже неявно храниться в ProcContext
    Identdef ident;
    ProcedureBody body;
//     TypeContext* resultType;   // Тип результата
};


#endif // PROCEDURE_H
