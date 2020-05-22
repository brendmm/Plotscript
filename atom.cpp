#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>
#include <iostream>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){

  setNumber(value);
}

Atom::Atom(const Token & token): Atom(){
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0])){
		bool startQuote = false;
		bool endQuote = false;
		bool middleQuote = true;
		int quoteCount = 0;
		if (token.asString()[0] == '"') {
			startQuote = true;
		}
		if (token.asString()[token.asString().length() - 1] == '"') {
			endQuote = true;
		}
		for (unsigned int i = 0; i < token.asString().length(); i++) {
			if (token.asString()[i] == '"') {
				quoteCount++;
			}
		}
		if (quoteCount == 2) {
			middleQuote = false;
		}
		if (startQuote && endQuote && !middleQuote) {
			setString(token.asString());
		}
		else {
			setSymbol(token.asString());
		}

    }
  }
}

Atom::Atom(const std::string & value): Atom() {
  if (value == "lambda") {
	  setLambda(value);
  }
  else if (value.front() == '"') {
	  setString(value);
  }
  else if (value.substr(0,5) == "error") {
	  setError(value.substr(5,value.size()-2));
  }
  else {
	  setSymbol(value);
  }
}

Atom::Atom(const std::complex<double> & value) {
	setComplex(value);
}

Atom::Atom(const bool & value) {
	setList(value);
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if (x.isComplex()) {
	  setComplex(x.complexValue);
  }
  else if (x.isList()) {
	  setList(x.listValue);
  }
  else if (x.isLambda()) {
	  setLambda(x.lambdaValue);
  }
  else if (x.isString()) {
	  setString(x.stringValue);
  }
  else if (x.isError()) {
	  setError(x.errorValue);
  }
}

Atom & Atom::operator=(const Atom & x){
  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
	else if (x.m_type == ComplexKind) {
		setComplex(x.complexValue);
	}
	else if (x.m_type == ListKind) {
		setList(x.listValue);
	}
	else if (x.m_type == LambdaKind) {
		setLambda(x.lambdaValue);
	}
	else if (x.m_type == StringKind) {
		setString(x.stringValue);
	}
	else if (x.m_type == ErrorKind) {
		setError(x.errorValue);
	}
  }
  return *this;
}
  
Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
  else if (m_type == LambdaKind) {
	 lambdaValue.~basic_string();
  }
  else if (m_type == StringKind) {
	  stringValue.~basic_string();
  }
  else if (m_type == ErrorKind) {
	  errorValue.~basic_string();
  }

}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}  

bool Atom::isComplex() const noexcept {
	return m_type == ComplexKind;
}

bool Atom::isList() const noexcept {
	return m_type == ListKind;
}

bool Atom::isLambda() const noexcept {
	return m_type == LambdaKind;
}

bool Atom::isString() const noexcept {
	return m_type == StringKind;
}

bool Atom::isError() const noexcept {
	return m_type == ErrorKind;
}

void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setSymbol(const std::string & value){
  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
    
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setComplex(const std::complex<double> & value){

	m_type = ComplexKind;
	complexValue = value;
}

void Atom::setList(const bool & value) {

	m_type = ListKind;

	new (&listValue) bool(value);
}

void Atom::setLambda(const std::string & value) {
	if (m_type == ListKind) {
	}

	m_type = LambdaKind;

	// copy construct in place
	new (&lambdaValue) std::string(value);
}

void Atom::setString(const std::string & value) {
	if (m_type == StringKind) {
	}

	m_type = StringKind;

	// copy construct in place
	new (&stringValue) std::string(value);
}

void Atom::setError(const std::string & value) {
	if (m_type == ErrorKind) {
	}

	m_type =ErrorKind;

	// copy construct in place
	new (&errorValue) std::string(value);
}

double Atom::asNumber() const noexcept{

  return (m_type == NumberKind) ? numberValue : 0.0;  
}


std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind){
    result = stringValue;
  }

  return result;
}

std::complex<double> Atom::asComplex() const noexcept {
	std::complex<double> emptyComplex(0.0, 0.0);
	return (m_type == ComplexKind) ? complexValue : emptyComplex;
}

bool Atom::asList() const noexcept {

	bool result = false;

	if (m_type == ListKind) {

		result = listValue;
		return result;
	}

	return result;
}

std::string Atom::asLambda() const noexcept {

	std::string result;

	if (m_type ==LambdaKind) {
		result = stringValue;
	}

	return result;
}

std::string Atom::asString() const noexcept {

	std::string result;

	if (m_type == StringKind) {
		result = stringValue;
	}

	return result;
}

std::string Atom::asError() const noexcept {

	std::string result;

	if (m_type == ErrorKind) {
		result = errorValue;
	}

	return result;
}

bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) ||
	 (diff > std::numeric_limits<double>::epsilon())) return false;
    }
    break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;

      return stringValue == right.stringValue;
    }
	break;
  case ComplexKind:
  {
	 if (right.m_type != ComplexKind) return false;
	  std::complex<double> dleft = complexValue;
	  std::complex<double> dright = right.complexValue;
	  double diff = abs(dleft - dright);
	  if (std::isnan(diff) ||
		  (diff > std::numeric_limits<double>::epsilon())) return false;
  }
	break;
  case ListKind:
  {
	  if (right.m_type != ListKind) return false;

  }
    break;
  case LambdaKind:
  {
	  if (right.m_type != LambdaKind) return false;

  }
  break;
  case StringKind:
  {
	  if (right.m_type != StringKind) return false;

	  return stringValue == right.stringValue;
  }
  break;
  case ErrorKind:
  {
	  if (right.m_type != ErrorKind) return false;

	  return errorValue == right.errorValue;
  }
  break;
  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}

std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  if (a.isComplex()) {
	  out << a.asComplex();
  }
  if (a.isList()) {
	  out << a.asList();
  }
  if (a.isLambda()) {
	  out << a.asLambda();
  }
  if (a.isString()) {
	  out << a.asString();
  }
  if (a.isError()) {
	  out << a.asError();
  }
  return out;
}
