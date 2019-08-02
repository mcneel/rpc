#pragma once


class CRpcDocument
{
public:
	CRpcDocument(void);
	~CRpcDocument(void);

public:
	int Version(void);

public:
	int AnimationFrame(void) const;
	void SetAnimationFrame(CRhinoDoc& doc, int iFrame);

	void Defaults(void);

public:
	bool ReadFromArchive(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileReadOptions& options);
	bool WriteToArchive(CRhinoDoc& doc, ON_BinaryArchive& archive, const CRhinoFileWriteOptions& options);

private:
	int m_iAnimationFrame;
};
