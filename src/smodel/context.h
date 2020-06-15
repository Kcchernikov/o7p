#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>

class NamedArtefact;

// Класс, определяющий назначение различных типов программных объектов
class Context {
public:
    // Установка общего начального контекста
    Context(NamedArtefact* na = nullptr);

    // Вывод отладочной информации о назначении артефакта
    virtual void debugOut() = 0;
    // Вывод отладочной иноформации (при необходимости) об имени контекста
    void debugInfoAboutName();

    // Получение указателя на объект с именем артефакта
    NamedArtefact* getNamedArtefact() { return nameOfArtefact; }
    // Установка указателя на именованный артефакт
    void setNamedArtefact(NamedArtefact* na) {nameOfArtefact = na;}
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

    // Вывод отладочной информации о именованном артефакте
    virtual void debugOut();
private:
    std::string name;   // Имя артефакта
    Context* context;   // Указатель на контекст именованного артефакта
    bool access;        // Уровень доступа к именованному артефакту
};

#endif // CONTEXT_H
