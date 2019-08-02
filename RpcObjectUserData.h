#pragma once


class CRpcObjectUserData : public CLBPRh_XMLUserData
{
private:
	ON_OBJECT_DECLARE(CRpcObjectUserData);

public:
	CRpcObjectUserData(void);
	CRpcObjectUserData(const CRpcObjectUserData& ud);
	virtual ~CRpcObjectUserData(void);

	const CRpcObjectUserData& operator = (const CRpcObjectUserData& ud);

public:
	bool operator==(const CRpcObjectUserData& src);
	bool operator!=(const CRpcObjectUserData& src);

public:
	static UUID Id(void);
	int VersionNumber() const;

public:
	void RpcData(CLBPBuffer& buf) const;
	void SetRpcData(const CLBPBuffer& buf);
	
private:
	// CLBPRh_XMLUserData overrides
	virtual void SetToDefaults() const;
	virtual void ReportVersionError(void) const;

	bool Archive(void) const override;
	bool Write(ON_BinaryArchive&) const override;
	bool Read(ON_BinaryArchive&) override;
};
