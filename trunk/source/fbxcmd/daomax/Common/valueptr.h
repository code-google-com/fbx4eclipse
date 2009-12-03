/**********************************************************************
*<
FILE: ValuePtr.h

DESCRIPTION:	ValuePtr and ValueRef classes

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once


#pragma region ValuePtr

template< class T > class ValuePtr;
template< class T > class ValueRef;

// --------------------------------------------------------------------
//
// Auxiliary traits class template providing default clone()
//   Users should specialize this template for types that have their
//   own self-copy operations; failure to do so may lead to slicing!
//
// --------------------------------------------------------------------
template< class T >
struct VPTraits  {
	static  T *  create()  { return new T(); }
	static  T *  clone( T const * p )  { return new T( *p ); }
	static  T *  construct( T const * p )  { return const_cast<T*>(p); }
	static  T *  release( T * p )  { delete(p); return NULL; }
};  // VPTraits

// --------------------------------------------------------------------
//
// Copyable smart pointer class template
//
// --------------------------------------------------------------------
template< class T >
class ValuePtr  {
public:
	// --------------------------------------------------
	// Default constructor/destructor:
	// --------------------------------------------------
	explicit ValuePtr( ) : myP ( VPTraits<T>::create() ) { ; }
	explicit ValuePtr( T * p ) { myP = VPTraits<T>::construct(p); }
	~ValuePtr()  { myP = VPTraits<T>::release(myP); }

	// --------------------------------------------------
	// Copy constructor/copy assignment:
	// --------------------------------------------------

	ValuePtr( ValuePtr const & orig )
		: myP( createFrom( orig.myP ) )
	{}

	ValuePtr &  operator = ( ValuePtr const & orig )  {
		ValuePtr<T>  temp( orig );
		swap( temp );
		return *this;
	}

	// --------------------------------------------------
	// Access mechanisms:
	// --------------------------------------------------
	bool isNull() const { return (myP == NULL); }

	T &  operator *  () const  { return *myP; }
	T *  operator -> () const  { return  myP; }

	T *  ToPointer() const     { return  myP; }

	// --------------------------------------------------
	// Manipulation:
	// --------------------------------------------------

	void  swap( ValuePtr & orig )  { std::swap( myP, orig.myP ); }

	// --------------------------------------------------
	// Copy-like construct/assign from compatible ValuePtr<>:
	// --------------------------------------------------

	template< class U >
	ValuePtr( ValuePtr<U> const & orig )
		: myP( createFrom( orig.operator->() ) )
	{}

	template< class U >
	ValuePtr &  operator = ( ValuePtr<U> const & orig )  {
		ValuePtr<T>  temp( orig );
		swap( temp );
		return *this;
	}

private:
	friend class ValueRef<T>;

	// --------------------------------------------------
	// Implementation aid:
	// --------------------------------------------------

	template< class U >
	T *  createFrom( U const * p ) const  {
		return p
			? VPTraits<U>::clone( p )
			: 0;
	}

	// --------------------------------------------------
	// Member data:
	// --------------------------------------------------

	T *  myP;

};  // ValuePtr


// --------------------------------------------------------------------
//
// Free-standing swap()
//
// --------------------------------------------------------------------
namespace std {
	template< class T >
	inline
		void  swap(  ValuePtr<T> & vp1, ValuePtr<T> & vp2 )  {
			vp1.swap( vp2 );
	}
}
#pragma endregion

#pragma region ValuePtr
// --------------------------------------------------------------------
//
// Auxiliary traits class template providing default clone()
//   Users should specialize this template for types that have their
//   own self-copy operations; failure to do so may lead to slicing!
//
// --------------------------------------------------------------------
template< class T >
struct VRTraits  {
	static  T *  create()  {return  NULL; }
	static  T *  clone( T const * p )  { return const_cast<T*>(p); }
	static  T *  release( T * p )  { return NULL; }
};  // VPTraits


// --------------------------------------------------------------------
//
// Copyable smart pointer class template
//
// --------------------------------------------------------------------
template< class T >
class ValueRef  {
public:
	// --------------------------------------------------
	// Default constructor/destructor:
	// --------------------------------------------------
	ValueRef( ) : myP ( VRTraits<T>::create() ) { ; }
	ValueRef( T * p ) : myP( p )  { ; }
	ValueRef( const ValuePtr<T>& p ) : myP( p.ToPointer() )  { ; }
	~ValueRef()  { myP = VRTraits<T>::release(myP); }

	// --------------------------------------------------
	// Copy constructor/copy assignment:
	// --------------------------------------------------

	ValueRef( ValueRef const & orig )
		: myP( createFrom( orig.myP ) )
	{}

	ValueRef &  operator = ( ValueRef const & orig )  {
		ValueRef<T>  temp( orig );
		swap( temp );
		return *this;
	}

	ValueRef &  operator = ( ValuePtr<T> const & orig )  {
		ValueRef<T>  temp( orig );
		swap( temp );
		return *this;
	}

	// --------------------------------------------------
	// Access mechanisms:
	// --------------------------------------------------
	bool isNull() const { return (myP == NULL); }

	T &  operator *  () const  { return *myP; }
	T *  operator -> () const  { return  myP; }

	T *  ToPointer() const     { return  myP; }

	// --------------------------------------------------
	// Manipulation:
	// --------------------------------------------------

	void  swap( ValueRef & orig )  { std::swap( myP, orig.myP ); }

	// --------------------------------------------------
	// Copy-like construct/assign from compatible ValueRef<>:
	// --------------------------------------------------

	template< class U >
	ValueRef( ValueRef<U> const & orig )
		: myP( createFrom( orig.operator->() ) )
	{}

	template< class U >
	ValueRef &  operator = ( ValueRef<U> const & orig )  {
		ValueRef<T>  temp( orig );
		swap( temp );
		return *this;
	}

private:

	// --------------------------------------------------
	// Implementation aid:
	// --------------------------------------------------

	template< class U >
	T *  createFrom( U const * p ) const  {
		return p ? VRTraits<U>::clone( p ) : 0;
	}

	// --------------------------------------------------
	// Member data:
	// --------------------------------------------------

	T *  myP;

};  // ValuePtr


// --------------------------------------------------------------------
//
// Free-standing swap()
//
// --------------------------------------------------------------------
namespace std {
	template< class T >
	inline
		void  swap(  ValueRef<T> & vp1, ValueRef<T> & vp2 )  {
			vp1.swap( vp2 );
	}
}
#pragma endregion
