// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of class QEqnSolutionsInFreeGroup
//
// Principal Author: Dmitry Pechkin, Eugeny Paderin
//
// Status: Under trial
//
// Description:
//
// * The program works with equations with (and without) quotients.
//   We used algorithm described in [1].
//
// Here is a brief description of Comerford & Edmunds main ideas.
//
// Any solution of W = 1 over group G can be represented as S*P*E,
// where S is an automorphism mapping W to itself (i.e. element of
// stabilizer of W in Aut(G), P is a certain endomorphism from a
// finite set of "basic solutions", and E is any "specializing"
// endomorphism. According to Comerford & Edmunds (C&E), we can
// choose P as some combinations of elementary Nielsen transformations
// (to be exact, elementary regular automorphisms (ERA) x -> xy, not
// lengthening the word, and elementary singular endomorphisms (ESE)
// x -> 1, shortening the word; here x is a variable). In this case,
// instead of stabilizer we can take its regular subset RegStab,
// generated by regular automorphisms (RA).
//
// Let W be a minimal word (i.e. it cannot be shortened by RA). We
// build a graph where vertices are images of W (their number is finite),
// and edges are ERA mapping one word to another. Then elements of
// RegStab are all the reduced paths starting and ending at W; if we
// find some (any) maximal subtree in the graph, then any edge not
// included in the tree defines certain generator of RegStab.
//
// If there is a path (RA) R leading from W to some vertex V, and T
// is a SE mapping V to 1, then TR is a solution of W=1. C&E say
// that all these TR make a set of basic solutions of W=1.
//
// If W is not minimal, then we take the surface form of W that is proved
// to be minimal. C&E say: if V is some minimal image of W
// and N is RA mapping W to V, then solutions of W and V are the same
// up to N. So we can find a stabilizer and basic solutions for V
// and then multiply them to N (if S is a generator of RegStab(V),
// then NSN^-1 is a generator of RegStab(W), and if P is a basic
// solution of V then NP is one of W).
//
// Usage:
//  
//   A brief scheme of usage:
// 
//   QEqnSolutions equation(givenGroup, givenWord, numberOfVariables);
//
//   equation.startComputation();                // initialization
//   while( !equation.isComputationDone() ) {
//     equation.continueComputation();           // do one step
//
//     while( equation.haveNewSolution() ) {     // take found solutions
//       Endomorphism solution = equation.getSolution();
//       // do something with the solution...
//       equation.nextSolution();
//     }
//
//     while( haveNewStabGenerator() ) {         // take found generators
//       Automorphism regStabGen = equation.getStabGenerator();
//       // do something with the generator...
//       equation.nextStabGenerator();
//     }
//   }
//
// References:
//
//   [1]. L.Comerford, C.Edmunds, Solutions of equations in free groups,
//        in Group Theory: proceedings of the Singapore Group Theory
//        Conference held at the National University of Singapore, 
//        June 8-19, 1987.   ISBN 3-11-011366-X.
//
//   [2]. R.I.Grigorchuk, P.F.Kurchanov, On Quadratic Equations in Free
//        groups. Contemporary Mathematics, vol. 131, 1992 (part I),
//        pp. 159-171.
//
// Revision History:
//
// * 10/96 EP added a routine checkEquation.
//   
// Special Notes:
//
// * Costness of algorithm is exponential by number of length of equation
//   (by memory and time requests). So an equation given by the user
//   may not be computed completely, but some basic solutions and generators
//   of regular stabiliser can be obtained by user at the running time,
//   without waiting for complete computation. 
//
// * The algorithm is very general and thus rather slow; in future, some
//   dedicated methods for special cases will be implemented, so the whole
//   structure of the class can be changed considerably.

#ifndef _QEQN_SOLUTIONS_H_
#define _QEQN_SOLUTIONS_H_

#include "Word.h"
#include "Vector.h"
#include "Queue.h"
#include "VectorPtr.h"
#include "QuickAssociations.h"
#include "NielsenTransformations.h"

class QEqnSolutionsInFreeGroup
{
public:

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // no default constructor

  QEqnSolutionsInFreeGroup(const FreeGroup& G, const Word& equation, int numOfVar);
  // Construct quadratic equation with given word in given group G
  // which is a formal group generated by variables and constants:
  // the first numOfVar generators are treated as variables,
  // other generators of the group are constants.

  // copy constructor, operator=, and destructor supplied by compiler

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Activation members for solving the equation:                        //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  void startComputation();
  void continueComputation();
  // These two methods launch and perform process of solving the equation.
  // After performing one step, some new basic solutions and generators
  // of RegStab are found, and user can require them. 
  // Simultaneously the rank of equation is computed,
  // but it is available only after the computation is completed.

  void disableBuildingRegStab() { status.enableBuildingRegStab = false; }
  void enableBuildingRegStab() { status.enableBuildingRegStab = true; }
  // Building of regular stabilizer is enabled by default. User can
  // disable this, accelerating performance and saving memory. After disabling,
  // all the following steps do not return corresponding generators and
  // all related information is lost. So when user disables the RegStab
  // building and then enables it again, one loses some generators.

  void disableBuildingSolutions() { status.enableBuildingSolutions = false; }
  void enableBuildingSolutions() { status.enableBuildingSolutions = true; }
  // The same about building partial solutions.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Status queries:                                                     //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool isComputationDone() const { return computationIsDone; }
  // Returns true iff computation is finished.

  bool isComputationStarted() const { return computationIsStarted; }
  // Returns true iff `startComputation' have been invoked.

  bool haveNewSolution() const { return solution<theBasicSolutions.length(); }
  // Returns true iff at least one new basic solution is available.

  bool nextSolution() {	
    if( !haveNewSolution() ) return false;
    solution++; 
    return true;
  }
  // Advances pointer to next basic solution, so that getSolution returns it.
  // Returns false iff there is no new solution.

  bool haveNewStabGenerator() const 
  { return generator<theRegStabGenerators.length(); }
  // Returns true iff at least one new generator of RegStab is available.

  bool nextStabGenerator() {
    if( !haveNewStabGenerator() ) return false;
    generator++;
    return true;
  }
  // Advances pointer to next generator, so that getStabGenerator returns it.
  // Returns false iff there is no new generator.
	
  bool isBuildingSolutionsEnabled() const { 
    return status.enableBuildingSolutions; 
  }

  bool isBuildingRegStabEnabled() const { 
    return status.enableBuildingRegStab; 
  }

  //////////////////////////////////////////////////////////////////////
  //                                                                  //
  // Accessors                                                        //
  //                                                                  //
  //////////////////////////////////////////////////////////////////////

  Endomorphism getSolution() {
    if( !haveNewSolution() )
      error("QEqnSolutionsInFreeGroup::getSolution(): attempt to retrieve "
	    "a new basic solution before it is computed.");
    Endomorphism sol = startPath | Endomorphism(theBasicSolutions[solution]);
    sol.reduceGenImages();
    return sol;
  }
  // Returns found basic solution.
  // @dp This should be const, but Map::operator| is not const.

  Automorphism getStabGenerator() {
    if( !haveNewStabGenerator() )
      error("QEqnSolutionsInFreeGroup::getStabGenerator(): attempt to retrieve"
	    " a new RegStab generator before it is computed.");
    Automorphism gen = startPath | 
      Automorphism(theRegStabGenerators[generator]) | invStartPath; 
    gen.reduceGenImages();
    return gen;
  }
  // Returns found generator of RegStab.
  // @dp This should be const, but Map::operator| is not const.

  Word surfaceForm() { 
    if( !surfaceFormIsComputed ) computeSurfaceForm();
    return theSurfaceForm;
  }
  // Returns a surface form of the equation.
  // @dp logically must be const.

  Automorphism toSurfaceForm() { 
    if( !surfaceFormIsComputed ) computeSurfaceForm();
    return toSurface;
  }
  // Returns an (regular) automorphism bringing the equation into a surface
  // form.
  // @dp logically must be const.

  FreeGroup group() const { return theGroup; }
  Word equation() const { return theEquation; }
  int numberOfVariables() const { return theNumberOfVariables; }
  int numberOfGenerators() const { return theNumberOfGenerators; }
  int numberOfConstants() const { return theNumberOfGenerators-theNumberOfVariables; }

  int rank() const { return theRank; }
  // returns -1 if rank is not computed yet, otherwise non-negative number 
  // which is the computed rank of the equation.

  VectorPtrOf<Endomorphism> basicSolutions() const { return theBasicSolutions;}
  // Returns all known basic solutions of the equation.
  // NB. This function returns `stripped' basic solutions. To get `true'
  // solutions the user must append (left) the prefixMap() automorphism.

  VectorPtrOf<Automorphism> regStabGenerators() const { return theRegStabGenerators; }
  // Returns all known generators of regular stabilizer of the equation.
  // NB. This function returns `stripped' generators. To get proper
  // generators the user must conjugate each of them by the prefixMap() 
  // automorphism.

  Automorphism prefixMap() const { return startPath; }
  // Returns automorphsim bringing the equation into a minimal form.
  // The minimal form is not necessary to be identical to the surface 
  // (canonic) form.

  struct EquationStatus; // forward declaration.

  EquationStatus getProcessStatus() const { return status; }
  // Returns full status of the computation process.


  Endomorphism eliminator() const { return theEliminator; }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator << ( ostream&, const QEqnSolutionsInFreeGroup& );
  // Outputs all known basic solutions in a form readable by the end user.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Type definitions:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  // Process status structure.

  struct EquationStatus
  {
    EquationStatus() : enableBuildingSolutions(true), 
      enableBuildingRegStab(true), verticesToPass(0), verticesPassed(0), 
      loopsToPass(0), loopsPassed(0), totalLengthOfLoops(0) {}

    // copy contsructor, operator=, destructor supplied by compiler.

    // data members:

    bool enableBuildingSolutions;
    bool enableBuildingRegStab;
    int verticesToPass;
    int verticesPassed;
    int loopsToPass;
    int loopsPassed;
    long totalLengthOfLoops;
  };

  //@njz: moved 'Edge' definition from private to public due to complain in 
  //'SetData<T>::hashElement' in QEqnSolutions.C
  struct Edge {

    // edge of a graph of regular transforms of the equation; includes
    // source vertex

    Word vertex;
    ElementaryRegularAuto edge;

    Edge() : edge(1,2) {} // `dummy' default constructor for use in Sets

    Edge(const Word& w, const ElementaryRegularAuto& e) : vertex(w), edge(e) 
    {}

    friend inline bool operator ==(const Edge& x, const Edge& y) {
      return ( x.vertex == y.vertex && x.edge == y.edge );
    }

    friend ostream& operator <<( ostream& o, const Edge& E);
    // This operator is required by SetOf<Edge> class.
    // We don't need it, so prohibit the one.

    int hash() const { return vertex.hash(); }
  };
  //


private:

  //@njz
  //  class Edge; // forward declaration
  //

  Bool isVariable(const Generator& g) const { 
#if SAFETY > 0
    if( abs(ord(g)) > theNumberOfGenerators )
      error("Generator out of bounds in QEqnSolution::isVariable(Generator&)");
#endif
    return ( abs(ord(g)) <= theNumberOfVariables ? true : false );
  }
  // Returns true iff the given generator is a variable.

  bool isConstant(const Generator& g) const {
#if SAFETY > 0
    if( abs(ord(g)) > theNumberOfGenerators )
      error("Generator out of bounds in QEqnSolution::isConstant(Generator&)");
#endif
    return ( abs(ord(g)) > theNumberOfVariables ? true : false );
  }
  // Returns true iff the given generator is a constant.


  // METHODS OF TREE BUILDER

  void buildNewVerticesAndLoopsFrom(const Word& vertex);
  // Expands tree: builds all `offsprings' the given vertex.

  RegularAuto buildPath(const Word& from, const Word& to);
  // Builds a path from one vertex to another within the built subtree.

  RegularAuto buildPathTo(const Word& to);
  // Builds path from theEquation to the given word.

  void registerEdge(const Word& source, const ElementaryRegularAuto& edge, 
		    const Word& target);
  // Tries to include the given edge to the maximal tree. If the edge
  // forms a loop it certainly should not include; instead, it places the one
  // to the list of loops.


  // METHODS OF BASIC SOLUTIONS BUILDER

  void buildSolutionsOfOneVertex(const Word& w);
  // Builds basic solutions for the given word.

  VectorOf<int> checkProperties(const Word& w);
  // Checks for every variable of the group whether it can/must/cannot
  // be eliminated in given word

  ListOf<SingularEndo> buildSingulars(const Word& w, const VectorOf<int>& varProperties);
  // This procedure immediately builds a graph of eliminations for given word.
  // varProperties defines a behavior of every variables of the word.

  void appendNewSolution(ListOf<SingularEndo>& solutions, 
			 const SingularEndo& newSolution);
  // Includes a new singular solution to the list of solutions, unless it is a
  // specialization of one of the list.


  // METHODS OF RANK EQUATION BUILDER

  int rankOfBasicSolution(const Endomorphism& solution) const;
  // Computes rank of the given endomorphism.

  // METHODS OF REGSTAB BUILDER

  void processOneLoop(const Edge& edge);
  // Builds a generator of stabilizer by the given edge of graph.

  // METHODS OF SURFACE FORM BUILDER
  //@dp these descriptions is not quite fine, more detailed comments
  //    to appear.

  void computeSurfaceForm();
  // Computes surface form of the equation and an automorphism bringing
  // the one to the surface form.

  Word extractSquare(Word& w, int firstLocation, int secondLocation);
  // Rewrites given word by extracting square of variable at given locations.

  Word extractPinch(Word& w, int firstLocation,	int secondLocation);
  // Rewrites given word by extracting pinch of variable at given locations.

  Word extractCommutator(Word& w, int xFirstLocation,	int xSecondLocation, 
			 int yFirstLocation);
  // Rewrites given word by extracting commutator of variables at given 
  // locations.

  int getLocationOfArbitraryVariable(const Word& w, int start, int stop) const;
  // Returns location of an arbitrary variable which occurrs in the given 
  // non-empty word, otherwise returns -1.

  int getPairOfVariable(Word& w, int& firstLocation, int& secondLocation, 
			bool oppositeOrder) const;

  Word bringToCanonicForm(Word& squares, Word& commutators, Word constants, 
			  Word& pinches);
  // Last stage of computing a surface form and a surface automorphism.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  Word theEquation;           // the given equation
  int theNumberOfGenerators;
  int theNumberOfVariables;
  FreeGroup theGroup; 

  VectorPtrOf<Automorphism> theRegStabGenerators;
  VectorPtrOf<Endomorphism> theBasicSolutions;

  int theRank; // rank of given equation
  Word theSurfaceForm; // surface form of the equation
  Automorphism toSurface; // auto bringing into surface form

  // Control variables of the process

  bool computationIsStarted;
  bool computationIsDone;

  int solution; // number of last retrieved basic solution by the user
  int generator; // number of last retieved generator of RegStab by the user


  Endomorphism theEliminator;
  // This endomorphism eliminates variables mapping them to 1. Constants are mapped
  // to themselves.


  // TREE BUILDER PROPERTIES

  // The maximal subtree is represented with pairs (associations) of
  // a word (some image of the root) and an edge -- an elementary
  // regular transformation mapping the word to its PARENT. So,
  // edges in the tree are directed from offsprings to parents,
  // and we can easily reconstruct a regular automorphism mapping
  // the root to the vertex.
  // The tree is stored as associative array because we need fast search 
  // of vertices.


  Automorphism startPath; // an automorhism mapping the initial word into its
  // minimal form (root)
  Automorphism invStartPath;
  Word root;             // current root of the tree
  QuickAssociationsOf<Word, ElementaryRegularAuto> maxSubTree;
  // maximal subtree of the graph of regular transforms of
  // theEquation's minimal form (root).

  QueueOf<Word> newVertices; // new transforms to be proceeded

  QueueOf<Edge> loops; //edges not included into the subtree which define loops


  // BASIC SOLUTION BUILDER PROPERTIES

  Endomorphism removeVarEndo;
  // Endomorphism which removes all variables from a word and leaves
  // constants only.
  SetOf<Endomorphism> setOfBasicSolutions; 
  // Temporary fast access storage of basic solutions to prevent duplicate 
  // ones. 

  enum { MUST_BE_MAPPED_TO_ITSELF,
	 MUST_BE_MAPPED_TO_1,
	 CAN_BE_MAPPED_TO_1
  };
  // Behavior of variables of a vertex of the tree when a specialization 
  // is searched.


  // RANK BUILDER PROPERTIES

  int tempRank;
  // Make intermediate computed value of rank unavailable for the user
  // while computation is incomplete.

  // REGSTAB BUILDER PROPERTIES

  SetOf<Automorphism> regStabSet;
  // Generators of RegStab which are already found to prevent duplicate them. 

  // SURFACE FORM BUILDER

  bool surfaceFormIsComputed;

  // CLASS INFORMATION

  EquationStatus status;

#ifdef DEBUG
  void debugPrint( ostream& o ) const;
  //friend int main();
#endif

};


// This is a procedure converting Set into Vector. It's a double of
// a makeVectorOf(const ListOf<T>&) from conversions.h and also
// should be placed there.

template <class T> VectorPtrOf<T> makeVectorPtrOf(const SetOf<T>& S)
{
  VectorPtrOf<T> result( S.cardinality() );

  int i = 0;
  SetIterator<T> I(S);

  while ( !I.done() ) {
    result[i++] = I.value();
    I.next();
  }

  return result;
}


///////////////@ep added on October1996 /////////////////////////////////////////////

Chars  checkEquation(const FreeGroup& G, const Word& equation, int numOfVar);

#endif

