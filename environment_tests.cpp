#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"
#include <iostream>
#include <cmath>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom(std::string("pi"))));
  REQUIRE(env.is_exp(Atom(std::string("pi"))));

  REQUIRE(env.is_known(Atom(std::string("e"))));
  REQUIRE(env.is_exp(Atom(std::string("e"))));

  REQUIRE(env.is_known(Atom(std::string("I"))));
  REQUIRE(env.is_exp(Atom(std::string("I"))));

  REQUIRE(!env.is_known(Atom(std::string("hi"))));
  REQUIRE(!env.is_exp(Atom(std::string("hi"))));

  REQUIRE(env.is_proc(Atom(std::string("+"))));
  REQUIRE(env.is_proc(Atom(std::string("-"))));
  REQUIRE(env.is_proc(Atom(std::string("*"))));
  REQUIRE(env.is_proc(Atom(std::string("/"))));
  REQUIRE(env.is_proc(Atom(std::string("sqrt"))));
  REQUIRE(env.is_proc(Atom(std::string("^"))));
  REQUIRE(env.is_proc(Atom(std::string("ln"))));
  REQUIRE(env.is_proc(Atom(std::string("sin"))));
  REQUIRE(env.is_proc(Atom(std::string("cos"))));
  REQUIRE(env.is_proc(Atom(std::string("tan"))));
  REQUIRE(env.is_proc(Atom(std::string("real"))));
  REQUIRE(env.is_proc(Atom(std::string("imag"))));
  REQUIRE(env.is_proc(Atom(std::string("mag"))));
  REQUIRE(env.is_proc(Atom(std::string("arg"))));
  REQUIRE(env.is_proc(Atom(std::string("conj"))));
  REQUIRE(env.is_proc(Atom(std::string("list"))));
  REQUIRE(env.is_proc(Atom(std::string("first"))));
  REQUIRE(env.is_proc(Atom(std::string("rest"))));
  REQUIRE(env.is_proc(Atom(std::string("length"))));
  REQUIRE(env.is_proc(Atom(std::string("append"))));
  REQUIRE(env.is_proc(Atom(std::string("join"))));
  REQUIRE(env.is_proc(Atom(std::string("range"))));
  REQUIRE(!env.is_proc(Atom(std::string("op"))));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom(std::string("pi"))) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom(std::string("e"))) == Expression(std::exp(1)));
  REQUIRE(env.get_exp(Atom(std::string("I"))) == Expression(std::complex<double>(0.0, 1.0)));
  REQUIRE(env.get_exp(Atom(std::string("hi"))) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom(std::string("one")), a);
  REQUIRE(env.is_known(Atom(std::string("one"))));
  REQUIRE(env.is_exp(Atom(std::string("one"))));
  REQUIRE(env.get_exp(Atom(std::string("one"))) == a);

  Expression b(Atom(std::string("hello")));
  env.add_exp(Atom(std::string("hi")), b);
  REQUIRE(env.is_known(Atom(std::string("hi"))));
  REQUIRE(env.is_exp(Atom(std::string("hi"))));
  REQUIRE(env.get_exp(Atom(std::string("hi"))) == b);
  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE("Test get built-in procedure", "[environment]") {
	Environment env;

	INFO("default procedure")
		Procedure p1 = env.get_proc(Atom("doesnotexist"));
	Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
	REQUIRE(p1 == p2);
	std::vector<Expression> args;
	REQUIRE(p1(args) == Expression());
	REQUIRE(p2(args) == Expression());

	INFO("trying add procedure")
		Procedure padd = env.get_proc(Atom(std::string("+")));
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	REQUIRE(padd(args) == Expression(3.0));
	std::complex<double>myComplexAdd(7, 4);
	args.emplace_back(myComplexAdd);
	std::complex<double>myAnswerComplexAdd(10, 4);
	REQUIRE(padd(args) == Expression(myAnswerComplexAdd));
	std::complex<double>myComplex2Add(0, 1);
	args.emplace_back(myComplex2Add);
	std::complex<double>myAnswerComplex2Add(10, 5);
	REQUIRE(padd(args) == Expression(myAnswerComplex2Add));
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(padd(args), SemanticError);

	INFO("trying mul procedure")
		args.clear();
	Procedure pmul = env.get_proc(Atom(std::string("*")));
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	REQUIRE(pmul(args) == Expression(2.0));
	args.emplace_back(5.0);
	REQUIRE(pmul(args) == Expression(10.0));
	std::complex<double>myComplexMul(0, 1);
	args.emplace_back(myComplexMul);
	std::complex<double>myAnswerComplexMul(0, 10);
	REQUIRE(pmul(args) == Expression(myAnswerComplexMul));
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pmul(args), SemanticError);

	INFO("trying subneg procedure")
		args.clear();
	Procedure psubneg = env.get_proc(Atom(std::string("-")));
	args.emplace_back(1.0);
	REQUIRE(psubneg(args) == Expression(-1.0));
	args.emplace_back(2.0);
	REQUIRE(psubneg(args) == Expression(-1.0));
	args.emplace_back(5.0);
	REQUIRE_THROWS_AS(psubneg(args), SemanticError);
	args.clear();
	std::complex<double>myComplexSubNeg(0, 1);
	args.emplace_back(myComplexSubNeg);
	std::complex<double>myAnswerComplexSubNeg(0, -1);
	REQUIRE(psubneg(args) == Expression(myAnswerComplexSubNeg));
	std::complex<double>myComplexSubNeg2(3, 5);
	args.emplace_back(myComplexSubNeg2);
	std::complex<double>myAnswerComplexSubNeg2(-3, -4);
	REQUIRE(psubneg(args) == Expression(myAnswerComplexSubNeg2));
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(psubneg(args), SemanticError);
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(psubneg(args), SemanticError);

	INFO("trying div procedure")
	args.clear();
	Procedure pdiv = env.get_proc(Atom(std::string("/")));
	args.emplace_back(10.0);
	args.emplace_back(5.0);
	REQUIRE(pdiv(args) == Expression(2.0));
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);
	args.clear();
	args.emplace_back(10.0);
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);
	args.clear();
	args.emplace_back(10.0);
	std::complex<double> myComplexDiv(0, 2);
	args.emplace_back(myComplexDiv);
	std::complex<double> myComplexAnswerDiv(0, -5);
	REQUIRE(pdiv(args) == Expression(myComplexAnswerDiv));
	args.clear();
	args.emplace_back(2.0);
	REQUIRE(pdiv(args) == Expression(0.5));
	args.clear();
	std::complex<double> myComplexDiv2(2.0, 1.0);
	args.emplace_back(myComplexDiv2);
	std::complex<double> myComplexAnswerDiv2(0.4, -0.2);
	REQUIRE(pdiv(args) == Expression(myComplexAnswerDiv2));
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);

	INFO("trying sqrt procedure")
	args.clear();
	Procedure psqrt = env.get_proc(Atom(std::string("sqrt")));
	args.emplace_back(25.0);
	REQUIRE(psqrt(args) == Expression(5.0));
	args.emplace_back(25.0);
	REQUIRE_THROWS_AS(psqrt(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(psqrt(args), SemanticError);
	args.clear();
	args.emplace_back(-25.0);
	std::complex<double>myComplexSqrtAnswer(0, 5);
	REQUIRE(psqrt(args) == Expression(myComplexSqrtAnswer));
	args.clear();
	std::complex<double>myComplexSqrt(4, 0);
	args.emplace_back(myComplexSqrt);
	std::complex<double>myComplexSqrtAnswer2(2, 0);
	REQUIRE(psqrt(args) == Expression(myComplexSqrtAnswer2));

	INFO("trying pow procedure")
	args.clear();
	Procedure ppow = env.get_proc(Atom(std::string("^")));
	args.emplace_back(5.0);
	args.emplace_back(2.0);
	REQUIRE(ppow(args) == Expression(25.0));
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(ppow(args), SemanticError);
	args.clear();
	args.emplace_back(5.0);
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(ppow(args), SemanticError);
	args.clear();
	std::complex<double>myComplexPow(0.0, 1.0);
	args.emplace_back(myComplexPow);
	std::complex<double>myComplexPow2(2.0, 0);
	args.emplace_back(myComplexPow2);
	std::complex<double>myComplexPowAnswer2(-1.0, 0);
	REQUIRE(ppow(args) == Expression(myComplexPowAnswer2));

	INFO("trying ln procedure")
	args.clear();
	Procedure pln = env.get_proc(Atom(std::string("ln")));
	args.emplace_back(env.get_exp(Atom(std::string("e"))));
	REQUIRE(pln(args) == Expression(1.0));
	args.emplace_back(env.get_exp(Atom(std::string("e"))));
	REQUIRE_THROWS_AS(pln(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pln(args), SemanticError);

	INFO("trying sine procedure")
	args.clear();
	Procedure psin = env.get_proc(Atom(std::string("sin")));
	args.emplace_back(env.get_exp(Atom(std::string("pi"))));
	REQUIRE(psin(args) == Expression(0.0));
	args.emplace_back(env.get_exp(Atom(std::string("pi"))));
	REQUIRE_THROWS_AS(psin(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(psin(args), SemanticError);

	INFO("trying cosine procedure")
	args.clear();
	Procedure pcos = env.get_proc(Atom(std::string("cos")));
	args.emplace_back(env.get_exp(Atom(std::string("pi"))));
	REQUIRE(pcos(args) == Expression(-1.0));
	args.emplace_back(env.get_exp(Atom(std::string("pi"))));
	REQUIRE_THROWS_AS(pcos(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pcos(args), SemanticError);

	INFO("trying tangent procedure")
	args.clear();
	Procedure ptan = env.get_proc(Atom(std::string("tan")));
	args.emplace_back(env.get_exp(Atom(std::string("pi"))));
	REQUIRE(ptan(args) == Expression(0.0));
	args.emplace_back(env.get_exp(Atom(std::string("pi"))));
	REQUIRE_THROWS_AS(ptan(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(ptan(args), SemanticError);

	INFO("trying real procedure")
	args.clear();
	Procedure preal = env.get_proc(Atom(std::string("real")));
	std::complex<double> myComplexReal(4.0, 5.0);
	args.emplace_back(myComplexReal);
	REQUIRE(preal(args) == Expression(4.0));
	std::complex<double> myComplexReal2(4.0, 5.0);
	args.emplace_back(myComplexReal2);
	REQUIRE_THROWS_AS(preal(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(preal(args), SemanticError);

	INFO("trying imaginary procedure")
	args.clear();
	Procedure pimag = env.get_proc(Atom(std::string("imag")));
	std::complex<double> myComplexImag(4.0, 5.0);
	args.emplace_back(myComplexImag);
	REQUIRE(pimag(args) == Expression(5.0));
	std::complex<double> myComplexImag2(4.0, 5.0);
	args.emplace_back(myComplexImag2);
	REQUIRE_THROWS_AS(pimag(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pimag(args), SemanticError);

	INFO("trying magnitude procedure")
	args.clear();
	Procedure pmag = env.get_proc(Atom(std::string("mag")));
	std::complex<double> myComplexMag(4.0, 3.0);
	args.emplace_back(myComplexMag);
	REQUIRE(pmag(args) == Expression(5.0));
	std::complex<double> myComplexMag2(4.0, 5.0);
	args.emplace_back(myComplexMag2);
	REQUIRE_THROWS_AS(pmag(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pmag(args), SemanticError);

	INFO("trying argument procedure")
	args.clear();
	Procedure parg = env.get_proc(Atom(std::string("arg")));
	std::complex<double> myComplexArg(1.0, 0.0);
	args.emplace_back(myComplexArg);
	REQUIRE(parg(args) == Expression(0.0));
	std::complex<double> myComplexArg2(4.0, 5.0);
	args.emplace_back(myComplexArg2);
	REQUIRE_THROWS_AS(parg(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(parg(args), SemanticError);

	INFO("trying conjugate procedure")
	args.clear();
	Procedure pconj = env.get_proc(Atom(std::string("conj")));
	args.emplace_back(env.get_exp(Atom(std::string("I"))));
	std::complex<double> myComplexConjAnswer(0.0, -1.0);
	REQUIRE(pconj(args) == Expression(myComplexConjAnswer));
	args.emplace_back(myComplexConjAnswer);
	REQUIRE_THROWS_AS(pconj(args), SemanticError);
	args.clear();
	args.emplace_back(std::string("hi"));
	REQUIRE_THROWS_AS(pconj(args), SemanticError);

	INFO("trying list procedure")
	args.clear();
	Procedure plist = env.get_proc(Atom(std::string("list")));
	args.emplace_back(env.get_exp(Atom(std::string("I"))));
	args.emplace_back(1.0);
	args.emplace_back(padd(args));
	std::complex<double> myComplexList(1.0, 1.0);
	std::list<Expression> myList;
	myList.push_back(Expression(env.get_exp(Atom(std::string("I")))));
	myList.push_back(Expression(1.0));
	myList.push_back(Expression(myComplexList));
	args.emplace_back(Expression(myList));
	myList.push_back(Expression(myList));
	REQUIRE(plist(args) == Expression(myList));

	INFO("trying first procedure")
	args.clear();
	Procedure pfirst = env.get_proc(Atom(std::string("first")));
	std::list<Expression> myFirst;
	REQUIRE_THROWS_AS(pfirst(args), SemanticError);
	myFirst.push_back(Expression(1.0));
	args.emplace_back(myFirst);
	REQUIRE(pfirst(args) == Expression(1.0));
	args.clear();
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pfirst(args), SemanticError);

	INFO("trying rest procedure")
	args.clear();
	Procedure prest = env.get_proc(Atom(std::string("rest")));
	args.emplace_back(env.get_exp(Atom(std::string("I"))));
	args.emplace_back(1.0);
	args.emplace_back(padd(args));
	std::complex<double> myComplexRest(1.0, 1.0);
	std::list<Expression> myRest;
	myRest.push_back(Expression(env.get_exp(Atom(std::string("I")))));
	myRest.push_back(Expression(1.0));
	myRest.push_back(Expression(myComplexRest));
	args.emplace_back(Expression(myRest));
	myRest.push_back(Expression(myRest));
	myRest.pop_front();
	std::vector<Expression> args2;
	args2.push_back(plist(args));
	REQUIRE(prest(args2) == Expression(myRest));
	args2.clear();
	REQUIRE_THROWS_AS(prest(args2), SemanticError);
	REQUIRE_THROWS_AS(prest(args), SemanticError);

	INFO("trying length procedure")
	args.clear();
	args2.clear();
	Procedure plength = env.get_proc(Atom(std::string("length")));
	args.emplace_back(env.get_exp(Atom(std::string("I"))));
	args.emplace_back(1.0);
	args.emplace_back(padd(args));
	std::complex<double> myComplexLength(1.0, 1.0);
	std::list<Expression> myLength;
	myLength.push_back(Expression(env.get_exp(Atom(std::string("I")))));
	myLength.push_back(Expression(1.0));
	myLength.push_back(Expression(myComplexLength));
	args.emplace_back(Expression(myLength));
	myLength.push_back(Expression(myLength));
	myLength.pop_front();
	args2.push_back(plist(args));
	REQUIRE(plength(args2) == Expression(4.0));
	REQUIRE_THROWS_AS(plength(args), SemanticError);

	INFO("trying append procedure")
	args.clear();
	args2.clear();
	Procedure pappend = env.get_proc(Atom(std::string("append")));
	args.emplace_back(env.get_exp(Atom(std::string("I"))));
	args.emplace_back(1.0);
	args.emplace_back(padd(args));
	std::complex<double> myComplexAppend(1.0, 1.0);
	std::list<Expression> myAppend;
	myAppend.push_back(Expression(env.get_exp(Atom(std::string("I")))));
	myAppend.push_back(Expression(1.0));
	myAppend.push_back(Expression(myComplexAppend));
	args.emplace_back(Expression(myAppend));
	myAppend.push_back(Expression(myAppend));
	args2.push_back(plist(args));
	args2.emplace_back(4.0);
	myAppend.push_back(Expression(4.0));
	REQUIRE(pappend(args2) == Expression(myAppend));
	REQUIRE_THROWS_AS(pappend(args), SemanticError);
	args2.clear();
	args2.emplace_back(4.0);
	args2.push_back(plist(args));
	REQUIRE_THROWS_AS(pappend(args2), SemanticError);

	INFO("trying join procedure")
	args.clear();
	args2.clear();
	Procedure pjoin = env.get_proc(Atom(std::string("join")));
	args.emplace_back(env.get_exp(Atom(std::string("I"))));
	args.emplace_back(1.0);
	args.emplace_back(padd(args));
	std::complex<double> myComplexJoin(1.0, 1.0);
	std::list<Expression> myJoin;
	myJoin.push_back(Expression(env.get_exp(Atom(std::string("I")))));
	myJoin.push_back(Expression(1.0));
	myJoin.push_back(Expression(myComplexJoin));
	args.emplace_back(Expression(myJoin));
	args2.push_back(plist(args));
	args2.push_back(plist(args));
	std::list<Expression> myJoin2;
	myJoin2.push_back(Expression(env.get_exp(Atom(std::string("I")))));
	myJoin2.push_back(Expression(1.0));
	myJoin2.push_back(Expression(myComplexJoin));
	myJoin2.push_back(Expression(myJoin));
	myJoin2.push_back(Expression(env.get_exp(Atom(std::string("I")))));
	myJoin2.push_back(Expression(1.0));
	myJoin2.push_back(Expression(myComplexJoin));
	myJoin2.push_back(Expression(myJoin));
	REQUIRE(pjoin(args2) == Expression(myJoin2));
	REQUIRE_THROWS_AS(pappend(args), SemanticError);
	args2.emplace_back(4.0);
	REQUIRE_THROWS_AS(pappend(args2), SemanticError);

	INFO("trying range procedure")
	args.clear();
	args2.clear();
	Procedure prange = env.get_proc(Atom(std::string("range")));
	args.emplace_back(0.0);
	args.emplace_back(5.0);
	args.emplace_back(1.0);
	std::list<Expression> myRange;
	myRange.push_back(Expression(0.0));
	myRange.push_back(Expression(1.0));
	myRange.push_back(Expression(2.0));
	myRange.push_back(Expression(3.0));
	myRange.push_back(Expression(4.0));
	myRange.push_back(Expression(5.0));
	REQUIRE(prange(args) == Expression(myRange));
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(prange(args), SemanticError);
	args.clear();
	args.emplace_back(0.0);
	args.emplace_back(5.0);
	args.emplace_back(env.get_exp(Atom(std::string("I"))));
	REQUIRE_THROWS_AS(prange(args), SemanticError);
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom(std::string("one")), a);
  Expression b(Atom(std::string("hello")));
  env.add_exp(Atom(std::string("hi")), b);

  env.reset();
  REQUIRE(!env.is_known(Atom(std::string("one"))));
  REQUIRE(!env.is_exp(Atom(std::string("one"))));
  REQUIRE(env.get_exp(Atom(std::string("one"))) == Expression());
  REQUIRE(!env.is_known(Atom(std::string("hi"))));
  REQUIRE(!env.is_exp(Atom(std::string("hi"))));
  REQUIRE(env.get_exp(Atom(std::string("hi"))) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom(std::string("begin")));
    
    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}

