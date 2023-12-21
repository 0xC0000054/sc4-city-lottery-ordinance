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

#include "CityLotteryOrdinance.h"
#include "ISettings.h"
#include "cIGZIStream.h"
#include "cIGZOStream.h"
#include "cISC4Demand.h"
#include "cISC4DemandSimulator.h"
#include "cISC4App.h"
#include "cISC4Region.h"
#include "cISC4RegionalCity.h"
#include "GZServPtrs.h"
#include "cRZAutoRefCount.h"
#include <array>

namespace
{
	OrdinancePropertyHolder CreateDefaultOrdinanceEffects()
	{
		OrdinancePropertyHolder properties;

		// Positive Effects:

		// School EQ Boost Effect: +2%
		properties.AddProperty(0xA92D9D7A, 102.0f);

		// Cs$ Demand Effect: +1%
		properties.AddProperty(0x2a653110, 1.01f);

		// Negative Effects:

		// Crime Effect Multiplier: +10%
		properties.AddProperty(0x28ED0380, 1.10f);

		return properties;
	}
}

static constexpr uint32_t kCityLotteryOrdianceCLSID = 0xe95f7779;

CityLotteryOrdinance::CityLotteryOrdinance()
	: OrdinanceBase(
		kCityLotteryOrdianceCLSID,
		"City-wide Lottery Program",
		StringResourceKey(0xE8D16EC8, 0xE34CDA8D),
		"A city-wide lottery program. Provides a slight boost to EQ and Cs§ demand at the cost of increased crime. "
		"The monthly income factor is based on the city's residential population.",
		StringResourceKey(0xE8D16EC8, 0x39385372),
		/* enactment income */		  0,
		/* retracment income */       0,
		/* monthly constant income */ 500,
		/* monthly income factor */   0.0f, // unused
		/* income ordinance */		  true,
		CreateDefaultOrdinanceEffects()),
	pDemandSimulator(nullptr),
	residentialLowWealthIncomeFactor(0.05f),
	residentialMedWealthIncomeFactor(0.03f),
	residentialHighWealthIncomeFactor(0.01f)
{
}

void CityLotteryOrdinance::UpdateOrdinanceData(const ISettings& settings)
{
	this->monthlyConstantIncome = settings.MonthlyConstantIncome();
	this->residentialLowWealthIncomeFactor = settings.ResidentialLowWealthFactor();
	this->residentialMedWealthIncomeFactor = settings.ResidentialMedWealthFactor();
	this->residentialHighWealthIncomeFactor = settings.ResidentialHighWealthFactor();
	this->miscProperties = settings.OrdinanceEffects();
}

int64_t CityLotteryOrdinance::GetCurrentMonthlyIncome()
{
	const int64_t monthlyConstantIncome = GetMonthlyConstantIncome();

	double monthlyIncome = static_cast<double>(monthlyConstantIncome);

	// Add the monthly income for each of the residential wealth groups.
	// If the income factor is 0.0 for any group they will not participate in the city lottery.

	if (residentialLowWealthIncomeFactor > 0.0f)
	{
		const double lowWealthPopulation = GetCityPopulation(0x1011);
		if (lowWealthPopulation > 0.0)
		{
			const double lotteryPopulationIncome = lowWealthPopulation * static_cast<double>(residentialLowWealthIncomeFactor);

			monthlyIncome += lotteryPopulationIncome;
		}
	}

	if (residentialMedWealthIncomeFactor > 0.0f)
	{
		const double medWealthPopulation = GetCityPopulation(0x1021);
		if (medWealthPopulation > 0.0)
		{
			const double lotteryPopulationIncome = medWealthPopulation * static_cast<double>(residentialMedWealthIncomeFactor);

			monthlyIncome += lotteryPopulationIncome;
		}
	}

	if (residentialHighWealthIncomeFactor > 0.0f)
	{
		const double highWealthPopulation = GetCityPopulation(0x1031);
		if (highWealthPopulation > 0.0)
		{
			const double lotteryPopulationIncome = highWealthPopulation * static_cast<double>(residentialHighWealthIncomeFactor);

			monthlyIncome += lotteryPopulationIncome;
		}
	}

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
		"%s: monthly income: constant=%lld, R$ factor=%f, R$$ factor=%f, R$$$ factor=%f, current=%lld",
		__FUNCTION__,
		monthlyConstantIncome,
		residentialLowWealthIncomeFactor,
		residentialMedWealthIncomeFactor,
		residentialHighWealthIncomeFactor,
		monthlyIncomeInteger);

	return monthlyIncomeInteger;
}

bool CityLotteryOrdinance::PostCityInit(cISC4City* pCity)
{
	bool result = OrdinanceBase::PostCityInit(pCity);

	if (result)
	{
		pDemandSimulator = pCity->GetDemandSimulator();
		result = pDemandSimulator != nullptr;
	}

	return result;
}

bool CityLotteryOrdinance::PreCityShutdown(cISC4City* pCity)
{
	bool result = OrdinanceBase::PreCityShutdown(pCity);
	pDemandSimulator = nullptr;

	return result;
}

bool CityLotteryOrdinance::Write(cIGZOStream& stream)
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

	if (!stream.SetFloat32(residentialLowWealthIncomeFactor))
	{
		return false;
	}

	if (!stream.SetFloat32(residentialMedWealthIncomeFactor))
	{
		return false;
	}

	if (!stream.SetFloat32(residentialHighWealthIncomeFactor))
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

bool CityLotteryOrdinance::Read(cIGZIStream& stream)
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

	if (!stream.GetFloat32(residentialLowWealthIncomeFactor))
	{
		return false;
	}

	if (!stream.GetFloat32(residentialMedWealthIncomeFactor))
	{
		return false;
	}

	if (!stream.GetFloat32(residentialHighWealthIncomeFactor))
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

uint32_t CityLotteryOrdinance::GetGZCLSID()
{
	logger.WriteLine(LogOptions::OrdinanceAPI, __FUNCTION__);

	return kCityLotteryOrdianceCLSID;
}

float CityLotteryOrdinance::GetCityPopulation(uint32_t groupID)
{
	float value = 0.0f;

	if (pDemandSimulator)
	{
		constexpr uint32_t cityCensusIndex = 0;

		const cISC4Demand* demand = pDemandSimulator->GetDemand(groupID, cityCensusIndex);

		if (demand)
		{
			value = demand->QuerySupplyValue();
		}
	}

	return value;
}
