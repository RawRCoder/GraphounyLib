#pragma once
#include "grfw_shared.h"
#include "..\Utils/gru_dictionary.h"

GRAPHOUNY_NAMESPACE_FRAMEWORK{

#define __SetT(T, v) inline void Set_##T(const std::wstring& key, T value, bool high = false) { Set(key, std::to_wstring(v), high); }
#define __GetT(T, dv) T Get_##T(const std::wstring& key, T defValue = dv) const

	class KeyValueManager
	{
	public:
		~KeyValueManager();

		void Set(const std::wstring& key, const std::wstring& value, bool high = false);
		__SetT(u8, value);
		__SetT(i8, value);
		__SetT(u16, value);
		__SetT(i16, value);
		__SetT(u32, value);
		__SetT(i32, value);
		__SetT(u64, value);
		__SetT(i64, value);
		__SetT(f32, value);
		__SetT(f64, value);
		__SetT(bool, value ? 1 : 0);

		std::wstring Get(const std::wstring& key, const std::wstring& defValue = L"") const;
		__GetT(u8, 0);
		__GetT(i8, 0);
		__GetT(u16, 0);
		__GetT(i16, 0); 
		__GetT(u32, 0);
		__GetT(i32, 0);
		__GetT(u64, 0);
		__GetT(i64, 0);
		__GetT(f32, 0);
		__GetT(f64, 0);
		__GetT(bool, false);

		inline bool IsDefinedHighPriority(const std::wstring& key) const { return m_pHighPriorityData->ContainsKey(key); }
		inline bool IsDefinedLowPriority(const std::wstring& key) const { return m_pLowPriorityData->ContainsKey(key); }
		inline bool IsDefined(const std::wstring& key) const { return IsDefinedHighPriority(key) | IsDefinedLowPriority(key); }
	private:
		WStringDictionary* m_pHighPriorityData = new WStringDictionary(Comparator<std::wstring>);
		WStringDictionary* m_pLowPriorityData = new WStringDictionary(Comparator<std::wstring>);
	};
}GRAPHOUNY_NAMESPACE_END