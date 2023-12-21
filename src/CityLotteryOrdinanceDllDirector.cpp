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

#include "version.h"
#include "CityLotteryOrdinance.h"
#include "Logger.h"
#include "Settings.h"
#include "cIGZFrameWork.h"
#include "cIGZApp.h"
#include "cIGZLanguageManager.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceManager.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC4Ordinance.h"
#include "cISC4OrdinanceSimulator.h"
#include "cISC4ResidentialSimulator.h"
#include "cISC4Simulator.h"
#include "cIGZMessageServer2.h"
#include "cIGZMessageTarget.h"
#include "cIGZMessageTarget2.h"
#include "cIGZString.h"
#include "cRZMessage2COMDirector.h"
#include "cRZMessage2Standard.h"
#include "cRZBaseString.h"
#include "GZServPtrs.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePreCityShutdown = 0x26D31EC2;

static constexpr uint32_t kCityLotteryOrdinanceDirectorID = 0xc8f8cd0f;

static constexpr std::string_view PluginConfigFileName = "SC4CityLotteryOrdinance.ini";
static constexpr std::string_view PluginLogFileName = "SC4CityLotteryOrdinance.log";

class CityLotteryOrdinanceDllDirector : public cRZMessage2COMDirector
{
public:

	CityLotteryOrdinanceDllDirector()
	{
		std::filesystem::path dllFolderPath = GetDllFolderPath();

		configFilePath = dllFolderPath;
		configFilePath /= PluginConfigFileName;

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogOptions::Errors);
		logger.WriteLogFileHeader("SC4CityLotteryOrdinance v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kCityLotteryOrdinanceDirectorID;
	}

	void EnumClassObjects(cIGZCOMDirector::ClassObjectEnumerationCallback pCallback, void* pContext)
	{
		// The classes you want to add must be initialized in the DLL constructor because
		// the framework calls this method before OnStart or any of the hook callbacks.
		// This method is called once when initializing a director, the list of class IDs
		// it returns is cached by the framework.
		pCallback(cityLotteryOrdinance.GetID(), 0, pContext);
	}

	bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj)
	{
		// To retrieve an instance of a registered class the framework will call the
		// GetClassObject method whenever it needs the director to provide one.

		bool result = false;

		if (rclsid == cityLotteryOrdinance.GetID())
		{
			result = cityLotteryOrdinance.QueryInterface(riid, ppvObj);
		}

		return result;
	}

	void PostCityInit(cIGZMessage2Standard* pStandardMsg)
	{
		cISC4City* pCity = reinterpret_cast<cISC4City*>(pStandardMsg->GetIGZUnknown());

		if (pCity)
		{
			cISC4OrdinanceSimulator* pOrdinanceSimulator = pCity->GetOrdinanceSimulator();

			if (pOrdinanceSimulator)
			{
				cISC4Ordinance* pOrdinance = pOrdinanceSimulator->GetOrdinanceByID(cityLotteryOrdinance.GetID());
				bool ordinanceInitialized = false;

				if (!pOrdinance)
				{
					// Only add the ordinance if it is not already present. If it is part
					// of the city save file it will have already been loaded at this point.
					cityLotteryOrdinance.PostCityInit(pCity);
					ordinanceInitialized = true;

					pOrdinanceSimulator->AddOrdinance(cityLotteryOrdinance);
					pOrdinance = pOrdinanceSimulator->GetOrdinanceByID(cityLotteryOrdinance.GetID());
				}

				if (pOrdinance)
				{
					CityLotteryOrdinance* pCityLotteryOrdinance = reinterpret_cast<CityLotteryOrdinance*>(pOrdinance);

					if (!ordinanceInitialized)
					{
						pCityLotteryOrdinance->PostCityInit(pCity);
					}

					pCityLotteryOrdinance->UpdateOrdinanceData(settings);
				}
				else
				{
					Logger::GetInstance().WriteLine(LogOptions::Errors, "Failed to add the ordinance.");
				}
			}
		}
	}

	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg)
	{
		cISC4City* pCity = reinterpret_cast<cISC4City*>(pStandardMsg->GetIGZUnknown());

		if (pCity)
		{
			cISC4OrdinanceSimulator* pOrdinanceSimulator = pCity->GetOrdinanceSimulator();

			if (pOrdinanceSimulator)
			{
				cityLotteryOrdinance.PreCityShutdown(pCity);
				pOrdinanceSimulator->RemoveOrdinance(cityLotteryOrdinance);
			}
		}
	}

	bool DoMessage(cIGZMessage2* pMessage)
	{
		cIGZMessage2Standard* pStandardMsg = static_cast<cIGZMessage2Standard*>(pMessage);
		uint32_t dwType = pMessage->GetType();

		switch (dwType)
		{
		case kSC4MessagePostCityInit:
			PostCityInit(pStandardMsg);
			break;
		case kSC4MessagePreCityShutdown:
			PreCityShutdown(pStandardMsg);
			break;
		}

		return true;
	}

	bool PostAppInit()
	{
		Logger& logger = Logger::GetInstance();

		try
		{
			settings.Load(configFilePath);
		}
		catch (const std::exception& e)
		{
			logger.WriteLine(LogOptions::Errors, e.what());
			return false;
		}

		cIGZMessageServer2Ptr pMsgServ;
		if (pMsgServ)
		{
			std::vector<uint32_t> requiredNotifications;
			requiredNotifications.push_back(kSC4MessagePostCityInit);
			requiredNotifications.push_back(kSC4MessagePreCityShutdown);

			for (uint32_t messageID : requiredNotifications)
			{
				if (!pMsgServ->AddNotification(this, messageID))
				{
					logger.WriteLine(LogOptions::Errors, "Failed to subscribe to the required notifications.");
					return false;
				}
			}
		}
		else
		{
			logger.WriteLine(LogOptions::Errors, "Failed to subscribe to the required notifications.");
			return false;
		}

		return true;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		cIGZFrameWork* const pFramework = RZGetFrameWork();

		if (pFramework->GetState() < cIGZFrameWork::kStatePreAppInit)
		{
			pFramework->AddHook(this);
		}
		else
		{
			PreAppInit();
		}
		return true;
	}

private:
	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}

	std::filesystem::path configFilePath;
	CityLotteryOrdinance cityLotteryOrdinance;
	Settings settings;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static CityLotteryOrdinanceDllDirector sDirector;
	return &sDirector;
}