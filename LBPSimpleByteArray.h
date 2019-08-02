
#pragma once

template <typename T>
class CLBPSimpleArray
{
public:
	CLBPSimpleArray(size_t iInitialAllocationOfObjects = 100)
		: m_pBuffer((T*)malloc(iInitialAllocationOfObjects * sizeof(T))),
		  m_dwSizeOfAllocationInObjects(iInitialAllocationOfObjects),
		  m_iNumObjects(0)
	{}

	virtual ~CLBPSimpleArray()
	{
		if (m_pBuffer)
		{
			free(m_pBuffer);
		}
	}

public:
	void Append(const T* p, size_t iSize)
	{
		Allocate(m_iNumObjects + iSize);
		memcpy(m_pBuffer + m_iNumObjects, p, iSize*sizeof(T));
		m_iNumObjects += iSize;
	}


	void Append(T b)							{ Append(&b, 1); }
	size_t Count(void) const					{ return m_iNumObjects; }

	//operator const BYTE*() const				{ return m_pBuffer; }
	const T* Bytes(void) const				{ return m_pBuffer; }

private:
	size_t Allocate(size_t iTotalObjects, bool bExact = false)
	{
		if (iTotalObjects < m_dwSizeOfAllocationInObjects)
			return m_dwSizeOfAllocationInObjects;

		size_t dwNewAllocationInObjects = iTotalObjects;

		if (!bExact)
		{
			if (dwNewAllocationInObjects < 32)
			{
				// Never allocate less than 32 bytes.
				dwNewAllocationInObjects = 32;
			}
			else
				if (dwNewAllocationInObjects > 100000)
				{
					// If the allocation is really big, don't bloat it - just add a reasonable amount on the end.
					dwNewAllocationInObjects = dwNewAllocationInObjects + 10000;
				}
				else
				{
					dwNewAllocationInObjects = dwNewAllocationInObjects * 4;
				}
		}

		if (dwNewAllocationInObjects > m_dwSizeOfAllocationInObjects)
		{
			m_dwSizeOfAllocationInObjects = dwNewAllocationInObjects;

			// realloc size plus 1 to make sure we never realloc zero bytes; probably can't happen.
			const size_t dwNumBytes = (m_dwSizeOfAllocationInObjects + 1) * sizeof(T);
			m_pBuffer = (T*)LBPREALLOC(m_pBuffer, dwNumBytes);
		}

		return m_dwSizeOfAllocationInObjects;
	}

private:
	T* m_pBuffer;
	size_t m_iNumObjects = 0;
	size_t m_dwSizeOfAllocationInObjects;
};

using CLBPSimpleByteArray = CLBPSimpleArray<BYTE>;

