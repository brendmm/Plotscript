/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <map>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;
/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty) 
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  /*! Construct an Expression with given list of Expressions 
  as a tail and true boolean as head
  \param list of expression to make the tail
  */
  Expression(const std::list<Expression> & a);

  /*! Construct an Expression with given vector of Expressions
  as a tail and string lambda as head
  \param vector of expression to make the tail
  */
  Expression(const std::vector<Expression> & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// convienience member to determine if head atom is a complex
  bool isHeadComplex() const noexcept;

  /// convienience member to determine if head atom is a list
  bool isHeadList() const noexcept;

  /// convienience member to determine if head atom is a lambda
  bool isHeadLambda() const noexcept;

  /// convienience member to determine if head atom is a lambda
  bool isHeadString() const noexcept;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;

  /// function that gives expression in tail at specified location
  Expression  getValueInTail(unsigned int location);

  /// function that gives size of tail
  unsigned  int getTailLength();

  ///function that returns property paired with atom
  Expression getProperty(Atom & a);

  ///function that checks if there is property paired with atom
  bool checkProperty(Atom & a);

private:

  // the head of the expression
  Atom m_head;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  std::map<std::string, Expression> propertymap;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;
  
  // internal helper methods


  //Handle Special Expression
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_lambda(Environment & env);
  Expression handle_apply(Environment & env);
  Expression handle_map(Environment & env);
  Expression handle_set_property(Environment & env);
  Expression handle_get_property(Environment & env);
  Expression handle_discrete_plot(Environment & env);
  Expression handle_continuous_plot(Environment & env);


  //Help with Creating Plots
  bool checkValidCoordinates();
  std::list<Expression> handlePoints(double minX, double maxX, double minY, double maxY);
  std::list<Expression> buildRect(double minX, double maxX, double minY, double maxY);
  std::list<Expression> buildOrigin(double minX, double maxX, double minY, double maxY);
  std::list<Expression> buildStems(double minX, double maxX, double minY, double maxY);
  std::list<Expression> listAxis(double minX, double maxX, double minY, double maxY, double textScale);
  std::list<Expression> listLabels(double minX, double maxX, double minY, double maxY, double textScale);
  std::list<Expression> buildLines(double minX, double maxX, double minY, double maxY);
  Expression fixAngles(double count, Expression function, Environment env);
  double getMinX();
  double getMaxX();
  double getMaxY();
  double getMinY();
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;
  
#endif
