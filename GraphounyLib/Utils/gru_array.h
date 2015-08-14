#pragma once
#include <algorithm>
#include <stdexcept>
#include "..\gr_shared.h"

template <class T>
// ������������ ����� ������������� ������������� ������ � ������������ ������� � �������� � ������������ ������ (k = 1.5)
class Array
{
public:
	PREDICATE(DIterateT, (T& a, pointer n));
	PREDICATE(DPtrIterateT, (T* a, pointer n));
	PREDICATE(DPtrIterateConstT, (const T* a, pointer n));
	PREDICATE(DConstIterateT, (const T& a, pointer n));

	// ������� ������ � ��������� ��������� ��������
	explicit Array(u32 cap = 10)
	{
		_pData = new T[cap];
		_iCapacity = cap;
		_iLength = 0;
	}

	// ��������� ���������� � ������� �� ��������� ��� �� �������. �������� - ������ ����� ������� ������ �� �������!!!
	Array(T* objs, u32 count)
	{
		_pData = objs;
		_iCapacity = count;
		_iLength = count;
	}

	// �� �������� ���������� ��������� ������!
	~Array()
	{
		delete[] _pData;
	}

	// ���������� ��������� � ������ ( === Length())
	inline u32 Count() const { return _iLength; }

	// ���������� ��������� � ������ ( === Count())
	inline u32 Length() const { return _iLength; }

	// ������� ������������ ����������� ������
	inline u32 Capacity() const { return _iCapacity; }

	// ���� ������ ��������, ���������� ������������� ������. k = 1.5.
	// �������������, ��� ����� ������ ����� ������ ������ �������.
	void Grow()
	{
		if (_iCapacity <= _iLength)
		{
			auto maxv = std::max<u32>(_iCapacity + _iCapacity / 2, _iCapacity + 1);
			Resize(maxv);
		}
	}

	inline void EnsureCapacity(u32 newCapacity)
	{
		if (newCapacity >= Capacity())
		{
			Resize(newCapacity);
		}
	}

	// ��������� ������� � ����� ������.
	void Add(const T& element)
	{
		Grow();
		_pData[_iLength] = element;
		++_iLength;
	}

	// ��������� �������� � ����� ������.
	void AddRange(const T* elements, u32 count)
	{
		auto targetLength = _iLength + count;
		if (targetLength > _iCapacity)
			Resize(std::max<u32>(_iCapacity + _iCapacity / 2, targetLength));
		u32 i = 0;
		while (i < count)
		{
			_pData[_iLength + i] = elements[i];
			++i;
		}
		_iLength += i;
	}

	// ��������� �������� � ����� ������.
	void AddRange(const Array<T>* elements)
	{
		u32 n = elements->_iLength;
		auto targetLength = _iLength + n;
		if (targetLength <= _iCapacity)
			Resize(std::max<u32>(_iCapacity + _iCapacity / 2, targetLength));
		u32 i = 0;
		while (i < n)
		{
			_pData[_iLength + i] = elements->_pData[i];
			++i;
		}
	}

	// ��������� ������� � ������������ ����� ������. ��� index >= ����� ������ - ������� �������� Add
	void Insert(const T& element, const u32 index)
	{
		if (index >= _iLength)
		{
			Add(element);
			return;
		}

		Grow();

		for (i32 i = (i32)_iLength - 1; i >= (i32)index; --i)
			_pData[i + 1] = _pData[i];

		_pData[index] = element;
		_iLength++;
	}

	// ���������� ��� �������� � ��������� [begin, end) � �������� ����� (begin < end). 
	// ���� �������� ���������� false, ������� ����������� � ������� ����� false. ����� ����� ���������� true
	bool IterateForward(DIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr)
	{
		for (u32 i = begin; i < end; i += step)		
			if (!fIterator(_pData[i], o)) 
				return false;		
		return true;
	}
	// ���������� ��� �������� � ��������� [begin, end) � �������� ����� (begin < end). 
	// ���� �������� ���������� false, ������� ����������� � ������� ����� false. ����� ����� ���������� true
	bool IterateForward(DConstIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr) const
	{
		for (u32 i = begin; i < end; i += step)
			if (!fIterator(_pData[i], o))
				return false;
		return true;
	}
	// ���������� ��� �������� � ������. 
	// ���� �������� ���������� false, ������� ����������� � ������� ����� false. ����� ����� ���������� true
	bool IterateAll(DPtrIterateT fIterator, pointer o = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!fIterator(&_pData[i], o))
				return false;
		return true;
	}

	// ���������� ��� �������� � ������. 
	// ���� �������� ���������� false, ������� ����������� � ������� ����� false. ����� ����� ���������� true
	bool IterateAll(DIterateT fIterator, pointer o = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!fIterator(_pData[i], o)) 
				return false;
		return true;
	}
	// ���������� ��� �������� � ������. 
	// ���� �������� ���������� false, ������� ����������� � ������� ����� false. ����� ����� ���������� true
	bool IterateAll(DConstIterateT fIterator, pointer o = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!fIterator(_pData[i], o))
				return false;
		return true;
	}

	// ���������� ��� �������� � ��������� [begin, end) � �������� ����� (begin > end). 
	// ���� �������� ���������� false, ������� ����������� � ������� ����� false. ����� ����� ���������� true
	bool IterateBackward(DIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr)
	{
		for (u32 i = begin; i > end; i -= step)		
			if (!fIterator(_pData[i], o)) 
				return false;
		
		return true;
	}
	// ���������� ��� �������� � ��������� [begin, end) � �������� ����� (begin > end). 
	// ���� �������� ���������� false, ������� ����������� � ������� ����� false. ����� ����� ���������� true
	bool IterateBackward(DConstIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr)
	{
		for (u32 i = begin; i > end; i -= step)
			if (!fIterator(_pData[i], o))
				return false;

		return true;
	}

	// ���������� true, ���� � ������ ���������� �������, ������ �������. ����� ����� ���������� false
	bool Contains(const T& element) const
	{
		for (u32 i = 0; i < _iLength; ++i)		
			if (_pData[i] == element)
				return true;		
		return false;
	}

	// ���������� true, ���� � ������ ���������� �������, ��� �������� ������� ��������. ����� ����� ���������� false
	bool Any(DConstIterateT func, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (func(_pData[i], obj))
				return true;
		return false;
	}

	// ���������� true <=> �������� ������� ��� ���� ���������
	bool All(DConstIterateT func, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!func(_pData[i], obj))
				return false;
		return true;
	}

	// ���������� ������ ���������, ��� ������� ������� ��������
	Array<T> Where(DConstIterateT p, pointer obj = nullptr) const
	{
		Array<T> newArray = new Array<T>();
		for (u32 i = 0; i < _iLength; ++i)
			if (p(_pData[i], obj))
				newArray->Add(p);
		return newArray;
	}
	TEMPLATE_DELEGATE(<class T2>, T2, DSelectorT, (const T& e, pointer o));

	template<class T2> Array<T2> Select(DSelectorT<T2> p, pointer obj = nullptr) const
	{
		Array<T2> newArray = new Array<T2>();
		for (u32 i = 0; i < _iLength; ++i)
			newArray->Add(p(_pData[i], obj));
		return newArray;
	}


	// ���������� ������ �������, ��� �������� ������� ��������. ���� ������ ��� - ���������� �������� ��-���������
	T FirstOrDefault(DConstIterateT p, T def, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (p(_pData[i], obj))
				return _pData[i];
		return def;
	}
	// ���������� ��������� �� ������ �������, ��� �������� ������� ��������. ���� ������ ��� - ���������� �������� ��-���������
	T* FirstOrDefaultP(DPtrIterateT p, T* def = nullptr, pointer obj = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (p(&_pData[i], obj))
				return &_pData[i];
		return def;
	}// ���������� ��������� �� ������ �������, ��� �������� ������� ��������. ���� ������ ��� - ���������� �������� ��-���������
	const T* FirstOrDefaultPC(DPtrIterateConstT p, T* def = nullptr, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (p(&_pData[i], obj))
				return &_pData[i];
		return def;
	}

	// ���������� ������ ������� �������� � ������, ������� �������, ���� -1, ���� ������ �������� � ������ ���
	i32 IndexOf(const T& element) const
	{
		for (u32 i = 0; i < _iLength; ++i)
		{
			if (_pData[i] == element)
				return i;
		}
		return -1;
	}

	// ������� ������� � ��������� ��������. �� �������� ����������. ���������� ��������� �� ������ �������
	T RemoveAt(const u32 index)
	{
		if (index >= _iLength)
			throw std::out_of_range("Index is out of range");

		T pObject = _pData[index];
		--_iLength;

		for (u32 i = index; i < _iLength; ++i)		
			_pData[i] = _pData[i + 1];
		
		return pObject;
	}

	// ������� � ��������������� ������� � ��������� ��������.
	inline void DeleteAt(const u32 index)
	{
		T pObject = RemoveAt(index);
		if (pObject != nullptr)
			delete pObject;
	}

	// ������� �� ������ ������ �������, ������ �������. ������������ ��������� � ��������� �������.
	// ���� ����� ��������� ���, ����������� OutOfRange exception
	inline T Remove(const T& element)
	{
		return RemoveAt(IndexOf(element));
	}

	// ������� �� ������ ������ �������, ������ �������. ���� ����� ��������� ���, ������������ false, ����� true. 
	// � removed ������������ ��������� �������
	inline bool Remove(const T& element, T& removed)
	{
		i32 id = IndexOf(element);
		if (id < 0) return false;

		removed = RemoveAt((u32)id);
		return true;
	}

	// ������� � ��������������� �� ������ ������ �������, ������ �������. ���� ����� ��������� ���, ����������� OutOfRange exception
	inline void Delete(const T& element)
	{
		DeleteAt(IndexOf(element));
	}

	// ������� � ��������������� �� ������ ������ �������, ������ �������. ���� ����� ��������� ���, ������������ false, ����� true. 
	inline bool rDelete(const T& element)
	{
		i32 id = IndexOf(element);
		if (id < 0) return false;
		DeleteAt(static_cast<u32>(id));
		return true;
	}

	// ������� ������ (�� ����������� ������ ���������)
	inline void Clear()
	{
		for (u32 i = _iLength; i > 0; --i)		
			RemoveAt(i - 1);		
	}

	// ������� ������ (����������� ������ ���������)
	inline void ClearDestroy()
	{
		for (u32 i = _iLength; i > 0; --i)
			DeleteAt(i - 1);
	}

	// ������������ ������ ��� ������, ������� ��� ����� ������������ ������
	void Resize(const u32 nsz)
	{
		T* _pNewData = new T[nsz];
		u32 end = std::min<u32>(nsz, _iCapacity);
		for (u32 i = 0; i < end; ++i)		
			_pNewData[i] = _pData[i];
		
		delete[] _pData;
		_pData = _pNewData;
		_iCapacity = nsz;
		if (_iLength > _iCapacity)
			_iLength = _iCapacity;
	}

	// ���������� ������ �� ������� � �������� ��������.
	T& operator[](u32 id) const
	{
		if (id >= _iLength)
			throw std::out_of_range("Index is out of range");
		return _pData[id];
	}

	// ���������� ������� � �������� ��������.
	const T& Get(u32 id) const
	{
		if (id >= _iLength)
			throw std::out_of_range("Index is out of range");
		return _pData[id];
	}

	// ���������� ���� ������ � ���� ������� � ������ == Capacity() (��� �����������).
	inline T* ToFullArray() const { return _pData; }

	// ���������� ����� ������ � ���� ������� � ������ == Length().
	inline const T* ToArray()
	{
		auto arr = new T[_iLength];
		for (u32 i = 0; i < _iLength; ++i)
			arr[i] = _pData[i];
		return arr;
	}
private:
	T* _pData;
	u32 _iCapacity;
	u32 _iLength;
};