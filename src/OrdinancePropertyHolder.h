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

#pragma once
#include "cISCPropertyHolder.h"
#include "cIGZSerializable.h"
#include "cSCBaseProperty.h"
#include <vector>

class OrdinancePropertyHolder : public cISCPropertyHolder, cIGZSerializable
{
public:

	OrdinancePropertyHolder();

	OrdinancePropertyHolder(const std::vector<cSCBaseProperty>& properties);

	OrdinancePropertyHolder(const OrdinancePropertyHolder& other);
	OrdinancePropertyHolder(OrdinancePropertyHolder&& other) noexcept;

	~OrdinancePropertyHolder();

	OrdinancePropertyHolder& operator=(const OrdinancePropertyHolder& other);
	OrdinancePropertyHolder& operator=(OrdinancePropertyHolder&& other) noexcept;

	bool QueryInterface(uint32_t riid, void** ppvObj);
	uint32_t AddRef();
	uint32_t Release();

	virtual bool HasProperty(uint32_t dwProperty);

	virtual bool GetPropertyList(cIGZUnknownList** ppList);
	virtual cISCProperty* GetProperty(uint32_t dwProperty);
	virtual bool GetProperty(uint32_t dwProperty, uint32_t& dwValueOut);
	virtual bool GetProperty(uint32_t dwProperty, cIGZString& szValueOut);
	virtual bool GetProperty(uint32_t dwProperty, uint32_t riid, void** ppvObj);
	virtual bool GetProperty(uint32_t dwProperty, void* pUnknown, uint32_t& dwUnknownOut);

	virtual bool AddProperty(cISCProperty* pProperty, bool bUnknown);
	virtual bool AddProperty(uint32_t dwProperty, cIGZVariant const* pVariant, bool bUnknown);
	virtual bool AddProperty(uint32_t dwProperty, uint32_t dwValue, bool bUnknown);
	virtual bool AddProperty(uint32_t dwProperty, cIGZString const& szValue);
	virtual bool AddProperty(uint32_t dwProperty, int32_t lValue, bool bUnknown);
	virtual bool AddProperty(uint32_t dwProperty, void* pUnknown, uint32_t dwUnknown, bool bUnknown);
	virtual bool AddProperty(uint32_t dwProperty, float value); // Not part of the SC4 API, but a convenience method.

	virtual bool CopyAddProperty(cISCProperty* pProperty, bool bUnknown);

	virtual bool RemoveProperty(uint32_t dwProperty);
	virtual bool RemoveAllProperties(void);

	virtual bool EnumProperties(FunctionPtr1 pFunction1, void* pData);
	virtual bool EnumProperties(FunctionPtr2 pFunction2, FunctionPtr1 pFunctionPipe);

	virtual bool CompactProperties(void);

	bool Write(cIGZOStream& stream);
	bool Read(cIGZIStream& stream);
	uint32_t GetGZCLSID();

private:
	
	uint32_t refCount;
	std::vector<cSCBaseProperty> properties;
};

