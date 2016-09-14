#define BOOST_TEST_MODULE EvaluationTests
#include <boost/test/unit_test.hpp>
#include "../src/evaluation.h"
#include "../src/parser.h"


using namespace boost::unit_test;


BOOST_AUTO_TEST_CASE(Test_0)
{
// X=3
// Y=X+1+2+z
auto context = EvaluationParser::CreateFromFile("data/test_0.xml");
context.setVariable("z", 0.5);
BOOST_CHECK(3 == context.calc("X"));
BOOST_CHECK(6.5 == context.calc("Y"));
}


BOOST_AUTO_TEST_CASE(Test_1)
{
// X=exp(y)
// Z=1*X
auto context = EvaluationParser::CreateFromFile("data/test_1.xml");
context.setVariable("y", 0.5);
BOOST_CHECK(1.6487212707001282 == context.calc("X"));
BOOST_CHECK(1.6487212707001282 == context.calc("Z"));
}

