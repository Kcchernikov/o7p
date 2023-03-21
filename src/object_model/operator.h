#ifndef OPERATOR_H
#define OPERATOR_H

enum Relation {
    EQUAL,
    UNEQUAL,
    LESS,
    LESS_OR_EQUAL,
    GREATER,
    GREATER_OR_EQUAL,
    SET_MEMBERSHIP,
    TYPE_TEST,
};

enum UnaryOperator {
    UN_PLUS,
    UN_MINUS,
    NEGATION,
    NONE,
};

enum BinaryOperator {
    PLUS,
    MINUS,
    OR,
    PRODUCT,
    QUOTIENT,
    INTEGER_QUOTIENT,
    MODULUS,
    LOGICAL_CONJUNCTION,
};


#endif // OPERATOR_H