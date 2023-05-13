#include "saveload.h"
#include <cstddef>
#include <iostream>
#include <fstream>
#include <new>
#include <sstream>
#include <string>

void ParseConst(DeclarationSequence* declaration, std::string line, DeclarationSequence* base) {
    std::stringstream ss(line);
    std::string type;
    ss >> type;
    char c;
    while (ss >> c && c == '{') {
        std::string name;
        ss >> name;
        ConstDeclaration* cd;
        ConstFactor* factor;
        size_t id;
        ss >> id;
        if (id == 0) {
            long long value; ss >> value;
            factor = new ConstFactor(value, base);
        } else if (id == 1) {
            double value; ss >> value;
            factor = new ConstFactor(value, base);
        } else if (id == 2) {
            bool value; ss >> value;
            factor = new ConstFactor(value, base);
        } else if (id == 3) {
            std::string value; ss >> value;
            factor = new ConstFactor(value, base);
        } else if (id == 4) {
            unsigned value; ss >> value;
            factor = new ConstFactor(value, base);
        } else if (id == 5) {
            NIL value;
            factor = new ConstFactor(value, base);
        }
        cd = new ConstDeclaration(name, factor, false);
        declaration->addConstNamedArtefact(cd);
        ss >> c;
    }
}

enum TYPE : std::size_t {
    BOOLEAN = 0,
    INTEGER = 1,
    REAL = 2,
    CHAR = 3,
    BYTE = 4,
    STRING = 5,
    NIL = 6,
    SET = 7,
    RECORD = 8,
    POINTER = 9,
    ARRAY = 10,
    PROCEDURE = 11,
};

void SkipSpace(std::stringstream& def) {
    char c;
    while (!def.eof() && def.peek() == ' ') {
        def >> std::noskipws >> c;
    }
}

VarContext* ImportVar(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def) {
    SkipSpace(def);
    char c;
    if (def.peek() == '}') {
        def >> c;
        return nullptr;
    }
    def >> c; // "{"
    std::string name;
    def >> name;
    Context* ctx = ImportType(declaration, base, def);
    VarContext* var = new VarContext(dynamic_cast<TypeContext*>(ctx));
    var->setIsImported(true);
    NamedArtefact* art = new NamedArtefact(name, var, false);
    var->setNamedArtefact(art);
    SkipSpace(def);
    def >> c; // "}"
    return var;
}

// {parent} {record_declaretion}
TypeRecordContext* ImportRecord(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def) {
    SkipSpace(def);
    if (def.peek() != '{') {
        if (def.peek() == '}') {
            return nullptr;
        }
        std::string name;
        def >> name;
        if (declaration->getArtefactByName(name) == nullptr) {
            TypeRecordContext* rec = new TypeRecordContext(nullptr, false);
            rec->setIsImported(true);
            NamedArtefact* art = new NamedArtefact(name, rec, false);
            rec->setNamedArtefact(art);
            return rec;
        }
        return dynamic_cast<TypeRecordContext*>(declaration->getArtefactByName(name)->getContext());
    }
    char c;
    def >> c; // "{"
    TypeRecordContext* parent = ImportRecord(declaration, base, def);
    def >> std::noskipws >> c >> c >> c; // "} {";
    TypeRecordContext* rec = new TypeRecordContext(parent);
    rec->setIsImported(true);
    DeclarationSequence* ds = rec->getDeclaration();
    SkipSpace(def);
    while (def.peek() != '}') {
        VarContext* var = ImportVar(declaration, base, def);
        ds->addNamedArtefact(var->getNamedArtefact());
        rec->incTypeSize(var->getType()->getTypeSize());
        SkipSpace(def);
    }
    def >> c; // "}"
    return rec;
}

// {record}
TypePointerContext* ImportPointer(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def) {
    SkipSpace(def);
    if (def.peek() != '{') {
        if (def.peek() == '}') {
            return nullptr;
        }
        std::string name;
        def >> name;
        return dynamic_cast<TypePointerContext*>(declaration->getArtefactByName(name)->getContext());
    }
    char c;
    def >> c; // "{"
    TypeRecordContext* rec = ImportRecord(declaration, base, def);
    TypePointerContext* p = new TypePointerContext(rec);
    p->setIsImported(true);
    def >> c; // "}";
    return p;
}

// {len1 len2 {type}}
TypeArrayContext* ImportArray(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def) {
    SkipSpace(def);
    if (def.peek() != '{') {
        if (def.peek() == '}') {
            return nullptr;
        }
        std::string name;
        def >> name;
        return dynamic_cast<TypeArrayContext*>(declaration->getArtefactByName(name)->getContext());
    }
    std::vector<size_t> lens;
    size_t sz = 0;
    char c;
    def >> c; // "{"
    int cnt = 0;
    SkipSpace(def);
    while (def.peek() != '{') {
        def >> sz;
        lens.push_back(sz);
        cnt++;
        SkipSpace(def);
    }
    def >> c; // "{"
    TypeContext* ctx = dynamic_cast<TypeContext*>(ImportType(declaration, base, def));
    TypeArrayContext* type = nullptr;
    type = new TypeArrayContext(lens.back(), ctx);
    for (int i = lens.size() - 2; i >= 0; --i) {
        type = new TypeArrayContext(lens[i], type);
    }
    def >> c >> c; // "}}"
    return type;
}

// { is_var var1 is_var var2: result_type}
ProcContext* ImportProcedure(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def) {
    SkipSpace(def);
    if (def.peek() != '{') {
        if (def.peek() == '}') {
            return nullptr;
        }
        std::string name;
        def >> name;
        return dynamic_cast<ProcContext*>(declaration->getArtefactByName(name)->getContext());
    }
    char c;
    def >> c; // "{"
    ProcContext* proc = new ProcContext(base);
    FormalParameters* fp = new FormalParameters();
    SkipSpace(def);
    while (def.peek() != ':') {
        bool isVar = false;
        def >> isVar;
        FPSection* section = new FPSection();
        section->setIsVar(isVar);
        def >> c; // " {";
        std::string name;
        def >> name;
        section->addParameter(name);
        Context* ctx = ImportType(declaration, base, def);
        def >> c; // "}";
        section->setType(dynamic_cast<TypeContext*>(ctx));
        fp->addFPSection(section);
        SkipSpace(def);
    }
    proc->setFormalParameters(fp);
    def >> c; // ":"
    Context* result = ImportType(declaration, base, def);
    proc->setResultType(dynamic_cast<TypeContext*>(result));
    fp->setResultType(dynamic_cast<TypeContext*>(result));
    def >> c; // "}";
    return proc;
}

Context* ImportType(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def, std::string name) {
    SkipSpace(def);
    if (def.peek() == '}') {
        return nullptr;
    }
    TYPE type;
    size_t typeId;
    def >> typeId;
    type = static_cast<TYPE>(typeId);
    Context* ctx = nullptr;
    bool createdContext = false;
    if (type == TYPE::BOOLEAN) {
        ctx = base->getArtefactByName("BOOLEAN")->getContext();
    } else if (type == TYPE::INTEGER) {
        ctx = base->getArtefactByName("INTEGER")->getContext();
    } else if (type == TYPE::REAL) {
        ctx = base->getArtefactByName("REAL")->getContext();
    } else if (type == TYPE::CHAR) {
        ctx = base->getArtefactByName("CHAR")->getContext();
    } else if (type == TYPE::BYTE) {
        ctx = base->getArtefactByName("BYTE")->getContext();
    } else if (type == TYPE::STRING) {
        size_t len;
        def >> len;
        ctx = new TypeStringContext(len);
        ctx->setIsImported(true);
    } else if (type == TYPE::NIL) {
        ctx = base->getArtefactByName("NIL")->getContext();
    } else if (type == TYPE::SET) {
        ctx = base->getArtefactByName("SET")->getContext();
    } else if (type == TYPE::RECORD) {
        ctx = ImportRecord(declaration, base, def);
        createdContext = true;
    } else if (type == TYPE::POINTER) {
        if (!name.empty()) {
            TypePointerContext* p = new TypePointerContext();
            p->setIsImported(true);
            NamedArtefact* art = new NamedArtefact(name, p, false);
            p->setNamedArtefact(art);
            declaration->addNamedArtefact(art);
            TypePointerContext* newP = ImportPointer(declaration, base, def);
            (*p) = (*newP);
            p->setNamedArtefact(art);
            delete newP;
            ctx = p;
        } else {
            ctx = ImportPointer(declaration, base, def);
        }
        createdContext = true;
    } else if (type == TYPE::ARRAY) {
        ctx = ImportArray(declaration, base, def);
        createdContext = true;
    } else if (type == TYPE::PROCEDURE) {
        ctx = ImportProcedure(declaration, base, def);
        createdContext = true;
    }
    if (!name.empty() && type != TYPE::POINTER) {
        NamedArtefact* oldArt = declaration->getArtefactByName(name);
        if (type == TYPE::RECORD && oldArt != nullptr) {
            TypeRecordContext* oldRec = dynamic_cast<TypeRecordContext*>(oldArt->getContext());
            TypeRecordContext* rec = dynamic_cast<TypeRecordContext*>(ctx);
            (*oldRec) = (*rec);
            oldRec->setNamedArtefact(oldArt);
            oldRec->setInit(true);
            delete rec;
            return oldRec;
        }
        NamedArtefact* art = new NamedArtefact(name, ctx, false);
        if (createdContext) {
            ctx->setNamedArtefact(art);
        }
        declaration->addNamedArtefact(art);
    }
    return ctx;
}

DeclarationSequence* ImportModule(std::string def, DeclarationSequence* base) {
    DeclarationSequence* declaration = new DeclarationSequence();
    std::stringstream ss(def);
    std::string line;
    while (std::getline(ss, line)) {
        std::string type = line.substr(0, line.find(":"));
        if (type == "CONST") {
            ParseConst(declaration, line, base);
        } else if (type == "TYPE") {
            std::stringstream ss(line);
            while (ss.tellg() != line.length()) {
                char c;
                while (c != '{') {
                    ss >> c;
                }
                std::string name;
                ss >> name;
                ImportType(declaration, base, ss, name);
                while (c != '}') {
                    ss >> c;
                }
            }
        } else if (type == "VAR") {
            std::stringstream ss(line);
            ss >> type;
            while (ss.tellg() != line.length()) {
                VarContext* var = ImportVar(declaration, base, ss);
                declaration->addNamedArtefact(var->getNamedArtefact());
            }
        } else if (type == "PROCEDURE") {
            std::stringstream ss(line);
            while (ss.tellg() != line.length()) {
                char c;
                while (c != '{') {
                    ss >> c;
                }
                std::string name;
                ss >> name;
                ProcContext* pr = ImportProcedure(declaration, base, ss);
                NamedArtefact* art = new NamedArtefact(name, pr, false);
                pr->setNamedArtefact(art);
                declaration->addNamedArtefact(art); 
                while (c != '}') {
                    ss >> c;
                }
            }
        }
    }
    return declaration;
}

DeclarationSequence* ImportModuleFromFile(std::filesystem::path path, DeclarationSequence* base) {
    std::ifstream def(path);
    if(def.fail()) {
        assert(false && "Imported file was not founded");
        return nullptr;
    }
    std::stringstream ss;
    ss << def.rdbuf();
    return ImportModule(ss.str(), base);
}

// {name type}
void ExportVar(DeclarationSequence* declaration, std::string name, VarContext* var, std::stringstream& def) {
    def << "{" << name << " ";
    ExportType(declaration, var->getType(), def);
    def << "}";
}

// {parent} {record_declaretion}
void ExportRecord(DeclarationSequence* declaration, TypeRecordContext* rec, std::stringstream& def, bool firstExp) {
    if (!firstExp && rec->getNamedArtefact() != nullptr && rec->getNamedArtefact()->getAccess()) {
        def << rec->getNamedArtefact()->getName();
        return;
    }
    def << "{";
    TypeRecordContext* parent = rec->getParent();
    if (parent != nullptr) {
        if (parent->getNamedArtefact() != nullptr && parent->getNamedArtefact()->getAccess()) {
            def << parent->getNamedArtefact()->getName() << " ";
        } else {
            ExportRecord(declaration, parent, def);
        }
    }
    def << "} {";
    bool isFirst = true;
    for (auto* art : rec->getDeclaration()->getNamedArtefacts()) {
        if (!art->getAccess()) {
            continue;
        }
        Context* ctx = art->getContext();
        if (ctx->getTypeName() == "VarContext") {
            if (!isFirst) {
                def << " ";
            } else {
                isFirst = false;
            }
            VarContext* var = dynamic_cast<VarContext*>(art->getContext());
            ExportVar(declaration, art->getName(), var, def);
        }
    }
    def << "}";
}

// {record}
void ExportPointer(DeclarationSequence* declaration, TypePointerContext* p, std::stringstream& def, bool firstExp) {
    if (!firstExp && p->getNamedArtefact() != nullptr && p->getNamedArtefact()->getAccess()) {
        def << p->getNamedArtefact()->getName() << " ";
        return;
    }
    def << "{";
    ExportRecord(declaration, p->getRecord(), def);
    def << "}";
}

// {len1 len2 {type}}
void ExportArray(DeclarationSequence* declaration, TypeArrayContext* arr, std::stringstream& def, bool firstExp) {
    if (!firstExp && arr->getNamedArtefact() != nullptr && arr->getNamedArtefact()->getAccess()) {
        def << arr->getNamedArtefact()->getName() << " ";
        return;
    }
    def << "{" << arr->getLenght();
    TypeContext* valueType = arr->getElementType();
    while (valueType->getTypeName() == "TypeArrayContext") {
        TypeArrayContext* valueArr = dynamic_cast<TypeArrayContext*>(valueType);
        def << " " << valueArr->getLenght();
        valueType = valueArr->getElementType();
    }
    def << " {";
    ExportType(declaration, valueType, def);
    def << "}}";
}

// { is_var var1 is_var var2: result_type}
void ExportProcedure(DeclarationSequence* declaration, ProcContext* proc, std::stringstream& def, bool firstExp) {
    if (!firstExp && proc->getNamedArtefact() != nullptr && proc->getNamedArtefact()->getAccess()) {
        def << proc->getNamedArtefact()->getName() << " ";
        return;
    }
    def << "{";
    bool isFirst = true;
    for (auto* section : proc->getFormalParameters()->getSections()) {
        for (auto& name : section->getParameters()) {
            if (!isFirst) {
                def << " ";
            } else {
                isFirst = false;
            }
            def << section->getIsVar();
            def << " {" << name << " ";
            ExportType(declaration, section->getType(), def);
            def << "}";
        }
    }
    def << ": ";
    if (proc->getResultType() != nullptr) {
        ExportType(declaration, proc->getResultType(), def);
    }
    def << "}";
}

void ExportType(DeclarationSequence* declaration, Context* ctx, std::stringstream& def, bool firstExp) {
    if (ctx->getTypeName() == "TypeBoolContext") {
        def << TYPE::BOOLEAN;
    } else if (ctx->getTypeName() == "TypeIntegerContext") {
        def << TYPE::INTEGER;
    } else if (ctx->getTypeName() == "TypeRealContext") {
        def << TYPE::REAL;
    } else if (ctx->getTypeName() == "TypeCharContext") {
        def << TYPE::CHAR;
    } else if (ctx->getTypeName() == "TypeByteContext") {
        def << TYPE::BYTE;
    } else if (ctx->getTypeName() == "TypeStringContext") {
        def << TYPE::STRING << " ";
        TypeStringContext* str = dynamic_cast<TypeStringContext*>(ctx);
        def << str->getLenght();
    } else if (ctx->getTypeName() == "TypeNilContext") {
        def << TYPE::NIL;
    } else if (ctx->getTypeName() == "TypeSetContext") {
        def << TYPE::SET;
    } else if (ctx->getTypeName() == "TypeRecordContext") {
        def << TYPE::RECORD << " ";
        ExportRecord(declaration, dynamic_cast<TypeRecordContext*>(ctx), def, firstExp);
    } else if (ctx->getTypeName() == "TypePointerContext") {
        def << TYPE::POINTER << " ";
        ExportPointer(declaration, dynamic_cast<TypePointerContext*>(ctx), def, firstExp);
    } else if (ctx->getTypeName() == "TypeArrayContext") {
        def << TYPE::ARRAY << " ";
        ExportArray(declaration, dynamic_cast<TypeArrayContext*>(ctx), def, firstExp);
    } else if (ctx->getTypeName() == "ProcContext") {
        def << TYPE::PROCEDURE << " ";
        ExportProcedure(declaration, dynamic_cast<ProcContext*>(ctx), def, firstExp);
    }
}

std::string ExportModule(DeclarationSequence* declaration) {
    std::stringstream def;
    def << "CONST:";
    for (auto& p : declaration->constNamedArtefacts) {
        if (!p.second->getAccess()) {
            continue;
        }
        auto value = p.second->getFactor()->getValue();
        def << " {" << p.first << " " << value.index() << " ";
        switch (value.index()) {
            case 0: def << std::get<0>(value); break;
            case 1: def << std::get<1>(value); break;
            case 2: def << std::get<2>(value); break;
            case 3: def << std::get<3>(value); break;
            case 4: def << std::get<4>(value); break;
            case 6: def << std::get<6>(value); break;
            default: break;
        }
        def << "}";
    }
    def << "\n";
    def << "TYPE:";
    for (auto* art : declaration->order) {
        if (!art->getAccess()) {
            continue;
        }
        Context* ctx = art->getContext();
        if (ctx->getTypeName() != "VarContext" && ctx->getTypeName() != "Procedure") {
            def << " {" << art->getName() << " ";
            ExportType(declaration, ctx, def, true);
            def << "}";
        }
    }
    def << "\n";
    def << "VAR:";
    for (auto* art : declaration->order) {
        if (!art->getAccess()) {
            continue;
        }
        Context* ctx = art->getContext();
        if (ctx->getTypeName() == "VarContext") {
            def << " ";
            VarContext* var = dynamic_cast<VarContext*>(art->getContext());
            ExportVar(declaration, art->getName(), var, def);
        }
    }
    def << "\n";
    def << "PROCEDURE:";
    for (auto* art : declaration->order) {
        if (!art->getAccess()) {
            continue;
        }
        Context* ctx = art->getContext();
        if (ctx->getTypeName() == "Procedure") {
            def << " {" << art->getName() << " ";
            ProcContext* proc = dynamic_cast<ProcContext*>(art->getContext());
            ExportProcedure(declaration, proc, def, true);
            def << "}";
        }
    }
    return def.str();
}

void ExportModuleToFile(DeclarationSequence* declaration, std::filesystem::path path) {
    std::ofstream def;
    def.open(path);
    def << ExportModule(declaration);
    def.close();
}
