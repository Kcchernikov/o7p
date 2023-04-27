#include "generator_c.h"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

void GeneratorC::GenerateModule(const Module& module, std::stringstream& hcode, std::stringstream& ccode) {
    // Некоторым типам нужна инициализация. Например массивам и структурам. В декларации модуля 
    // инициализация будет происходить в функции "InitModule", в декларации структуры производить
    // инициализацию невозможно, поэтому она будет произведена, либо в декларации модуля, либо в декларации
    // функции
    std::string uname = module.moduleName + "_H";
    std::string lname = module.moduleName;
    std::transform(uname.begin(), uname.end(), uname.begin(), ::toupper);
    std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);
    hcode << "#ifndef " << uname << "\n#define "<< uname << "\n\n";
    hcode << "#include \"baselib.h\"\n\n";
    ccode << "#include \"" << lname << ".h\"\n\n";
    // Генерируем экспортируемые переменные в hcode, а остальное в ccode
    GenerateModuleDeclaration(module.declaration, hcode, ccode);
    hcode << "void InitModule" << module.moduleName << "();\n";
    ccode << "void InitModule" << module.moduleName << "() {\n";
    tabcnt++;
    GenerateInitialisation(module.declaration, ccode);
    if (module.statement) {
        GenerateStatement(*module.statement, ccode);
    }
    tabcnt--;
    ccode << "}\n\n";
    hcode << "\n#endif // " << uname << "\n";
}

void GeneratorC::GenerateModuleDeclaration(const DeclarationSequence& declaration, std::stringstream& hcode, std::stringstream& ccode) {
    if (declaration.importArtefacts) {
        for (auto kv : *declaration.importArtefacts) {
            hcode << "#include \"" << kv.first << "\"\n";
        }
        hcode << "\n";
    }
    for (auto kv : declaration.constNamedArtefacts) {
        kv.second->generate(this, hcode, ccode);
    }
    for (auto art : declaration.order) {
        std::string name = art->getName();
        Context* ctx = art->getContext();
        std::stringstream cur;
        Procedure* p = ctx->getTypeName() == "Procedure" ? dynamic_cast<Procedure*>(ctx) : nullptr;
        if (p) {
            if (art->getAccess()) {
                GenerateProcedureHeading(*p, hcode, name);
                hcode << ";\n";
            }
            p->generate(this, ccode, name);
            ccode << "\n";
        } else {
            VarContext* var = (ctx->getTypeName() == "VarContext" 
                ? dynamic_cast<VarContext*>(ctx)
                : nullptr
            );
            if (var == nullptr) {
                cur << "typedef ";
            }
            ctx->generate(this, cur, name);
            if (art->getAccess()) {
                hcode << cur.str() << ";\n";
            } else {
                ccode << cur.str() << ";\n";
            }
            if (var && var->getType()->getTypeName() == "TypeArrayContext") {
                std::vector<size_t> lengths;
                GenerateArrayBaseRef(*dynamic_cast<TypeArrayContext*>(var->getType()), ccode, name, lengths);
            }
        }
    }
}

void GeneratorC::GenerateDeclaration(const DeclarationSequence& declaration, std::stringstream& cur, bool needInit) {
    for (auto kv : declaration.constNamedArtefacts) {
        // Убирается дублирование с переменными из объявления функции
        if (declaration.hideAertefacts.count(kv.first) == 0) {
            GenerateTabs(cur);
            kv.second->generate(this, cur);
        }
    }
    for (auto art : declaration.order) {
        std::string name = art->getName();
        // Убирается дублирование с переменными из объявления функции
        if (declaration.hideAertefacts.count(name) == 0) {
            GenerateTabs(cur);
            Context* ctx = art->getContext();
            VarContext* var = (ctx->getTypeName() == "VarContext" 
                ? dynamic_cast<VarContext*>(ctx)
                : nullptr
            );
            if (var == nullptr) {
                cur << "typedef ";
            }
            ctx->generate(this, cur, name);
            if (!dynamic_cast<Procedure*>(ctx)) {
                cur << ";\n";
            }
            if (var && var->getType()->getTypeName() == "TypeArrayContext") {
                std::vector<size_t> lengths;
                GenerateArrayBaseRef(*dynamic_cast<TypeArrayContext*>(var->getType()), cur, name, lengths);
            }
        }
    }
    if (needInit) {
        GenerateInitialisation(declaration, cur);
    }
}

void GeneratorC::GenerateConstDeclaration(const ConstDeclaration& declaration, std::stringstream& hcode, std::stringstream& ccode) {
    std::stringstream cur;
    cur << "const ";
    declaration.value->getType()->generate(this, cur, declaration.name);
    cur << " = ";
    declaration.value->generate(this, cur, "");
    cur << ";\n";
    if (declaration.access) {
        hcode << cur.str();
    } else {
        ccode << cur.str();
    }
}

void GeneratorC::GenerateConstDeclaration(const ConstDeclaration& declaration, std::stringstream& cur) {
    cur << "const ";
    declaration.value->getType()->generate(this, cur, declaration.name);
    cur << " = ";
    declaration.value->generate(this, cur, "");
    cur << ";\n";
}

void GeneratorC::GenerateTypeBool(const TypeBoolContext& type, std::stringstream& cur, const std::string& name) {
    cur << "short int"; // bool
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateTypeInteger(const TypeIntegerContext& type, std::stringstream& cur, const std::string& name) {
    cur << "long long";
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateTypeReal(const TypeRealContext& type, std::stringstream& cur, const std::string& name) {
    cur << "float";
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateTypeChar(const TypeCharContext& type, std::stringstream& cur, const std::string& name) {
    cur << "char";
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateTypeByte(const TypeByteContext& type, std::stringstream& cur, const std::string& name) {
    cur << "unsigned char";
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateTypeString(const TypeStringContext& type, std::stringstream& cur, const std::string& name) {
    cur << "char*";
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateTypeNil(const TypeNilContext& type, std::stringstream& cur, const std::string& name) {
    // Возможно стоит создать структуру NIL, хотя в нормальной программе нет смысла создавать такой тип
}

void GeneratorC::GenerateTypeSet(const TypeSetContext& type, std::stringstream& cur, const std::string& name) {
    cur << "unsigned int";
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateTypeRecord(const TypeRecordContext& type, std::stringstream& cur, const std::string& name) {
    // Для уже определенных типов генерируем их тип как имя
    std::string artName = name;
    if (type.getNamedArtefact()) {
        artName = type.getNamedArtefact()->getName();
        if (!artName.empty() && artName != name && tmpRecords.count(artName) == 0) {
            cur << artName;
            if (!name.empty()) {
                cur << " " << name;
            }
            return;
        }
    }
    cur << "struct";
    if (artName != "") {
        cur << " " << artName;
    }
    cur << " {\n";
    tabcnt++;
    GenerateTabs(cur); cur << "char* __record_type;\n";
    GenerateRecordElements(type, cur);
    tabcnt--;
    GenerateTabs(cur); cur << "}";
    if (name != "") {
        cur << " " << name;
        isRecordDeclarated[name] = true;
    }
}

void GeneratorC::GenerateRecordElements(const TypeRecordContext& type, std::stringstream& cur) {
    if (type.parentContext) {
        GenerateRecordElements(*type.parentContext, cur);
    }
    GenerateDeclaration(type.declaration, cur, false);
}

void GeneratorC::GenerateTypePointer(const TypePointerContext& type, std::stringstream& cur, const std::string& name) {
    // Чтобы обеспечить предекларацию, структура, на которую указывает текущий указатель
    // при отсутсвии своего имени получает временное имя
    std::string pointerName;
    if (type.getNamedArtefact()) {
        pointerName = type.getNamedArtefact()->getName();
    }
    if (!pointerName.empty() && isParticalDeclaratedPointer[pointerName]) {
        cur << "struct rec_" << pointerName << "*";
        if (name != "") {
            cur << " " << name;
        }
        return;
    }
    if (!pointerName.empty()) {
        isParticalDeclaratedPointer[name] = true;
    }
    if (type.recordType->getNamedArtefact()) {
        std::string recName = type.recordType->getNamedArtefact()->getName();
        if (!isRecordDeclarated[recName]) {
            cur << "struct ";
        }
        cur << recName;
    } else {
        if (!pointerName.empty()) {
            NamedArtefact na("rec_" + pointerName, type.recordType);
            tmpRecords.insert(na.getName());
            type.recordType->setNamedArtefact(&na);
            type.recordType->generate(this, cur, "");
            type.recordType->setNamedArtefact(nullptr);
            tmpRecords.erase(na.getName());
        } else {
            type.recordType->generate(this, cur, "");
        }
    }
    cur << "*";
    if (name != "") {
        cur << " " << name;
    }
    if (!pointerName.empty()) {
        isParticalDeclaratedPointer[name] = false;
    }
}

void GeneratorC::GenerateTypeArray(const TypeArrayContext& type, std::stringstream& cur, const std::string& name) {
    cur << "struct";
    cur << " {\n";
    tabcnt++;
    GenerateTabs(cur); cur << "size_t lenght;\n";
    if (type.valueType->getNamedArtefact()) {
        GenerateTabs(cur); cur << type.valueType->getNamedArtefact()->getName();
    } else {
        GenerateTabs(cur); type.valueType->generate(this, cur, "");
    }
    cur << "* value" << ";\n";
    // GenerateTabs(cur); cur << "unsigned lenght = " << type.length << ";\n";
    tabcnt--;
    GenerateTabs(cur); cur << "}";
    if (name != "") {
        cur << " " << name;
    }
}

void GeneratorC::GenerateConstFactor(const ConstFactor& ctx, std::stringstream& cur, const std::string&) {
    const auto value = ctx.getValue();
    switch (value.index()) {
        case 0: cur << std::get<0>(value); break;
        case 1: cur << std::get<1>(value); break;
        case 2: cur << (std::get<2>(value) ? "1" : "0"); break;
        case 3: cur << std::get<3>(value); break;
        case 4: cur << std::get<4>(value); break;
        case 5: cur << "NULL"; break;
        case 6: assert(false && "Const procedure is not avaliable in code generation");
    }
}

void GeneratorC::GenerateProcedureContext(const ProcContext &ctx, std::stringstream &cur, const std::string &name) {
    if (ctx.heading && ctx.heading->getResultType()) {
        ctx.heading->getResultType()->generate(this, cur, "");
    } else {
        cur << "void";
    }
    cur << " (*" << name << ")(";
    if (!ctx.heading) {
        cur << ")";
        return;
    }
    bool start = true;
    for (const auto& param : ctx.heading->parameters) {
        for (const auto& name : param->parameters) {
            if (!start) {
                cur << ", ";
            } else {
                start = false;
            }
            param->type->generate(this, cur, "");
            if (param->getIsVar()) {
                cur << "* __ptr_" << name;
            } else {
                cur << " "  << name;
            }
        }
    }
    cur << ")";
}

void GeneratorC::GenerateProcedureHeading(const Procedure& ctx, std::stringstream& cur, const std::string& name) {
    if (ctx.heading && ctx.heading->getResultType()) {
        ctx.heading->getResultType()->generate(this, cur, "");
    } else {
        cur << "void";
    }
    cur << " " << name << "(";
    if (!ctx.getFormalParameters()) {
        cur << ")";
        return;
    }
    bool start = true;
    for (const auto& param : ctx.getFormalParameters()->parameters) {
        for (const auto& name : param->parameters) {
            if (!start) {
                cur << ", ";
            } else {
                start = false;
            }
            if (param->type->getTypeName() == "TypeArrayContext") {
                if (!param->getIsVar()) {
                    cur << "const ";
                }
                cur << "BASE_ARRAY* __base_ptr_" << name;
            } else {
                param->type->generate(this, cur, "");
                if (param->getIsVar()) {
                    cur << "* __ptr_" << name;
                } else {
                    cur << " " << name;
                }
            }
        }
    }
    cur << ")";
}

void GeneratorC::GenerateProcedureBody(const Procedure& ctx, std::stringstream& cur, const std::string& name) {
    // Везде в фнукции подменяем name на *__ptr_name, а
    // потом возвращаем прежнее имя
    cur << " {\n";
    tabcnt++;
    GenerateDeclaration(*ctx.declaration, cur, true);
    for (auto* art : ctx.refs) {
        std::cout << art->getName() << " " << art->getContext()->getTypeName() << std::endl;
        VarContext* var = (art->getContext()->getTypeName() == "VarContext"
            ? dynamic_cast<VarContext*>(art->getContext())
            : nullptr
        );
        if (var == nullptr) {
            std::cout << "NULL VAR" << std::endl;
            continue;
        }
        if (var->getType()->getTypeName() == "TypeArrayContext") {
            TypeArrayContext* arr = dynamic_cast<TypeArrayContext*>(var->getType());
            GenerateTabs(cur);
            arr->generate(this, cur, "");
            cur << "* __ptr_" << art->getName() << " = (void *) __base_ptr_" << art->getName() << ";\n";
        }
        art->setName("(*__ptr_" + art->getName() + ")");
    }
    GenerateStatement(*ctx.body.statement, cur);
    for (auto* art : ctx.refs) {
        std::string name = art->getName();
        art->setName(std::string(name.begin() + 8, --name.end()));
    }
    tabcnt--;
    GenerateTabs(cur);
    cur << "}\n";
}

void GeneratorC::GenerateStatement(const StatementSequence& statements, std::stringstream& cur) {
    for (const auto& statement : statements.statements) {
        GenerateTabs(cur);
        statement->generate(this, cur);
    }
}

void GeneratorC::GenerateAssigment(const Assigment& statement, std::stringstream& cur) {
    GenerateDesignator(*statement.designator, cur);
    cur << " = ";
    if (statement.designator->type->getTypeName() == "TypePointerContext") {
        cur << "(";
        TypePointerContext* p = dynamic_cast<TypePointerContext*>(statement.designator->type);
        GenerateTypePointer(*p, cur, "");
        cur << ") ";
    }
    GenerateExpression(*statement.expression, cur);
    cur << ";\n";
}

void GeneratorC::GenerateProcedureCall(const ProcedureCall& statement, std::stringstream& cur) {
    // Переменные передаются по указателю, а остальное по значению
    GenerateDesignator(*statement.designator, cur);
    cur << "(";
    std::string type = statement.designator->getType()->getTypeName();
    ProcContext* proc = (type == "ProcContext" || type == "Procedure"
        ? dynamic_cast<ProcContext*>(statement.designator->getType())
        : nullptr
    );
    if (!proc) {
        assert(!false && "Calling of empty procedure is not avaliable");
    }
    if (statement.params.empty()) {
        cur << ");\n";
        return;
    }
    bool start = true;
    size_t i = 0;
    auto formalParams = proc->heading->parameters;
    size_t cnt_p = formalParams[i]->parameters.size();
    bool isVar = formalParams[i]->getIsVar();
    for (auto p : statement.params) {
        if (!start) {
            cur << ", ";
        } else {
            start = false;
        }
        if (cnt_p == 0) {
            i++;
            cnt_p = formalParams[i]->parameters.size();
            isVar = formalParams[i]->getIsVar();
        }
        if (formalParams[i]->getType()->getTypeName() == "TypeArrayContext") {
            cur << "(BASE_ARRAY*) ";
            isVar = true;
        }
        if (isVar) {
            cur << "&";
        }
        GenerateExpression(*p, cur);
        cnt_p--;
    }
    cur << ");\n";
}

void GeneratorC::GenerateIf(const IfStatement& statement, std::stringstream& cur) {
    cur << "if (";
    GenerateExpression(*statement.main.first, cur);
    cur << ") {\n";
    tabcnt++;
    GenerateStatement(*statement.main.second, cur);
    tabcnt--;
    for (auto elsif : statement.elsif) {
        GenerateTabs(cur); cur << "} else if (";
        GenerateExpression(*elsif.first, cur);
        cur << ") {\n";
        tabcnt++;
        GenerateStatement(*elsif.second, cur);
        tabcnt--;
    }
    if (statement.els) {
        GenerateTabs(cur); cur << "} else {\n";
        tabcnt++;
        GenerateStatement(*statement.els, cur);
        tabcnt--;
    }
    GenerateTabs(cur); cur << "}\n";
}

void GeneratorC::GenerateCase(const CaseStatement& statement, std::stringstream& cur) {
    // TODO Generate Case
}

void GeneratorC::GenerateWhile(const WhileStatement& statement, std::stringstream& cur) {
    cur << "// While loop\n";
    size_t startMark = markId++;
    cur << "_" << startMark << ":\n";
    GenerateTabs(cur); cur << "if (";
    GenerateExpression(*statement.main.first, cur);
    cur << ") {\n";
    tabcnt++;
    GenerateStatement(*statement.main.second, cur);
    GenerateTabs(cur); cur << "goto _" << startMark << ";\n";
    tabcnt--;
    for (auto elsif : statement.elsif) {
        GenerateTabs(cur); cur << "} else if (";
        GenerateExpression(*elsif.first, cur);
        cur << ") {\n";
        tabcnt++;
        GenerateStatement(*elsif.second, cur);
        GenerateTabs(cur); cur << "goto _" << startMark << ";\n";
        tabcnt--;
    }
    GenerateTabs(cur); cur << "}\n";
}

void GeneratorC::GenerateRepeat(const RepeatStatement& statement, std::stringstream& cur) {
    cur << "// Repeat loop\n";
    size_t startMark = markId++;
    cur << "_" << startMark << ":\n";
    GenerateStatement(*statement.stSeq, cur);
    GenerateTabs(cur); cur << "if (!(";
    GenerateExpression(*statement.expression, cur);
    cur << ")) {\n";
    tabcnt++;
    GenerateTabs(cur); cur << "goto _" << startMark << ";\n";
    tabcnt--;
    GenerateTabs(cur); cur << "}\n";
}

void GeneratorC::GenerateFor(const ForStatement& statement, std::stringstream& cur) {
    cur << "// For loop\n";
    size_t startMark = markId++;
    if (statement.start) {
        GenerateTabs(cur);
        cur << statement.ident->getNamedArtefact()->getName() << " = ";
        GenerateExpression(*statement.start, cur);
        cur << ";\n";
    }
    cur << "_" << startMark << ":\n";
    GenerateTabs(cur); cur << "if (";
    if (statement.by->getIntValue() > 0)  {
        cur << statement.ident->getNamedArtefact()->getName() << " <= ";
    } else {
        cur << statement.ident->getNamedArtefact()->getName() << " <= ";
    }
    GenerateExpression(*statement.stop, cur);
    cur << ") {\n";
    tabcnt++;
    GenerateStatement(*statement.stSeq, cur);
    GenerateTabs(cur); cur << statement.ident->getNamedArtefact()->getName() << " += ";
    GenerateConstFactor(*statement.by, cur, "");
    cur << ";\n";

    GenerateTabs(cur); cur << "goto _" << startMark << ";\n";
    tabcnt--;
    GenerateTabs(cur); cur << "}\n";
}

void GeneratorC::GenerateDesignator(const Designator& designator, std::stringstream& cur) {
    if (designator.qualident.idents.size() == 1 && designator.qualident.varArtefact) {
        cur << designator.qualident.varArtefact->getName();
    } else {
        cur << designator.qualident.idents[0];
    }
    for (size_t i = 1; i < designator.qualident.idents.size(); ++i) {
        cur << "." << designator.qualident.idents[i];
    }
    std::vector<std::string> assertTypes;
    std::vector<bool> isPointer;
    for (const auto selector : designator.selectors) {
        AssertSelector* as = (selector->getTypeName() == "AssertSelector"
            ? dynamic_cast<AssertSelector*>(selector)
            : nullptr
        );
        if (as) {
            if (as->isAlwaysTrue) {
                continue;
            }
            TypePointerContext* ptr = (as->type->getTypeName() == "TypePointerContext" 
                ? dynamic_cast<TypePointerContext*>(as->type)
                : nullptr
            );
            if (ptr) {
                assertTypes.push_back(ptr->recordType->getNamedArtefact()->getName());
                isPointer.push_back(true);
            } else {
                assertTypes.push_back(as->type->getNamedArtefact()->getName());
                isPointer.push_back(false);
            }
        }
    }
    if (!assertTypes.empty()) {
        for (size_t i = assertTypes.size() - 1; i >= 0; --i) {
            if (!isPointer[i]) {
                cur << "(*(";
            }
            cur << "((" << assertTypes[i] << "*) ConvertPtr((BASE_REC*)(";
        }
    }

    for (const auto selector : designator.selectors) {
        selector->generate(this, cur);
    }
}

void GeneratorC::GenerateRecordSelector(const RecordSelector& selector, std::stringstream& cur) {
    if (selector.type->getTypeName() == "TypePointerContext") {
        cur << "->";
    } else {
        cur << ".";
    }
    cur << selector.ident;
}

void GeneratorC::GenerateIndexSelector(const IndexSelector& selector, std::stringstream& cur) {
    for (const auto* exp : selector.expList) {
        cur << ".value[";
        GenerateExpression(*exp, cur);
        cur << "]";
    }
}

void GeneratorC::GenerateAssertSelector(const AssertSelector& selector, std::stringstream& cur) {
    if (selector.isAlwaysTrue) {
        return;
    }
    TypePointerContext* ptr = (selector.type->getTypeName() == "TypePointerContext"
        ? dynamic_cast<TypePointerContext*>(selector.type)
        : nullptr
    );
    if (ptr) {
        cur << "), \"";
        cur << ptr->recordType->getNamedArtefact()->getName() << "\"))";
    } else {
        // TODO возможно стоит сделать также для Record;
    }
}

void GeneratorC::GeneratePointerSelector(const PointerSelector& selector, std::stringstream& cur) {
    cur << "*";
}

void GeneratorC::GenerateExpression(const Expression& expression, std::stringstream& cur) {
    if (expression.secondSimpleExpression != std::nullopt) {
        Relation rel = expression.secondSimpleExpression->first;
        if (rel == Relation::SET_MEMBERSHIP) {
            cur << "(1ll << ";
            GenerateSimpleExpression(*expression.firstSimpleExpression, cur);
            cur << ")";
        } else {
            if (rel == Relation::TYPE_TEST) {
                if (expression.firstSimpleExpression->resultType->getTypeName() == "TypePointerContext") {
                    GenerateSimpleExpression(*expression.firstSimpleExpression, cur);
                    cur << "->__record_type == ";
                    GenerateSimpleExpression(*expression.secondSimpleExpression->second, cur);
                    cur << "->__record_type";
                } else if (expression.firstSimpleExpression->resultType->getTypeName() == "TypeRecordContext") {
                    GenerateSimpleExpression(*expression.firstSimpleExpression, cur);
                    cur << ".__record_type == ";
                    GenerateSimpleExpression(*expression.secondSimpleExpression->second, cur);
                    cur << ".__record_type";
                } else {
                    cur << "1";
                }
                return;
            } else {
                GenerateSimpleExpression(*expression.firstSimpleExpression, cur);
            }
        }
        switch (rel) {
            case Relation::EQUAL : cur << " == "; break;
            case Relation::GREATER_OR_EQUAL : cur << " >= "; break;
            case Relation::LESS_OR_EQUAL : cur << " <= "; break;
            case Relation::GREATER : cur << " > "; break;
            case Relation::LESS : cur << " < "; break;
            case Relation::UNEQUAL : cur << " != "; break;
            case Relation::SET_MEMBERSHIP : cur << " & "; break;
            case Relation::TYPE_TEST : break;
        }
        GenerateSimpleExpression(*expression.secondSimpleExpression->second, cur);
    } else {
        GenerateSimpleExpression(*expression.firstSimpleExpression, cur);
    }
}

void GeneratorC::GenerateSimpleExpression(const SimpleExpression& expression, std::stringstream& cur) {
    // Операции для SET переопределяются явно, либо с помощью функции set_difference
    TypeSetContext s;
    if (expression.startTerm->getResultType()->getTypeName() == "TypeSetContext") {
        if (expression.unaryOperator == UnaryOperator::UN_MINUS) {
            cur << "~";
        }
    } else {
        switch (expression.unaryOperator) {
            case UnaryOperator::UN_MINUS: cur << "-"; break;
            case UnaryOperator::UN_PLUS: cur << "+"; break;
            case UnaryOperator::NEGATION: cur << "!"; break;
            default: break;
        } 
    }
    size_t cntSetDifference = 0;
    for (const auto p : expression.terms) {
        if (p.first == BinaryOperator::MINUS && p.second->resultType->getTypeName() == "TypeSetContext") {
            cntSetDifference++;
        }
    }
    for (size_t i = 0; i < cntSetDifference; ++i) {
        cur << "set_difference(";
    }
    GenerateTerm(*expression.startTerm, cur);
    for (const auto p : expression.terms) {
        bool setOp = false;
        if (p.second->resultType->getTypeName() == "TypeSetContext") {
            switch (p.first) {
                case BinaryOperator::PLUS: cur << " | "; break;
                case BinaryOperator::MINUS: cur << ", "; setOp = true; break;
                default: break;
            }
        } else {
            switch (p.first) {
                case BinaryOperator::PLUS: cur << " + "; break;
                case BinaryOperator::MINUS: cur << " - "; break;
                case BinaryOperator::OR: cur << " || "; break;
                default: break;
            }
        }
        GenerateTerm(*p.second, cur);
        if (setOp) {
            cur << ")";
        }
    }
}

void GeneratorC::GenerateTerm(const Term& term, std::stringstream& cur) {
    GenerateFactor(*term.startFactor, cur);
    for (auto factor : term.factors) {
        if (factor.second->resultType->getTypeName() == "TypeSetContext") {
            switch (factor.first) {
                case BinaryOperator::PRODUCT: cur << " & "; break;
                case BinaryOperator::QUOTIENT: cur << " ^ "; break;
                default: break;
            }
        } else {
            switch (factor.first) {
                case BinaryOperator::PRODUCT: cur << " * "; break;
                case BinaryOperator::QUOTIENT: cur << " / (float) "; break;
                case BinaryOperator::INTEGER_QUOTIENT: cur << " / "; break;
                case BinaryOperator::MODULUS: cur << " % "; break;
                case BinaryOperator::LOGICAL_CONJUNCTION: cur << " && "; break;
                default: break;
            }
        }
        GenerateFactor(*factor.second, cur);
    }
}

void GeneratorC::GenerateFactor(const Factor& factor, std::stringstream& cur) {
    switch (factor.value.index()) {
        case 0: GenerateConstFactor(*std::get<0>(factor.value), cur, ""); break;
        case 1: GenerateSet(*std::get<1>(factor.value), cur); break;
        case 2: GenerateDesignatorWrapper(*std::get<2>(factor.value), cur); break;
        case 3: cur << "("; GenerateExpression(*std::get<3>(factor.value), cur); cur << ")"; break;
        case 4: cur << "!"; GenerateFactor(*std::get<4>(factor.value), cur); break;
    }
}

void GeneratorC::GenerateSet(const Set& st, std::stringstream& cur) {
    cur << "(unsigned long long) (";
    size_t leftCnt = st.elements.size();
    for (auto el : st.elements) {
        // Генерация set через битовые операции
        if (el.second) {
            cur << "((1ll << ("; GenerateExpression(*el.second, cur); cur << " + 1)) - 1";
            cur << " - ";
            cur << "((1ll << "; GenerateExpression(*el.first, cur); cur << ") - 1)";
        } else {
            cur << "(1ll << ";
            GenerateExpression(*el.first, cur);
        }
        cur << ")";
        leftCnt--;
        if (leftCnt != 0) {
            cur << "|";
        }
    }
    cur << ")";
}

void GeneratorC::GenerateDesignatorWrapper(const DesignatorWrapper& st, std::stringstream& cur) {
    GenerateDesignator(*st.designator, cur);
    if (st.params) {
        cur << "(";
        bool start = true;
        for (auto p : st.params->params) {
            if (!start) {
                cur << ", ";
            } else {
                start = false;
            }
            if (p->resultType->getTypeName() == "TypeArrayContext") {
                cur << "(BASE_ARRAY*) ";
            }
            if (p->getIsVar()) {
                cur << "&";
            }
            GenerateExpression(*p, cur);
        }
        cur << ")";
    }
}

void GeneratorC::GenerateInitialisation(const DeclarationSequence& declaration, std::stringstream& cur) {
    for (auto art : declaration.order) {
        std::string name = art->getName();
        // Убирается дублирование с переменными из объявления функции
        if (declaration.hideAertefacts.count(name) == 0) {
            Context* ctx = art->getContext();
            VarContext* var = ctx->getTypeName() == "VarContext" ? dynamic_cast<VarContext*>(ctx) : nullptr;
            if (var) {
                TypeRecordContext* rec = (var->context->getTypeName() == "TypeRecordContext" 
                    ? dynamic_cast<TypeRecordContext*>(var->context)
                    : nullptr
                );
                if (rec) {
                    InitRecord(*rec, cur, name);
                } else if (var->context->getTypeName() == "TypeArrayContext") {
                    TypeArrayContext* arr = dynamic_cast<TypeArrayContext*>(var->context);
                    std::vector<size_t> id;
                    InitArray(*arr, cur, name, name, id);
                }
            }
        }
    }
}

void GeneratorC::GenerateArrayBaseRef(
    const TypeArrayContext& arr,
    std::stringstream& cur,
    const std::string& name,
    std::vector<size_t>& lenghts
) {
    TypeContext* valueType = arr.valueType;
    GenerateTabs(cur);
    valueType->generate(this, cur, name);
    cur << "_base_ref" << lenghts.size();
    lenghts.push_back(arr.length);
    for (size_t i : lenghts) {
        cur << "[" << i << "]";
    }
    cur << ";\n";
    if (valueType->getTypeName() == "TypeArrayContext") {
        TypeArrayContext* valueArr = dynamic_cast<TypeArrayContext*>(valueType);
        GenerateArrayBaseRef(*valueArr, cur, name, lenghts);
    }
}

void GeneratorC::InitArray(const TypeArrayContext& arr, std::stringstream& cur, const std::string& name, const std::string& base_name, std::vector<size_t>& indexes) {
    GenerateTabs(cur); cur << name << ".lenght = " << arr.length << ";\n";
    GenerateTabs(cur); cur << name << ".value = (void*) " << base_name << "_base_ref" << indexes.size();
    for (size_t id : indexes) {
        cur << "[i" << id << "]";
    }
    cur << ";\n";
    TypeContext* valueType = arr.valueType;
    size_t curId = 0;
    if (!indexes.empty()) {
        curId = indexes.back() + 1;
    }
    indexes.push_back(curId);
    std::stringstream ss;
    ss << ".value[i" << curId << "]";
    std::string new_name = name + ss.str(); 
    TypeRecordContext* rec = (valueType->getTypeName() == "TypeRecordContext" 
        ? dynamic_cast<TypeRecordContext*>(valueType)
        : nullptr
    );
    TypeArrayContext* inArr = (valueType->getTypeName() == "TypeArrayContext"
        ? dynamic_cast<TypeArrayContext*>(valueType)
        : nullptr
    );
    if (rec || inArr) {
        GenerateTabs(cur); cur << "for (size_t i" << curId << " = 0; i" << curId << " < " << arr.length << "; i" << curId << " += 1) {\n";
        tabcnt++;
        if (inArr) {
            InitArray(*inArr, cur, new_name, base_name, indexes);
        } else {
            InitRecord(*rec, cur, new_name);
        }
        tabcnt--;
        GenerateTabs(cur); cur << "}\n";
    }
}

void GeneratorC::InitRecord(const TypeRecordContext& record, std::stringstream& cur, const std::string& name) {
    // Из-за существования массивов внутри струтуры, инициализация не рекурсивная
    GenerateTabs(cur); cur << name << ".__record_type = \"" << record.getNamedArtefact()->getName() << "\";\n";
    DeclarationSequence declaration = record.declaration;
    for (auto art : declaration.order) {
        std::string insightName = art->getName();
        // Убирается дублирование с переменными из объявления функции
        if (declaration.hideAertefacts.count(name) == 0) {
            Context* ctx = art->getContext();
            VarContext* var = ctx->getTypeName() == "VarContext" ? dynamic_cast<VarContext*>(ctx) : nullptr;
            if (var) {
                TypeRecordContext* rec = (var->context->getTypeName() == "TypeRecordContext"
                    ? dynamic_cast<TypeRecordContext*>(var->context)
                    : nullptr
                );
                if (rec) {
                    InitRecord(*rec, cur, name + "." + insightName);
                } else if (var->context->getTypeName() == "TypeArrayContext") {
                    TypeArrayContext* arr = dynamic_cast<TypeArrayContext*>(var->context);
                    std::vector<size_t> id;
                    InitArray(*arr, cur, name + "." + insightName, name + "." + insightName, id);
                }
            }
        }
    }
}

void GeneratorC::GenerateTabs(std::stringstream& cur) {
    for (size_t i = 0; i < tabcnt; ++i) {
        for (size_t j = 0; j < tabsz; ++j) {
            cur << " ";
        }
    }
}
