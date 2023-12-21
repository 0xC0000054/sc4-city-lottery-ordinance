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

#include "OrdinanceBase.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include "cIGZDate.h"
#include "cIGZIStream.h"
#include "cIGZOStream.h"
#include "cISC4ResidentialSimulator.h"
#include "cISC4Simulator.h"
#include "SC4Percentage.h"
#include <algorithm>
#include <stdlib.h>

static const uint32_t GZIID_OrdinanceBase = 0x3cb94c9e;

OrdinanceBase::OrdinanceBase(
	uint32_t clsid,
	const char* name,
	const char* description,
	int64_t enactmentIncome,
	int64_t retracmentIncome,
	int64_t monthlyConstantIncome,
	float monthlyIncomeFactor,
	bool isIncomeOrdinance)
	: clsid(clsid),
	  refCount(0),
	  name(name),
	  nameKey(),
	  description(description),
	  descriptionKey(),
	  enactmentIncome(enactmentIncome),
	  retracmentIncome(retracmentIncome),
	  monthlyConstantIncome(monthlyConstantIncome),
	  monthlyIncomeFactor(monthlyIncomeFactor),
	  isIncomeOrdinance(isIncomeOrdinance),
	  monthlyAdjustedIncome(0),
	  initialized(false),
	  available(false),
	  on(false),
	  enabled(false),
	  haveDeserialized(false),
	  pResidentialSimulator(nullptr),
	  pSimulator(nullptr),
	  miscProperties(),
	  logger(Logger::GetInstance())
{
}

OrdinanceBase::OrdinanceBase(
	uint32_t clsid,
	const char* name,
	const char* description,
	int64_t enactmentIncome,
	int64_t retracmentIncome,
	int64_t monthlyConstantIncome,
	float monthlyIncomeFactor,
	bool isIncomeOrdinance,
	const OrdinancePropertyHolder& properties)
	: clsid(clsid),
	  refCount(0),
	  name(name),
	  nameKey(),
	  description(description),
	  descriptionKey(),
	  enactmentIncome(enactmentIncome),
	  retracmentIncome(retracmentIncome),
	  monthlyConstantIncome(monthlyConstantIncome),
	  monthlyIncomeFactor(monthlyIncomeFactor),
	  isIncomeOrdinance(isIncomeOrdinance),
	  monthlyAdjustedIncome(0),
	  initialized(false),
	  available(false),
	  on(false),
	  enabled(false),
	  haveDeserialized(false),
	  pResidentialSimulator(nullptr),
	  pSimulator(nullptr),
	  miscProperties(properties),
	  logger(Logger::GetInstance())
{
}

OrdinanceBase::OrdinanceBase(
	uint32_t clsid,
	const char* name,
	const StringResourceKey& nameKey,
	const char* description,
	const StringResourceKey& descriptionKey,
	int64_t enactmentIncome,
	int64_t retracmentIncome,
	int64_t monthlyConstantIncome,
	float monthlyIncomeFactor,
	bool isIncomeOrdinance)
	: clsid(clsid),
	  refCount(0),
	  name(name),
	  nameKey(nameKey),
	  description(description),
	  descriptionKey(descriptionKey),
	  enactmentIncome(enactmentIncome),
	  retracmentIncome(retracmentIncome),
	  monthlyConstantIncome(monthlyConstantIncome),
	  monthlyIncomeFactor(monthlyIncomeFactor),
	  isIncomeOrdinance(isIncomeOrdinance),
	  monthlyAdjustedIncome(0),
	  initialized(false),
	  available(false),
	  on(false),
	  enabled(false),
	  haveDeserialized(false),
	  pResidentialSimulator(nullptr),
	  pSimulator(nullptr),
	  miscProperties(),
	  logger(Logger::GetInstance())
{
}

OrdinanceBase::OrdinanceBase(
	uint32_t clsid,
	const char* name,
	const StringResourceKey& nameKey,
	const char* description,
	const StringResourceKey& descriptionKey,
	int64_t enactmentIncome,
	int64_t retracmentIncome,
	int64_t monthlyConstantIncome,
	float monthlyIncomeFactor,
	bool isIncomeOrdinance,
	const OrdinancePropertyHolder& properties)
	: clsid(clsid),
	  refCount(0),
	  name(name),
	  nameKey(nameKey),
	  description(description),
	  descriptionKey(descriptionKey),
	  enactmentIncome(enactmentIncome),
	  retracmentIncome(retracmentIncome),
	  monthlyConstantIncome(monthlyConstantIncome),
	  monthlyIncomeFactor(monthlyIncomeFactor),
	  isIncomeOrdinance(isIncomeOrdinance),
	  monthlyAdjustedIncome(0),
	  initialized(false),
	  available(false),
	  on(false),
	  enabled(false),
	  haveDeserialized(false),
	  pResidentialSimulator(nullptr),
	  pSimulator(nullptr),
	  miscProperties(properties),
	  logger(Logger::GetInstance())
{
}

OrdinanceBase::OrdinanceBase(const OrdinanceBase& other)
	: clsid(other.clsid),
	  refCount(0),
	  name(other.name),
	  nameKey(other.nameKey),
	  description(other.description),
	  descriptionKey(other.descriptionKey),
	  enactmentIncome(other.enactmentIncome),
	  retracmentIncome(other.retracmentIncome),
	  monthlyConstantIncome(other.monthlyConstantIncome),
	  monthlyIncomeFactor(other.monthlyIncomeFactor),
	  isIncomeOrdinance(other.isIncomeOrdinance),
	  monthlyAdjustedIncome(other.monthlyAdjustedIncome),
	  initialized(other.initialized),
	  available(other.available),
	  on(other.on),
	  enabled(other.enabled),
	  haveDeserialized(other.haveDeserialized),
	  pResidentialSimulator(other.pResidentialSimulator),
	  pSimulator(other.pSimulator),
	  miscProperties(other.miscProperties),
	  logger(Logger::GetInstance())
{
}

OrdinanceBase::OrdinanceBase(OrdinanceBase&& other) noexcept
	: clsid(other.clsid),
	  refCount(0),
	  name(std::move(name)),
	  nameKey(other.nameKey),
	  description(std::move(other.description)),
	  descriptionKey(other.descriptionKey),
	  enactmentIncome(other.enactmentIncome),
	  retracmentIncome(other.retracmentIncome),
	  monthlyConstantIncome(other.monthlyConstantIncome),
	  monthlyIncomeFactor(other.monthlyIncomeFactor),
	  isIncomeOrdinance(other.isIncomeOrdinance),
	  monthlyAdjustedIncome(other.monthlyAdjustedIncome),
	  initialized(other.initialized),
	  available(other.available),
	  on(other.on),
	  enabled(other.enabled),
	  haveDeserialized(other.haveDeserialized),
	  pResidentialSimulator(other.pResidentialSimulator),
	  pSimulator(other.pSimulator),
	  miscProperties(std::move(other.miscProperties)),
	  logger(Logger::GetInstance())
{
	other.pResidentialSimulator = nullptr;
	other.pSimulator = nullptr;
}

OrdinanceBase& OrdinanceBase::operator=(const OrdinanceBase& other)
{
	if (this == &other)
	{
		return *this;
	}

	clsid = other.clsid;
	refCount = 0;
	name = name;
	nameKey = other.nameKey;
	description = other.description;
	descriptionKey = other.descriptionKey;
	enactmentIncome = other.enactmentIncome;
	retracmentIncome = other.retracmentIncome;
	monthlyConstantIncome = other.monthlyConstantIncome;
	monthlyIncomeFactor = other.monthlyIncomeFactor;
	isIncomeOrdinance = other.isIncomeOrdinance;
	monthlyAdjustedIncome = other.monthlyAdjustedIncome;
	initialized = other.initialized;
	available = other.available;
	on = other.on;
	enabled = other.enabled;
	haveDeserialized = other.haveDeserialized;
	pResidentialSimulator = other.pResidentialSimulator;
	pSimulator = other.pSimulator;
	miscProperties = other.miscProperties;

	return *this;
}

OrdinanceBase& OrdinanceBase::operator=(OrdinanceBase&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}

	clsid = other.clsid;
	refCount = 0;
	name = std::move(name);
	nameKey = other.nameKey;
	description = std::move(other.description);
	descriptionKey = other.descriptionKey;
	enactmentIncome = other.enactmentIncome;
	retracmentIncome = other.retracmentIncome;
	monthlyConstantIncome = other.monthlyConstantIncome;
	monthlyIncomeFactor = other.monthlyIncomeFactor;
	isIncomeOrdinance = other.isIncomeOrdinance;
	monthlyAdjustedIncome = other.monthlyAdjustedIncome;
	initialized = other.initialized;
	available = other.available;
	on = other.on;
	enabled = other.enabled;
	haveDeserialized = other.haveDeserialized;
	pResidentialSimulator = other.pResidentialSimulator;
	pSimulator = other.pSimulator;
	miscProperties = std::move(other.miscProperties);

	other.pResidentialSimulator = nullptr;
	other.pSimulator = nullptr;

	return *this;
}

bool OrdinanceBase::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_OrdinanceBase)
	{
		AddRef();
		*ppvObj = this;

		return true;
	}
	else if (riid == GZIID_cISC4Ordinance)
	{
		AddRef();
		*ppvObj = static_cast<cISC4Ordinance*>(this);

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
		*ppvObj = static_cast<cIGZUnknown*>(static_cast<cISC4Ordinance*>(this));

		return true;
	}

	return false;
}

uint32_t OrdinanceBase::AddRef()
{
	return ++refCount;
}

uint32_t OrdinanceBase::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}
	return refCount;
}

bool OrdinanceBase::Init(void)
{
	if (!haveDeserialized)
	{
		enabled = true;
	}
	return true;
}

bool OrdinanceBase::Shutdown(void)
{
	enabled = false;
	return true;
}

int64_t OrdinanceBase::GetCurrentMonthlyIncome(void)
{
	const int64_t monthlyConstantIncome = GetMonthlyConstantIncome();
	const double monthlyIncomeFactor = GetMonthlyIncomeFactor();

	if (!pResidentialSimulator)
	{
		return monthlyConstantIncome;
	}

	// The monthly income factor is multiplied by the city population.
	const int32_t cityPopulation = pResidentialSimulator->GetPopulation();
	const double populationIncome = monthlyIncomeFactor * static_cast<double>(cityPopulation);

	const double monthlyIncome = static_cast<double>(monthlyConstantIncome) + populationIncome;

	int64_t monthlyIncomeInteger = 0;

	if (monthlyIncome < std::numeric_limits<int64_t>::min())
	{
		monthlyIncomeInteger = std::numeric_limits<int64_t>::min();
	}
	else if (monthlyIncome > std::numeric_limits<int64_t>::max())
	{
		monthlyIncomeInteger = std::numeric_limits<int64_t>::max();
	}
	else
	{
		monthlyIncomeInteger = static_cast<int64_t>(monthlyIncome);
	}

	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: monthly income: constant=%lld, factor=%f, population=%d, current=%lld",
		__FUNCTION__,
		monthlyConstantIncome,
		monthlyIncomeFactor,
		cityPopulation,
		monthlyIncomeInteger);

	return monthlyIncomeInteger;
}

uint32_t OrdinanceBase::GetID(void) const
{
	return clsid;
}

cIGZString* OrdinanceBase::GetName(void)
{
	return &name;
}

cIGZString* OrdinanceBase::GetDescription(void)
{
	return &description;
}

uint32_t OrdinanceBase::GetYearFirstAvailable(void)
{
	return 0;
}

SC4Percentage OrdinanceBase::GetChanceAvailability(void)
{
	SC4Percentage percentage{ 100.0f };

	return percentage;
}

int64_t OrdinanceBase::GetEnactmentIncome(void)
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	return enactmentIncome;
}

int64_t OrdinanceBase::GetRetracmentIncome(void)
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	return retracmentIncome;
}

int64_t OrdinanceBase::GetMonthlyConstantIncome(void)
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	return monthlyConstantIncome;
}

float OrdinanceBase::GetMonthlyIncomeFactor(void)
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	return monthlyIncomeFactor;
}

cISCPropertyHolder* OrdinanceBase::GetMiscProperties()
{
	return &miscProperties;
}

uint32_t OrdinanceBase::GetAdvisorID(void)
{
	return 0;
}

bool OrdinanceBase::IsAvailable(void)
{
	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: result=%d"
		__FUNCTION__,
		available);

	return available;
}

bool OrdinanceBase::IsOn(void)
{
	bool result = available && on;

	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: result=%d"
		__FUNCTION__,
		result);

	return result;
}

bool OrdinanceBase::IsEnabled(void)
{
	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: result=%d"
		__FUNCTION__,
		enabled);

	return enabled;
}

int64_t OrdinanceBase::GetMonthlyAdjustedIncome(void)
{
	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: result=%lld",
		__FUNCTION__,
		monthlyAdjustedIncome);

	return monthlyAdjustedIncome;
}

bool OrdinanceBase::CheckConditions(void)
{
	bool result = false;

	if (enabled)
	{
		if (pSimulator)
		{
			cIGZDate* simDate = pSimulator->GetSimDate();

			if (simDate)
			{
				result = simDate->Year() >= GetYearFirstAvailable();
			}
		}
	}

	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: result=%d",
		__FUNCTION__,
		result);

	return result;
}

bool OrdinanceBase::IsIncomeOrdinance(void)
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	return isIncomeOrdinance;
}

bool OrdinanceBase::Simulate(void)
{
	monthlyAdjustedIncome = GetCurrentMonthlyIncome();

	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: monthlyAdjustedIncome=%lld",
		__FUNCTION__,
		monthlyAdjustedIncome);

	return true;
}

bool OrdinanceBase::SetAvailable(bool isAvailable)
{
	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: value=%d",
		__FUNCTION__,
		isAvailable);

	available = isAvailable;
	monthlyAdjustedIncome = 0;
	return true;
}

bool OrdinanceBase::SetOn(bool isOn)
{
	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: value=%d",
		__FUNCTION__,
		isOn);

	on = isOn;

	if (isOn)
	{
		monthlyAdjustedIncome = enactmentIncome;
	}
	else
	{
		monthlyAdjustedIncome = retracmentIncome;
	}

	return true;
}

bool OrdinanceBase::SetEnabled(bool isEnabled)
{
	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: value=%d",
		__FUNCTION__,
		isEnabled);

	enabled = isEnabled;
	return true;
}

bool OrdinanceBase::ForceAvailable(bool isAvailable)
{
	return SetAvailable(isAvailable);
}

bool OrdinanceBase::ForceOn(bool isOn)
{
	return SetOn(isOn);
}

bool OrdinanceBase::ForceEnabled(bool isEnabled)
{
	return SetEnabled(isEnabled);
}

bool OrdinanceBase::ForceMonthlyAdjustedIncome(int64_t monthlyAdjustedIncome)
{
	logger.WriteLineFormatted(
		LogOptions::OrdinanceAPI,
		"%s: value=%lld",
		__FUNCTION__,
		monthlyAdjustedIncome);

	monthlyAdjustedIncome = monthlyAdjustedIncome;
	return true;
}

bool OrdinanceBase::PostCityInit(cISC4City* pCity)
{
	bool result = false;

	if (pCity)
	{
		pResidentialSimulator = pCity->GetResidentialSimulator();
		pSimulator = pCity->GetSimulator();

		if (pResidentialSimulator && pSimulator)
		{
			result = Init();

			if (result)
			{
				LoadLocalizedStringResources();
			}
		}
	}

	return result;
}

bool OrdinanceBase::PreCityShutdown(cISC4City* pCity)
{
	bool result = Shutdown();

	pResidentialSimulator = nullptr;
	pSimulator = nullptr;

	return result;
}

bool OrdinanceBase::ReadBool(cIGZIStream& stream, bool& value)
{
	uint8_t temp = 0;
	// We use GetVoid because GetUint8 always returns false.
	if (!stream.GetVoid(&temp, 1))
	{
		return false;
	}

	value = temp != 0;
	return true;
}

bool OrdinanceBase::WriteBool(cIGZOStream& stream, bool value)
{
	const uint8_t uint8Value = static_cast<uint8_t>(value);

	return stream.SetVoid(&uint8Value, 1);
}

bool OrdinanceBase::Write(cIGZOStream& stream)
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	if (stream.GetError() != 0)
	{
		return false;
	}

	const uint32_t version = 1;
	if (!stream.SetUint32(version))
	{
		return false;
	}

	if (!stream.SetUint32(clsid))
	{
		return false;
	}

	if (!stream.SetGZStr(name))
	{
		return false;
	}

	if (!stream.SetGZStr(description))
	{
		return false;
	}

	if (!stream.SetSint64(enactmentIncome))
	{
		return false;
	}

	if (!stream.SetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.SetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.SetSint64(monthlyConstantIncome))
	{
		return false;
	}

	if (!stream.SetSint64(monthlyAdjustedIncome))
	{
		return false;
	}

	if (!stream.SetFloat32(monthlyIncomeFactor))
	{
		return false;
	}

	if (!WriteBool(stream, isIncomeOrdinance))
	{
		return false;
	}

	if (!miscProperties.Write(stream))
	{
		return false;
	}

	if (!WriteBool(stream, initialized))
	{
		return false;
	}

	if (!WriteBool(stream, available))
	{
		return false;
	}

	if (!WriteBool(stream, on))
	{
		return false;
	}

	if (!WriteBool(stream, enabled))
	{
		return false;
	}

	return true;
}

bool OrdinanceBase::Read(cIGZIStream& stream)
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	if (stream.GetError() != 0)
	{
		return false;
	}

	uint32_t version = 0;
	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	if (!stream.GetUint32(clsid))
	{
		return false;
	}

	if (!stream.GetGZStr(name))
	{
		return false;
	}

	if (!stream.GetGZStr(description))
	{
		return false;
	}

	if (!stream.GetSint64(enactmentIncome))
	{
		return false;
	}

	if (!stream.GetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.GetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.GetSint64(monthlyConstantIncome))
	{
		return false;
	}

	if (!stream.GetSint64(monthlyAdjustedIncome))
	{
		return false;
	}

	if (!stream.GetFloat32(monthlyIncomeFactor))
	{
		return false;
	}

	if (!ReadBool(stream, isIncomeOrdinance))
	{
		return false;
	}

	if (!miscProperties.Read(stream))
	{
		return false;
	}

	if (!ReadBool(stream, initialized))
	{
		return false;
	}

	if (!ReadBool(stream, available))
	{
		return false;
	}

	if (!ReadBool(stream, on))
	{
		return false;
	}

	if (!ReadBool(stream, enabled))
	{
		return false;
	}

	haveDeserialized = true;
	return true;
}

uint32_t OrdinanceBase::GetGZCLSID()
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	return clsid;
}

void OrdinanceBase::LoadLocalizedStringResources()
{
	cIGZString* localizedName = nullptr;
	cIGZString* localizedDescription = nullptr;

	if (StringResourceManager::GetLocalizedString(nameKey, &localizedName))
	{
		if (StringResourceManager::GetLocalizedString(descriptionKey, &localizedDescription))
		{
			if (localizedName->Strlen() > 0 && !localizedName->IsEqual(this->name, false))
			{
				this->name.Copy(*localizedName);
			}

			if (localizedDescription->Strlen() > 0 && !localizedDescription->IsEqual(this->description, false))
			{
				this->description.Copy(*localizedDescription);
			}

			localizedDescription->Release();
		}

		localizedName->Release();
	}
}
