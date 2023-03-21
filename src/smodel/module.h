#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "context.h"
#include "../object_model/declaration.h"
#include "../object_model/statement.h"

// Класс определяющий данные, непосредственно доступные для модуля и процедур
class CommonData {
protected:
    static std::string moduleName;  // имя модуля
    // Список артефактов с зарезервированными именами
    static std::unordered_map<std::string, NamedArtefact*> reservedNamedArtefacts;
    // Список деклараций импорта
    static std::unordered_map<std::string, DeclarationSequence*> importArtefacts;
    // Список именованных артефактов, порождаемых в модул
    static DeclarationSequence declaration;
    // // Список именованных артефактов, порождаемых в модуле
    // static std::vector<NamedArtefact*> namedArtefacts;
    // // Список констнантных артефактов, пораждаемых в модуле
    // std::vector<ConstDeclaration*> constNamedArtefacts;
    // std::string moduleName;  // имя модуля
    // // Список артефактов с зарезервированными именами
    // std::vector<NamedArtefact*> reservedNamedArtefacts;
    // // Список именованных артефактов, порождаемых в модуле
    // std::vector<NamedArtefact*> namedArtefacts;
    // Добавление именованного артефакта
public:
    void AddNamedArtefact(std::string name, Context* context, bool access = false);
};

// Класс, определяющий модуль семантической модели.
// Содержит все данные, описывающие артефакты языка программирования
class Module: public CommonData {
    friend class Creator;
public:
    Module();

    // Установка имени модуля
    void setModuleName(const std::string& mn) { moduleName = mn;}
    // Установка DeclarationSequence
    void setDeclarationSequence(const DeclarationSequence* ds);
    // Установка DeclarationSequence
    void copyDeclarationSequence(const DeclarationSequence* ds);
    // Получение указателя на декларацию модуля
    DeclarationSequence* getDeclarationSequence() {return &declaration;}
    // Получение указателя на импорт
    std::unordered_map<std::string, DeclarationSequence*>* getImport() {return &importArtefacts;}
    // Получение указателя на зарезервированные артефакты
    std::unordered_map<std::string, NamedArtefact*>* getReserved() {return &reservedNamedArtefacts;}
    // Установка StatementSequence
    void setStatementSequence(StatementSequence* st) {statement = st;}
    // Вывод отладочной информации о модуле
    void debugOut(size_t tabcnt = 0);


    // // Добавление именованного артефакта
    // void AddNamedArtefact(std::string name, Context* context, bool access = false);

private:
    StatementSequence* statement;
};

#endif // MODULE_H
