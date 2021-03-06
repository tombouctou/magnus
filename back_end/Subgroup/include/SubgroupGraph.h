// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition and implementation of the SubgroupGraph class.
//           A SubgroupGraph is a user class for representing
//           a finitely generated subgroup of a free group.
//           The representation of the subgroup is as a graph, which
//           can be used as a finite state automaton accepting words
//           in the subgroup.
//           There are methods which transmute the graph into a represen-
//           tation of a different subgroup; see, e.g., MHallComplete().
//
// Principal Author: Roger Needham
//
// Status: in progress
//
// Revision History:
//
// * 9/94 Roger wrote this new class to wrap C. Miller's original graph code.
//
// * 05/97 Dmitry B. implemented IPC tools.
//
// Bugs:
//
// * SubgroupGraph(int ambientRank, const SetOf<Word>& S)
//   can lose if any words in S are not freely reduced.
//
// Special Notes:
//
// * For the sake of efficiency all functions of SubgroupGraphRep work
//   only with freely reduced words.
//
// Next implementation steps:
//


#ifndef _SUBGROUPGRAPH_H_
#define _SUBGROUPGRAPH_H_


#include "SubgroupGraphRep.h"


class SubgroupGraph : public ObjectOf<SubgroupGraphRep> {
  
public:

  typedef SubgroupGraphRep::VertexType VertexType;

  typedef SubgroupGraphRep::LabelType LabelType;

  SubgroupGraph(int ambientRank, const SetOf<Word>& S) :
    ObjectOf<SubgroupGraphRep>( new SubgroupGraphRep(ambientRank, S) ) { }
  // Construct a subgroup graph from the rank of the ambient free group,
  // and the subgroup generators.

  SubgroupGraph(int ambientRank, const VectorOf<Word>& V) :
    ObjectOf<SubgroupGraphRep>( new SubgroupGraphRep(ambientRank, V) ) { }
  // Construct a subgroup graph from the rank of the ambient free group,
  // and the subgroup generators.

  // Copy constructor, operator=, and destructor supplied by compiler.

  int rank( ) const { return look()->rank(); }
  // Returns the rank of this subgroup as a free group.

  VectorOf<Word> normalizer( ) { return enhance()->normalizer(); }

  VectorOf<Word> nielsenBasis( ) const { return enhance()->nielsenBasis(); }
  // Returns a Nielsen basis for this subgroup as a free group.

  Word nielsenWord(int i) const { return enhance()->nielsenWord(i); }
  // Returns an  i^th element of the Nielsen basis.

  Word inNielsenWords(const Word& w) const { 
    return enhance()->inNielsenWords(w); 
  }
  // Returns the word `w' written in elements of the Nielsen basis.

  SubgroupGraph join(const SubgroupGraph& SG) const {
	 return SubgroupGraph( look()->join( *(SG.look()) ) );
  }
  // Returns a SubgroupGraph which represents the join of this
  // subgroup and the argument (i.e. the subgroup generated by the
  // union of generating sets for the two subgroups).

  SubgroupGraph intersection(const SubgroupGraph& SG) const {
	 return SubgroupGraph( look()->intersection( *(SG.look()) ) );
  }
  // Returns a SubgroupGraph which represents the intersection of
  // this subgroup and the argument.

  Bool contains(const Word& w) const { return look()->contains(w); }
  // Returns TRUE iff this subgroup contains `w'.

  Bool contains(const SetOf<Word>& S) const { return look()->contains(S); }
  // Returns TRUE iff this subgroup contains the subgroup generated by `S'.

  Bool contains(const VectorOf<Word>& V) const { return look()->contains(V); }
  // Returns TRUE iff this subgroup contains the subgroup generated by `V'.

  Bool contains(SubgroupGraph& SG) const { 
         return look()->contains(*SG.change());
  }

  Bool equalTo(const SetOf<Word>& S) { return enhance()->equalTo(S); }
  // Returns TRUE iff this subgroup and the subgroup generated by `S' are equal.

  Bool equalTo(SubgroupGraph& SG) { 
         return enhance()->equalTo(*SG.enhance());
  }
  // Returns TRUE iff this subgroup and the argument are equal.

  Bool conjugateInSubgroup(const Word& w, Word& conjugator) const {
         return look()->conjugateInSubgroup(w,conjugator);
  }
  // Returns TRUE iff some conjugate of `w' is in the subgroup.
  // If TRUE, `conjugator' is set to the conjugator.

  Bool conjugateInSubgroup(const SetOf<Word>& S, Word& conjugator) {
         return enhance()->conjugateInSubgroup(S,conjugator);
  }
  // Returns TRUE iff some conjugate of the subgroup generated by `S' is
  // in the subgroup. If TRUE, `conjugator' is set to the conjugator.

  bool conjugateTo(const SetOf<Word>& S) {
    return enhance()->conjugateTo(S);
  }
  // Returns true iff this subgroup and the argument are conjugate.

  long powerInSubgroup( const Word& w ) const {
         return look()->powerInSubgroup(w);
  }
  // returns `the minimal power' or 0 if there are no powers of the
  // element `aWord' in H.

  int findIndex() { return enhance()->findIndex(); }
  // Returns the index of the subgroup or 0 if infinite.

  VectorOf<Word> findWhiteheadBasis() { 
    return enhance()->findWhiteheadBasis(); 
  }
  // Finds the subgroup of the free group authomorphic to this with
  // smallest sum of lengths of generators.
  // Returns a vector of generators.

  Bool isAFreeFactor() { return enhance()->isAFreeFactor(); }
  // Returns TRUE iff this subgroup is a free factor.

  Bool generatesTheFreeGroup() const { 
	 return look()->generatesTheFreeGroup(); 
  }

  Word rightSchreierRepresentative(const Word& w) {
	 return enhance()->rightSchreierRepresentative(w);
  }

  SubgroupGraph MHallCompletion( ) const {
	 SubgroupGraphRep* copy = new SubgroupGraphRep(*(look()));
	 copy->MHallComplete();
	 return SubgroupGraph(copy);
  }
  // Returns a SubgroupGraph which represents a subgroup of the
  // ambient free group of finite index, containing this one.
  // This one is a free factor of the result.

  void MHallComplete( ) {
	 change()->MHallComplete();
  }
  // An `in place' version of above.

  void joinConjugate(Generator g) {
	 change()->joinConjugate( ord(g) );
  }
  // Alters this SubgroupGraph in place so that it also contains the
  // conjugate by `g' of every element it contained before.

  float completeness( ) const { return look()->completeness(); }
  // Returns ratio of existing edges to number of edges in complete graph.

  Bool isComplete( ) const { return look()->isComplete(); }
  // Returns TRUE iff this graph is complete (can't trust floats!).

  VertexType vertexCount( ) const { return look()->vertexCount(); }

  #ifdef DEBUG
    void debugPrint(ostream& ostr) const {
	   look()->debugPrint(ostr);
    }
    Bool consistentData( ) const {
	   return look()->consistentData();
    }
  #endif


  VertexType targetOfGenerator(VertexType source, int generator) const {
	 return look()->targetOfGenerator(source, generator);
  }


  VertexType targetOfLabel(VertexType source, LabelType label) const {
	 return look()->targetOfLabel(source, label);
  }


  long getValence( ) const { return look()->getValence(); }


  LabelType inverseLabel(LabelType label) const {
    return look()->inverseLabel(label);
  }

  int labelToGenerator(LabelType label) const {
    return look()->labelToGenerator(label);
  }

  LabelType generatorToLabel(int g) const {
    return look()->generatorToLabel(g);
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < ( ostream& ostr, const SubgroupGraph& g )
  {
    g.look()->write(ostr);
    return ostr;
  }
  
  friend istream& operator > ( istream& istr, SubgroupGraph& g )
  {
    g.change()->read(istr);
    return istr;
  }

  bool readPiece( istream& istr, const class Timer& timer ) {
    return change()->readPiece(istr,timer);
  }
  // To read a big amount of information piece by piece. Returns true
  // if the last is read, false otherwise.


//@dbprivate:
protected:

  SubgroupGraph(SubgroupGraphRep* SGRp) : ObjectOf<SubgroupGraphRep>(SGRp) { }

};

#endif
