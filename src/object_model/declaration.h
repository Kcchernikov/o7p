#ifndef DECLARATION_H
#define DECLARATION_H

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <unordered_map>
#include <unordered_set>

#include "operator.h"
#include "../smodel/context.h"
#include "../smodel/type_context.h"

class Generator;

struct Identdef {
    std::string name;
    bool access = false;
};

struct FieldList {
    void clear() {
        idents.clear();
        type = nullptr;
    }

    std::vector<Identdef> idents;
    TypeContext* type;
};

class DeclarationSequence;
class ConstFactor;

class NIL {

public:
    bool operator == (NIL n) const {return true;}
    bool operator != (NIL n) const {return false;}
    bool operator < (NIL n) const {return false;}
    bool operator <= (NIL n) const {return false;}
    bool operator > (NIL n) const {return false;}
    bool operator >= (NIL n) const {return false;}
};

class ConstProcedure {
public:
    virtual ConstFactor* execute(ConstFactor* arg, DeclarationSequence* resultDecl) const {
        assert(false && "Incorect call of constant function"); 
    }
    virtual ConstFactor* execute(ConstFactor* arg1, ConstFactor* arg2, DeclarationSequence* resultDecl) const {
        assert(false && "Incorect call of constant function"); 
    }
    virtual void debugOut(size_t tabcnt = 0) const {

    }
};

ConstProcedure* CreateConstABS();
ConstProcedure* CreateConstODD();
ConstProcedure* CreateConstLSL();
ConstProcedure* CreateConstASR();
ConstProcedure* CreateConstROR();
ConstProcedure* CreateConstFLOOR();
ConstProcedure* CreateConstFLT();
ConstProcedure* CreateConstORD();
ConstProcedure* CreateConstCHR();

class ConstFactor: public Context {
    friend ConstProcedure;
public:
    ConstFactor(long long val, DeclarationSequence* prevDecl);
    ConstFactor(double val, DeclarationSequence* prevDecl);
    ConstFactor(bool val, DeclarationSequence* prevDecl);
    ConstFactor(std::string val, DeclarationSequence* prevDecl);
    ConstFactor(unsigned val, DeclarationSequence* prevDecl);
    ConstFactor(NIL val, DeclarationSequence* prevDecl);
    ConstFactor(char val, DeclarationSequence* prevDecl);
    ConstFactor(ConstProcedure* val, DeclarationSequence* prevDecl);
    ConstFactor(ConstFactor* factor);

    // Вывод отладочной информации
    void debugOut(size_t tabcnt = 0) override;

    void generate(class Generator* generator, std::stringstream& cur, const std::string& name) override;

    // Применение унарного оператора
    void applyUnaryOperator(UnaryOperator op);

    // Применение relation
    void applyRelation(Relation rel, ConstFactor* factor);

    // Применение бинарного оператора
    void applyBinaryOperator(BinaryOperator op, ConstFactor* factor);

    // Возвращает целую константну, если она действительно целая
    long long getIntValue() const;

    // Возвращает set константну, если она действительно set
    unsigned getSetValue() const;

    // Получить тип результата
    TypeContext* getResultType();

    TypeContext* getType() override;

    // Вызывает константную функцию
    ConstFactor* execute(std::vector<ConstFactor*> args, DeclarationSequence* ds) const;

    std::variant<long long, double, bool, std::string, unsigned, NIL, ConstProcedure*> getValue() const {
        return value;
    }
 
private:
    std::variant<long long, double, bool, std::string, unsigned, NIL, ConstProcedure*> value; 
    DeclarationSequence* prevDeclaration;
    TypeContext* resultType;
};

class ConstDeclaration {
    friend class GeneratorC;
public:
    ConstDeclaration(const std::string& name, ConstFactor* value, bool access)
        : name(name)
        , value(value)
        , access(access)
    {
    }
    // Добавление именованного артефакта
    // void AddNamedArtefact(const std::string& name, Context* context, bool access);

    void setName(const std::string& newName) {name = newName;}
    void setContext(ConstFactor* factor) {value = factor;}
    void setAccess(bool newAccess) {access = newAccess;}

    std::string getName() const {return name;}
    ConstFactor* getFactor() const {return value;}
    bool getAccess() const {return access;}
    void generate(class Generator* gen, std::stringstream& hcode, std::stringstream& ccode);
    void generate(class Generator* gen, std::stringstream& cur);

private:
    std::string name;
    ConstFactor* value;
    bool access;
    // DeclarationSequence* prevDeclaration;
};

class DeclarationSequence {
    friend class GeneratorC;
public:
    DeclarationSequence(
        DeclarationSequence* prev = nullptr,
        std::unordered_map<std::string, NamedArtefact*>* reserved = nullptr,
        std::unordered_map<std::string, DeclarationSequence*>* import = nullptr
    ): prevDeclaration(prev), notInitCnt(0), reservedNamedArtefacts(nullptr), importArtefacts(nullptr) {
        if (prev) {
            if (!reserved) {
                reservedNamedArtefacts = prev->reservedNamedArtefacts;
            } else {
                reservedNamedArtefacts = reserved;
            }
            if (!import) {
                importArtefacts = prev->importArtefacts;
            } else {
                importArtefacts = import;
            }
        }
    }
    
    std::vector<NamedArtefact*> getNamedArtefacts() const {
        std::vector<NamedArtefact*> result;
        result.reserve(namedArtefacts.size());
        for (auto kv : namedArtefacts) {
            result.push_back(kv.second[0]);
        }
        return result;
    }

    std::vector<ConstDeclaration*> getConstNamedArtefacts() const {
        std::vector<ConstDeclaration*> result;
        result.reserve(constNamedArtefacts.size());
        for (auto kv : constNamedArtefacts) {
            result.push_back(kv.second);
        }
        return result;
    }

    void addNamedArtefact(NamedArtefact* art, bool checkExists = true) {
        if (!art) {
            assert(false && "Empty artefact");
        }
        if (reservedNamedArtefacts && reservedNamedArtefacts->count(art->getName())) {
            assert(false && "This name is reserved");
        }
        if (checkExists && (namedArtefacts.count(art->getName()) || constNamedArtefacts.count(art->getName()))) {
            assert(false && "Var with this name already exists");
        }
        namedArtefacts[art->getName()].push_back(art);
        order.push_back(art);
    }

    void addConstNamedArtefact(ConstDeclaration* cd) {
        if (!cd) {
            assert(false && "Empty artefact");
        }
        if (reservedNamedArtefacts && reservedNamedArtefacts->count(cd->getName())) {
            assert(false && "This name is reserved");
        }
        if (namedArtefacts.count(cd->getName()) || constNamedArtefacts.count(cd->getName())) {
            assert(false && "Const var with this name already exists");
        }
        constNamedArtefacts[cd->getName()] = cd;
    }

    void addHideArtefact(const std::string& s) {
        hideAertefacts.insert(s);
    }

    // Ищет константную переменную по имени в импорте
    DeclarationSequence* getDeclarationSequenceFromImport(const std::string& ident);

    // Возвращает константную переменную по имени
    ConstFactor* getConstFactorByName(const std::string& ident);

    // Возвращает NamedArtefact по имени
    NamedArtefact* getArtefactByName(const std::string& ident);

    // Возвращает NamedArtefact по имени только из текущего скоупа
    NamedArtefact* getCurrentArtefactByName(const std::string& ident);

    // Увеличивает число неинициализированных объектов
    void incNotInit() {
        notInitCnt++;
    }

    // Уменьшает число неинициализированных объектов
    void decNotInit() {
        notInitCnt--;
    }

    uint32_t getNotInit() const {
        return notInitCnt;
    }

    void debugOut(size_t tabcnt = 0);

private:
    std::unordered_map<std::string, std::vector<NamedArtefact*>> namedArtefacts;
    std::unordered_set<std::string> hideAertefacts;
    std::unordered_map<std::string, ConstDeclaration*> constNamedArtefacts;
    std::unordered_map<std::string, NamedArtefact*>* reservedNamedArtefacts;
    std::unordered_map<std::string, DeclarationSequence*>* importArtefacts;
    std::vector<NamedArtefact*> order;
    DeclarationSequence* prevDeclaration;
    uint32_t notInitCnt;
};

#endif // DECLARATION_H