/**********************************************************************
*<
FILE: GFFList.h

DESCRIPTION:	GR2 File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once 

#include "GFFCommon.h"
#include "GFFField.h"

#include <pshpack1.h>

namespace DAO {
namespace GFF {

///<summary>
/// Implementation of a struct
///</summary>
class GFFList
{
protected:
	GFFFieldDefRef define;
	GFFStructRef owner;
	void *data;
	int len;
protected:
	friend class GFFField;
	GFFList(GFFStructRef owner, GFFFieldDefRef define);
public:
	static GFFList* Construct(GFFStructRef owner, GFFFieldDefRef define);
	~GFFList();

	virtual GFFFieldDefRef getDefinition() const { return define; }
	virtual size_t elementsize() const = 0;
	virtual LPCVOID pdata() const;
	virtual size_t size() const;
	virtual size_t count() const;
	virtual void resize(size_t len);

	virtual Text asText(int idx);
	virtual signed char asInt8(int idx);
	virtual unsigned char asUInt8(int idx);
	virtual signed short asInt16(int idx);
	virtual unsigned short asUInt16(int idx);
	virtual signed long asInt32(int idx);
	virtual unsigned long asUInt32(int idx);
	virtual signed __int64 asInt64(int idx);
	virtual unsigned __int64 asUInt64(int idx);
	virtual float asFloat32(int idx);
	virtual double asFloat64(int idx);
	virtual Vector3f asVector3f(int idx);
	virtual Vector4f asVector4f(int idx);
	virtual Quaternion asQuat(int idx);
	virtual Text asECString(int idx);
	virtual Color4 asColor4(int idx);
	virtual Matrix44 asMatrix4(int idx);
	virtual Text asTlkString(int idx);
	virtual GFFFieldRef asField(int idx);
	virtual GFFStructRef asStruct(int idx);

	virtual void assign(int idx, char rhs);
	virtual void assign(int idx, unsigned char rhs);
	virtual void assign(int idx, short rhs);
	virtual void assign(int idx, unsigned short rhs);
	virtual void assign(int idx, int rhs);
	virtual void assign(int idx, unsigned int rhs);
	virtual void assign(int idx, float rhs);
	virtual void assign(int idx, double rhs);
	virtual void assign(int idx, __int64 rhs);
	virtual void assign(int idx, unsigned __int64 rhs);
	virtual void assign(int idx, LPCSTR rhs);
	virtual void assign(int idx, LPCWSTR rhs);
	virtual void assign(int idx, LPVOID rhs, int n);
	virtual void assign(int idx, const Vector3f& rhs);
	virtual void assign(int idx, const Vector4f& rhs);
	virtual void assign(int idx, const Quaternion& rhs);
	virtual void assign(int idx, const Color4& rhs);
	virtual void assign(int idx, const Matrix44& rhs);

	virtual size_t CopyData(LPVOID dst, size_t size, int idx);
	virtual int CopyText(LPTSTR value, int len, int idx);


	virtual bool Recall(IDAOStream& in);
	virtual bool Store(IDAOStream& in) const;
	virtual void Dump(IDAODumpStream& out, LPCTSTR name) const;

};
typedef ValueRef<GFFList> GFFListRef;
typedef ValuePtr<GFFList> GFFListPtr;



/*!
* Dynamically cast from a collection of objects to another collection
* \param objs A collection of object references to be dynamically casted to the specified type.
* \return A collection of objects that support the requested type.
*/
template <typename T>
inline bool DynamicCast( const GFFListRef& list, DAOArray< ValuePtr<T> >& retval ) {
	size_t sz = 0;
	if (list.isNull()) return false;
	GFFFieldDefRef define = list->getDefinition();
	if (define->Flags & FF_LIST && define->TypeID == DT_GENERIC) {
		for (size_t i=0, n=list->count(); i<n; ++i) {
			GFFFieldRef field = list->asField(i);
			if (!field.isNull()){
				GFFStructRef ptr = field->asStruct();
				if ( !ptr.isNull() && 0 == ptr->getDefinition()->StructType.Compare(T::Type(), true) ) {
					DAOArray< ValuePtr<T> >::iterator itr = retval.insert(retval.end(), ValuePtr<T>(NULL));
					(*itr).swap( ValuePtr<T>( new T( ptr ) ) );
					++sz;
				}
			}
		}
	} else if (define->Flags & FF_LIST && define->Flags & FF_STRUCT) {
		for (size_t i=0, n=list->count(); i<n; ++i) {
			GFFStructRef ptr = list->asStruct(i);
			if ( !ptr.isNull() && 0 == ptr->getDefinition()->StructType.Compare(T::Type(), true) ) {
				DAOArray< ValuePtr<T> >::iterator itr = retval.insert(retval.end(), ValuePtr<T>(NULL));
				(*itr).swap( ValuePtr<T>( new T( ptr ) ) );
				++sz;
			}
		}
	}
	return (sz > 0);
}

/*!
* Dynamically cast from a collection of objects to another collection
* \param objs A collection of object references to be dynamically casted to the specified type.
* \return A collection of objects that support the requested type.
*/
template <typename T>
inline ValuePtr<T> GetFirstOfType( const GFFListRef& list ) {
	GFFFieldDefRef define = list->getDefinition();
	if (define->Flags & FF_LIST && define->TypeID == DT_GENERIC) {
		for (size_t i=0, n=list->count(); i<n; ++i) {
			GFFFieldRef field = list->asField(i);
			if (!field.isNull()){
				GFFStructRef ptr = field->asStruct();
				if ( !ptr.isNull() && 0 == ptr->getDefinition()->StructType.Compare(T::Type(), true) ) {
					return ValuePtr<T>( new T( ptr ) );
				}
			}
		}
	} else if (define->Flags & FF_LIST && define->Flags & FF_STRUCT) {
		for (size_t i=0, n=list->count(); i<n; ++i) {
			GFFStructRef ptr = list->asStruct(i);
			if ( !ptr.isNull() && 0 == ptr->getDefinition()->StructType.Compare(T::Type(), true) ) {
				return ValuePtr<T>( new T( ptr ) );
			}
		}
	}
	return ValuePtr<T>(NULL);
}

} // namespace GFF
} // namespace DAO

ROOTVARTYPE(DAO::GFF::GFFList);

#include <poppack.h>
