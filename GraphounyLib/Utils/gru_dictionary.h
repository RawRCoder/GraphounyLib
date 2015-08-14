#pragma once
#include "..\gr_shared.h"
#include "gru_array.h"

//template <class T> using DComparator = i32 (*)(const T& a, const T& b);
TEMPLATE_DELEGATE(<class T>, i32, DComparator, (const T& a, const T& b));

template <typename TKey, class TValue>
class Dictionary;

template <typename TKey, class TValue>
struct KeyValuePair
{
	TKey m_tKey;
	TValue m_tValue;

	KeyValuePair() {}

	explicit KeyValuePair(const TKey& key)
		: m_tKey(key)
		, m_iHash(GetHashCode(key)) { }
	KeyValuePair(const TKey& key, const TValue& value)
		: m_tKey(key)
		, m_tValue(value)
		, m_iHash(GetHashCode(key)) { }
	bool Equals(const KeyValuePair& other, DComparator<TKey> comp) const
	{
		if (m_iHash != other.m_iHash)
			return false;
		return !comp(m_tKey, other.m_tKey);
	}
private:
	u64 m_iHash;
};

template <typename TKey, class TValue>
class Dictionary
{
public:
	PROCEDURE(DSimpleConstIteratorT, (const TKey& key, const TValue& value, pointer obj));
	PROCEDURE(DSimpleIteratorT, (const TKey& key, TValue& value, pointer obj));

	explicit Dictionary(DComparator<TKey> comp)
	{
		m_funcComparator = comp;
	}	

	inline u32 Count() const { return m_aData.Count(); }
	void Set(const TKey& key, const TValue& value)
	{
		auto kvp = GetKVPByKey(key);
		if (kvp)
			kvp->m_tValue = value;
		else
			m_aData.Add(KeyValuePair<TKey, TValue>(key, value));
	}	
	bool TryGet(const TKey& key, TValue* outValue) const
	{
		auto kvp = GetKVPByKey(key);
		if (!kvp) return false;
		*outValue = kvp->m_tValue;
		return true;		
	}
	TValue Get(const TKey& key) const
	{
		auto kvp = GetKVPByKey(key);
		if (!kvp) throw exception("Key not found in dictionary");
		return kvp->m_tValue;
	}
	TValue* Get(const TKey& key, const TValue* defValue)
	{
		auto kvp = GetKVPByKey(key);
		if (!kvp) return defValue;
		return &(kvp->m_tValue);
	}	
	bool ContainsKey(const TKey& key) const
	{
		return GetKVPByKey(key) != nullptr;
	}
	bool ContainsKey(const TKey& key, DComparator<TKey> comp) const
	{
		for (u32 i = 0; i < m_aData.Length(); ++i)
		{
			auto kvp = m_aData.Get(i);
			if (!comp(kvp.m_tKey, key))
				return true;
		}
		return false;
	}
	bool ContainsValue(const TValue& v) const
	{
		for (u32 i = 0; i < m_aData.Length(); ++i)
		{
			auto kvp = m_aData.Get(i);
			if (kvp.m_tValue == v)
				return true;
		}
		return false;
		return m_aData.FirstOrDefaultPC([&](const KeyValuePair<TKey, TValue>* kvp, pointer obj) { return kvp->m_tValue == v; });
	}
	bool ContainsValue(const TValue& v, DComparator<TValue> comp) const
	{
		for (u32 i = 0; i < m_aData.Length(); ++i)
		{
			auto el = m_aData.Get(i);
			if (!comp(el.m_tValue, v))
				return true;
		}
		return false;
	}
	TValue* operator[](const TKey& key)
	{
		auto kvp = GetKVPByKey(key);
		return kvp ? &(kvp->m_tValue) : nullptr;
	}

	void IterateAll(DSimpleIteratorT func, pointer obj = nullptr)
	{
		for (u32 i = 0; i < Count(); ++i)
		{
			auto kvp = m_aData[i];
			func(kvp.m_tKey, kvp.m_tValue, obj);
		}
	}
	void IterateAll(DSimpleConstIteratorT func, pointer obj = nullptr) const
	{
		for (u32 i = 0; i < Count(); ++i)
		{
			auto kvp = m_aData[i];
			func(kvp.m_tKey, kvp.m_tValue, obj);
		}
	}

	bool Remove(const TKey& key)
	{
		KeyValuePair<TKey, TValue> myKVP(key);
		for (u32 i = 0; i < m_aData.Count(); ++i)
		{
			KeyValuePair<TKey, TValue>* kvp = &m_aData[i];
			if (!kvp->Equals(myKVP, m_funcComparator)) 
				continue;			
			m_aData.RemoveAt(i);
			return true;			
		}
		return false;
	}

	bool RemoveDelete(const TKey& key)
	{
		KeyValuePair<TKey, TValue> myKVP(key);
		for (u32 i = 0; i < m_aData.Count(); ++i)
		{
			KeyValuePair<TKey, TValue>* kvp = &m_aData[i];
			if (!kvp->Equals(myKVP, m_funcComparator))
				continue;
			m_aData.RemoveAt(i);
			delete kvp->m_tValue;
			return true;
		}
		return false;
	}
	bool RemoveDeleteArr(const TKey& key)
	{
		KeyValuePair<TKey, TValue> myKVP(key);
		for (u32 i = 0; i < m_aData.Count(); ++i)
		{
			KeyValuePair<TKey, TValue>* kvp = &m_aData[i];
			if (!kvp->Equals(myKVP, m_funcComparator))
				continue;
			m_aData.RemoveAt(i);
			delete[] kvp->m_tValue;
			return true;
		}
		return false;
	}

private:
	KeyValuePair<TKey, TValue>* GetKVPByKey(const TKey& key) const
	{
		KeyValuePair<TKey, TValue> myKVP(key);
		for (u32 i = 0; i < m_aData.Count(); ++i)
		{
			KeyValuePair<TKey, TValue>* kvp = &m_aData[i];
			if (kvp->Equals(myKVP, m_funcComparator))
				return kvp;
		}
		return nullptr;
	}

	DComparator<TKey> m_funcComparator;
	Array<KeyValuePair<TKey, TValue>> m_aData;
public:
	class Iterator
	{
	public:
		Iterator(Dictionary<TKey, TValue>* v) : m_pDict(v) {}
		inline void Reset() { id = 0; }
		inline bool Finished() { return id >= m_pDict->m_aData.Count(); }
		inline void Next() { ++id; }

		inline const TKey& GetKey() { return m_pDict->m_aData[id].m_tKey; }
		inline const TValue& GetValue() { return m_pDict->m_aData[id].m_tValue; }

		const TValue& operator()(){ return GetValue(); }
		inline Iterator& operator++() { Next(); return *this; }
	private:
		Dictionary<TKey, TValue>* m_pDict;
		u32 id = 0;
	};
	Iterator GetIterator() { return Iterator(this); }
};

template <class T> static i32 EqComparator(const T& a, const T& b) { return a == b ? 0 : 1; }
template <class T> static i32 Comparator(const T& a, const T& b) { return a-b; }
#define SPECIAL_COMPARATOR(T) template <> static i32 Comparator<T>(const T& a, const T& b)

SPECIAL_COMPARATOR(bool) { return a == b ? 0 : 1; }
SPECIAL_COMPARATOR(std::string) { return a.compare(b); }
SPECIAL_COMPARATOR(std::wstring) { return a.compare(b); }

typedef Dictionary<std::string, pointer> NamedPointerDictionary;
typedef Dictionary<std::wstring, pointer> WNamedPointerDictionary;
typedef Dictionary<u32, pointer> IndexedPointerDictionary;
typedef Dictionary<u64, pointer> LongIndexedPointerDictionary;
typedef Dictionary<std::string, std::string> StringDictionary;
typedef Dictionary<std::string, std::wstring> String2WStringDictionary;
typedef Dictionary<std::wstring, std::string> WString2StringDictionary;
typedef Dictionary<std::wstring, std::wstring> WStringDictionary;