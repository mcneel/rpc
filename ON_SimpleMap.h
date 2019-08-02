
#pragma once
#ifndef LBPRHLIB
#error RHLIB header included in non-Rhino compile
#endif

template<class KEY, unsigned int HashTableSize>
__forceinline unsigned int OSM_HashIndex(const KEY& key)
{
	return ON_CRC16(0, sizeof(KEY), &key) % HashTableSize;
}

template <class KEY, class VALUE, unsigned int HashTableSize = 256>
class ON_SimpleMap
{
private:
	struct HashElement
	{
		HashElement* m_next;
		KEY          m_key;
		VALUE        m_value;
	};

public:
	ON_SimpleMap()
		: m_block_list(NULL),
		  m_element_list(NULL),
		  m_iCount(0)
	{
		memset(m_table, 0, sizeof(m_table));
	}

	~ON_SimpleMap() { Destroy(); }

	VALUE* 	Lookup(const KEY&) const;
	bool    Lookup(const KEY&, VALUE&) const;
	void    Destroy(void);
	bool    Remove(const KEY&); // **************** Do not call Remove() during iteration of the map.
	bool    SetAt(const KEY&, const VALUE&);
	bool    IsEmpty(void) const;
	UINT_PTR Count(void) const;

	class Iterator
	{
	public:
		Iterator(HashElement* const * t) : table(t), index(0), element(NULL) {}

		VALUE* Next(void);
		bool   Next(VALUE&);
		bool   Next(KEY&, VALUE&);
		void   Reset(void) { index = 0; element=NULL; }

	protected:
		HashElement* const * table;
		HashElement* element;
		INT_PTR index;
	};

	// **************** Do not call Remove() during iteration of the map.
	Iterator GetIterator(void) const { return Iterator(m_table); }

private:
	__forceinline int HashIndex(const KEY& key) const { return OSM_HashIndex<KEY, HashTableSize>(key); }

	struct HashElementBlock
	{
		HashElementBlock* m_next;
		HashElement       m_memory_block[1024];
	};

	HashElement*      m_table[HashTableSize];
	HashElement*      m_element_list;     // unused elements
	HashElementBlock* m_block_list;       // unused blocks
	UINT_PTR m_iCount;

	VALUE GetElementObject(const HashElement*) const;

private:
	// no implementation
	ON_SimpleMap(const ON_SimpleMap&);
	ON_SimpleMap& operator=(const ON_SimpleMap&);
};

template <class KEY, class VALUE, unsigned int HashTableSize>
void ON_SimpleMap<KEY, VALUE, HashTableSize>::Destroy(void)
{
	HashElementBlock* blk = m_block_list;
	while (NULL != blk)
	{
		void* p = blk;
		blk = blk->m_next;
		onfree(p);
	}

	m_element_list = NULL;
	m_block_list = NULL;
	memset(m_table, 0, sizeof(m_table));

	m_iCount = 0;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
bool ON_SimpleMap<KEY, VALUE, HashTableSize>::Lookup(const KEY& key, VALUE& val) const
{
	VALUE* p = Lookup(key);

	if (NULL == p)
		return false;

	val = *p;

	return true;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
VALUE* ON_SimpleMap<KEY, VALUE, HashTableSize>::Lookup(const KEY& key) const
{
	const int hash_index = HashIndex(key);

	HashElement* elem = m_table[hash_index];
	while (NULL != elem)
	{
		if (key == elem->m_key)
			return &elem->m_value;

		elem = elem->m_next;
	}

	return NULL;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
bool ON_SimpleMap<KEY, VALUE, HashTableSize>::Remove(const KEY& key)
{
	const int hash_index = HashIndex(key);

	HashElement* eprev = NULL;
	HashElement* elem = m_table[hash_index];
	while (NULL != elem)
	{
		if (key == elem->m_key)
		{
			if (NULL != eprev)
			{
				eprev->m_next = elem->m_next;
			}
			else
			{
				m_table[hash_index] = elem->m_next;
			}

			elem->m_next = m_element_list;
			m_element_list = elem;

			m_iCount--;

			return true;
		}

		eprev = elem;
		elem = elem->m_next;
	}

	return false;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
bool ON_SimpleMap<KEY, VALUE, HashTableSize>::SetAt(const KEY& key, const VALUE& value)
{
	VALUE* p = Lookup(key);
	if (NULL != p)
	{
		*p = value;
		return true;
	}

	if (NULL == m_element_list)
	{
		HashElementBlock* pBlock = (HashElementBlock*)onmalloc(sizeof(HashElementBlock));
		memset(pBlock, 0, sizeof(HashElementBlock));

		for (int i = 0; i < 1023; i++)
		{
			pBlock->m_memory_block[i].m_next = &pBlock->m_memory_block[i+1];
		}

		pBlock->m_next = m_block_list;
		m_block_list = pBlock;
		m_element_list = pBlock->m_memory_block;
	}

	const int hash_index = HashIndex(key);

	HashElement* elem = m_element_list;
	m_element_list = m_element_list->m_next;

	elem->m_key = key;
	elem->m_value = value;
	elem->m_next = m_table[hash_index];

	m_table[hash_index] = elem;

	m_iCount++;

	return true;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
UINT_PTR ON_SimpleMap<KEY, VALUE, HashTableSize>::Count(void) const
{
	return m_iCount;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
bool ON_SimpleMap<KEY, VALUE, HashTableSize>::IsEmpty(void) const
{
	return 0 == m_iCount;

	/*for (int i = 0; i < HashTableSize; i++)
	{
		if (NULL != m_table[i])
			return false;
	}

	return true;*/
}

template <class KEY, class VALUE, unsigned int HashTableSize>
bool ON_SimpleMap<KEY, VALUE, HashTableSize>::Iterator::Next(VALUE& value)
{
	VALUE* p = Next();
	if(NULL == p) return false;
	value = *p;
	return true;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
VALUE* ON_SimpleMap<KEY, VALUE, HashTableSize>::Iterator::Next(void)
{
	if (NULL != element)
	{
		element = element->m_next;
	}

	while (NULL == element)
	{
		if (index == HashTableSize)
			return NULL;

		element = table[index++];
	}

	return &element->m_value;
}

template <class KEY, class VALUE, unsigned int HashTableSize>
bool ON_SimpleMap<KEY, VALUE, HashTableSize>::Iterator::Next(KEY& key, VALUE& value)
{
	if (NULL != element)
	{
		element = element->m_next;
	}

	while (NULL == element)
	{
		if (index == HashTableSize)
			return false;

		element = table[index++];
	}

	if(NULL == element)
		return false;

	value = element->m_value;
	key = element->m_key;

	return true;
}

template <class VALUE>
class ON_SimpleUuidMap : public ON_SimpleMap<UUID, VALUE, 256>
{
};
