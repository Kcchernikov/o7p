#include "type.h"

// Вывод отладочной информации об общем контексте типа
void TypeContext::debugOut(size_t tabcnt) {
    std::cout << "TYPE ";
    ///debugInfoAboutName();
}

// Вывод отладочной информации о булевском типе
void TypeBoolContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "BOOLEAN, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о целочисленном типе
void TypeIntegerContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "INTEGER, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о действительном типе
void TypeRealContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "REAL, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о символьном типе
void TypeCharContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "CHAR, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о байтовом типе
void TypeByteContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "BYTE, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о строковом типе
void TypeStringContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "STRING, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о Nil
void TypeNilContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "NIL, size = " << typeSize; // << std::endl;
}

// Вывод отладочной информации о множественном типе
void TypeSetContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "SET, size = " << typeSize; // << std::endl;
}

// Добавление к записи, объявленной в модуле именованного поля
void TypeRecordContext::AddNamedModuleField(std::string name, TypeContext* typeContext, bool access) {
    NamedArtefact tmpNamed{name, typeContext, access};
    // TODO
    // namedFields.push_back(tmpNamed);
    typeSize += typeContext->getTypeSize();
}

NamedArtefact* TypeRecordContext::getArtefactByName(const std::string& ident) {
    return declaration.getArtefactByName(ident);
}

// Вывод отладочной информации о записи
void TypeRecordContext::debugOut(size_t tabcnt) {
    for (size_t i = 0; i < tabcnt; ++i) {
        std::cout << "  ";
    }
    TypeContext::debugOut(tabcnt);
    std::cout << "RECORD, size = " << typeSize << std::endl;
    if(parentContext != nullptr) {
        std::cout << "(\n";
        parentContext->debugOut(tabcnt);
        std::cout << ")\n";
    }
    // TODO
    std::cout << "RECORD DECLARATION {\n";
    declaration.debugOut(tabcnt + 1);
    // for(NamedArtefact field: namedFields) {
    //     std::cout << std::endl;
    //     std::cout << "    " << field.getName() << (field.getAccess()? "*: ":": ");
    //     field.getContext()->debugOut();
    // }
    std::cout << "}";
}

// Вывод отладочной информации о типе - указателе
void TypePointerContext::debugOut(size_t tabcnt) {
    for (size_t i = 0; i < tabcnt; ++i) {
        std::cout << "  ";
    }
    TypeContext::debugOut(tabcnt);
    std::cout << "POINTER TO (size = " << typeSize << ") {";
    // recordType->debugOut(tabcnt);
    std::cout << "}"; // << std::endl;
}

// Вывод отладочной информации о типе - массиве
void TypeArrayContext::debugOut(size_t tabcnt) {
    for (size_t i = 0; i < tabcnt; ++i) {
        std::cout << "  ";
    }
    TypeContext::debugOut(tabcnt);
    std::cout << "ARRAY WITH LEN " << length << " OF TYPE:\n\t";
    if (valueType) {
        valueType->debugOut(tabcnt + 1);
    } else {
        std::cout << "\e[1;31m ERROR: VALUE TYPE IS EMPTY \e[1m \n";
    }
    // std::cout << "}"; // << std::endl;
}

