#include "interpreter.hpp"

// system includes
#include <stdexcept>
#include <iostream>

// module includes
#include "token.hpp"
#include "parse.hpp"
#include "expression.hpp"
#include "environment.hpp"
#include "semantic_error.hpp"


bool Interpreter::parseStream(std::istream & expression) noexcept{

  TokenSequenceType tokens = tokenize(expression);

  ast = parse(tokens);

  return (ast != Expression());
};
				     

Expression Interpreter::evaluate(){
  return ast.eval(env);
};

Environment Interpreter::getEnv() {
	return env;
}

void Interpreter::setEnv(Environment newEnv) {
	env = newEnv;
}

void Interpreter::throwIntInterrupt() {
	env.throwEnvInterrupt();
}

void Interpreter::resetIntInterrupt() {
	env.resetEnvInterrupt();
}