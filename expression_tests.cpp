#include "catch.hpp"

#include "expression.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
  REQUIRE(!exp.isHeadComplex());
  REQUIRE(!exp.isHeadList());
  REQUIRE(!exp.isHeadLambda());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);

  REQUIRE(exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
  REQUIRE(!exp.isHeadComplex());
  REQUIRE(!exp.isHeadList());
  REQUIRE(!exp.isHeadLambda());
}


TEST_CASE( "Test symbol expression", "[expression]" ) {

  Expression exp(Atom(std::string("asymbol")));

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(exp.isHeadSymbol());
  REQUIRE(!exp.isHeadComplex());
  REQUIRE(!exp.isHeadList());
  REQUIRE(!exp.isHeadLambda());
}


TEST_CASE("Test complex expression", "[expression]") {

	Expression exp(Atom(std::complex<double>(1.0, 1.0)));

	REQUIRE(!exp.isHeadNumber());
	REQUIRE(!exp.isHeadSymbol());
	REQUIRE(exp.isHeadComplex());
	REQUIRE(!exp.isHeadList());
	REQUIRE(!exp.isHeadLambda());
}

TEST_CASE("Test list expression", "[expression]") {

	Expression exp(Atom(true));

	REQUIRE(!exp.isHeadNumber());
	REQUIRE(!exp.isHeadSymbol());
	REQUIRE(!exp.isHeadComplex());
	REQUIRE(exp.isHeadList());
	REQUIRE(!exp.isHeadLambda());
}



TEST_CASE("Test lambda expression", "[expression]") {

	Expression exp1(1.0);
	Expression exp2(2.0);
	std::vector<Expression> myVector;
	myVector.emplace_back(exp1);
	myVector.emplace_back(exp2);
	Expression myExp(myVector);

	REQUIRE(!myExp.isHeadNumber());
	REQUIRE(!myExp.isHeadSymbol());
	REQUIRE(!myExp.isHeadComplex());
	REQUIRE(!myExp.isHeadList());
	REQUIRE(myExp.isHeadLambda());
}