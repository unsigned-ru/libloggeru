#include "Logerru.h"
#include "loggeru/ConsoleColors.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

using namespace loggeru;

Loggeru::ConsoleLogger* Loggeru::m_ConsoleLogger = nullptr;
Loggeru::FileLogger* Loggeru::m_FileLogger = nullptr;
bool Loggeru::m_AppendTimestamp = false;


#define SAFE_DELETE(p) if (p) { delete (p); (p) = nullptr; }

std::map<Loggeru::LogLevel, std::string> Loggeru::m_LevelToStr = {
	{LogLevel::Debug, "DEBUG"},
	{LogLevel::Info, "INFO"},
	{LogLevel::Warning, "WARNING"},
	{LogLevel::Error, "ERROR"},
	{LogLevel::Critical, "CRITICAL"}
};

std::map<Loggeru::LogLevel, std::string> Loggeru::m_LevelToConsoleStyle = {
	{LogLevel::Debug, KCYN},
	{LogLevel::Info, KWHT},
	{LogLevel::Warning, KYEL},
	{LogLevel::Error, KRED},
	{LogLevel::Critical, KRED},
};

void Loggeru::BaseLogger::Log(const std::string& message, bool appendTimestamp)
{
	if (appendTimestamp)
	{
		const time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm now;

#ifdef WIN32
		std::ignore = localtime_s(&now, &tt);
#else
		std::ignore = localtime_r(&tt, &now);
#endif

		const std::string timeStamp = fmt::format("[{:04}-{:02}-{:02} > {:02}:{:02}:{:02}] ", now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
		(*m_os) << timeStamp;
	}

	(*m_os) << message;
	m_os->flush();
}

Loggeru::FileLogger::FileLogger(std::string fileName)
	: m_filename(std::move(fileName))
{
	m_os = new std::ofstream(m_filename.c_str());
}

Loggeru::FileLogger::~FileLogger()
{
	if (m_os)
	{
		auto* of = static_cast<std::ofstream*>(m_os);
		of->close();
		delete m_os;
	}
}

Loggeru::ConsoleLogger::ConsoleLogger()
{
	m_os = &std::cout;
}

void Loggeru::Initialize()
{
	m_ConsoleLogger = new ConsoleLogger();
}

void Loggeru::Release()
{
	SAFE_DELETE(m_ConsoleLogger);
	SAFE_DELETE(m_FileLogger);
}

void Loggeru::ClearConsole()
{
	std::ignore = std::system("cls");
}

void Loggeru::StartFileLogging()
{
	const time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm time;
#ifdef WIN32
	std::ignore = localtime_s(&time, &tt);
#else
	std::ignore = localtime_r(&tt, &time);
#endif

	StartFileLogging(fmt::format("Log_{:02}-{:02}-{}_{:02}-{:02}-{:02}.log", time.tm_mday, time.tm_mon, time.tm_year, time.tm_hour, time.tm_min, time.tm_sec));
}

void Loggeru::StartFileLogging(const std::string& fileName)
{
	SAFE_DELETE(m_FileLogger);

	m_FileLogger = new FileLogger(fileName);
}

void Loggeru::StopFileLogging()
{
	SAFE_DELETE(m_FileLogger);
}

bool Loggeru::ProcessLog(LogLevel level, const LogString& fmt, fmt::format_args args)
{
	//Skip Debug message in release build
#ifdef NDEBUG
	if (level == LogLevel::Debug) return false;
#endif

	//Generate Message
	std::string logMsg = fmt.format ? fmt::vformat(fmt.message, args) : fmt.message; //DEFAULT FORMATTING

	const auto filename = fs::path(fmt.file).filename().string();
	const std::string strLevel = m_LevelToStr[level];

	const auto full_log = fmt::format("[{}] > {} (line {}) :: {}\n", strLevel, filename, fmt.line, logMsg);

	//Console Log
	if (m_ConsoleLogger)
	{
		const auto colorCode = m_LevelToConsoleStyle[level];
		const auto color_log = fmt::format("[{}{}{}] > {} (line {}) :: {}\n", colorCode, strLevel, RST, filename, fmt.line, logMsg);
		m_ConsoleLogger->Log(color_log);
	}

	//File Log
	if (m_FileLogger)
	{
		m_FileLogger->Log(full_log, true);
	}

	//Show MessageBox
	if (level == LogLevel::Critical)
	{
		throw std::runtime_error(full_log);
	}

	return true;
}