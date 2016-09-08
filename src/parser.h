#ifndef PARSER_H
#define PARSER_H

#include "evaluation.h"

class EvaluationParser {
   public:
    static UnaryOperatorNode::Function GetUnaryFunction(
        const std::string& name);
    static BinaryOperatorNode::Function GetBinaryFunction(
        const std::string& name);
    static EvaluationContext CreateFromFile(const std::string& fname);
};

#endif
