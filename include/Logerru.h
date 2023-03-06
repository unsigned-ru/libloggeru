#pragma once

#include <map>
#include <ostream>
#include <fmt/format.h>


#define LOG_DEBUG(msg)  Loggeru::LogDebug(Loggeru::LogString{msg, __FILE__, __LINE__, false})
#define LOG_DEBUG_(fmt, ...)  Loggeru::LogDebug(Loggeru::LogString{fmt, __FILE__, __LINE__, true}, __VA_ARGS__)

#define LOG_INFO(msg)  Loggeru::LogInfo(Loggeru::LogString{msg, __FILE__, __LINE__, false})
#define LOG_INFO_(fmt, ...)  Loggeru::LogInfo(Loggeru::LogString{fmt, __FILE__, __LINE__, true}, __VA_ARGS__)

#define LOG_WARNING(msg)  Loggeru::LogWarning(Loggeru::LogString{msg, __FILE__, __LINE__, false})
#define LOG_WARNING_(fmt, ...)  Loggeru::LogWarning(Loggeru::LogString{fmt, __FILE__, __LINE__, true}, __VA_ARGS__)

#define LOG_ERROR(msg)  Loggeru::LogError(Loggeru::LogString{msg, __FILE__, __LINE__, false})
#define LOG_ERROR_(fmt, ...)  Loggeru::LogError(Loggeru::LogString{fmt, __FILE__, __LINE__, true}, __VA_ARGS__)

#define LOG_CRITICAL(msg)  Loggeru::LogCritical(Loggeru::LogString{msg, __FILE__, __LINE__, false})
#define LOG_CRITICAL_(fmt, ...)  Loggeru::LogCritical(Loggeru::LogString{fmt, __FILE__, __LINE__, true}, __VA_ARGS__)

#define ASSERT(ptr) if (!(ptr)) {LOG_ERROR("ASSERTION FAILED:" #ptr " was nullptr/false\n"); return;}
#define ASSERT_MSG(ptr, msg) if (!(ptr)) {LOG_ERROR(msg); throw;}
#define ASSERT_MSG_(ptr, fmt, ...) if (!(ptr)) { LOG_ERROR_(fmt, ...); throw; }


namespace loggeru
{
	class Loggeru
	{
		enum class LogLevel : uint8_t
		{
			Debug = 0b0000'0001,
			Info = 0b0000'0010,
			Warning = 0b0000'0011,
			Error = 0b0000'0100,
			Critical = 0b0000'0101,
		};

		class BaseLogger
		{
		protected:
			std::ostream* m_os = nullptr;
		public:
			BaseLogger() = default;
			virtual ~BaseLogger() = default;

			virtual void Log(const std::string& message, bool appendTimestamp = false);
		};

		class FileLogger final : public BaseLogger
		{
			std::string m_filename;
		public:
			FileLogger(const FileLogger& other) = delete;
			FileLogger(FileLogger&& other) noexcept = delete;
			FileLogger& operator=(const FileLogger& other) = delete;
			FileLogger& operator=(FileLogger&& other) noexcept = delete;

			explicit FileLogger(std::string fileName);

			~FileLogger() override;
		};

		class ConsoleLogger final : public BaseLogger
		{
		public:
			ConsoleLogger();
		};

	public:
		struct LogString
		{
			std::string message{};
			std::string file{};
			long line{};
			bool format{ false };
		};

		~Loggeru() = default;
		Loggeru(const Loggeru& other) = delete;
		Loggeru(Loggeru&& other) noexcept = delete;
		Loggeru& operator=(const Loggeru& other) = delete;
		Loggeru& operator=(Loggeru&& other) noexcept = delete;

		static void Initialize();
		static void Release();

		template <typename ... Args>
		static void LogDebug(const LogString& fmt, Args&&... args);

		template <typename ... Args>
		static void LogWarning(const LogString& fmt, Args&&... args);

		template <typename ... Args>
		static void LogInfo(const LogString& fmt, Args&&... args);

		template <typename ... Args>
		static bool LogError(const LogString& fmt, Args&&... args);

		template <class ... Args>
		static bool LogCritical(const LogString& fmt, Args&& ... args);

		static void ClearConsole();
		static void AppendTimestamp(bool append) { m_AppendTimestamp = append; }

		// Starts logging with default filename: Log_{day}_{month}_{year}_{hours}_{minutes}_{seconds}.log
		static void StartFileLogging();
		static void StartFileLogging(const std::string& fileName);
		static void StopFileLogging();

	private:
		Loggeru() = default;

		static bool ProcessLog(LogLevel level, const LogString& fmt, fmt::format_args args);

		static bool m_AppendTimestamp;

		static ConsoleLogger* m_ConsoleLogger;
		static FileLogger* m_FileLogger;

		static std::map<LogLevel, std::string> m_LevelToStr;
		static std::map<LogLevel, std::string> m_LevelToConsoleStyle;
	};

	template <typename ... Args>
	void Loggeru::LogInfo(const LogString& fmt, Args&&... args)
	{
		ProcessLog(LogLevel::Info, fmt, fmt::make_format_args(args...));
	}

	template <typename ... Args>
	void Loggeru::LogDebug(const LogString& fmt, Args&&... args)
	{
		ProcessLog(LogLevel::Debug, fmt, fmt::make_format_args(args...));
	}

	template <typename ... Args>
	void Loggeru::LogWarning(const LogString& fmt, Args&&... args)
	{
		ProcessLog(LogLevel::Warning, fmt, fmt::make_format_args(args...));
	}

	template <typename ... Args>
	bool Loggeru::LogError(const LogString& fmt, Args&&... args)
	{
		return ProcessLog(LogLevel::Error, fmt, fmt::make_format_args(args...));
	}

	template <typename ... Args>
	bool Loggeru::LogCritical(const LogString& fmt, Args&&... args)
	{
		return ProcessLog(LogLevel::Critical, fmt, fmt::make_format_args(args...));
	}

}