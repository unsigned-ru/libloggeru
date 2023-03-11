#include "loggeru.h"
#include "loggeru/console_colors.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;


loggeru::logger_t::console_logger_t* loggeru::logger_t::p_console_logger = nullptr;
loggeru::logger_t::file_logger_t* loggeru::logger_t::p_file_logger = nullptr;
bool loggeru::logger_t::should_append_timestamp = false;

#define SAFE_DELETE(p) if (p) { delete (p); (p) = nullptr; }

std::map<loggeru::logger_t::log_level, std::string> loggeru::logger_t::level_strings = {
	{ll_debug,		"DEBUG"},
	{ll_info,		"INFO"},
	{ll_warning,	"WARNING"},
	{ll_error,		"ERROR"},
	{ll_critical,	"CRITICAL"}
};

std::map<loggeru::logger_t::log_level, std::string> loggeru::logger_t::level_color_codes = {
	{ll_debug,		KCYN},
	{ll_info,		KWHT},
	{ll_warning,	KYEL},
	{ll_error,		KRED},
	{ll_critical,	KRED},
};

void loggeru::logger_t::base_logger_t::log(const std::string& message, bool append_timestamp)
{
	if (append_timestamp)
	{
		const time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm now;

#ifdef WIN32
		std::ignore = localtime_s(&now, &tt);
#else
		std::ignore = localtime_r(&tt, &now);
#endif

		const std::string time_stamp = fmt::format("[{:04}-{:02}-{:02} > {:02}:{:02}:{:02}] ", now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
		(*os) << time_stamp;
	}

	(*os) << message;
	os->flush();
}

loggeru::logger_t::file_logger_t::file_logger_t(std::string filename)
	: filename(std::move(filename))
{
	os = new std::ofstream(this->filename.c_str());
}

loggeru::logger_t::file_logger_t::~file_logger_t()
{
	if (os)
	{
		auto* of = dynamic_cast<std::ofstream*>(os);
		of->close();
		delete os;
	}
}

loggeru::logger_t::console_logger_t::console_logger_t()
{
	os = &std::cout;
}

void loggeru::logger_t::initialize()
{
	p_console_logger = new console_logger_t();
}

void loggeru::logger_t::release()
{
	SAFE_DELETE(p_console_logger);
	SAFE_DELETE(p_file_logger);
}

void loggeru::logger_t::clear_console()
{
	std::ignore = std::system("cls");
}

void loggeru::logger_t::start_file_logging()
{
	const time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm time;
#ifdef WIN32
	std::ignore = localtime_s(&time, &tt);
#else
	std::ignore = localtime_r(&tt, &time);
#endif

	start_file_logging(fmt::format("Log_{:02}-{:02}-{}_{:02}-{:02}-{:02}.log", time.tm_mday, time.tm_mon, time.tm_year, time.tm_hour, time.tm_min, time.tm_sec));
}

void loggeru::logger_t::start_file_logging(const std::string& filename)
{
	SAFE_DELETE(p_file_logger);

	p_file_logger = new file_logger_t(filename);
}

void loggeru::logger_t::stop_file_logging()
{
	SAFE_DELETE(p_file_logger);
}

bool loggeru::logger_t::process_log(const log_level level, const log_string_t& fmt, const fmt::format_args args)
{
	//Skip Debug message in release build
#ifdef NDEBUG
	if (level == ll_debug) return false;
#endif

	//Generate Message
	std::string log_msg = fmt.format ? fmt::vformat(fmt.message, args) : fmt.message; //DEFAULT FORMATTING

	const auto filename = fs::path(fmt.file).filename().string();
	const std::string level_string = level_strings[level];

	const auto full_log = fmt::format("[{}] > {} (line {}) :: {}\n", level_string, filename, fmt.line, log_msg);

	//Console Log
	if (p_console_logger)
	{
		const std::string& color_code = level_color_codes[level];
		const auto color_log = fmt::format("[{}{}{}] > {} (line {}) :: {}\n", color_code, level_string, RST, filename, fmt.line, log_msg);
		p_console_logger->log(color_log);
	}

	//File Log
	if (p_file_logger)
	{
		p_file_logger->log(full_log, true);
	}

	//throw on critical
	if (level == ll_critical)
	{
		throw std::runtime_error(full_log);
	}

	return true;
}