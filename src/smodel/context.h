#ifndef CONTEXT_H
#define CONTEXT_H

// #include "type_context.h"
#include <sstream>
#include <iostream>
#include <string>

class NamedArtefact;
class TypeContext;
// Класс, определяющий назначение различных типов программных объектов
class Context {
    friend class GeneratorC;
public:
    // Установка общего начального контекста
    Context(NamedArtefact* na = nullptr);

    virtual TypeContext* getType();
    
    // Вывод отладочной информации о назначении артефакта
    virtual void debugOut(size_t tabcnt = 0) = 0;
    // Вывод отладочной иноформации (при необходимости) об имени контекста
    void debugInfoAboutName();

    virtual void generate(class Generator* generator, std::stringstream& cur, const std::string& name) {}

    // Получение указателя на объект с именем артефакта
    NamedArtefact* getNamedArtefact() const { return nameOfArtefact; }
    // Установка указателя на именованный артефакт
    void setNamedArtefact(NamedArtefact* na) {nameOfArtefact = na;}

    virtual std::string getTypeName() const {
        return "Context";
    }
private:
    NamedArtefact* nameOfArtefact;
};

// Класс, задающий именованный контекст
class NamedArtefact {
public:
    // Формирование именованного артиефакта
    NamedArtefact(std::string name, Context* context, bool access = false);

    // Получение ссылки на имя контекста
    std::string& getName() {return name;}
    // Получение указателя на контекст
    Context* getContext() {return context;}
    // Получение значения уровная доступа к артефакту
    bool getAccess() {return access;}

    // Установка имени контекста
    void setName(const std::string& newName) {name = newName;}
    // Установка контекста
    void setContext(Context* ctx) {context = ctx;}
    // Установка значения уровная доступа к артефакту
    void setAccess(bool newAcces) {access = newAcces;}

    // Вывод отладочной информации о именованном артефакте
    virtual void debugOut(size_t tabcnt = 0);
private:
    std::string name;   // Имя артефакта
    Context* context;   // Указатель на контекст именованного артефакта
    bool access;        // Уровень доступа к именованному артефакту
};

#endif // CONTEXT_H
