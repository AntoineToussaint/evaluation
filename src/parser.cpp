#include "parser.h"
#include <iostream>
#include <map>
#include <memory>

#include "evaluation.h"

#include "pugixml.hpp"

UnaryOperatorNode::Function
EvaluationParser::GetUnaryFunction(const std::string &name) {
  if (name == std::string("!"))
    return [](auto x) { return x; }; // factorial TODO
  if (name == std::string("-"))
    return [](auto x) { return -x; };
  if (name == std::string("cos"))
    return [](auto x) { return cos(x); };
  if (name == std::string("sin"))
    return [](auto x) { return sin(x); };
  if (name == std::string("exp"))
    return [](auto x) { return exp(x); };
  if (name == std::string("log"))
    return [](auto x) { return log(x); };
  throw std::runtime_error(std::string("Unknown unary function: ") + name);
}

BinaryOperatorNode::Function
EvaluationParser::GetBinaryFunction(const std::string &name) {
  if (name == std::string("+"))
    return [](auto x, auto y) { return x + y; };
  if (name == std::string("-"))
    return [](auto x, auto y) { return x - y; };
  if (name == std::string("*"))
    return [](auto x, auto y) { return x * y; };
  if (name == std::string("/"))
    return [](auto x, auto y) { return x / y; };
  if (name == std::string("max"))
    return [](auto x, auto y) { return std::max(x,y); };
  if (name == std::string("min"))
    return [](auto x, auto y) { return std::min(x, y); };
  if (name == std::string("^"))
    return [](auto x, auto y) { return std::pow(x, y); };
  throw std::runtime_error(std::string("Unknown binary function: ") + name);
}

namespace {

EvalNode::Ptr CreateNode(const pugi::xml_node &node, EvaluationContext &context,
                         size_t level) {

  // Constants
  if (node.name() == std::string("constant")) {
    auto value = node.attribute("value").value();
    return std::make_shared<ConstantNode>(std::stod(value));
  }
  // Variable & Expressions
  if (node.name() == std::string("variable")) {
    // At level 0: these are expressions
    auto variable_name = node.attribute("value").value();
    if (level == 0) {
      auto first = *std::begin(node.children());
      auto expression = std::make_shared<ExpressionNode>(
          node.attribute("value").value(), CreateNode(first, context, ++level));
      context.addExpression(variable_name, expression);
      return expression;
    } else {
      // Check if we know the expression
      if (context.isKnownExpression(variable_name)) {
        return context.getExpression(variable_name);
        // Check if we know the variable
      } else if (context.isKnownVariable(variable_name)) {
        return context.getVariable(variable_name);
      } else {
        // else create a new variable
        auto variable = std::make_shared<VariableNode>(variable_name);
        context.addVariable(variable_name, variable);
        return variable;
      }
    }
  }
  // Unary operation
  if (node.name() == std::string("un_op")) {
    auto first = *std::begin(node.children());
    // map the right operation
    auto unary_function =
        EvaluationParser::GetUnaryFunction(node.attribute("type").value());
    return std::make_shared<UnaryOperatorNode>(
        CreateNode(first, context, ++level), unary_function);
  }
  // Binary operation
  if (node.name() == std::string("bin_op")) {
    // need the two childs
    auto children = node.children();
    auto iter = std::begin(children);
    auto left = *iter;
    auto right = *(++iter);
    auto binary_function =
        EvaluationParser::GetBinaryFunction(node.attribute("type").value());
    ++level;
    return std::make_shared<BinaryOperatorNode>(
        CreateNode(left, context, level), CreateNode(right, context, level),
        binary_function);
  }
  throw std::runtime_error(std::string("Unknown node = ") + node.name());
}

} // namespace

EvaluationContext EvaluationParser::CreateFromFile(const std::string &fname) {
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_file(fname.c_str());
  // We need to keep track of expressions and variables
  auto context = EvaluationContext{};
  for (const auto &expr_ : doc.child("root")) {
    // we should always have an expression
    auto node_name = expr_.name();
    if (node_name != std::string("variable")) {
      throw std::runtime_error(
          "Should have only expression/variable at root level");
    }
    auto level = 0;
    CreateNode(expr_, context, level);
  }
  return context;
}
