////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-city-lottery-ordinance, a DLL Plugin for
// SimCity 4 that adds a city lottery to the game.
//
// Copyright (c) 2023 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#include "OrdinancePropertyHolder.h"
#include "cIGZIStream.h"
#include "cIGZOStream.h"
#include "Logger.h"

static constexpr uint32_t GZCLSID_OrdinancePropertyHolder = 0xd0f95c79;
static constexpr uint32_t GZIID_OrdinancePropertyHolder = 0x84672560;

namespace
{
	const char* GetPropertyDescription(uint32_t propertyID)
	{
		const char* value = nullptr;

		switch (propertyID)
		{
		case 0x28ed0380:
			value = "Crime Effect (float32[1])";
			break;
		case 0xaa5b8407:
			value = "Mayor Rating (int32[1])";
			break;
		case 0x08f79b8e:
			value = "Air Effect (float32[1])";
			break;
		case 0x28f42aa0:
			value = "Flammability Effect (float32[1])";
			break;
		case 0xe8f79c8b:
			value = "Water Effect (float32[1])";
			break;
		case 0xe8f79c90:
			value = "Garbage Effect (float32[1])";
			break;
		case 0xa8f4eb0c:
			value = "Water Use Reduction (float32[1])";
			break;
		case 0x0911e117:
			value = "Power Reduction Effect (float32[1])";
			break;
		case 0x2a633000:
			value = "Commercial Demand Effect (float32[1])";
			break;
		case 0x2a653110:
			value = "Demand Effect:Cs$ (float32[1])";
			break;
		case 0x2a653120:
			value = "Demand Effect:Cs$$ (float32[1])";
			break;
		case 0x2a653130:
			value = "Demand Effect:Cs$$$ (float32[1])";
			break;
		case 0x2a653320:
			value = "Demand Effect:Co$$ (float32[1])";
			break;
		case 0x2a653330:
			value = "Demand Effect:Co$$$ (float32[1])";
			break;
		case 0x2a634000:
			value = "Industrial Demand Effect (float32[1])";
			break;
		case 0x2a654100:
			value = "Demand Effect:IR (float32[1])";
			break;
		case 0x2a654200:
			value = "Demand Effect:ID (float32[1])";
			break;
		case 0x2a654300:
			value = "Demand Effect:IM (float32[1])";
			break;
		case 0x2a654400:
			value = "Demand Effect:IHT (float32[1])";
			break;
		case 0x491b3ad5:
			value = "Health Coverage Radius % Effect (float32[1])";
			break;
		case 0x891b3ae6:
			value = "Health Effectiveness vs. Distance Effect (float32, general response curve)";
			break;
		case 0xe91b3aee:
			value = "Health Quotient Boost Effect (float32[1])";
			break;
		case 0xc92d9c7a:
			value = "Health Quotient Decay Effect (float32[1])";
			break;
		case 0x092d909b:
			value = "Health Capacity Effect (float32[1])";
			break;
		case 0xe92d9db4:
			value = "Health Effectiveness vs. Average Age Effect (float32, general response curve)";
			break;
		case 0xa91b3af4:
			value = "School Coverage Radius % Effect (float32[1])";
			break;
		case 0xa91b3afa:
			value = "School Effectiveness vs. Distance Effect (float32, general response curve)";
			break;
		case 0xa92d9d7a:
			value = "School EQ Boost Effect (float32[1])";
			break;
		case 0x692ef65a:
			value = "School EQ Decay Effect (float32[1])";
			break;
		case 0x892d9d02:
			value = "School Capacity Effect (float32[1])";
			break;
		case 0xc91b3b02:
			value = "School Effectiveness vs. Average Age Effect (float32, general response curve)";
			break;
		case 0x8a612fee:
			value = "Travel Strategy Modifier (int32[9])";
			break;
		case 0x8a67e373:
			value = "Air Effect by zone type (float32[16])";
			break;
		case 0x8a67e374:
			value = "Water Effect by zone type (float32[16])";
			break;
		case 0x8a67e376:
			value = "Garbage Effect by zone type (float32[16])";
			break;
		case 0x8a67e378:
			value = "Traffic Air Pollution Effect (float32[1])";
			break;
		}

		return value;
	}

	void LogPropertyId(const char* methodName, uint32_t propertyId)
	{
		Logger& logger = Logger::GetInstance();

		const char* propertyDescription = GetPropertyDescription(propertyId);

		if (propertyDescription)
		{
			logger.WriteLineFormatted(
				LogOptions::OrdinancePropertyAPI,
				"%s: propertyId=0x%08x (%s)",
				methodName,
				propertyId,
				propertyDescription);
		}
		else
		{
			logger.WriteLineFormatted(
				LogOptions::OrdinancePropertyAPI,
				"%s: propertyId=0x%08x",
				methodName,
				propertyId);
		}
	}
}

OrdinancePropertyHolder::OrdinancePropertyHolder()
	: refCount(0), properties()
{
}

OrdinancePropertyHolder::OrdinancePropertyHolder(const std::vector<cSCBaseProperty>& properties)
	: refCount(0), properties(properties)
{
}

OrdinancePropertyHolder::OrdinancePropertyHolder(const OrdinancePropertyHolder& other)
	: refCount(0), properties(other.properties)
{

}

OrdinancePropertyHolder::OrdinancePropertyHolder(OrdinancePropertyHolder&& other) noexcept
	: refCount(0), properties(std::move(other.properties))
{
}

OrdinancePropertyHolder::~OrdinancePropertyHolder()
{
}

OrdinancePropertyHolder& OrdinancePropertyHolder::operator=(const OrdinancePropertyHolder& other)
{
	if (this == &other)
	{
		return *this;
	}

	properties = other.properties;

	return *this;
}

OrdinancePropertyHolder& OrdinancePropertyHolder::operator=(OrdinancePropertyHolder&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	properties = std::move(other.properties);

	return *this;
}

bool OrdinancePropertyHolder::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_OrdinancePropertyHolder)
	{
		AddRef();
		*ppvObj = this;

		return true;
	}
	else if (riid == GZIID_cISCPropertyHolder)
	{
		AddRef();
		*ppvObj = static_cast<cISCPropertyHolder*>(this);

		return true;
	}
	else if (riid == GZIID_cIGZSerializable)
	{
		AddRef();
		*ppvObj = static_cast<cIGZSerializable*>(this);

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		AddRef();
		*ppvObj = static_cast<cIGZUnknown*>(static_cast<cISCPropertyHolder*>(this));

		return true;
	}

	return false;
}

uint32_t OrdinancePropertyHolder::AddRef()
{
	return ++refCount;
}

uint32_t OrdinancePropertyHolder::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool OrdinancePropertyHolder::HasProperty(uint32_t dwProperty)
{
	LogPropertyId(__FUNCTION__, dwProperty);

	for (const auto& property : properties)
	{
		if (property.GetPropertyID() == dwProperty)
		{
			return true;
		}
	}

	return false;
}

bool OrdinancePropertyHolder::GetPropertyList(cIGZUnknownList** ppList)
{
	return false;
}

cISCProperty* OrdinancePropertyHolder::GetProperty(uint32_t dwProperty)
{
	LogPropertyId(__FUNCSIG__, dwProperty);

	for (auto& property : properties)
	{
		if (property.GetPropertyID() == dwProperty)
		{
			cISCProperty* pProperty = static_cast<cISCProperty*>(&property);
			pProperty->AddRef();

			return pProperty;
		}
	}

	return nullptr;
}

bool OrdinancePropertyHolder::GetProperty(uint32_t dwProperty, uint32_t& dwValueOut)
{
	LogPropertyId(__FUNCSIG__, dwProperty);

	bool result = false;

	for (const auto& property : properties)
	{
		if (property.GetPropertyID() == dwProperty)
		{
			const auto variant = property.GetPropertyValue();

			result = variant && variant->GetValUint32(dwValueOut);
			break;
		}
	}

	return result;
}

bool OrdinancePropertyHolder::GetProperty(uint32_t dwProperty, cIGZString& szValueOut)
{
	LogPropertyId(__FUNCSIG__, dwProperty);

	return false;
}

bool OrdinancePropertyHolder::GetProperty(uint32_t dwProperty, uint32_t riid, void** ppvObj)
{
	LogPropertyId(__FUNCSIG__, dwProperty);

	return false;
}

bool OrdinancePropertyHolder::GetProperty(uint32_t dwProperty, void* pUnknown, uint32_t& dwUnknownOut)
{
	LogPropertyId(__FUNCSIG__, dwProperty);

	return false;
}

bool OrdinancePropertyHolder::AddProperty(cISCProperty* pProperty, bool bUnknown)
{
	if (pProperty)
	{
		properties.push_back(cSCBaseProperty(*pProperty));
		return true;
	}

	return false;
}

bool OrdinancePropertyHolder::AddProperty(uint32_t dwProperty, cIGZVariant const* pVariant, bool bUnknown)
{
	properties.push_back(cSCBaseProperty(dwProperty, pVariant));
	return true;
}

bool OrdinancePropertyHolder::AddProperty(uint32_t dwProperty, uint32_t dwValue, bool bUnknown)
{
	properties.push_back(cSCBaseProperty(dwProperty, dwValue));
	return true;
}

bool OrdinancePropertyHolder::AddProperty(uint32_t dwProperty, cIGZString const& szValue)
{
	return false;
}

bool OrdinancePropertyHolder::AddProperty(uint32_t dwProperty, int32_t lValue, bool bUnknown)
{
	properties.push_back(cSCBaseProperty(dwProperty, lValue));
	return true;
}

bool OrdinancePropertyHolder::AddProperty(uint32_t dwProperty, void* pUnknown, uint32_t dwUnknown, bool bUnknown)
{
	return false;
}

bool OrdinancePropertyHolder::AddProperty(uint32_t dwProperty, float value)
{
	properties.push_back(cSCBaseProperty(dwProperty, value));
	return true;
}

bool OrdinancePropertyHolder::CopyAddProperty(cISCProperty* pProperty, bool bUnknown)
{
	return false;
}

bool OrdinancePropertyHolder::RemoveProperty(uint32_t dwProperty)
{
	for (std::vector<cSCBaseProperty>::iterator it = properties.begin(); it != properties.end();)
	{
		if (it->GetPropertyID() == dwProperty)
		{
			properties.erase(it);
			return true;
		}
	}

	return false;
}

bool OrdinancePropertyHolder::RemoveAllProperties(void)
{
	properties.clear();
	return true;
}

bool OrdinancePropertyHolder::EnumProperties(FunctionPtr1 pFunction1, void* pData)
{
	size_t propertyCount = properties.size();

	for (size_t i = 0; i < propertyCount; i++)
	{
		cISCProperty* property = &properties[i];

		pFunction1(property, pData);
	}

	return true;
}

bool OrdinancePropertyHolder::EnumProperties(FunctionPtr2 pFunction2, FunctionPtr1 pFunctionPipe)
{
	return false;
}

bool OrdinancePropertyHolder::CompactProperties(void)
{
	return false;
}

bool OrdinancePropertyHolder::Write(cIGZOStream& stream)
{
	if (stream.GetError() != 0)
	{
		return false;
	}

	const uint32_t version = 1;
	const uint32_t propertyCount = static_cast<uint32_t>(properties.size());

	if (!stream.SetUint32(version) || !stream.SetUint32(propertyCount))
	{
		return false;
	}

	for (uint32_t i = 0; i < propertyCount; i++)
	{
		if (!properties[i].Write(stream))
		{
			return false;
		}
	}

	return true;
}

bool OrdinancePropertyHolder::Read(cIGZIStream& stream)
{
	if (stream.GetError() != 0)
	{
		return false;
	}

	uint32_t version = 0;
	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	uint32_t propertyCount = 0;
	if (!stream.GetUint32(propertyCount))
	{
		return false;
	}

	properties.clear();

	for (uint32_t i = 0; i < propertyCount; i++)
	{
		cSCBaseProperty prop;

		if (!prop.Read(stream))
		{
			return false;
		}

		properties.push_back(prop);
	}

	return true;
}

uint32_t OrdinancePropertyHolder::GetGZCLSID()
{
	return GZCLSID_OrdinancePropertyHolder;
}
