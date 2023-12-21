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

#include "Settings.h"
#include "Logger.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"

namespace
{
	// Throws an exception of the value is out of range.
	float CheckValueRange(float value, float min, float max, const char* name)
	{
		if (value < min)
		{
			char buffer[1024]{};

			std::snprintf(
				buffer,
				sizeof(buffer),
				"%s is less than %f.",
				name,
				min);

			throw std::runtime_error(buffer);
		}
		else if (value > max)
		{
			char buffer[1024]{};

			std::snprintf(
				buffer,
				sizeof(buffer),
				"%s is less than %f.",
				name,
				min);

			throw std::runtime_error(buffer);
		}

		return value;
	}
}

Settings::Settings()
	: monthlyConstantIncome(500),
	  residentialLowWealthFactor(0.05f),
	  residentialMedWealthFactor(0.03f),
	  residentialHighWealthFactor(0.01f),
	  cityLotteryOrdinanceEffects()
{
}

void Settings::Load(const std::filesystem::path& path)
{
	std::ifstream stream(path, std::ifstream::in);

	if (!stream)
	{
		throw std::runtime_error("Failed top open the settings file.");
	}

	boost::property_tree::ptree tree;

	boost::property_tree::ini_parser::read_ini(stream, tree);

	monthlyConstantIncome = tree.get<int64_t>("CityLotteryOrdinance.MonthlyConstantIncome");
	residentialLowWealthFactor = tree.get<float>("CityLotteryOrdinance.R$IncomeFactor");
	residentialMedWealthFactor = tree.get<float>("CityLotteryOrdinance.R$$IncomeFactor");
	residentialHighWealthFactor = tree.get<float>("CityLotteryOrdinance.R$$$IncomeFactor");

	float crimeEffectMultiplier = CheckValueRange(
		tree.get<float>("CityLotteryOrdinance.CrimeEffectMultiplier"),
		0.01f,
		2.0f,
		"CrimeEffectMultiplier");

	float cs1Demandffect = CheckValueRange(
		tree.get<float>("CityLotteryOrdinance.Cs$DemandEffect"),
		0.01f,
		2.0f,
		"Cs$DemandEffect");

	float schoolEQBoostEffect = CheckValueRange(
		tree.get<float>("CityLotteryOrdinance.SchoolEQBoostEffect"),
		0,
		200,
		"SchoolEQBoostEffect");

	cityLotteryOrdinanceEffects.RemoveAllProperties();

	if (crimeEffectMultiplier != 1.0f)
	{
		cityLotteryOrdinanceEffects.AddProperty(0x28ed0380, crimeEffectMultiplier);
	}

	if (cs1Demandffect != 1.0f)
	{
		cityLotteryOrdinanceEffects.AddProperty(0x2a653110, cs1Demandffect);
	}

	if (schoolEQBoostEffect != 100)
	{
		cityLotteryOrdinanceEffects.AddProperty(0xa92d9d7a, schoolEQBoostEffect);
	}
}

int64_t Settings::MonthlyConstantIncome() const
{
	return monthlyConstantIncome;
}

float Settings::ResidentialLowWealthFactor() const
{
	return residentialLowWealthFactor;
}

float Settings::ResidentialMedWealthFactor() const
{
	return residentialMedWealthFactor;
}

float Settings::ResidentialHighWealthFactor() const
{
	return residentialHighWealthFactor;
}

OrdinancePropertyHolder Settings::OrdinanceEffects() const
{
	return cityLotteryOrdinanceEffects;
}
