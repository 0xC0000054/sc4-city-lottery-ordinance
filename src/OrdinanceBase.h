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
#include "cISC4Ordinance.h"
#include "cISC4City.h"
#include "cIGZSerializable.h"
#include "cRZBaseString.h"
#include "OrdinancePropertyHolder.h"
#include "Logger.h"
#include "StringResourceKey.h"

// The base class for a custom ordinance.
class OrdinanceBase : public cISC4Ordinance, protected cIGZSerializable
{
public:

	/**
	 * @brief Constructs an instance of the class.
	 * @param clsid The unique ID that identifies this ordinance.
	 * @param name The name of the ordinance.
	 * @param description The ordinance description.
	 * @param enactmentIncome Income generated by enacting the ordinance.
	 * @param retracmentIncome Cost of retracting the ordinance.
	 * @param monthlyConstantIncome The monthly constant income/expense.
	 * @param monthlyIncomeFactor The factor applied to the monthly cost based on the city population.
	 * @param isIncomeOrdinance True if the ordinance generates income; otherwise, false.
	*/
	OrdinanceBase(
		uint32_t clsid,
		const char* name,
		const char* description,
		int64_t enactmentIncome,
		int64_t retracmentIncome,
		int64_t monthlyConstantIncome,
		float monthlyIncomeFactor,
		bool isIncomeOrdinance);

	/**
	 * @brief Constructs an instance of the class.
	 * @param clsid The unique ID that identifies this ordinance.
	 * @param name The name of the ordinance.
	 * @param description The ordinance description.
	 * @param enactmentIncome Income generated by enacting the ordinance.
	 * @param retracmentIncome Cost of retracting the ordinance.
	 * @param monthlyConstantIncome The monthly constant income/expense.
	 * @param monthlyIncomeFactor The factor applied to the monthly cost based on the city population.
	 * @param isIncomeOrdinance True if the ordinance generates income; otherwise, false.
	 * @param properties A list of in-game effects that the ordinance has.
	*/
	OrdinanceBase(
		uint32_t clsid,
		const char* name,
		const char* description,
		int64_t enactmentIncome,
		int64_t retracmentIncome,
		int64_t monthlyConstantIncome,
		float monthlyIncomeFactor,
		bool isIncomeOrdinance,
		const OrdinancePropertyHolder& properties);

	/**
	 * @brief Constructs an instance of the class.
	 * @param clsid The unique ID that identifies this ordinance.
	 * @param name The name of the ordinance.
	 * @param nameKey The key that identifies the LTEXT resource containing the localized name.
	 * @param description The ordinance description.
	 * @param descriptionKey The key that identifies the LTEXT resource containing the localized description.
	 * @param enactmentIncome Income generated by enacting the ordinance.
	 * @param retracmentIncome Cost of retracting the ordinance.
	 * @param monthlyConstantIncome The monthly constant income/expense.
	 * @param monthlyIncomeFactor The factor applied to the monthly cost based on the city population.
	 * @param isIncomeOrdinance True if the ordinance generates income; otherwise, false.
	*/
	OrdinanceBase(
		uint32_t clsid,
		const char* name,
		const StringResourceKey& nameKey,
		const char* description,
		const StringResourceKey& descriptionKey,
		int64_t enactmentIncome,
		int64_t retracmentIncome,
		int64_t monthlyConstantIncome,
		float monthlyIncomeFactor,
		bool isIncomeOrdinance);

	/**
	 * @brief Constructs an instance of the class.
	 * @param clsid The unique ID that identifies this ordinance.
	 * @param name The name of the ordinance.
	 * @param nameKey The key that identifies the LTEXT resource containing the localized name.
	 * @param description The ordinance description.
	 * @param descriptionKey The key that identifies the LTEXT resource containing the localized description.
	 * @param enactmentIncome Income generated by enacting the ordinance.
	 * @param retracmentIncome Cost of retracting the ordinance.
	 * @param monthlyConstantIncome The monthly constant income/expense.
	 * @param monthlyIncomeFactor The factor applied to the monthly cost based on the city population.
	 * @param isIncomeOrdinance True if the ordinance generates income; otherwise, false.
	 * @param properties A list of in-game effects that the ordinance has.
	*/
	OrdinanceBase(
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
		const OrdinancePropertyHolder& properties);

	OrdinanceBase(const OrdinanceBase& other);
	OrdinanceBase(OrdinanceBase&& other) noexcept;

	OrdinanceBase& operator=(const OrdinanceBase& other);
	OrdinanceBase& operator=(OrdinanceBase&& other) noexcept;

	bool QueryInterface(uint32_t riid, void** ppvObj);

	uint32_t AddRef();

	uint32_t Release();

	bool Init(void);

	bool Shutdown(void);

	/**
	 * @brief Gets the monthly income/expense for this ordinance.
	 * @return The monthly income/expense for this ordinance.
	 * @remarks This method uses a default algorithm of
	 * <monthly constent income> + (<city population> x <monthly income factor>).
	 * This method can be overridden to use a custom algorithm.
	*/
	virtual int64_t GetCurrentMonthlyIncome(void);

	/**
	 * @brief Gets the unique ordinance ID.
	 * @return The unique ordinance ID.
	*/
	uint32_t GetID(void) const;

	/**
	 * @brief Gets the ordinance name.
	 * @return The ordinance name.
	*/
	cIGZString* GetName(void);

	/**
	 * @brief Gets the ordinance description.
	 * @return The ordinance description.
	*/
	cIGZString* GetDescription(void);

	/**
	 * @brief Gets the in-game year that the ordinance becomes available.
	 * @return The in-game year that the ordinance becomes available.
	 *
	 * @remarks By default the ordinances will be available at the start of the game.
	 * This method can be overridden to set a custom start year.
	 * Note that in SimCity 4 the in-game date starts in the year 2000.
	*/
	virtual uint32_t GetYearFirstAvailable(void);

	/**
	 * @brief The chance that the ordinance will become available each month.
	 * This feature is not implemented.
	 *
	 * @return The chance that the ordinance will become available each month.
	*/
	virtual SC4Percentage GetChanceAvailability(void);

	/**
	 * @brief Gets the revenue generated by enacting this ordinance.
	 * @return The revenue generated by enacting this ordinance.
	*/
	int64_t GetEnactmentIncome(void);

	/**
	 * @brief Gets the cost of retracting this ordinance.
	 * @return The cost of retracting this ordinance.
	*/
	int64_t GetRetracmentIncome(void);

	/**
	 * @brief Gets the monthly constant income/expense value for the ordinance.
	 * @return The monthly constant income/expense value for the ordinance.
	*/
	int64_t GetMonthlyConstantIncome(void);

	/**
	 * @brief Gets the factor applied to the ordinance cost.
	 * @return The factor applied to the ordinance cost.
	*/
	float GetMonthlyIncomeFactor(void);

	/**
	 * @brief Gets a collection of the ordinance properties/effects.
	 * @return The ordinance properties/effects.
	*/
	cISCPropertyHolder* GetMiscProperties();

	/**
	 * @brief Gets a value that identifies the in-game advisor related to this
	 * ordinance. This is unused in SimCity 4 and always returns zero.
	 * @return Zero.
	*/
	uint32_t GetAdvisorID(void);

	bool IsAvailable(void);

	bool IsOn(void);

	bool IsEnabled(void);

	/**
	 * @brief Gets the monthly adjusted income.
	 * @return The monthly adjusted income.
	*/
	virtual int64_t GetMonthlyAdjustedIncome(void);

	/**
	 * @brief Defines the conditions that are required for the ordinance to become available.
	 * @return True if the ordinance should become available in the menu; otherwise, false.
	 * @remarks By default the only required condition is the starting year @see GetYearFirstAvailable.
	 * This method can be overridden to provide custom conditions for the ordinance availability.
	*/
	virtual bool CheckConditions(void);

	/**
	 * @brief Gets a value indicating whether this ordinance generates income.
	 * @return True if this ordinance generates income; otherwise, false.
	*/
	bool IsIncomeOrdinance(void);

	/**
	 * @brief This is called during the monthly ordinance simulation.
	 * @return True if successful; otherwise, false.
	*/
	virtual bool Simulate(void);

	bool SetAvailable(bool isAvailable);

	bool SetOn(bool isOn);

	bool SetEnabled(bool isEnabled);

	bool ForceAvailable(bool isAvailable);

	bool ForceOn(bool isOn);

	bool ForceEnabled(bool isEnabled);

	bool ForceMonthlyAdjustedIncome(int64_t monthlyAdjustedIncome);

	/**
	 * @brief This method must be called to initialize the ordinance.
	 *
	 * @param A pointer to the cISC4City object.
	 * @return True on success; otherwise, false.
	*/
	virtual bool PostCityInit(cISC4City* pCity);

	/**
	* @brief This method must be called to shutdown the ordinance.
	*
	* @param A pointer to the cISC4City object.
	* @return True on success; otherwise, false.
	*/
	virtual bool PreCityShutdown(cISC4City* pCity);

protected:

	static bool ReadBool(cIGZIStream& stream, bool& value);
	static bool WriteBool(cIGZOStream& stream, bool value);

	virtual bool Write(cIGZOStream& stream);
	virtual bool Read(cIGZIStream& stream);
	uint32_t GetGZCLSID();

	Logger& logger;

	uint32_t clsid;
	cRZBaseString name;
	cRZBaseString description;
	int64_t enactmentIncome;
	int64_t retracmentIncome;
	int64_t monthlyConstantIncome;
	int64_t monthlyAdjustedIncome;
	float monthlyIncomeFactor;
	bool isIncomeOrdinance;
	OrdinancePropertyHolder miscProperties;
	bool initialized;
	bool available;
	bool on;
	bool enabled;
	bool haveDeserialized;

private:

	void LoadLocalizedStringResources();

	uint32_t refCount;
	cISC4ResidentialSimulator* pResidentialSimulator;
	cISC4Simulator* pSimulator;
	StringResourceKey nameKey;
	StringResourceKey descriptionKey;
};
