#ifndef DESIGNATOR_H
#define DESIGNATOR_H

#include "../smodel/type.h"
#include "expression.h"

#include <cstddef>
#include <string>
#include <vector>

struct Qualident {
    std::vector<std::string> idents;
    TypeContext* type;
    NamedArtefact* varArtefact = nullptr;
    // Адресс переменной первого ident, заполняется только, если есть второй ident
    VarContext* firstVar = nullptr;
    bool isVariable;
    bool isConstant;
    // Является ли первый ident указателем
    bool isFirstPointer;
};

class Selector {
    friend class GeneratorC;
public:
    virtual void debugOut(size_t tabcnt = 0);
    virtual void generate(Generator* generator, std::stringstream& cur) = 0;
    TypeContext* getType();
    virtual std::string getTypeName() const {
        return "Selector";
    }
    
protected:
    TypeContext* type;
};

// selector .ident
class RecordSelector: public Selector {
    friend class GeneratorC;
public:
    RecordSelector(const std::string& id, TypeRecordContext* prev);
    std::string getIdent();
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
    std::string getTypeName() const override {
        return "RecordSelector";
    }
private:
    std::string ident;
};

// selector [ ExpList ]
class IndexSelector: public Selector {
    friend class GeneratorC;
public:
    IndexSelector(const std::vector<Expression*>& exps, TypeContext* prev);
    std::vector<Expression*> getExpressionList();
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
    std::string getTypeName() const override {
        return "IndexSelector";
    }
private:
    std::vector<Expression*> expList;
};

// selector (qualident)
class AssertSelector: public Selector {
    friend class GeneratorC;
public:
    AssertSelector(const Qualident& qual, TypeContext* prev);
    void debugOut(size_t tabcnt = 0) override;
    void generate(Generator* generator, std::stringstream& cur) override;
    std::string getTypeName() const override {
        return "AssertSelector";
    }
private:
    Qualident qualident;
    TypeContext* prevType;
    bool isAlwaysTrue = false;
};

// selector ^
class PointerSelector: public Selector {
    friend class GeneratorC;
public:
    PointerSelector(TypeContext* prev);
    void debugOut(size_t tabcnt) override;
    void generate(Generator* generator, std::stringstream& cur) override;
    std::string getTypeName() const override {
        return "PointerSelector";
    }
};

class Designator {
    friend class GeneratorC;
public:
    void debugOut(size_t tabcnt = 0);
    TypeContext* getType();
    VarContext* getVar();
    bool getIsVar();
    void addQualident(Qualident qual);
    void addRecordSelector(std::string ident);
    void addIndexSelector(const std::vector<Expression*> exps);
    void addAssertSelector(const Qualident& qual);
    void addPointerSelector();
    Qualident getQualident() const;
private:
    Qualident qualident;
    std::vector<Selector*> selectors;
    TypeContext* type;
    bool isVar = true;
    VarContext* var;
};


#endif // DESIGNATOR_H
