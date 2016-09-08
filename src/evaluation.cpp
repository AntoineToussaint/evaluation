#include "evaluation.h"

EvalNode::~EvalNode() {}

void EvaluationContext::setVariable(const std::string& name, double value) {
    auto variable = d_variableMap.find(name);
    if (variable == d_variableMap.end()) {
        // should LOG something gracefully
        std::cout << "Trying to set an unknown variable: " << name << std::endl;
        return;
    }
    variable->second->set(value);
}
