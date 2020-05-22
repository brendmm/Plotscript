#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

void runWithError(const std::string & program) {

	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);
	if (!ok) {
		std::cerr << "Failed to parse: " << program << std::endl;
	}
	REQUIRE(ok == true);

	Expression result;
	REQUIRE_THROWS_AS(result = interp.evaluate(), SemanticError);

}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE("Check Environment", "[interpreter]") {



	Interpreter interp;
	interp.getEnv();
	Environment env;
	interp.setEnv(env);
	interp.throwIntInterrupt();
	REQUIRE(interp.getEnv().checkEnvInterrupt() == true);
	interp.resetIntInterrupt();
	REQUIRE(interp.getEnv().checkEnvInterrupt() == false);
	
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE("Test Interpreter special forms: lambda, apply and map", "[interpreter]") {

	{
		std::string program = "(begin (define a 1) (define x 100) (define f (lambda (x) (begin (define b 12) (+ a b x)))) (f 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(15.));
	}

	{
		std::string program = "(begin (define a 1) (define x 100) (define f (lambda (x) (begin (define b 12) (+ a b x)) (1))) (f 2))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (define a 1) (define x 100) (define f (lambda (1.0) (begin (define b 12) (+ a b x)))) (f 2))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (define a 1) (define x 100) (define f (lambda (define) (begin (define b 12) (+ a b x)))) (f 2))";
		INFO(program);
		runWithError(program);
	}


	{
		std::string program = "(apply + (list 1 2 3 4))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(10.));
	}

	{
		std::string program = "(begin (define complexAsList (lambda (x) (list (real x) (imag x)))) (apply complexAsList (list (+ 1 (* 3 I)))))";
		INFO(program);
		Expression result = run(program);
		std::list<Expression> myList;
		Expression exp1(1.0);
		myList.push_back(exp1);
		Expression exp2(3.0);
		myList.push_back(exp2);
		REQUIRE(result == Expression(myList));
	}

	{
		std::string program = "(begin (define linear (lambda (a b x) (+ (* a x) b))) (apply linear(list 3 4 5)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(19.));
	}

	{
		std::string program = "(apply + 3)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(apply (+ z I) (list 0))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(apply / (list 1 2 4))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(apply / (list 1 2 4) (1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(apply 1.0 (list 1 2 4))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(apply define (list 1 2 4))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(apply (lambda (x y z) (+ x y z)) (list 1 2 4))";
		Expression result = run(program);
		REQUIRE(result == Expression(7.));
	}

	{
		std::string program = "(begin (define temporaryLambda 1) (apply (lambda (x y z) (+ x y z)) (list 1 2 4))) ";
		Expression result = run(program);
		REQUIRE(result == Expression(7.));
	}

	{
		std::string program = "(begin (define f (lambda (x) (sin x))) (map f (list (- pi) (/ (- pi) 2) 0 (/ pi 2) pi)))";
		INFO(program);
		std::list<Expression> myList;
		Expression exp1(0.0);
		myList.push_back(exp1);
		Expression exp2(-1.0);
		myList.push_back(exp2);	
		myList.push_back(exp1);
		Expression exp3(1.0);
		myList.push_back(exp3);
		myList.push_back(exp1);
		Expression result = run(program);
		REQUIRE(result == Expression(myList));
	}

	{
		std::string program = "(map / (list 1 2 4))";
		INFO(program);
		std::list<Expression> myList;
		Expression exp1(1.0);
		myList.push_back(exp1);
		Expression exp2(0.5);
		myList.push_back(exp2);
		Expression exp3(0.25);
		myList.push_back(exp3);
		Expression result = run(program);
		REQUIRE(result == Expression(myList));
	}

	{
		std::string program = "(map + 3)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(map 3 (list 1 2 3))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (define addtwo (lambda (x y) (+ x y))) (map addtwo (list 1 2 3)))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(map 3 (list 1 2 3) (1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(map define (list 1 2 3))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (define temporaryLambda 1) (map (lambda (x) (/ x)) (list 1 2 4)))";
		INFO(program);
		std::list<Expression> myList;
		Expression exp1(1.0);
		myList.push_back(exp1);
		Expression exp2(0.5);
		myList.push_back(exp2);
		Expression exp3(0.25);
		myList.push_back(exp3);
		Expression result = run(program);
		REQUIRE(result == Expression(myList));
	}

	{
		std::string program = "(set-property \"name\" \"one\" 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.0));
	}

	{
		std::string program = "(get-property \"name\" (set-property \"name\" \"one\" 1))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.head().asString() == std::string("\"one\""));
	}

	{
		std::string program = "(begin (define a 1) (define a 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2.0));
	}

	{
		std::string program = "(set-property \"name\" 1 (set-property \"name\" \"one\" 1))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1.0));
	}

	{
		std::string program = "(begin (define f (lambda (x) (+ (sin x) 1))) (continuous-plot f (list (- pi) (pi)) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.getTailLength() == 85);
	}

	{
		std::string program = "(begin (define f (lambda (x) (list x (+ (* 2 x) 1)))) (discrete-plot (map f (range -2 2 0.5)) (list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1))))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.getTailLength() == 31);

	}

	{
		std::string program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list -2 2) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result.getTailLength() == 61);
	}

	{
		std::string program = "(begin (define f (lambda (x) (+ (sin x) 1))) (continuous-plot f (+ 2 2) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (define f (lambda (x) (+ (sin x) 1))) (continuous-plot f (list (- pi) (pi) 3) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (define f (lambda (x) (+ (sin x) 1))) (continuous-plot f (list (- pi) (+)) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (define f (lambda (x) (+ (sin x) 1))) (continuous-plot (+ 2 2) (list (- pi) (pi)) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(begin (+ 2 2) (continuous-plot f (list -2 2) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(set-property 1 1 (set-property \"name\" \"one\" 1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(get-property 1 (set-property \"name\" \"one\" 1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(get-property \"name\" 1 (set-property \"name\" \"one\" 1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(get-property \"a\" 1 (set-property \"name\" \"one\" 1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(set-property 1 \"one\" 1)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(set-property \"name\" 1 \"one\" 1)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(set-property + 1 \"one\" 1)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(set-property pi 1 \"one\" 1)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(get-property + (set-property \"name\" \"one\" 1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = "(get-property pi (set-property \"name\" \"one\" 1))";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
			(begin
			(define f (lambda (x)
			(list x (+ (* 2 x) 1))))
				(discrete-plot (map a (range -2 2 0.5))
			(list
			(list "title" "The Data")
				(list "abscissa-label" "X Label")
				(list "ordinate-label" "Y Label")
				(list "text-scale" 1))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(discrete-plot (a) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label") 
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label")) 1)
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
		  (list "text-scale")
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
		  (list "text-scale" -1)
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
		  (list "text-scale" a)
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
		  (1)
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
	(discrete-plot (list (list -1 -1) (list 1 1)) 
		(list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
		  (list 1 1)
          (list "ordinate-label" "Y Label")))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list 2 -2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list a -2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot a (list -2 2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2)
		(list
		(list "title")
			(list "abscissa-label" "x")
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label")
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "ordinate-label"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "text-scale" -1)
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "text-scale" a)
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "text-scale")
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2) 1
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list "text-scale" 1)
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2) 
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(1)
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}

	{
		std::string program = R"(
		(begin
	(define f (lambda (x)
		(+ (* 2 x) 1)))
		(continuous-plot f (list -2 2)
		(list
		(list "title" "A continuous linear function")
			(list "abscissa-label" "x")
			(list 1 1)
			(list "ordinate-label" "y"))))
		)";
		INFO(program);
		runWithError(program);
	}
	
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}
