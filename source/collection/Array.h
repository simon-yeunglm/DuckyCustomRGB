#pragma once

// by simon yeung, 17/10/2020
// all rights reserved

#include <assert.h>
#include <memory>

#define ARRAY_PRE_ALLOC_SIZE	(4)

// a simple grow only array for POD data type
template <typename Type>
class Array
{
private:
	int		m_size;
	int		m_num;
	Type*	m_data;

	void preAlloc()
	{
		m_num	= 0;
		m_size	= ARRAY_PRE_ALLOC_SIZE;
		m_data	= (Type*)malloc(sizeof(Type)*m_size);
	}
	
public:

	Array()
	{
		preAlloc();
	}
	~Array()
	{
		free(m_data);
	}

	void clear(bool isKeepAllocMem= false)
	{
		if (isKeepAllocMem)
			m_num= 0;
		else
		{
			free(m_data);
			preAlloc();
		}
	}

	void reserve(int newSize)
	{
		if (newSize <= m_size)
			return;	// already have enough space
		Type* newData= (Type*)malloc(sizeof(Type)*newSize);
		assert(newData != nullptr);
		if (m_num)	// move existing data
			memcpy(newData, m_data, sizeof(Type)*m_num);
		free(m_data);
		m_data= newData;
		m_size= newSize;
	}

	void add(const Type& data)
	{
		assert(m_size > 0);
		if (m_num >= m_size)
			reserve(m_size * 2);
		m_data[m_num]= data;
		m_num= m_num + 1;
	}
	bool addUnique(const Type& data)
	{
		int num= m_num;
		for(int i=0; i<num; ++i)
			if (m_data[i] == data)
				return false;
		add(data);
		return true;
	}
	
	// order not preserved
	bool remove(const Type& data)
	{
		for(int i=0; i<m_num; ++i)
			if (m_data[i] == data)
			{
				m_data[i]= m_data[m_num - 1];
				--m_num;
				return true;
			}
		return false;
	}
	
	void removeAt(int idx)
	{
		assert(idx < m_num);
		m_data[idx]= m_data[m_num - 1];
		--m_num;
	}

	Type removeLast()
	{
		assert(m_num > 0);
		--m_num;
		return m_data[m_num];
	}

	int num()
	{
		return m_num;
	}
	
	inline Type& operator[](int idx)
	{
		assert(idx < m_num && idx >= 0);
		return m_data[idx];
	}

	inline const Type& operator[](int idx) const
	{
		assert(idx < m_num && idx >= 0);
		return m_data[idx];
	}
	
	inline Type*	getData()
	{
		return m_data;
	}
};
