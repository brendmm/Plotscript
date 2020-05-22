#include "expression.hpp"

#include <sstream>
#include <iomanip>
#include <list>
#include <iostream>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression(){

}

Expression::Expression(const Atom & a){
	
  m_head = a;
}

Expression::Expression(const std::list<Expression> & a) {
	m_head = true;
	for (auto & args : a) {
		m_tail.push_back(args);
	}
	
}

Expression::Expression(const std::vector<Expression> & a) {
	m_head = std::string("lambda");
	for (auto & args : a) {
		m_tail.push_back(args);
	}

}

// recursive copy
Expression::Expression(const Expression & a){
  m_head = a.m_head;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
  propertymap = a.propertymap;
}

Expression & Expression::operator=(const Expression & a){
  // prevent self-assignment
  if(this != &a){
    m_head = a.m_head;
    m_tail.clear();
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    } 
	propertymap.clear();
	propertymap = a.propertymap;
  }
  
  return *this;
}

Atom & Expression::head(){
  return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}  

bool Expression::isHeadComplex() const noexcept {
	return m_head.isComplex();
}

bool Expression::isHeadList() const noexcept {
	return m_head.isList();
}

bool Expression::isHeadLambda() const noexcept {
	return m_head.isLambda();
}

bool Expression::isHeadString() const noexcept {
	return m_head.isString();
}

void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}

Expression * Expression::tail(){
  Expression * ptr = nullptr;
  
  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env){

  // head must be a symbol
  if(!op.isSymbol()){
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }
  
  // must map to a proc
  
  if(!env.is_proc(op)){
		throw SemanticError("Error during evaluation: symbol does not name a procedure");
  }
  
  // map from symbol to proc
  Procedure proc = env.get_proc(op);
  
  // call proc with args
  return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol()){ // if symbol is in env return value
      if(env.is_exp(head)){
	return env.get_exp(head);
      }
      else{
	throw SemanticError("Error during evaluation: unknown symbol");
      }
    }
    else if(head.isNumber()){
      return Expression(head);
    }
	else if (head.isComplex()) {
		return Expression(head);
	}
	else if (head.isString()) {
		return Expression(head);
	}
    else{
      throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){
  
  if(m_tail.size() == 0){
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }
  
  return result;
}

Expression Expression::handle_define(Environment & env){

  // tail must have size 3 or error
  if(m_tail.size() != 2){
    throw SemanticError("Error during evaluation: invalid number of arguments to define");
  }
  
  // tail[0] must be symbol
  if(!m_tail[0].isHeadSymbol()){
    throw SemanticError("Error during evaluation: first argument to define not symbol");
  }

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if((s == "define") || (s == "begin") || (s == "lambda") || (s == "map") || (s == "apply")){
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");
  }
  
  if(env.is_proc(m_head)){
    throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
  }
  if (env.is_exp(m_head)) {
	  throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
  }

  
  Expression result;
	  result = m_tail[1].eval(env);
  //and add to env
  env.add_exp(m_tail[0].head(), result);
  
  return result;
}

Expression Expression::handle_lambda(Environment & env) {

	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}
	Expression result;
	std::vector<Expression> lambdaVector;
	std::list<Expression> variableList;

	//store variables in one expression in first argument
	variableList.push_back(m_tail[0].head());
	for (auto e =m_tail[0].tailConstBegin(); e != m_tail[0].tailConstEnd(); ++e) {
		variableList.push_back(Expression(*e));
	}
	Expression variables = variableList;
	lambdaVector.push_back(variables);

	//store equation in one expression in second argument
	lambdaVector.push_back(m_tail[1]);
	result = lambdaVector;


	return result;
}

Expression Expression::handle_apply(Environment & env) {

	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}
	bool definedLambda = false;
	//check if first argument is defined lambda procedure
	if (env.is_exp(m_tail[0].head())) {
		if (env.get_exp(m_tail[0].head()).head().isLambda()) {
			definedLambda = true;
		}
	}
	bool isLambda = false;
	//check if first argument is undefined lambda procedure
	if (m_tail[0].head().asSymbol() == "lambda") {
		isLambda = true;
	}
	bool firstArgProcedure = false;
	//check if first argument is defined procedure
	if (env.is_proc(m_tail[0].head()) || definedLambda || isLambda) {
		if (env.is_proc(m_tail[0].head())) {
			if (m_tail[0].m_tail.size() != 0) {
				throw SemanticError("Error during apply: first argument is not a procedure");
			}
		}
		firstArgProcedure = true;
	}

	//check if second argument is list
	bool secondArgList = false;
	if (m_tail[1].head().asSymbol() == "list" || m_tail[1].isHeadList()) {
		secondArgList = true;
	}


	//check if arguments are correct type
	if (firstArgProcedure && secondArgList) {
		Expression result = m_tail[1].eval(env);
		Expression Answer;
		std::vector<Expression> arguments;
		for (auto e = result.tailConstBegin(); e != result.tailConstEnd(); e++) {
			arguments.emplace_back(*e);
		}
		result.m_tail = arguments;
		//must define lambda expression
		if (isLambda) {			
			Expression myLambda;
			myLambda.m_head = std::string("define");
			myLambda.m_tail.emplace_back(Expression(std::string("temporaryLambda")));
			myLambda.m_tail.emplace_back(Expression(m_tail[0]));
			bool expressionExists = env.is_exp(std::string("temporaryLambda"));
			Expression expressionHolder;
			//name is used sovalue must be stored in a temporary value
			if (expressionExists) {
				expressionHolder = env.get_exp(std::string("temporaryLambda"));
				env.delete_exp(Atom(std::string("temporaryLambda")));
			}
			myLambda.eval(env);
			result.m_head = Atom(std::string("temporaryLambda"));
			Answer = result.eval(env);
			env.delete_exp(Atom(std::string("temporaryLambda")));
			//repopulate with temporary value if needed
			if (expressionExists) {
				env.add_exp(Atom(std::string("temporaryLambda")), expressionHolder);
			}
		}
		else {
			result.m_head = Atom(m_tail[0].head());
			Answer = result.eval(env);
		}
		return Answer;
	}
	else {
		throw SemanticError("Error during apply: argument is wrong type: First should be procedure, Second should be list");
	}

}

Expression Expression::handle_map(Environment & env) {

	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}
	bool definedLambda = false;
	//check if first argument is defined lambda procedure
	if (env.is_exp(m_tail[0].head())) {
		if (env.get_exp(m_tail[0].head()).head().isLambda()) {
			definedLambda = true;
		}
	}
	bool isLambda = false;
	//check if first argument is undefined lambda procedure
	if (m_tail[0].head().asSymbol() == "lambda") {
		isLambda = true;
	}
	bool firstArgProcedure = false;
	//check if first argument is defined procedure
	if (env.is_proc(m_tail[0].head()) || definedLambda || isLambda) {
		firstArgProcedure = true;
	}


	bool secondArgList = false;
	//check if second argument is list

	if (m_tail[1].eval(env).head().asSymbol() == "list" || m_tail[1].eval(env).isHeadList()) {
		secondArgList = true;
	}
	if (firstArgProcedure && secondArgList) {
		Expression result = m_tail[1].eval(env);
		std::list<Expression> Answer;
		std::vector<Expression> arguments;
		for (auto e = result.tailConstBegin(); e != result.tailConstEnd(); e++) {
			arguments.emplace_back(*e);
		}
		result.m_tail.clear();
		if (isLambda) {
			Expression myLambda;
			myLambda.m_head = std::string("define");
			myLambda.m_tail.emplace_back(Expression(std::string("temporaryLambda")));
			myLambda.m_tail.emplace_back(Expression(m_tail[0]));
			bool expressionExists = env.is_exp(std::string("temporaryLambda"));
			Expression expressionHolder;
			//name already defined in exp. Must copy to temporary expression
			if (expressionExists) {
				expressionHolder = env.get_exp(std::string("temporaryLambda"));
				env.delete_exp(Atom(std::string("temporaryLambda")));
			}
			myLambda.eval(env);
			result.m_head = Atom(std::string("temporaryLambda"));
			for (unsigned int i = 0; i < arguments.size(); i++) {
				result.m_tail.emplace_back(arguments[i]);
				Answer.push_back(result.eval(env));
				result.m_tail.pop_back();
			}
			env.delete_exp(Atom(std::string("temporaryLambda")));
			//if needed, place temporary value back in env
			if (expressionExists) {
				env.add_exp(Atom(std::string("temporaryLambda")), expressionHolder);
			}
		}
		else {
			result.m_head = Atom(m_tail[0].head());
			for (unsigned int i = 0; i < arguments.size(); i++) {
				result.m_tail.emplace_back(arguments[i]);
				Answer.push_back(result.eval(env));
				result.m_tail.pop_back();
			}
		}
		return Expression(Answer);
	}
	else {
		throw SemanticError("Error during map: argument is wrong type: First should be procedure, Second should be list");
	}


}

Expression Expression::handle_set_property(Environment & env) {
	
	// tail must have size 3 or error
	if (m_tail.size() != 3) {
		throw SemanticError("Error: invalid number of arguments");
	}

	// tail[0] must be string
	if (!m_tail[0].isHeadString()) {
		throw SemanticError("Error: first argument to set-property not a string");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}

	Expression result;
	Atom propertyName(m_tail[0].head());
	Expression propertyValue = m_tail[1].eval(env);
	Expression myExpression = m_tail[2].eval(env);
	if (myExpression.checkProperty(m_tail[0].head())) {
		myExpression.propertymap.erase(m_tail[0].head().asString());
	}
	myExpression.propertymap.emplace(m_tail[0].head().asString(), propertyValue);
	return myExpression;

}

Expression Expression::handle_get_property(Environment & env) {

	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error: invalid number of arguments");
	}

	// tail[0] must be string
	if (!m_tail[0].isHeadString()) {
		throw SemanticError("Error: first argument to get-property not a string");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}
	Expression answer = m_tail[1].eval(env);
	Expression result;
	if (answer.propertymap.find(m_tail[0].head().asString()) != answer.propertymap.end()) {
		result = answer.propertymap.find(m_tail[0].head().asString())->second;
	}
	return result;

}

Expression Expression::handle_discrete_plot(Environment & env) {
	// tail must have size 3 or error
	std::list<Expression> resultList;

	if (m_tail.size() != 2) {
		throw SemanticError("Error: invalid number of arguments");
	}

	// tail[0] must be string
	else if (!m_tail[0].eval(env).isHeadList()) {
		throw SemanticError("Error: first argument is not a list on coordinates");
	}
	else if (!m_tail[0].eval(env).checkValidCoordinates()) {
		throw SemanticError("Error: one or more coordinates were invalid");
	}
	else if (!m_tail[1].eval(env).isHeadList()) {
		throw SemanticError("Error: second argument is not a list ");
	}


	double minX = m_tail[0].eval(env).getMinX();
	double maxX = m_tail[0].eval(env).getMaxX();
	double minY = m_tail[0].eval(env).getMinY();
	double maxY = m_tail[0].eval(env).getMaxY();

	double textScale=1;

	for (unsigned int i = 0; i < m_tail[1].eval(env).getTailLength(); i++) {
		if (!m_tail[1].eval(env).m_tail[i].isHeadList()) {
			throw SemanticError("Error: one or more options is not a list");
		}
		else if (m_tail[1].eval(env).m_tail[i].getTailLength() != 2) {
			throw SemanticError("Error: one or more options has incorrect amount of properties");
		}
		else if (!m_tail[1].eval(env).m_tail[i].m_tail[0].isHeadString()) {
			throw SemanticError("Error: Option type is not a string");
		}

		if (m_tail[1].eval(env).m_tail[i].m_tail[0].head().asString() == "\"text-scale\"") {
			if (!m_tail[1].eval(env).m_tail[i].m_tail[1].isHeadNumber()) {
				throw SemanticError("Error: Text-scale not given a number property");
			}
			else if (m_tail[1].eval(env).m_tail[i].m_tail[1].head().asNumber() <= 0) {
				throw SemanticError("Error: Number value is not positive");
			}
			else {
				textScale = m_tail[1].eval(env).m_tail[i].m_tail[1].head().asNumber();
			}
		}

	}

	resultList.splice(resultList.end(), m_tail[0].eval(env).handlePoints(minX, maxX, minY, maxY));
	resultList.splice(resultList.end(), m_tail[0].eval(env).buildRect(minX, maxX, minY, maxY));
	resultList.splice(resultList.end(), m_tail[0].eval(env).buildOrigin(minX, maxX, minY, maxY));
	resultList.splice(resultList.end(), m_tail[0].eval(env).buildStems(minX, maxX, minY, maxY));
	resultList.splice(resultList.end(), m_tail[0].eval(env).listAxis(minX, maxX, minY, maxY, textScale));
	resultList.splice(resultList.end(), m_tail[1].eval(env).listLabels(minX, maxX, minY, maxY, textScale));

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}

	Expression result(resultList);
	return result;
}

Expression Expression::handle_continuous_plot(Environment & env) {
	// tail must have size 3 or error
	std::list<Expression> resultList;
	std::list<Expression> myCoordinates;
	if (!(m_tail.size() == 3 || m_tail.size() == 2)) {
		throw SemanticError("Error: invalid number of arguments");
	}

	// tail[0] must be string
	else if (!m_tail[0].eval(env).isHeadLambda()) {
		throw SemanticError("Error: first argument is not a function");
	}
	else if (!m_tail[1].eval(env).isHeadList()) {
		throw SemanticError("Error: second argument is not a list ");
	}
	else if (m_tail[1].eval(env).getTailLength()!=2) {
		throw SemanticError("Error: second argument does not have 2 bounds");
	}
	else if (!m_tail[1].eval(env).m_tail[0].isHeadNumber() && !m_tail[1].eval(env).m_tail[1].isHeadNumber()) {
		throw SemanticError("Error: One or more bounds is not a number");
	}
	else if (!(m_tail[1].eval(env).m_tail[0].head().asNumber() < m_tail[1].eval(env).m_tail[1].head().asNumber())) {
		throw SemanticError("Error:Lower bound is greater than upper bound");
	}
	Expression myList(Atom(std::string("list")));

	double minX = m_tail[1].eval(env).m_tail[0].head().asNumber();
	double maxX = m_tail[1].eval(env).m_tail[1].head().asNumber();
	double xRange = maxX - minX;


	for (int i = 0; i <= 50; i++) {
		myList.m_tail.emplace_back(Expression(Atom(minX+((xRange/50)*i))));
	}


	Expression myMap(Atom(std::string("map")));
	myMap.m_tail.emplace_back(m_tail[0]);
	myMap.m_tail.emplace_back(myList);
	Expression xPoints(myList);
	Expression yPoints(myMap.eval(env));
	std::list<Expression> aCoordinate;

	for (unsigned int i = 0; i < yPoints.getTailLength(); i++) {
		aCoordinate.emplace_back(xPoints.getValueInTail(i));
		aCoordinate.emplace_back(yPoints.getValueInTail(i));
		myCoordinates.emplace_back(Expression(aCoordinate));
		aCoordinate.clear();
	}
	Expression coordinateList(myCoordinates);

	coordinateList = coordinateList.fixAngles(0, m_tail[0], env);

	double minY = coordinateList.getMinY();
	double maxY = coordinateList.getMaxY();

	double textScale = 1;
	if (m_tail.size() == 3) {
		for (unsigned int i = 0; i < m_tail[2].eval(env).getTailLength(); i++) {
			if (!m_tail[2].eval(env).m_tail[i].isHeadList()) {
				throw SemanticError("Error: one or more options is not a list");
			}
			else if (m_tail[2].eval(env).m_tail[i].getTailLength() != 2) {
				throw SemanticError("Error: one or more options has incorrect amount of properties");
			}
			else if (!m_tail[2].eval(env).m_tail[i].m_tail[0].isHeadString()) {
				throw SemanticError("Error: Option type is not a string");
			}

			if (m_tail[2].eval(env).m_tail[i].m_tail[0].head().asString() == "\"text-scale\"") {
				if (!m_tail[2].eval(env).m_tail[i].m_tail[1].isHeadNumber()) {
					throw SemanticError("Error: Text-scale not given a number property");
				}
				else if (m_tail[2].eval(env).m_tail[i].m_tail[1].head().asNumber() <= 0) {
					throw SemanticError("Error: Number value is not positive");
				}
				else {
					textScale = m_tail[2].eval(env).m_tail[i].m_tail[1].head().asNumber();
				}
			}

		}
		resultList.splice(resultList.end(), m_tail[2].eval(env).listLabels(minX, maxX, minY, maxY, textScale));
	}
	resultList.splice(resultList.end(), coordinateList.buildLines(minX, maxX, minY, maxY));
	resultList.splice(resultList.end(), coordinateList.buildRect(minX, maxX, minY, maxY));
	resultList.splice(resultList.end(), coordinateList.buildOrigin(minX, maxX, minY, maxY));
	resultList.splice(resultList.end(), coordinateList.listAxis(minX, maxX, minY, maxY, textScale));
	

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}

	Expression result(resultList);
	return result;
}
// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env) {

	if (env.checkEnvInterrupt()) {
		throw SemanticError("Error: interpreter kernel interrupted");
	}
	else {
		if (m_tail.empty() && m_head.asSymbol() != "list") {
			return handle_lookup(m_head, env);
		}
		// handle begin special-form
		else if (m_head.isSymbol() && m_head.asSymbol() == "begin") {
			return handle_begin(env);
		}
		// handle define special-form
		else if (m_head.isSymbol() && m_head.asSymbol() == "define") {
			return handle_define(env);
		}
		// handle lambda special-form
		else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
			return handle_lambda(env);
		}
		// handle apply procedure
		else if (m_head.isSymbol() && m_head.asSymbol() == "apply") {
			return handle_apply(env);
		}
		// handle map procedure
		else if (m_head.isSymbol() && m_head.asSymbol() == "map") {
			return handle_map(env);
		}
		// handle map procedure
		else if (m_head.isSymbol() && m_head.asSymbol() == "set-property") {
			return handle_set_property(env);
		}
		// handle map procedure
		else if (m_head.isSymbol() && m_head.asSymbol() == "get-property") {
			return handle_get_property(env);
		}
		// handle map procedure
		else if (m_head.isSymbol() && m_head.asSymbol() == "discrete-plot") {
			return handle_discrete_plot(env);
		}
		else if (m_head.isSymbol() && m_head.asSymbol() == "continuous-plot") {
			return handle_continuous_plot(env);
		}
		else {
			// else attempt to treat as procedure
			std::vector<Expression> results;
			for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
				results.push_back(it->eval(env));
			}
			//evaluate lambda function
			if (!m_tail.empty() && env.is_exp(m_head)) {
				//create temporary environment
				Environment lambdaEnv = env;
				//make sure the correct amount of parameters are used
				if (env.get_exp(m_head).getValueInTail(0).getTailLength() != results.size()) {
					throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
				}
				//delete repeated expressions
				for (unsigned int i = 0; i < env.get_exp(m_head).getValueInTail(0).getTailLength(); i++) {
					if (env.is_exp(env.get_exp(m_head).getValueInTail(0).getValueInTail(i).head())) {
						lambdaEnv.delete_exp(env.get_exp(m_head).getValueInTail(0).getValueInTail(i).head());
					}
					lambdaEnv.add_exp(env.get_exp(m_head).getValueInTail(0).getValueInTail(i).head(), results[i]);
				}
				return env.get_exp(m_head).getValueInTail(1).eval(lambdaEnv);
			}
			return apply(m_head, results, env);
		}
	}
}

std::ostream & operator<<(std::ostream & out, const Expression & exp){
  Environment env;

	  if (!exp.head().isComplex() && !exp.head().isNone() && !exp.head().isError()) {
		  out << "(";
	  }
	  if (!exp.head().isList() && !exp.head().isLambda() && !exp.head().isNone()) {
		  out << exp.head();
		  if (env.is_proc(exp.head()) || exp.head().isLambda()) {
			  out << " ";
		  }
	  }
	  if (exp.head().isNone()) {
		  out << "NONE";
	  }

	  auto f=exp.tailConstBegin();
	  for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
		out << *e;
		f = e+1;
		if (f != exp.tailConstEnd()) {
			out << " ";
		}
	  }

	  if (!exp.head().isComplex() && !exp.head().isNone() && !exp.head().isError()) {
		  out << ")";
	  }
  return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
	(lefte != m_tail.end()) && (righte != exp.m_tail.end());
	++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}

Expression  Expression::getValueInTail(unsigned int location) {
	if (location >= m_tail.size()) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}
	return m_tail[location];
}

unsigned int Expression::getTailLength() {
	return m_tail.size();
}

Expression Expression::getProperty(Atom & a) {
	if (propertymap.find(a.asString()) != propertymap.end()) {
		return propertymap.find(a.asString())->second;
	}
	else {
		throw SemanticError("Error during evaluation: This property was not set for this expression");
	}
}

bool Expression::checkProperty(Atom & a){
	if (propertymap.find(a.asString()) != propertymap.end()) {
		return true;
	}
	else {
		return false;
	}

}

bool Expression::checkValidCoordinates(){

	for (unsigned int i = 0; i < getTailLength(); i++) {
		if (!getValueInTail(i).isHeadList()) {
			return false;
		}
		else if (getValueInTail(i).getTailLength() != 2) {
			return false;
		}
		else if (!(getValueInTail(i).m_tail[0].isHeadNumber() && getValueInTail(i).m_tail[1].isHeadNumber())) {
			return false;
		}
	}
	return true;
}

std::list<Expression> Expression::handlePoints(double minX, double maxX ,double minY, double maxY) {
	Expression xCoordinate;
	Expression yCoordinate;
	std::list<Expression> coordinateList;

	Expression coordinate;
	std::list<Expression> allCoordinates;
	double xRange = maxX - minX;
	double yRange = maxY - minY;

	for (unsigned int i = 0; i < getTailLength(); i++) {
		xCoordinate = Atom(((getValueInTail(i).m_tail[0].head().asNumber())/xRange)*20);
		yCoordinate = Atom((-((getValueInTail(i).m_tail[1].head().asNumber()))/yRange)*20);
		coordinateList.emplace_back(xCoordinate);
		coordinateList.emplace_back(yCoordinate);
		coordinate = coordinateList;
		coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"point\""))));
		coordinate.propertymap.emplace(std::string("\"size\""), Expression(Atom(0.5)));
		allCoordinates.emplace_back(coordinate);
		coordinateList.clear();
	}

	return allCoordinates;
}

std::list<Expression> Expression::buildRect(double minX, double maxX, double minY, double maxY) {
	Expression xCoordinate;
	Expression yCoordinate;
	std::list<Expression> coordinateList;
	std::list<Expression> line;
	Expression coordinate;
	std::list<Expression> allCoordinates;
	double xRange = maxX - minX;
	double yRange = maxY - minY;

	xCoordinate = Atom(((minX ) / xRange) * 20);
	yCoordinate = Atom(-((minY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	xCoordinate = Atom(((minX ) / xRange) * 20);
	yCoordinate = Atom(-((maxY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	coordinate = line;
	coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
	coordinate.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
	allCoordinates.emplace_back(coordinate);
	line.clear();

	xCoordinate = Atom(((minX ) / xRange) * 20);
	yCoordinate = Atom(-((minY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	xCoordinate = Atom(((maxX ) / xRange) * 20);
	yCoordinate = Atom(-((minY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	coordinate = line;
	coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
	coordinate.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
	allCoordinates.emplace_back(coordinate);
	line.clear();

	xCoordinate = Atom(((maxX ) / xRange) * 20);
	yCoordinate = Atom(-((minY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	xCoordinate = Atom(((maxX ) / xRange) * 20);
	yCoordinate = Atom(-((maxY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	coordinate = line;
	coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
	coordinate.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
	allCoordinates.emplace_back(coordinate);
	line.clear();

	xCoordinate = Atom(((minX ) / xRange) * 20);
	yCoordinate = Atom(-((maxY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	xCoordinate = Atom(((maxX ) / xRange) * 20);
	yCoordinate = Atom(-((maxY ) / yRange) * 20);
	coordinateList.emplace_back(xCoordinate);
	coordinateList.emplace_back(yCoordinate);
	coordinate = coordinateList;
	line.emplace_back(coordinate);
	coordinateList.clear();
	coordinate = line;
	coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
	coordinate.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
	allCoordinates.emplace_back(coordinate);
	line.clear();

	return allCoordinates;
}

std::list<Expression> Expression::buildOrigin(double minX, double maxX, double minY, double maxY) {
	Expression xCoordinate;
	Expression yCoordinate;
	std::list<Expression> coordinateList;
	std::list<Expression> line;
	Expression coordinate;
	std::list<Expression> allCoordinates;
	double xRange = maxX - minX;
	double yRange = maxY - minY;

	if (minX <= 0 && maxX>=0) {
		xCoordinate = Atom(((0 ) / xRange) * 20);
		yCoordinate = Atom(-((minY ) / yRange) * 20);
		coordinateList.emplace_back(xCoordinate);
		coordinateList.emplace_back(yCoordinate);
		coordinate = coordinateList;
		line.emplace_back(coordinate);
		coordinateList.clear();
		xCoordinate = Atom(((0 ) / xRange) * 20);
		yCoordinate = Atom(-((maxY ) / yRange) * 20);
		coordinateList.emplace_back(xCoordinate);
		coordinateList.emplace_back(yCoordinate);
		coordinate = coordinateList;
		line.emplace_back(coordinate);
		coordinateList.clear();
		coordinate = line;
		coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
		coordinate.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
		allCoordinates.emplace_back(coordinate);
		line.clear();
	}

	if (minY <= 0 && maxY>=0) {
		xCoordinate = Atom(((minX ) / xRange) * 20);
		yCoordinate = Atom(-((0 ) / yRange) * 20);
		coordinateList.emplace_back(xCoordinate);
		coordinateList.emplace_back(yCoordinate);
		coordinate = coordinateList;
		line.emplace_back(coordinate);
		coordinateList.clear();
		xCoordinate = Atom(((maxX ) / xRange) * 20);
		yCoordinate = Atom(-((0 ) / yRange) * 20);
		coordinateList.emplace_back(xCoordinate);
		coordinateList.emplace_back(yCoordinate);
		coordinate = coordinateList;
		line.emplace_back(coordinate);
		coordinateList.clear();
		coordinate = line;
		coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
		coordinate.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
		allCoordinates.emplace_back(coordinate);
		line.clear();
	}

	return allCoordinates;
}

std::list<Expression> Expression::buildStems(double minX, double maxX, double minY, double maxY) {
	Expression xCoordinate;
	Expression yCoordinate;
	std::list<Expression> coordinateList;
	std::list<Expression> line;
	Expression coordinate;
	std::list<Expression> allCoordinates;
	double xRange = maxX - minX;
	double yRange = maxY - minY;
	double bottom=0;

	for (unsigned int i = 0; i < getTailLength(); i++) {
		if (getValueInTail(i).m_tail[1].head().asNumber() < 0) {
			if (maxY < 0) {
				bottom = maxY;
			}
		}
		else {
			if (minY > 0) {
				bottom = minY;
			}
		}
		xCoordinate = Atom(((getValueInTail(i).m_tail[0].head().asNumber() ) / xRange) * 20);
		yCoordinate = Atom(-((bottom ) / yRange) * 20);
		coordinateList.emplace_back(xCoordinate);
		coordinateList.emplace_back(yCoordinate);
		coordinate = coordinateList;
		line.emplace_back(coordinate);
		coordinateList.clear();
		xCoordinate = Atom(((getValueInTail(i).m_tail[0].head().asNumber() ) / xRange) * 20);
		yCoordinate = Atom(-((getValueInTail(i).m_tail[1].head().asNumber() ) / yRange) * 20);
		coordinateList.emplace_back(xCoordinate);
		coordinateList.emplace_back(yCoordinate);
		coordinate = coordinateList;
		line.emplace_back(coordinate);
		coordinateList.clear();
		coordinate = line;
		coordinate.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
		coordinate.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
		allCoordinates.emplace_back(coordinate);
		line.clear();
	}

	return allCoordinates;
}

std::list<Expression> Expression::listAxis(double minX, double maxX, double minY, double maxY, double textScale) {

	Expression xPoint;
	Expression yPoint;
	std::list<Expression> textPoint;
	Expression myPoint;
	Expression myText;
	std::list<Expression> axisLabels;
	std::stringstream myPrecisionXmin;
	std::stringstream myPrecisionXmax;
	std::stringstream myPrecisionYmin;
	std::stringstream myPrecisionYmax;

	double minX1 = (minX) / (maxX - minX) * 20;
	double maxX1 = (maxX) / (maxX - minX) * 20;
	double minY1 = (minY) / (maxY - minY) * 20;
	double maxY1 = (maxY) / (maxY - minY) * 20;


	myPrecisionXmin << std::setprecision(2) << minX;
	myPrecisionXmax << std::setprecision(2) << maxX;
	myPrecisionYmin << std::setprecision(2) << minY;
	myPrecisionYmax << std::setprecision(2) << maxY;

	xPoint = Atom(minX1);
	textPoint.emplace_back(xPoint);
	yPoint = Atom(-minY1+2);
	textPoint.emplace_back(yPoint);
	myPoint = textPoint;
	myText = Atom(std::string("\"") + myPrecisionXmin.str() + std::string("\""));
	myText.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"text\""))));
	myText.propertymap.emplace(std::string("\"position\""), myPoint);
	myText.propertymap.emplace(std::string("\"scale\""), Expression(Atom(textScale)));
	textPoint.clear();
	axisLabels.emplace_back(myText);


	xPoint = Atom(minX1-2);
	textPoint.emplace_back(xPoint);
	yPoint = Atom(-minY1);
	textPoint.emplace_back(yPoint);
	myPoint = textPoint;
	myText = Atom(std::string("\"") + myPrecisionYmin.str() + std::string("\""));
	myText.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"text\""))));
	myText.propertymap.emplace(std::string("\"position\""), myPoint);
	myText.propertymap.emplace(std::string("\"scale\""), Expression(Atom(textScale)));
	textPoint.clear();
	axisLabels.emplace_back(myText);


	xPoint = Atom(minX1 - 2);
	textPoint.emplace_back(xPoint);
	yPoint = Atom(-maxY1);
	textPoint.emplace_back(yPoint);
	myPoint = textPoint;
	myText = Atom(std::string("\"") + myPrecisionYmax.str() + std::string("\""));
	myText.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"text\""))));
	myText.propertymap.emplace(std::string("\"position\""), myPoint);
	myText.propertymap.emplace(std::string("\"scale\""), Expression(Atom(textScale)));
	textPoint.clear();
	axisLabels.emplace_back(myText);


	xPoint = Atom(maxX1);
	textPoint.emplace_back(xPoint);
	yPoint = Atom(-minY1+2);
	textPoint.emplace_back(yPoint);
	myPoint = textPoint;
	myText = Atom(std::string("\"") + myPrecisionXmax.str() + std::string("\""));
	myText.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"text\""))));
	myText.propertymap.emplace(std::string("\"position\""), myPoint);
	myText.propertymap.emplace(std::string("\"scale\""), Expression(Atom(textScale)));
	textPoint.clear();
	axisLabels.emplace_back(myText);
	return axisLabels;
}

std::list<Expression> Expression::listLabels(double minX, double maxX, double minY, double maxY, double textScale) {

	std::list<Expression> resultList;
	std::list<Expression> buildList;
	std::string title;
	std::string xAxis;
	std::string yAxis;
	for (unsigned int i = 0; i < getTailLength(); i++) {
		if (m_tail[i].m_tail[0].head().asString() == "\"title\"") {
			if (!m_tail[i].m_tail[1].isHeadString()) {

			}
			else {
				title = m_tail[i].m_tail[1].head().asString();

			}
		}
		if (m_tail[i].m_tail[0].head().asString() == "\"abscissa-label\"") {
			if (!m_tail[i].m_tail[1].isHeadString()) {

			}
			else {
				xAxis = m_tail[i].m_tail[1].head().asString();
			}
		}
		if (m_tail[i].m_tail[0].head().asString() == "\"ordinate-label\"") {
			if (!m_tail[i].m_tail[1].isHeadString()) {

			}
			else {
				yAxis = m_tail[i].m_tail[1].head().asString();
			}
		}
	}
	if (title.length() != 0) {
		buildList.emplace_back(Expression(Atom(((minX) / (maxX - minX) * 20) + 10)));
		buildList.emplace_back(Expression(Atom((-(maxY) / (maxY - minY) * 20) - 3)));
		Expression builderTitle(buildList);
		buildList.clear();
		Expression myText = Atom(title);
		myText.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"text\""))));
		myText.propertymap.emplace(std::string("\"scale\""), Expression(Atom(textScale)));
		myText.propertymap.emplace(std::string("\"position\""), builderTitle);
		resultList.emplace_back(myText);
	}
	if (xAxis.length() != 0) {
		buildList.emplace_back(Expression(Atom(((minX) / (maxX - minX) * 20) + 10)));
		buildList.emplace_back(Expression(Atom((-(minY) / (maxY - minY) * 20) + 3)));
		Expression builderTitle(buildList);
		buildList.clear();
		Expression myText = Atom(xAxis);
		myText.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"text\""))));
		myText.propertymap.emplace(std::string("\"scale\""), Expression(Atom(textScale)));
		myText.propertymap.emplace(std::string("\"position\""), builderTitle);
		resultList.emplace_back(myText);
	}
	if (yAxis.length() != 0) {
		buildList.emplace_back(Expression(Atom(((minX) / (maxX - minX) * 20) - 3)));
		buildList.emplace_back(Expression(Atom((-(minY) / (maxY - minY) * 20) - 10)));
		Expression builderTitle(buildList);
		buildList.clear();
		Expression myText = Atom(yAxis);
		myText.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"text\""))));
		myText.propertymap.emplace(std::string("\"scale\""), Expression(Atom(textScale)));
		myText.propertymap.emplace(std::string("\"rotation\""), Expression(Atom((std::atan2(-1, 0)))));
		myText.propertymap.emplace(std::string("\"position\""), builderTitle);
		resultList.emplace_back(myText);
	}
	return resultList;
}

std::list<Expression>  Expression::buildLines(double minX, double maxX, double minY, double maxY) {
	
	std::list<Expression> resultList;
	std::vector<Expression> adjustedX;
	std::vector<Expression> adjustedY;
	Expression firstLineCoordinate;
	Expression secondLineCoordinate;
	Expression xPointCoordinate;
	Expression yPointCoordinate;
	std::list<Expression> pointList;
	std::list<Expression> lineList;
	Expression line;
	Expression point;
	std::vector<Expression> myPoints;
	for (unsigned int i = 0; i < getTailLength(); i++) {
		adjustedX.emplace_back(Expression(Atom(m_tail[i].m_tail[0].head().asNumber() / (maxX - minX) * 20)));
		adjustedY.emplace_back(Expression(Atom(-m_tail[i].m_tail[1].head().asNumber() / (maxY - minY) * 20)));
	}
	for (unsigned int i = 0; i < adjustedX.size(); i++) {
		xPointCoordinate= adjustedX[i];
		yPointCoordinate= adjustedY[i];
		pointList.emplace_back(xPointCoordinate);
		pointList.emplace_back(yPointCoordinate);
		point = pointList;
		myPoints.emplace_back(point);
		pointList.clear();
	}

	for (unsigned int i = 0; i <myPoints.size()-1; i++) {
		firstLineCoordinate =myPoints[i];
		secondLineCoordinate = myPoints[i+1];
		lineList.emplace_back(firstLineCoordinate);
		lineList.emplace_back(secondLineCoordinate);
		line = lineList;
		line.propertymap.emplace(std::string("\"object-name\""), Expression(Atom(std::string("\"line\""))));
		line.propertymap.emplace(std::string("\"thickness\""), Expression(Atom(0.0)));
		resultList.emplace_back(line);
		lineList.clear();
	}

	return resultList;
}

Expression Expression::fixAngles(double count, Expression function, Environment env) {
	if (count < 1) {
		std::list<Expression> resultList;
		double point1x;
		double point2x;
		double point3x;
		double point1y;
		double point2y;
		double point3y;
		double angle;
		double newPoint1x;
		double newPoint1y;
		double newPoint2x;
		double newPoint2y;

		double p12;
		double p13;
		double p23;
		Expression myList(Atom(std::string("list")));
		Expression myMap(Atom(std::string("map")));
		resultList.emplace_back(m_tail[0]);
		for (unsigned int i = 1; i < getTailLength() - 1; i++) {

			point1x = m_tail[i - 1].m_tail[0].head().asNumber();
			point2x = m_tail[i].m_tail[0].head().asNumber();
			point3x = m_tail[i + 1].m_tail[0].head().asNumber();
			point1y = m_tail[i - 1].m_tail[1].head().asNumber();
			point2y = m_tail[i].m_tail[1].head().asNumber();
			point3y = m_tail[i + 1].m_tail[1].head().asNumber();

			p12 = sqrt(pow((point1x - point2x),2) + pow((point1y - point2y), 2));
			p13 = sqrt(pow((point1x - point3x), 2) + pow((point1y - point3y), 2));
			p23 = sqrt(pow((point3x - point2x), 2) + pow((point3y - point2y), 2));

			angle = acos((pow(p12, 2) + pow(p23, 2) - pow(p13, 2)) / (2 * p12*p23));

			angle = angle * 180 / (std::atan2(0, -1));

			if (angle < 175) {

				newPoint1x = point2x + (point1x - point2x)/2;
				newPoint2x = point2x - (point2x - point3x)/2;

				myList.m_tail.emplace_back(Expression(Atom(newPoint1x)));
				myList.m_tail.emplace_back(Expression(Atom(newPoint2x)));
				myMap.m_tail.emplace_back(function);
				myMap.m_tail.emplace_back(myList);

				Expression yPoints(myMap.eval(env));

				newPoint1y = yPoints.m_tail[0].head().asNumber();
				newPoint2y = yPoints.m_tail[1].head().asNumber();
				Expression point1(Atom(std::string("list")));
				Expression point2(Atom(std::string("list")));
				point1.m_tail.emplace_back(newPoint1x);
				point1.m_tail.emplace_back(newPoint1y);
				point2.m_tail.emplace_back(newPoint2x);
				point2.m_tail.emplace_back(newPoint2y);
				resultList.emplace_back(point1.eval(env));
				resultList.emplace_back(m_tail[i]);
				resultList.emplace_back(point2.eval(env));
				myList.m_tail.clear();
				myMap.m_tail.clear();

				m_tail.insert(m_tail.begin() + i+1, point2.eval(env));
				i++;
			}
			else {
				resultList.emplace_back(m_tail[i]);
			}
		}

		resultList.emplace_back(m_tail[getTailLength()-1]);
		Expression myResult(resultList);

		myResult = myResult.fixAngles(count + 1, function, env);
		return myResult;
	}
	return *this;
}

double Expression::getMinX() {

	double lastX = getValueInTail(0).m_tail[0].head().asNumber();
	double X;
	for (unsigned int i = 1; i < getTailLength(); i++) {
		X = getValueInTail(i).m_tail[0].head().asNumber();
		if (X < lastX) {
			lastX = X;
		}
	}
	return lastX;
}

double Expression::getMaxX() {
	double lastX = getValueInTail(0).m_tail[0].head().asNumber();
	double X;
	for (unsigned int i = 1; i < getTailLength(); i++) {
		X = getValueInTail(i).m_tail[0].head().asNumber();
		if (X > lastX) {
			lastX = X;
		}
	}
	return lastX;
}

double Expression::getMinY() {
	double lastY = getValueInTail(0).m_tail[1].head().asNumber();
	double Y;
	for (unsigned int i = 1; i < getTailLength(); i++) {
		Y = getValueInTail(i).m_tail[1].head().asNumber();
		if (Y < lastY) {
			lastY = Y;
		}
	}
	return lastY;
}

double Expression::getMaxY() {
	double lastY = getValueInTail(0).m_tail[1].head().asNumber();
	double Y;
	for (unsigned int i = 1; i < getTailLength(); i++) {
		Y = getValueInTail(i).m_tail[1].head().asNumber();
		if (Y > lastY) {
			lastY = Y;
		}
	}
	return lastY;
}