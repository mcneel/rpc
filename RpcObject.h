#pragma once


class CRpcObjectUserDataWrapper;
typedef CLBPRhObjectWrapperRef<CRpcObjectUserDataWrapper> CRpcObjectUserDataWrapperRef;


class CRpcObject
{
public:
	CRpcObject(const CRhinoObject * pRhinoObject);
	virtual ~CRpcObject(void);

public:
	UUID ObjectId(void) const;

	bool IsValid(void) const;

	bool Tag(const CLBPBuffer& buf);
	bool Untag(void);
	bool IsTagged(void) const;

	bool RpcData(CLBPBuffer& buf) const;
	void SetRpcData(const CLBPBuffer& buf);

private:
	void Construct(const CRhinoObject* pObject);

	CRpcObjectUserDataWrapper& Wrapper();
	const CRpcObjectUserDataWrapper& Wrapper() const;

private:
	CRpcObject(const CRpcObject& src);
	const CRpcObject& operator=(const CRpcObject& src);

private:
	CRpcObjectUserDataWrapper* m_pWrapperDirect;
	CRpcObjectUserDataWrapperRef* m_pWrapper;
	CRhinoPointObject* m_pDefaultObject;
};
