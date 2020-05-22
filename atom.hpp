/*! \file atom.hpp
Defines the Atom type and associated functions.
 */
#ifndef ATOM_HPP
#define ATOM_HPP

#include "token.hpp"
#include <complex>
#include <list>

class Expression;
/*! \class Atom
\brief A variant type that may be a Number or Symbol or the default type None.

This class provides value semantics.
*/
class Atom {
public:

  /// Construct a default Atom of type None
  Atom();

  /// Construct an Atom of type Number with value
  Atom(double value);

  /// Construct an Atom of type Symbol or Lambda named value
  Atom(const std::string & value); 

  /// Construct an Atom of type Complex named value
  Atom(const std::complex<double> & value);

  /// Construct an Atom of type List named value
  Atom(const bool & value);

  /// Construct an Atom directly from a Token
  Atom(const Token & token);

  /// Copy-construct an Atom
  Atom(const Atom & x);

  /// Assign an Atom
  Atom & operator=(const Atom & x);

  /// Atom destructor
  ~Atom();

  /// predicate to determine if an Atom is of type None
  bool isNone() const noexcept;

  /// predicate to determine if an Atom is of type Number
  bool isNumber() const  noexcept;

  /// predicate to determine if an Atom is of type Symbol
  bool isSymbol() const noexcept;

  /// predicate to determine if an Atom is of type Complex
  bool isComplex() const noexcept;

  /// predicate to determine if an Atom is of type List
  bool isList() const noexcept;

  /// predicate to determine if an Atom is of type Lambda
  bool isLambda() const noexcept;

  /// predicate to determine if an Atom is of type Lambda
  bool isString() const noexcept;

  /// predicate to determine if an Atom is of type Lambda
  bool isError() const noexcept;

  /// value of Atom as a number, return 0 if not a Number
  double asNumber() const noexcept;

  /// value of Atom as a number, returns empty-string if not a Symbol
  std::string asSymbol() const noexcept;

  /// value of Atom as a complex, returns 0 if not a Complex
  std::complex<double> asComplex() const noexcept;

  /// value of Atom as a list, returns false if not a List
  bool asList() const noexcept;

  /// value of Atom as a lambda, returns empty-string if not a Lambda
  std::string asLambda() const noexcept;

  /// value of Atom as a number, returns empty-string if not a string
  std::string asString() const noexcept;

  /// value of Atom as a number, returns empty-string if not a error
  std::string asError() const noexcept;

  /// equality comparison based on type and value
  bool operator==(const Atom & right) const noexcept;

private:

  // internal enum of known types
  enum Type {NoneKind, NumberKind, SymbolKind, ComplexKind, ListKind, LambdaKind, StringKind, ErrorKind};

  // track the type
  Type m_type;

  // values for the known types. Note the use of a union requires care
  // when setting non POD values (see setSymbol)
  union {
    double numberValue;
	std::string stringValue;
	std::complex<double> complexValue;
	bool listValue;
	std::string lambdaValue;
	std::string errorValue;
  };

  // helper to set type and value of Number
  void setNumber(double value);

  // helper to set type and value of Symbol
  void setSymbol(const std::string & value);

  // helper to set type and value of Complex
  void setComplex(const std::complex<double> & value);

  // helper to set type and value of List
  void setList(const bool & value);

  // helper to set type and value of Lambda
  void setLambda(const std::string & value);

  // helper to set type and value of string
  void setString(const std::string & value);

  // helper to set type and value of error
  void setError(const std::string & value);

};

/// inequality comparison for Atom
bool operator!=(const Atom &left, const Atom & right) noexcept;

/// output stream rendering
std::ostream & operator<<(std::ostream & out, const Atom & a);

#endif
