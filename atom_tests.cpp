#include "catch.hpp"

#include "atom.hpp"

TEST_CASE( "Test constructors", "[atom]" ) {

  {
    INFO("Default Constructor");
    Atom a;

    REQUIRE(a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(!a.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
	REQUIRE(!a.isError());
  }

  {
    INFO("Number Constructor");
    Atom a(1.0);

    REQUIRE(!a.isNone());
    REQUIRE(a.isNumber());
    REQUIRE(!a.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
	REQUIRE(!a.isError());
  }

  {
    INFO("Symbol Constructor");
	std::string myString = "hi";
    Atom a(myString);

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
	REQUIRE(!a.isError());
  }

  {
	  INFO("Complex Constructor");
	  std::complex<double> myComplex(0.0, 0.0);
	  Atom a(myComplex);

	  REQUIRE(!a.isNone());
	  REQUIRE(!a.isNumber());
	  REQUIRE(!a.isSymbol());
	  REQUIRE(a.isComplex());
	  REQUIRE(!a.isList());
	  REQUIRE(!a.isString());
	  REQUIRE(!a.isError());
  }

  {
	  INFO("List Constructor");
	  Atom a(true);

	  REQUIRE(!a.isNone());
	  REQUIRE(!a.isNumber());
	  REQUIRE(!a.isSymbol());
	  REQUIRE(!a.isComplex());
	  REQUIRE(a.isList());
	  REQUIRE(!a.isString());
	  REQUIRE(!a.isError());
  }

  {
	  INFO("String Constructor");
	  Atom a(std::string("\"test\""));

	  REQUIRE(!a.isNone());
	  REQUIRE(!a.isNumber());
	  REQUIRE(!a.isSymbol());
	  REQUIRE(!a.isComplex());
	  REQUIRE(!a.isList());
	  REQUIRE(a.isString());
	  REQUIRE(!a.isError());
  }

  {
	  INFO("Error Constructor");
	  Atom a(std::string("error..."));

	  REQUIRE(!a.isNone());
	  REQUIRE(!a.isNumber());
	  REQUIRE(!a.isSymbol());
	  REQUIRE(!a.isComplex());
	  REQUIRE(!a.isList());
	  REQUIRE(!a.isString());
	  REQUIRE(a.isError());
  }


  {
    INFO("Token Constructor");
    Token t(std::string("hi"));
    Atom a(t);

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
  }

  {
    INFO("Copy Constructor");
	std::string myString = "hi";
    Atom a(myString);
    Atom b(1.0);

    
    Atom c = a;
    REQUIRE(!c.isNone());
    REQUIRE(!c.isNumber());
    REQUIRE(c.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());

    Atom d = b;
    REQUIRE(!d.isNone());
    REQUIRE(d.isNumber());
    REQUIRE(!d.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
  }

  {
	  INFO("Atom Constructor");
	  std::string myString = "hi";
	  Atom a(myString);
	  Atom b(1.0);
	  std::complex<double> myComplex(1.0, 1.0);
	  Atom c(myComplex);
	  Atom d(true);
	  std::string myLambda = "lambda";
	  Atom e(myLambda);
	  Atom x(std::string("\"test\""));
	  Atom y(std::string("error..."));


	  Atom f(a);
	  REQUIRE(!f.isNone());
	  REQUIRE(!f.isNumber());
	  REQUIRE(f.isSymbol());
	  REQUIRE(!f.isComplex());
	  REQUIRE(!f.isList());
	  REQUIRE(!f.isLambda());
	  REQUIRE(!a.isString());

	  Atom g(b);
	  REQUIRE(!g.isNone());
	  REQUIRE(g.isNumber());
	  REQUIRE(!g.isSymbol());
	  REQUIRE(!g.isComplex());
	  REQUIRE(!g.isList());
	  REQUIRE(!g.isLambda());
	  REQUIRE(!a.isString());

	  Atom h(c);
	  REQUIRE(!h.isNone());
	  REQUIRE(!h.isNumber());
	  REQUIRE(!h.isSymbol());
	  REQUIRE(h.isComplex());
	  REQUIRE(!h.isList());
	  REQUIRE(!h.isLambda());
	  REQUIRE(!a.isString());

	  Atom i(d);
	  REQUIRE(!i.isNone());
	  REQUIRE(!i.isNumber());
	  REQUIRE(!i.isSymbol());
	  REQUIRE(!i.isComplex());
	  REQUIRE(i.isList());
	  REQUIRE(!i.isLambda());
	  REQUIRE(!a.isString());

	  Atom j(e);
	  REQUIRE(!j.isNone());
	  REQUIRE(!j.isNumber());
	  REQUIRE(!j.isSymbol());
	  REQUIRE(!j.isComplex());
	  REQUIRE(!j.isList());
	  REQUIRE(j.isLambda());
	  REQUIRE(!a.isString());

	  Atom k(x);
	  REQUIRE(!k.isNone());
	  REQUIRE(!k.isNumber());
	  REQUIRE(!k.isSymbol());
	  REQUIRE(!k.isComplex());
	  REQUIRE(!k.isList());
	  REQUIRE(!k.isLambda());
	  REQUIRE(k.isString());
	  REQUIRE(!k.isError());

	  Atom l(y);
	  REQUIRE(!l.isNone());
	  REQUIRE(!l.isNumber());
	  REQUIRE(!l.isSymbol());
	  REQUIRE(!l.isComplex());
	  REQUIRE(!l.isList());
	  REQUIRE(!l.isLambda());
	  REQUIRE(!l.isString());
	  REQUIRE(l.isError());
  }
}

TEST_CASE( "Test assignment", "[atom]" ) {

  {
    INFO("default to default");
    Atom a;
    Atom b;
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
  }

  {
    INFO("number to default");
    Atom a;
    Atom b(1.0);
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
  }

  {
    INFO("symbol to default");
    Atom a;
    Atom b(std::string("hi"));
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
	REQUIRE(!a.isComplex());
	REQUIRE(!a.isList());
	REQUIRE(!a.isString());
  }

  {
	  INFO("complex to default");
	  Atom a;
	  std::complex<double> myComplex(0.0, 0.0);
	  Atom b(myComplex);
	  b = a;
	  REQUIRE(b.isNone());
	  REQUIRE(!b.isNumber());
	  REQUIRE(!b.isSymbol());
	  REQUIRE(!a.isComplex());
	  REQUIRE(!a.isList());
	  REQUIRE(!a.isString());
  }

  {
	  INFO("list to default");
	  Atom a;
	  Atom b(true);
	  b = a;
	  REQUIRE(b.isNone());
	  REQUIRE(!b.isNumber());
	  REQUIRE(!b.isSymbol());
	  REQUIRE(!a.isComplex());
	  REQUIRE(!a.isList());
	  REQUIRE(!a.isString());
  }

  {
	  INFO("string to default");
	  Atom a;
	  Atom b(std::string("\"string\""));
	  b = a;
	  REQUIRE(b.isNone());
	  REQUIRE(!b.isNumber());
	  REQUIRE(!b.isSymbol());
	  REQUIRE(!a.isComplex());
	  REQUIRE(!a.isList());
	  REQUIRE(!a.isString());
  }

  {
    INFO("default to number");
    Atom a(1.0);
    Atom b;
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to number");
    Atom a(1.0);
    Atom b(2.0);
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to symbol");
	std::string myString = "hi";
    Atom a(myString);
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
	  INFO("number to complex");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a(myComplex);
	  Atom b(1.0);
	  b = a;
	  REQUIRE(b.isComplex());
	  REQUIRE(b.asComplex() == myComplex);
  }

  {
	  INFO("number to list");
	  Atom a(true);
	  Atom b(1.0);
	  b = a;
	  REQUIRE(b.isList());
	  REQUIRE(b.asList() == true);
  }

  {
	  INFO("default to symbol");
	std::string myString = "hi";
	Atom a(myString);
    Atom b;
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("number to symbol");
	std::string myString = "hi";
	Atom a(myString);
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to symbol");
	std::string myString = "hi";
	Atom a(myString);
    Atom b("bye");
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
	  INFO("complex to symbol");
	  std::complex<double> myComplex(1.0, 2.0);
	  std::string myString = "hi";
	  Atom a(myComplex);
	  Atom b(myString);
	  a = b;
	  REQUIRE(a.isSymbol());
	  REQUIRE(a.asSymbol() == "hi");
  }

  {
	  INFO("list to symbol");
	  std::string myString = "hi";
	  Atom a(true);
	  Atom b(myString);
	  a = b;
	  REQUIRE(a.isSymbol());
	  REQUIRE(a.asSymbol() == "hi");
  }

  {
	  INFO("default to list");
	  Atom a(true);
	  Atom b;
	  b = a;
	  REQUIRE(b.isList());
	  REQUIRE(b.asList() == true);
  }

  {
	  INFO("number to list");
	  Atom a(true);
	  Atom b(1.0);
	  b = a;
	  REQUIRE(b.isList());
	  REQUIRE(b.asList() == true);
  }

  {
	  INFO("symbol to list");
	  Atom a(true);
	  Atom b("bye");
	  b = a;
	  REQUIRE(b.isList());
	  REQUIRE(b.asList() == true);
  }

  {
	  INFO("complex to list");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a(myComplex);
	  Atom b(true);
	  a = b;
	  REQUIRE(a.isList());
	  REQUIRE(a.asList() == true);
  }

  {
	  INFO("list to list");
	  Atom a(true);
	  Atom b(true);
	  a = b;
	  REQUIRE(a.isList());
	  REQUIRE(a.asList() == true);
  }

  {
	  INFO("default to complex");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a(myComplex);
	  Atom b;
	  b = a;
	  REQUIRE(b.isComplex());
	  REQUIRE(b.asComplex() == myComplex);
  }

  {
	  INFO("number to complex");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a(myComplex);
	  Atom b(1.0);
	  b = a;
	  REQUIRE(b.isComplex());
	  REQUIRE(b.asComplex() == myComplex);
  }

  {
	  INFO("symbol to complex");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a(myComplex);
	  Atom b(std::string("bye"));
	  b = a;
	  REQUIRE(b.isComplex());
	  REQUIRE(b.asComplex() == myComplex);
  }

  {
	  INFO("complex to complex");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a(std::complex<double>(3.0,7.0));
	  Atom b(myComplex);
	  a = b;
	  REQUIRE(a.isComplex());
	  REQUIRE(a.asComplex() == myComplex);
  }

  {
	  INFO("list to complex");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a(true);
	  Atom b(myComplex);
	  a = b;
	  REQUIRE(b.isComplex());
	  REQUIRE(b.asComplex() == myComplex);
  }

  {
	  INFO("list to lambda");
	  std::string myLambda("lambda");
	  Atom a(true);
	  Atom b(myLambda);
	  a = b;
	  REQUIRE(a.isLambda());
	  REQUIRE(a.asLambda() == myLambda);
  }

  {
	  INFO("list to string");
	  std::string myString("\"test\"");
	  Atom a(true);
	  Atom b(myString);
	  a = b;
	  REQUIRE(a.isString());
	  REQUIRE(a.asString() == myString);
  }

  {
	  INFO("list to error");
	  std::string myError("error...");
	  Atom a(true);
	  Atom b(myError);
	  a = b;
	  REQUIRE(a.isError());
	  REQUIRE(a.asError() == "...");
  }
}

TEST_CASE( "test comparison", "[atom]" ) {

  {
    INFO("compare default to default");
    Atom a;
    Atom b;
    REQUIRE(a == b);
  }

  {
    INFO("compare default to number");
    Atom a;
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare default to symbol");
	std::string myString = "hi";
    Atom a;
    Atom b(myString);
    REQUIRE(a != b);
  }

  {
	  INFO("compare default to complex");
	  std::complex<double> myComplex(1.0, 2.0);
	  Atom a;
	  Atom b(myComplex);
	  REQUIRE(a != b);
  }

  {
	  INFO("compare default to list");
	  Atom a;
	  Atom b(true);
	  REQUIRE(a != b);
  }

  {
    INFO("compare number to default");
    Atom a(1.0);
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare number to number");
    Atom a(1.0);
    Atom b(1.0);
    Atom c(2.0);
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare number to symbol");
    Atom a(1.0);
    Atom b(std::string("hi"));
    REQUIRE(a != b);
  }

  {
	  INFO("compare number to complex");
	  std::complex<double> myComplex(1.0, 0.0);
	  Atom a(1.0);
	  Atom b(myComplex);
	  REQUIRE(a != b);
  }

  {
	  INFO("compare number to list");
	  Atom a(1.0);
	  Atom b(true);
	  REQUIRE(a != b);
  }

  {
    INFO("compare symbol to default");
    Atom a(std::string("hi"));
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to number");
    Atom a(std::string("hi"));
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to symbol");
	std::string myString1 = "hi";
	std::string myString2 = "hi";
	std::string myString3 = "bye";
    Atom a(myString1);
    Atom b(myString2);
    Atom c(myString3);
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
	  INFO("compare symbol to complex");
	  std::complex<double> myComplex(1.0, 0.0);
	  Atom a(1.0);
	  Atom b(myComplex);
	  REQUIRE(a != b);
  }

  {
	  INFO("compare symbol to list");
	  Atom a(std::string(std::string("hi")));
	  Atom b(true);
	  REQUIRE(a != b);
  }

  {
	  INFO("compare complex to default");
	  std::complex<double> myComplex(1.0, 0.0);
	  Atom a(myComplex);
	  Atom b;
	  REQUIRE(a != b);
  }

  {
	  INFO("compare complex to number");
	  std::complex<double> myComplex(1.0, 0.0);
	  Atom a(myComplex);
	  Atom c(2.0);
	  REQUIRE(a != c);
  }

  {
	  INFO("compare complex to symbol");
	  std::complex<double> myComplex(1.0, 0.0);
	  Atom a(myComplex);
	  Atom b(std::string("hi"));
	  REQUIRE(a != b);
  }

  {
	  INFO("compare complex to complex");
	  std::complex<double> myComplex1(1.0, 0.0);
	  std::complex<double> myComplex2(1.0, 0.0);
	  std::complex<double> myComplex3(2.0, 3.0);
	  Atom a(myComplex1);
	  Atom b(myComplex2);
	  Atom c(myComplex3);
	  REQUIRE(a == b);
	  REQUIRE(a != c);
  }

  {
	  INFO("compare complex to list");
	  std::complex<double> myComplex(1.0, 0.0);
	  Atom a(myComplex);
	  Atom b(true);
	  REQUIRE(a != b);
  }

  {
	  INFO("compare list to default");
	  Atom a(true);
	  Atom b;
	  REQUIRE(a != b);
  }

  {
	  INFO("compare list to number");
	  Atom a(true);
	  Atom c(2.0);
	  REQUIRE(a != c);
  }

  {
	  INFO("compare list to symbol");
	  Atom a(true);
	  Atom b(std::string("hi"));
	  REQUIRE(a != b);
  }

  {
	  INFO("compare list to complex");
	  std::complex<double> myComplex(1.0, 0.0);
	  Atom a(true);
	  Atom b(myComplex);
	  REQUIRE(a != b);
  }

  {
	  INFO("compare list to list");
	  Atom a(true);
	  Atom b(true);
	  REQUIRE(a == b);
  }

  {
	  INFO("compare lambda to lambda");
	  Atom a(std::string("lambda"));
	  Atom b(std::string("lambda"));
	  REQUIRE(a == b);
  }

  {
	  INFO("compare string to string");
	  Atom a(std::string("\"string\""));
	  Atom b(std::string("\"string\""));
	  REQUIRE(a == b);
  }

  {
	  INFO("compare error to error");
	  Atom a(std::string("error..."));
	  Atom b(std::string("error..."));
	  Atom c(std::string("error.."));
	  REQUIRE(a == b);
	  REQUIRE(a != c);
  }

}





