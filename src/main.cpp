#include <iostream>
#include "evaluation.h"
#include "parser.h"

int main() {
    auto context = EvaluationParser::CreateFromFile("sample.xml");
    context.setVariable("y", 2);
    std::cout << "Result = " << context.calc("E") << std::endl;
}
