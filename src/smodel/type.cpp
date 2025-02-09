#include "type.h"
#include "../object_model/expression.h"
#include "../generator/generator.h"

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

void TypeBoolContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeBool(*this, cur, name);
}

// Вывод отладочной информации о целочисленном типе
void TypeIntegerContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "INTEGER, size = " << typeSize; // << std::endl;
}

void TypeIntegerContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeInteger(*this, cur, name);
}

// Вывод отладочной информации о действительном типе
void TypeRealContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "REAL, size = " << typeSize; // << std::endl;
}

void TypeRealContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeReal(*this, cur, name);
}

// Вывод отладочной информации о символьном типе
void TypeCharContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "CHAR, size = " << typeSize; // << std::endl;
}

void TypeCharContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeChar(*this, cur, name);
}

// Вывод отладочной информации о байтовом типе
void TypeByteContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "BYTE, size = " << typeSize; // << std::endl;
}

void TypeByteContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeByte(*this, cur, name);
}

// Вывод отладочной информации о строковом типе
void TypeStringContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "STRING, size = " << typeSize; // << std::endl;
}

void TypeStringContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeString(*this, cur, name);
}

// Вывод отладочной информации о Nil
void TypeNilContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "NIL, size = " << typeSize; // << std::endl;
}

void TypeNilContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeNil(*this, cur, name);
}

// Вывод отладочной информации о множественном типе
void TypeSetContext::debugOut(size_t tabcnt) {
    TypeContext::debugOut(tabcnt);
    std::cout << "SET, size = " << typeSize; // << std::endl;
}

void TypeSetContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeSet(*this, cur, name);
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

void TypeRecordContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeRecord(*this, cur, name);
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

void TypePointerContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypePointer(*this, cur, name);
}

TypeArrayContext::TypeArrayContext(size_t len, TypeContext* v, bool allowEmpty): length(len), expLen(nullptr), valueType(v) {
    if (v) {
        typeSize = v->getTypeSize() * len;
    } else if (!allowEmpty) {
        std::cout << "\e[1;31m ERROR: VALUE TYPE IS EMPTY \e[0m" << std::endl;
    }
}

TypeArrayContext::TypeArrayContext(TypeContext* v, Expression* expLen, size_t len): length(len), expLen(expLen), valueType(v) {
    if (v) {
        typeSize = v->getTypeSize();
    } else {
        std::cout << "\e[1;31m ERROR: VALUE TYPE IS EMPTY \e[0m" << std::endl;
    }
}

TypeArrayContext::TypeArrayContext(TypeContext* v, DeclarationSequence* ds, size_t len)
    : length(len),
      expLen(nullptr),
      valueType(v) {
    if (len != 0) {
        expLen = new Expression();
        SimpleExpression* simple = new SimpleExpression();
        Term* term = new Term();
        ConstFactor* cf = new ConstFactor(static_cast<long long>(len), ds);
        Factor* factor = new Factor(cf);
        term->setStartFactor(factor);
        simple->setStartTerm(UnaryOperator::NONE, term);
        expLen->setFirstSimpleExpression(simple);
    }
    if (v) {
        typeSize = v->getTypeSize() * len;
    } else {
        std::cout << "\e[1;31m ERROR: VALUE TYPE IS EMPTY \e[0m" << std::endl;
    }
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

void TypeArrayContext::generate(Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateTypeArray(*this, cur, name);
}

