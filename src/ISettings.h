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
#include "stdint.h"
#include "OrdinancePropertyHolder.h"

class ISettings
{
public:

	virtual int64_t MonthlyConstantIncome() const = 0;

	virtual float ResidentialLowWealthFactor() const = 0;

	virtual float ResidentialMedWealthFactor() const = 0;

	virtual float ResidentialHighWealthFactor() const = 0;

	virtual OrdinancePropertyHolder OrdinanceEffects() const = 0;
};