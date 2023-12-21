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
#include "OrdinanceBase.h"

class ISettings;

class CityLotteryOrdinance final : public OrdinanceBase
{
public:

	CityLotteryOrdinance();

	void UpdateOrdinanceData(const ISettings& settings);

	int64_t GetCurrentMonthlyIncome() override;

	bool PostCityInit(cISC4City* pCity) override;

	bool PreCityShutdown(cISC4City* pCity) override;

	bool Write(cIGZOStream& stream) override;

	bool Read(cIGZIStream& stream) override;

	uint32_t GetGZCLSID() override;

private:

	float GetCityPopulation(uint32_t groupID);

	cISC4DemandSimulator* pDemandSimulator;
	float residentialLowWealthIncomeFactor;
	float residentialMedWealthIncomeFactor;
	float residentialHighWealthIncomeFactor;
};

