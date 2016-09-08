#define BOOST_TEST_MODULE EvaluationTests
#include <boost/test/unit_test.hpp>

#include "evaluation.h"

BOOST_AUTO_TEST_CASE(ConstantNodeDoubleTest)
{
    /*
    auto v = 3.;
    auto c = ConstantNode{v};
    BOOST_TEST(v == c.eval());
    */
}

BOOST_AUTO_TEST_CASE(ConstantNodeIntTest)
{
    /*
    auto v = 3;
    auto c = ConstantNode{v};
    BOOST_TEST(v == c.eval());
    */
}

BOOST_AUTO_TEST_CASE(VariableNodeTest)
{
}

BOOST_AUTO_TEST_CASE(UnaryOperatorNodeTest)
{
    /*
    auto node = std::make_shared<ConstantNode>(3);
    auto op = [](auto x) { return -x; };
    auto unary_node = UnaryOperatorNode(node, op);
    BOOST_TEST(unary_node.eval() == -3.);
    */
}
