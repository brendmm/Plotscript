#include "environment.hpp"

#include <cassert>
#include <cmath>
#include <complex>
#include <iostream>
#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> & args){

  // check all aruments are numbers or complex, while adding
  std::complex<double> result (0.0, 0.0);
  for( auto & a :args){
    if(a.isHeadNumber()){
		result= result + a.head().asNumber();
    }
	else if (a.isHeadComplex()) {
		result = result + a.head().asComplex();
	}
    else{
      throw SemanticError("Error in call to add, argument not a number");
    }
  }
  for (auto & a : args) {
	  if (a.isHeadComplex()) {
		  return Expression(result);
	  }
  }
  return Expression(result.real());
};

Expression mul(const std::vector<Expression> & args){
 
  // check all aruments are numbers or complex, while multiplying
	std::complex<double> result(1.0, 0.0);
  for( auto & a :args){
    if(a.isHeadNumber()){
      result = result * a.head().asNumber();      
    }
	else if (a.isHeadComplex()) {
		result = result * a.head().asComplex();
	}
    else{
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }

  for (auto & a : args) {
	  if (a.isHeadComplex()) {
		  return Expression(result);
	  }
  }
  return Expression(result.real());
};

Expression subneg(const std::vector<Expression> & args){
	// check all aruments are numbers or complex, while subtracting or negating
	std::complex<double> result(0.0, 0.0);
	
  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      result = -args[0].head().asNumber();
    }
	else if (args[0].isHeadComplex()) {
			result = -args[0].head().asComplex();
	}
    else{
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  }
  else if(nargs_equal(args,2)){
	if (((args[0].isHeadNumber()) || (args[0].isHeadComplex())) && ((args[1].isHeadNumber()) || (args[1].isHeadComplex()))) {
		if (args[0].isHeadNumber()) {
			result = args[0].head().asNumber();
		}
		else {
			result = args[0].head().asComplex();
		}
		if (args[1].isHeadNumber()) {
			result = result - args[1].head().asNumber();
		}
		else {
			result = result - args[1].head().asComplex();
		}
	}
	else{      
		throw SemanticError("Error in call to subtraction: invalid argument.");
	}
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }
  for (auto & a : args) {
	  if (a.isHeadComplex()) {
		  return Expression(result);
	  }
  }
  return Expression(result.real());

};

Expression div(const std::vector<Expression> & args){
	// check all aruments are numbers or complex, while dividing
	std::complex<double> result(0.0, 0.0);
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber() || args[0].isHeadComplex()) {
			if (args[0].isHeadNumber()) {
				result = 1 / args[0].head().asNumber();
			}

			else if (args[0].isHeadComplex()) {
				if ((real(args[0].head().asComplex()) != 0) || (imag(args[0].head().asComplex()) != 0))  {
					double a = real(args[0].head().asComplex());
					double b = imag(args[0].head().asComplex());
					result.real(a / ((a*a) + (b*b)));
					result.imag(-b / ((a*a) + (b*b)));
				}

				
			}
		}
		else {
			throw SemanticError("Error in call to division: invalid argument.");
		}
	}

  else if(nargs_equal(args,2)){
	  if (((args[0].isHeadNumber()) || (args[0].isHeadComplex())) && ((args[1].isHeadNumber()) || (args[1].isHeadComplex()))) {
		  if (args[0].isHeadNumber()) {
			  result = args[0].head().asNumber();
		  }
		  else {
			  result = args[0].head().asComplex();
		  }
		  if (args[1].isHeadNumber()) {
			  result = result / args[1].head().asNumber();
		  }
		  else {
			  result = result / args[1].head().asComplex();
		  }
	  }
    else{      
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  for (auto & a : args) {
	  if (a.isHeadComplex()) {
		  return Expression(result);
	  }
  }
  return Expression(result.real());
};

Expression sqrt(const std::vector<Expression> & args) {
	// check all aruments are numbers or complex, while square rooting
	std::complex<double> result (0.0, 0.0);

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			if (args[0].head().asNumber() >= 0 ) {
				result=pow(args[0].head().asNumber(), 0.5);
			}
			else if (args[0].head().asNumber() < 0) {
				result.imag(pow(-args[0].head().asNumber(), 0.5));
			}
		}
		else if (args[0].isHeadComplex()) {
			result = pow(args[0].head().asComplex(), 0.5);
		}
		else {
			throw SemanticError("Error in call to sqrt: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sqrt: invalid number of arguments.");
	}
	for (auto & a : args) {
		if (a.isHeadComplex()) {
			return Expression(result);
		}
	}
	if (result.imag() == 0) {
		return Expression(result.real());
	}
	return Expression(result);
};

Expression power(const std::vector<Expression> & args) {
	// check all aruments are numbers or complex, while computing power
	std::complex<double> result (0.0, 0.0);
	if (nargs_equal(args, 2)) {
		if (((args[0].isHeadNumber()) || (args[0].isHeadComplex())) && ((args[1].isHeadNumber()) || (args[1].isHeadComplex()))) {
			if ((args[0].isHeadNumber())) {
				if ((args[1].isHeadNumber())) {
					result = pow(args[0].head().asNumber(), args[1].head().asNumber());
				}
				else {
					result = pow(args[0].head().asNumber(), args[1].head().asComplex());
				}
			}
			else {
				if ((args[1].isHeadNumber())) {
					result = pow(args[0].head().asComplex(), args[1].head().asNumber());
				}
				else {
					result = pow(args[0].head().asComplex(), args[1].head().asComplex());
				}
			}
		}
		else {
			throw SemanticError("Error in call to pow: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to pow: invalid number of arguments.");
	}
	for (auto & a : args) {
		if (a.isHeadComplex()) {
			return Expression(result);
		}
	}
	return Expression(result.real());
};

Expression naturalLog(const std::vector<Expression> & args) {
	// check all aruments are numbers, while taking natural log
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			if (args[0].head().asNumber() >= 0) {
				result = log(args[0].head().asNumber());
			}
			else {
				throw SemanticError("Error in call to natural log: invalid argument.");
			}
		}
		else {
			throw SemanticError("Error in call to natural log: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to natural log: invalid number of arguments.");
	}
	return Expression(result);
};

Expression sine(const std::vector<Expression> & args) {
	// check all aruments are numbers, while taking sine
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = sin(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to sin: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}
	return Expression(result);
};

Expression cosine(const std::vector<Expression> & args) {
	// check all aruments are numbers, while taking cosine
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = cos(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to cos: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to cos: invalid number of arguments.");
	}
	return Expression(result);
};

Expression tangent(const std::vector<Expression> & args) {
	// check all aruments are numbers, while taking tangent
	double result = 0;
	double distance;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			distance = args[0].head().asNumber();
			if (distance > 0) {
				while (distance > (atan2(0, -1) / 2)) {
					distance = distance - (atan2(0, -1) / 2);
				}
			}
			else {
				while (distance < (-atan2(0, -1) / 2)) {
					distance = distance + (atan2(0, -1) / 2);
				}
			}
			distance = args[0].head().asNumber();
			result = tan(distance);

		}
		else {
			throw SemanticError("Error in call to tangent: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to tangent: invalid number of arguments.");
	}
	return Expression(result);
};

Expression real(const std::vector<Expression> & args) {
	// check all aruments are complex, while finding real value
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = real(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to real: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to real: invalid number of arguments.");
	}
	return Expression(result);
};

Expression imaginary(const std::vector<Expression> & args) {
	// check all aruments are complex, while finding imag value
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = imag(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to imag: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to imag: invalid number of arguments.");
	}
	return Expression(result);
};

Expression magnitude(const std::vector<Expression> & args) {
	// check all aruments are complex, while finding magnitude
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = abs(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to mag: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to mag: invalid number of arguments.");
	}
	return Expression(result);
};

Expression argument(const std::vector<Expression> & args) {
	// check all aruments are complex, while finding argument
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = arg(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to arg: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to arg: invalid number of arguments.");
	}
	return Expression(result);
};

Expression conjugate(const std::vector<Expression> & args) {
	// check all aruments are complex, while finding conjugate
	std::complex<double> result (0.0, 0.0);
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = conj(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to conj: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to conj: invalid number of arguments.");
	}
	return Expression(result);
};

Expression list(const std::vector<Expression> & args) {
	// check all aruments are expressions, while finding conjugate
	std::list<Expression> result = {};
	
	for (auto & a : args) {
		result.push_back(a);
	}
	return Expression(result);
};

Expression firstElementList(const std::vector<Expression> & args) {
	// check all aruments are lists, while finding first
	Expression result;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadList()) {
			auto e = args[0].tailConstBegin();
			if (e == args[0].tailConstEnd()) {
				throw SemanticError("Error in call to first: list is empty.");
			}
			else {
				result = *e;
				return Expression(result);
			}
		}
		else {
			throw SemanticError("Error in call to first: argument is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to sqrt: invalid number of arguments.");
	}
	
};

Expression restList(const std::vector<Expression> & args) {
	// check all aruments are lists, while finding rest
	std::list<Expression> result;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadList()) {
			auto e = args[0].tailConstBegin();
			if (e == args[0].tailConstEnd()) {
				throw SemanticError("Error in call to rest: list is empty.");
			}
			else {
				e = e + 1;
				while( e != args[0].tailConstEnd()){
					result.push_back(*e);
					++e;
				}
				return Expression(result);
			}
		}
		else {
			throw SemanticError("Error in call to rest: argument is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to rest: invalid number of arguments.");
	}

};

Expression lengthList(const std::vector<Expression> & args) {
	// check all aruments are lists, while finding length
	double result=0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadList()) {
			for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); ++e) {
				result++;
			}
			return Expression(result);
		}
		else {
			throw SemanticError("Error in call to length: argument is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to length: invalid number of arguments.");
	}

};

Expression appendLists(const std::vector<Expression> & args) {
	// check all aruments are lists, while executing append
	std::list<Expression> result;
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadList()) {
			for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); ++e) {
				result.push_back(*e);
			}
			result.push_back(args[1]);
			return Expression(result);
		}
		else {
			throw SemanticError("Error in call to append: first argument is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to append: invalid number of arguments.");
	}

};

Expression joinLists(const std::vector<Expression> & args) {
	// check all aruments are lists, while executing join
	std::list<Expression> result;
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadList() && args[1].isHeadList()) {
			for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); ++e) {
				result.push_back(*e);
			}
			for (auto e = args[1].tailConstBegin(); e != args[1].tailConstEnd(); ++e) {
				result.push_back(*e);
			}
			return Expression(result);
		}
		else {
			throw SemanticError("Error in call to join: argument is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to join: invalid number of arguments.");
	}

};

Expression rangeLists(const std::vector<Expression> & args) {
	// check all aruments are lists, while finding range
	if (nargs_equal(args, 3)) {
		if (args[0].isHeadNumber() && args[1].isHeadNumber() && args[2].isHeadNumber()) {
			if (args[0].head().asNumber() <= args[1].head().asNumber()) {
				if (args[2].head().asNumber() > 0) {
					std::list<Expression> listResult;
					for (double temp = args[0].head().asNumber(); temp <= args[1].head().asNumber(); temp = temp + args[2].head().asNumber()) {
						listResult.push_back(Expression(temp));
						
					}
					Expression result(listResult);
					return Expression(result);
				}
				else {
					throw SemanticError("Error in call to range: increment is negative or zero.");
				}
			}
			else {
				throw SemanticError("Error in call to range: upperbound is less than lowerbound.");
			}
		}
		else {
			throw SemanticError("Error in call to range: argument is not a number.");
		}
	}
	else {
		throw SemanticError("Error in call to range: invalid number of arguments.");
	}

};


const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I (0.0, 1.0);
bool Environment::interruptThrown = false;

Environment::Environment(){
  reset();
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;
  
  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }
    
  // error if overwriting symbol map
  if(envmap.find(sym.asSymbol()) != envmap.end()){
	delete_exp(sym);
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp)); 
}

void Environment::delete_exp(const Atom &sym) {
	if (!sym.isSymbol()) {
		throw SemanticError("Attempt to add non-symbol to environment");
	}

	if (envmap.find(sym.asSymbol()) != envmap.end()) {
		envmap.erase(sym.asSymbol());
	}
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}



/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();
  
  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add)); 

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg)); 

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul)); 

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div)); 

  // Built-In value of e;
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: power;
  envmap.emplace("^", EnvResult(ProcedureType, power));

  // Procedure: natural log;
  envmap.emplace("ln", EnvResult(ProcedureType, naturalLog));

  // Procedure: sine;
  envmap.emplace("sin", EnvResult(ProcedureType, sine));

  // Procedure: cosine;
  envmap.emplace("cos", EnvResult(ProcedureType, cosine));

  // Procedure: tangent;
  envmap.emplace("tan", EnvResult(ProcedureType, tangent));

  // Built-In value of i;
  envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

  // Procedure: real value;
  envmap.emplace("real", EnvResult(ProcedureType, real));

  // Procedure: imaginary value;
  envmap.emplace("imag", EnvResult(ProcedureType, imaginary));

  // Procedure: magnitude;
  envmap.emplace("mag", EnvResult(ProcedureType, magnitude));

  // Procedure: argument;
  envmap.emplace("arg", EnvResult(ProcedureType, argument));

  // Procedure: conjugate;
  envmap.emplace("conj", EnvResult(ProcedureType, conjugate));

  // Procedure: conjugate;
  envmap.emplace("list", EnvResult(ProcedureType, list));

  // Procedure: conjugate;
  envmap.emplace("first", EnvResult(ProcedureType, firstElementList));

  // Procedure: conjugate;
  envmap.emplace("rest", EnvResult(ProcedureType, restList));

  // Procedure: conjugate;
  envmap.emplace("length", EnvResult(ProcedureType, lengthList));

  // Procedure: conjugate;
  envmap.emplace("append", EnvResult(ProcedureType, appendLists));

  // Procedure: conjugate;
  envmap.emplace("join", EnvResult(ProcedureType, joinLists));

  // Procedure: conjugate;
  envmap.emplace("range", EnvResult(ProcedureType, rangeLists));

}

void Environment::throwEnvInterrupt() {
	interruptThrown = true;
}

bool Environment::checkEnvInterrupt() {
	return interruptThrown;
}

void Environment::resetEnvInterrupt() {
	interruptThrown = false;
}
