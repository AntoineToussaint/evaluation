#ifndef EVALUATION_H
#define EVALUATION_H

#include <string>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <functional>
#include <vector>
#include <map>

class EvalNode {
    double d_cachedValue = nan("");
    public:
    using Ptr = std::shared_ptr<EvalNode>;
    double calc() {
        if (std::isnan(d_cachedValue) || needCalc()) {
            auto value = eval();
            d_cachedValue = value;
            return value;
        } else {
            std::cout << "Using cache" << std::endl;
            return d_cachedValue;
        }
    }
    virtual double eval() = 0;
    virtual bool needCalc() { return true; }
    virtual ~EvalNode();
};

class ExpressionNode : public EvalNode {
    EvalNode::Ptr d_expression;
    std::string d_name;
    public:
    using Ptr = std::shared_ptr<ExpressionNode>;
    virtual double eval() {
        return d_expression->eval();
    }
    ExpressionNode(const std::string &name, const EvalNode::Ptr &expression)
        : d_expression(expression), d_name(name) {
      std::cout << "Expression created: " << d_name << std::endl;
    }
    virtual bool needCalc() { return d_expression->needCalc(); }
    
};

class ConstantNode : public EvalNode {
    double d_value;
    public:
    virtual double eval() {
        return d_value;
    }
    //! Constant node.
    /*!
      Right now, values are double only but takes anything that cast to a double.
    */
    template<class T>
    ConstantNode(const T& value) : d_value(static_cast<double>(value)) {
      std::cout << "Constant created: " << d_value << std::endl;
    }
    virtual bool needCalc() { return false; }
};

class VariableNode: public EvalNode {
    bool d_needCalc = true;
    double d_value = nan("");
    std::string d_name;
    public:
    using Ptr = std::shared_ptr<VariableNode>;
    virtual double eval() {
        if (std::isnan(d_value)) {
            throw std::runtime_error("Variable not set");
        } else {
            return d_value;
        }
    };
    VariableNode(const std::string& name) : d_name(name) {
      std::cout << "Variable created: " << d_name << std::endl;
    }
    void set(double value) {
        d_value = value;
        d_needCalc = true;
    }
    void cache() {
        d_needCalc = false;
    }
    virtual bool needCalc() { return d_needCalc; }
};

class UnaryOperatorNode : public EvalNode {
    public:
    using Function = std::function<double(double)>;
    private:
    EvalNode::Ptr d_node;
    Function d_function;
    public:
    virtual double eval() {
        return d_function(d_node->eval());   
    };
    UnaryOperatorNode(const EvalNode::Ptr &node, const Function &function)
        : d_node(node), d_function(function) {
      std::cout << "UnaryOperatorNode created: " << std::endl;
    }
    virtual bool needCalc() { return d_node->needCalc(); }
};

class BinaryOperatorNode : public EvalNode {
    public:
    using Function = std::function<double(double, double)>;
    private:
    EvalNode::Ptr d_leftNode, d_rightNode;
    Function d_function;
    public:
    virtual double eval() {
        return d_function(d_leftNode->eval(), d_rightNode->eval());   
    };
    BinaryOperatorNode(const EvalNode::Ptr& leftNode, const EvalNode::Ptr& rightNode, const Function& function) :
        d_leftNode(leftNode), d_rightNode(rightNode), d_function(function) {
      std::cout << "BinaryOperatorNode created: " << std::endl;
    }
    virtual bool needCalc() { return d_leftNode->needCalc() || d_rightNode->needCalc(); }
};

class EvaluationContext {
    private:
    using ExpressionMap = std::map<std::string, ExpressionNode::Ptr>;
    using VariableMap= std::map<std::string, VariableNode::Ptr>;
    ExpressionMap d_expressionMap;
    VariableMap d_variableMap;
    // This is a collection of expressions
    // The order of evaluation matters
    std::vector<EvalNode::Ptr> d_expressions;
    public:
    // We need
    bool isKnownExpression(const std::string& name) {
        return d_expressionMap.find(name) != d_expressionMap.end();
    }
    bool isKnownVariable(const std::string& name) {
        return d_variableMap.find(name) != d_variableMap.end();
    }
    EvalNode::Ptr getExpression(const std::string& name) {
        return d_expressionMap[name];
    }
    EvalNode::Ptr getVariable(const std::string& name) {
        return d_variableMap[name];
    }
    void addExpression(const std::string& name, const ExpressionNode::Ptr& expression) {
        d_expressionMap[name] = expression;
        d_expressions.push_back(expression);
    }
    void addVariable(const std::string& name, const VariableNode::Ptr& variable) {
        d_variableMap[name] = variable;
    }
    
    //! Set a variable to a given value when it exists.
    /*!
      Doesn't do anything if variable isn't known to context.
    */
    void setVariable(const std::string& name, double value);
    
    
    double calc(const std::string& expression_name) {
        for(auto& expression : d_expressions) {
            expression->calc();
        }
        for(auto& kv : d_variableMap) {
            kv.second->cache();
        }
        if (d_expressionMap.find(expression_name) == d_expressionMap.end())
            throw std::runtime_error("Not found");
        return d_expressionMap[expression_name]->calc();
    }
    
    
};

#endif
