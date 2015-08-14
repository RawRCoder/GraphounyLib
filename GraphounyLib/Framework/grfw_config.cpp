#include "grfw_config.h"
#include <boost/spirit/include/qi.hpp>
using namespace std;
GRAPHOUNY_NAMESPACE_FRAMEWORK{
	KeyValueManager::~KeyValueManager()
	{
		if (m_pHighPriorityData) delete m_pHighPriorityData;
		if (m_pLowPriorityData) delete m_pLowPriorityData;
	}

	void KeyValueManager::Set(const wstring& key, const wstring& value, bool high)
	{
		auto pDict = high ? m_pHighPriorityData : m_pLowPriorityData;
		pDict->Set(key, value);
	}

	wstring KeyValueManager::Get(const wstring& key, const wstring& defValue) const
	{
		wstring value;
		if (m_pHighPriorityData->TryGet(key, &value))
			return value;
		if (m_pLowPriorityData->TryGet(key, &value))
			return value;
		return defValue;
	}

	bool KeyValueManager::Get_bool(const wstring& key, bool defValue) const
	{
		return Get_i64(key, defValue ? 1 : 0) != 0;
	}

	i8 KeyValueManager::Get_i8(const wstring& key, i8 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		i8 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	u8 KeyValueManager::Get_u8(const wstring& key, u8 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		u8 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	i16 KeyValueManager::Get_i16(const wstring& key, i16 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		i16 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	u16 KeyValueManager::Get_u16(const wstring& key, u16 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		u16 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	i32 KeyValueManager::Get_i32(const wstring& key, i32 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		i32 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	u32 KeyValueManager::Get_u32(const wstring& key, u32 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		u32 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	i64 KeyValueManager::Get_i64(const wstring& key, i64 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		i64 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	u64 KeyValueManager::Get_u64(const wstring& key, u64 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		u64 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	f32 KeyValueManager::Get_f32(const wstring& key, f32 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		f32 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

	f64 KeyValueManager::Get_f64(const wstring& key, f64 defValue) const
	{
		auto sValue = Get(key, to_wstring(defValue));

		f64 num;
		if (boost::spirit::qi::parse(sValue.begin(), sValue.end(), num))
			return num;
		return defValue;
	}

} GRAPHOUNY_NAMESPACE_END