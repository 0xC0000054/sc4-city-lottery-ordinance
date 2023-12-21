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

#include "Logger.h"
#include <Windows.h>

namespace
{
	std::string GetTimeStamp()
	{
		char buffer[1024]{};

		SYSTEMTIME time;

		GetLocalTime(&time);

		std::snprintf(
			buffer,
			sizeof(buffer),
			"[%hu:%hu:%hu.%hu] ",
			time.wHour,
			time.wMinute,
			time.wSecond,
			time.wMilliseconds);

		return std::string(buffer);
	}

#ifdef _DEBUG
	void PrintLineToDebugOutput(const char* line)
	{
		OutputDebugStringA(line);
		OutputDebugStringA("\n");
	}
#endif // _DEBUG
}

Logger& Logger::GetInstance()
{
	static Logger logger;

	return logger;
}

Logger::Logger() : initialized(false), logFile(), logOptions(LogOptions::Errors)
{
}

Logger::~Logger()
{
	initialized = false;
}

void Logger::Init(std::filesystem::path logFilePath, LogOptions options)
{
	if (!initialized)
	{
		initialized = true;

		logFile.open(logFilePath, std::ofstream::out | std::ofstream::trunc);
		logOptions = options;
	}
}

bool Logger::IsEnabled(LogOptions option) const
{
	return (logOptions & option) != LogOptions::None;
}

void Logger::WriteLogFileHeader(const char* const text)
{
	if (initialized && logFile)
	{
		logFile << text << std::endl;
	}
}

void Logger::WriteLine(LogOptions options, const char* const message)
{
	if ((logOptions & options) == LogOptions::None)
	{
		return;
	}

	WriteLineCore(message);
}

void Logger::WriteLineFormatted(LogOptions options, const char* const format, ...)
{
	if ((logOptions & options) == LogOptions::None)
	{
		return;
	}

	va_list args;
	va_start(args, format);

	va_list argsCopy;
	va_copy(argsCopy, args);

	int formattedStringLength = std::vsnprintf(nullptr, 0, format, argsCopy);

	va_end(argsCopy);

	if (formattedStringLength > 0)
	{
		size_t formattedStringLengthWithNull = static_cast<size_t>(formattedStringLength) + 1;

		std::unique_ptr<char[]> buffer = std::make_unique_for_overwrite<char[]>(formattedStringLengthWithNull);

		std::vsnprintf(buffer.get(), formattedStringLengthWithNull, format, args);

		WriteLineCore(buffer.get());
	}

	va_end(args);
}

void Logger::WriteLineCore(const char* const message)
{
#ifdef _DEBUG
	PrintLineToDebugOutput(message);
#endif // _DEBUG

	if (initialized && logFile)
	{
		logFile << GetTimeStamp() << message << std::endl;
	}
}