#include "module.h"
#include "context.h"
#include "../object_model/declaration.h"

std::string CommonData::moduleName{""};  // имя модуля
// Список артефактов с зарезервированными именами
std::unordered_map<std::string, NamedArtefact*> CommonData::reservedNamedArtefacts;
// Список деклараций импорта
std::unordered_map<std::string, DeclarationSequence*> CommonData::importArtefacts;
// Список именованных артефактов, порождаемых в модуле
// std::vector<NamedArtefact*> CommonData::namedArtefacts;
DeclarationSequence CommonData::declaration;

Module::Module() :statement(nullptr)
{
    // Константные функции
    reservedNamedArtefacts["ABS"] = new NamedArtefact("ABS", nullptr, false);
    reservedNamedArtefacts["ODD"] = new NamedArtefact("ODD", nullptr, false);
    reservedNamedArtefacts["LSL"] = new NamedArtefact("LSL", nullptr, false);
    reservedNamedArtefacts["ASR"] = new NamedArtefact("ASR", nullptr, false);
    reservedNamedArtefacts["ROR"] = new NamedArtefact("ROR", nullptr, false);
    reservedNamedArtefacts["FLOOR"] = new NamedArtefact("FLOOR", nullptr, false);
    reservedNamedArtefacts["FLT"] = new NamedArtefact("FLT", nullptr, false);
    reservedNamedArtefacts["ORD"] = new NamedArtefact("ORD", nullptr, false);
    reservedNamedArtefacts["CHR"] = new NamedArtefact("CHR", nullptr, false);

    // Базовые типы
    reservedNamedArtefacts["BOOLEAN"] = new NamedArtefact("BOOLEAN", nullptr, false);
    reservedNamedArtefacts["CHAR"] = new NamedArtefact("CHAR", nullptr, false);
    reservedNamedArtefacts["INTEGER"] = new NamedArtefact("INTEGER", nullptr, false);
    reservedNamedArtefacts["REAL"] = new NamedArtefact("REAL", nullptr, false);
    reservedNamedArtefacts["BYTE"] = new NamedArtefact("BYTE", nullptr, false);
    reservedNamedArtefacts["SET"] = new NamedArtefact("SET", nullptr, false);
}

// Добавление именованного артефакта
void CommonData::AddNamedArtefact(std::string name, Context* context, bool access) {
    NamedArtefact* tmpNamed = new NamedArtefact{name, context, access};
    declaration.addNamedArtefact(tmpNamed);
    // Контекст получает обратную ссылку на именованный артефакт
    context->setNamedArtefact(tmpNamed);
}

// Установка DeclarationSequence
void Module::setDeclarationSequence(const DeclarationSequence* ds) {
    if (ds) {
        declaration = *ds;
    }
}

// Установка DeclarationSequence
void Module::copyDeclarationSequence(const DeclarationSequence* ds) {
    if (ds) {
        declaration = *ds;
    }
}

// Вывод отладочной информации о модуле
void Module::debugOut(size_t tabcnt) {
    std::cout << "\e[1m" << std::endl;
    std::cout << "Module " << CommonData::moduleName << std::endl;
    declaration.debugOut(tabcnt);
    if (statement) {
        statement->debugOut(tabcnt);
    }
    // std::cout << "Reserved artefats: " << std::endl;
    // std::cout << "------------------ " << std::endl;
    // std::cout << "Artefats: " << std::endl;
    // std::cout << "--------- " << std::endl;
    // for(NamedArtefact* artefact: declaration.getNamedArtefacts()) {
    //     std::cout << artefact->getName() << (artefact->getAccess()? "*: ":": ");
    //     artefact->getContext()->debugOut();
    //     std::cout << std::endl;
    // }
    // std::cout << "Const Artefats: " << std::endl;
    // std::cout << "--------- " << std::endl;
    // for(ConstDeclaration* artefact: declaration.getConstNamedArtefacts()) {
    //     std::cout << artefact->getName() << (artefact->getAccess()? "*: ":": ");
    //     artefact->getFactor()->debugOut();
    //     // std::cout << std::endl;
    // }
    std::cout << "\e[0m" << std::endl;
}
