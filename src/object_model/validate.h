#ifndef VALIDATE_H
#define VALIDATE_H

#include "../smodel/type.h"
#include "designator.h"

class Expression;

bool IsComparable(TypeContext* formalType, TypeContext* actualType);

TypeContext* ValidateParameters(Designator* des, const std::vector<Expression*>& params);

#endif // VALIDATE_H
