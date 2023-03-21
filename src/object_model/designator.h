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
    bool isVariable;
    bool isConstant;
};

class Selector {
public:
    virtual void debugOut(size_t tabcnt = 0);
    TypeContext* getType();
protected:
    TypeContext* type;
};

// selector .ident
class RecordSelector: public Selector {
public:
    RecordSelector(const std::string& id, TypeRecordContext* prev);
    std::string getIdent();
    void debugOut(size_t tabcnt = 0) override;
private:
    std::string ident;
};

// selector [ ExpList ]
class IndexSelector: public Selector {
public:
    IndexSelector(const std::vector<Expression*>& exps, TypeContext* prev);
    std::vector<Expression*> getExpressionList();
    void debugOut(size_t tabcnt = 0) override;
private:
    std::vector<Expression*> expList;
};

// selector (qualident)
class AssertSelector: public Selector {
public:
    AssertSelector(const Qualident& qual, TypeContext* prev);
    void debugOut(size_t tabcnt = 0) override;
private:
    std::vector<std::string> idents;
};

// selector ^
class PoinerSelector: public Selector {
public:
    PoinerSelector(TypeContext* prev);
    void debugOut(size_t tabcnt);
};

class Designator {
public:
    void debugOut(size_t tabcnt = 0);
    TypeContext* getType();
    bool getIsVar();
    void addQualident(Qualident qual);
    void addRecordSelector(std::string ident);
    void addIndexSelector(const std::vector<Expression*> exps);
    void addAssertSelector(const Qualident& qual);
    void addPointerSelector();
private:
    Qualident qualident;
    std::vector<Selector*> selectors;
    TypeContext* type;
    bool isVar = true;
};


#endif // DESIGNATOR_H
