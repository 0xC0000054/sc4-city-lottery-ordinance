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
#include "ISettings.h"
#include "OrdinancePropertyHolder.h"
#include <filesystem>

class Settings : public ISettings
{
public:
	Settings();

	void Load(const std::filesystem::path& path);

	// Inherited via ISettings

	int64_t MonthlyConstantIncome() const override;
	float ResidentialLowWealthFactor() const override;
	float ResidentialMedWealthFactor() const override;
	float ResidentialHighWealthFactor() const override;
	OrdinancePropertyHolder OrdinanceEffects() const override;


private:

	int64_t monthlyConstantIncome;
	float residentialLowWealthFactor;
	float residentialMedWealthFactor;
	float residentialHighWealthFactor;
	OrdinancePropertyHolder cityLotteryOrdinanceEffects;
};

