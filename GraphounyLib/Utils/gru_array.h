#pragma once
#include <algorithm>
#include <stdexcept>
#include "..\gr_shared.h"

template <class T>
// Представляет собой автоматически расширяющийся массив с возможностью вставки и удаления в произвольных местах (k = 1.5)
class Array
{
public:
	PREDICATE(DIterateT, (T& a, pointer n));
	PREDICATE(DPtrIterateT, (T* a, pointer n));
	PREDICATE(DPtrIterateConstT, (const T* a, pointer n));
	PREDICATE(DConstIterateT, (const T& a, pointer n));

	// Создает список с указанной начальной ёмкостью
	explicit Array(u32 cap = 10)
	{
		_pData = new T[cap];
		_iCapacity = cap;
		_iLength = 0;
	}

	// Позволяет обращаться с данными по указателю как со списком. Внимание - данные будут удалены вместе со списком!!!
	Array(T* objs, u32 count)
	{
		_pData = objs;
		_iCapacity = count;
		_iLength = count;
	}

	// Не вызывает деструктор элементов списка!
	~Array()
	{
		delete[] _pData;
	}

	// Количество элементов в списке ( === Length())
	inline u32 Count() const { return _iLength; }

	// Количество элементов в списке ( === Count())
	inline u32 Length() const { return _iLength; }

	// Текущая максимальная вместимость списка
	inline u32 Capacity() const { return _iCapacity; }

	// Если список заполнен, происходит перевыделение памяти. k = 1.5.
	// Гарантируется, что новый размер будет строго больше старого.
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

	// Добавляет элемент в конец списка.
	void Add(const T& element)
	{
		Grow();
		_pData[_iLength] = element;
		++_iLength;
	}

	// Добавляет элементы в конец списка.
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

	// Добавляет элементы в конец списка.
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

	// Вставляет элемент в произвольное место списка. при index >= длины списка - функция вызывает Add
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

	// Перебирает все элементы в диапозоне [begin, end) с заданным шагом (begin < end). 
	// Если предикат возвращает false, перебор завершается и функция вернёт false. Иначе будет возвращено true
	bool IterateForward(DIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr)
	{
		for (u32 i = begin; i < end; i += step)		
			if (!fIterator(_pData[i], o)) 
				return false;		
		return true;
	}
	// Перебирает все элементы в диапозоне [begin, end) с заданным шагом (begin < end). 
	// Если предикат возвращает false, перебор завершается и функция вернёт false. Иначе будет возвращено true
	bool IterateForward(DConstIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr) const
	{
		for (u32 i = begin; i < end; i += step)
			if (!fIterator(_pData[i], o))
				return false;
		return true;
	}
	// Перебирает все элементы в списке. 
	// Если предикат возвращает false, перебор завершается и функция вернёт false. Иначе будет возвращено true
	bool IterateAll(DPtrIterateT fIterator, pointer o = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!fIterator(&_pData[i], o))
				return false;
		return true;
	}

	// Перебирает все элементы в списке. 
	// Если предикат возвращает false, перебор завершается и функция вернёт false. Иначе будет возвращено true
	bool IterateAll(DIterateT fIterator, pointer o = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!fIterator(_pData[i], o)) 
				return false;
		return true;
	}
	// Перебирает все элементы в списке. 
	// Если предикат возвращает false, перебор завершается и функция вернёт false. Иначе будет возвращено true
	bool IterateAll(DConstIterateT fIterator, pointer o = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!fIterator(_pData[i], o))
				return false;
		return true;
	}

	// Перебирает все элементы в диапозоне [begin, end) с заданным шагом (begin > end). 
	// Если предикат возвращает false, перебор завершается и функция вернёт false. Иначе будет возвращено true
	bool IterateBackward(DIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr)
	{
		for (u32 i = begin; i > end; i -= step)		
			if (!fIterator(_pData[i], o)) 
				return false;
		
		return true;
	}
	// Перебирает все элементы в диапозоне [begin, end) с заданным шагом (begin > end). 
	// Если предикат возвращает false, перебор завершается и функция вернёт false. Иначе будет возвращено true
	bool IterateBackward(DConstIterateT fIterator, const u32 begin, const u32 end, const u32 step = 1, pointer o = nullptr)
	{
		for (u32 i = begin; i > end; i -= step)
			if (!fIterator(_pData[i], o))
				return false;

		return true;
	}

	// Возвращает true, если в списке содержится элемент, равный данному. Иначе будет возвращено false
	bool Contains(const T& element) const
	{
		for (u32 i = 0; i < _iLength; ++i)		
			if (_pData[i] == element)
				return true;		
		return false;
	}

	// Возвращает true, если в списке содержится элемент, для которого истинен предикат. Иначе будет возвращено false
	bool Any(DConstIterateT func, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (func(_pData[i], obj))
				return true;
		return false;
	}

	// Возвращает true <=> предикат истинен для всех элементов
	bool All(DConstIterateT func, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (!func(_pData[i], obj))
				return false;
		return true;
	}

	// Возвращает массив элементов, для которых истинен предикат
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


	// Возвращает первый элемент, для которого истинен предикат. Если такого нет - возвращает заданный по-умолчанию
	T FirstOrDefault(DConstIterateT p, T def, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (p(_pData[i], obj))
				return _pData[i];
		return def;
	}
	// Возвращает указатель на первый элемент, для которого истинен предикат. Если такого нет - возвращает заданный по-умолчанию
	T* FirstOrDefaultP(DPtrIterateT p, T* def = nullptr, pointer obj = nullptr)
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (p(&_pData[i], obj))
				return &_pData[i];
		return def;
	}// Возвращает указатель на первый элемент, для которого истинен предикат. Если такого нет - возвращает заданный по-умолчанию
	const T* FirstOrDefaultPC(DPtrIterateConstT p, T* def = nullptr, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < _iLength; ++i)
			if (p(&_pData[i], obj))
				return &_pData[i];
		return def;
	}

	// Возвращает индекс первого элемента в списке, равного данному, либо -1, если такого элемента в списке нет
	i32 IndexOf(const T& element) const
	{
		for (u32 i = 0; i < _iLength; ++i)
		{
			if (_pData[i] == element)
				return i;
		}
		return -1;
	}

	// Удаляет элемент с указанным индексом. Не вызывает деструктор. Возвращает удаленный из списка элемент
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

	// Удаляет и деструктурирует элемент с указанным индексом.
	inline void DeleteAt(const u32 index)
	{
		T pObject = RemoveAt(index);
		if (pObject != nullptr)
			delete pObject;
	}

	// Удаляет из списка первый элемент, равный данному. Возвращается найденный и удаленный элемент.
	// Если таких элементов нет, поднимается OutOfRange exception
	inline T Remove(const T& element)
	{
		return RemoveAt(IndexOf(element));
	}

	// Удаляет из списка первый элемент, равный данному. Если таких элементов нет, возвращается false, иначе true. 
	// В removed записывается удаленный элемент
	inline bool Remove(const T& element, T& removed)
	{
		i32 id = IndexOf(element);
		if (id < 0) return false;

		removed = RemoveAt((u32)id);
		return true;
	}

	// Удаляет и деструктурирует из списка первый элемент, равный данному. Если таких элементов нет, поднимается OutOfRange exception
	inline void Delete(const T& element)
	{
		DeleteAt(IndexOf(element));
	}

	// Удаляет и деструктурирует из списка первый элемент, равный данному. Если таких элементов нет, возвращается false, иначе true. 
	inline bool rDelete(const T& element)
	{
		i32 id = IndexOf(element);
		if (id < 0) return false;
		DeleteAt(static_cast<u32>(id));
		return true;
	}

	// Очищает список (не освобождает память элементов)
	inline void Clear()
	{
		for (u32 i = _iLength; i > 0; --i)		
			RemoveAt(i - 1);		
	}

	// Очищает список (освобождает память элементов)
	inline void ClearDestroy()
	{
		for (u32 i = _iLength; i > 0; --i)
			DeleteAt(i - 1);
	}

	// Перевыделяет память под список, задавая ему новый максимальный размер
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

	// Вовзращает ссылку на элемент с заданным индексом.
	T& operator[](u32 id) const
	{
		if (id >= _iLength)
			throw std::out_of_range("Index is out of range");
		return _pData[id];
	}

	// Вовзращает элемент с заданным индексом.
	const T& Get(u32 id) const
	{
		if (id >= _iLength)
			throw std::out_of_range("Index is out of range");
		return _pData[id];
	}

	// Вовзращает весь список в виде массива с длиной == Capacity() (без копирования).
	inline T* ToFullArray() const { return _pData; }

	// Вовзращает копию списка в виде массива с длиной == Length().
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