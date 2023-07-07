#pragma once

#include <cstdint>
#include <map>
#include <ostream>
#include <format>


#define LOG_DEBUG(msg)  loggeru::logger_t::log_debug(loggeru::logger_t::log_string_t(msg, __FILE__, __LINE__, false))
#define LOG_DEBUG_(fmt, ...)  loggeru::logger_t::log_debug(loggeru::logger_t::log_string_t(fmt, __FILE__, __LINE__, true), __VA_ARGS__)

#define LOG_INFO(msg)  loggeru::logger_t::log_info(loggeru::logger_t::log_string_t(msg, __FILE__, __LINE__, false))
#define LOG_INFO_(fmt, ...)  loggeru::logger_t::log_info(loggeru::logger_t::log_string_t(fmt, __FILE__, __LINE__, true), __VA_ARGS__)

#define LOG_WARNING(msg)  loggeru::logger_t::log_warning(loggeru::logger_t::log_string_t(msg, __FILE__, __LINE__, false))
#define LOG_WARNING_(fmt, ...)  loggeru::logger_t::log_warning(loggeru::logger_t::log_string_t(fmt, __FILE__, __LINE__, true), __VA_ARGS__)

#define LOG_ERROR(msg)  loggeru::logger_t::log_error(loggeru::logger_t::log_string_t(msg, __FILE__, __LINE__, false))
#define LOG_ERROR_(fmt, ...)  loggeru::logger_t::log_error(loggeru::logger_t::log_string_t(fmt, __FILE__, __LINE__, true), __VA_ARGS__)

#define LOG_CRITICAL(msg)  loggeru::logger_t::log_critical(loggeru::logger_t::log_string_t(msg, __FILE__, __LINE__, false))
#define LOG_CRITICAL_(fmt, ...) loggeru::logger_t::log_critical(loggeru::logger_t::log_string_t(fmt, __FILE__, __LINE__, true), __VA_ARGS__)

#define ASSERT(ptr) if (!(ptr)) {LOG_ERROR("ASSERTION FAILED:" #ptr " was nullptr/false\n"); return;}
#define ASSERT_MSG(ptr, msg) if (!(ptr)) {LOG_ERROR(msg); throw;}
#define ASSERT_MSG_(ptr, fmt, ...) if (!(ptr)) { LOG_ERROR_(fmt, ...); throw; }


namespace loggeru
{
	class logger_t
	{
		enum log_level : uint8_t
		{
			ll_debug = 0b0000'0001,
			ll_info = 0b0000'0010,
			ll_warning = 0b0000'0011,
			ll_error = 0b0000'0100,
			ll_critical = 0b0000'0101,
		};

		class base_logger_t
		{
		protected:
			std::ostream* os = nullptr;
		public:
			base_logger_t() = default;
			virtual ~base_logger_t() = default;

			virtual void log(const std::string& message, bool append_timestamp = false);
		};

		class file_logger_t final : public base_logger_t
		{
		public:
			file_logger_t(const file_logger_t& other) = delete;
			file_logger_t(file_logger_t&& other) noexcept = delete;
			file_logger_t& operator=(const file_logger_t& other) = delete;
			file_logger_t& operator=(file_logger_t&& other) noexcept = delete;

			explicit file_logger_t(std::string filename);

			~file_logger_t() override;

		private:
			std::string filename;
		};

		class console_logger_t final : public base_logger_t
		{
		public:
			console_logger_t();
		};

	public:
		struct log_string_t
		{
			std::string message{};
			std::string file{};
			long line{};
			bool format{ false };
		};

		~logger_t() = default;
		logger_t(const logger_t& other) = delete;
		logger_t(logger_t&& other) noexcept = delete;
		logger_t& operator=(const logger_t& other) = delete;
		logger_t& operator=(logger_t&& other) noexcept = delete;

		static void initialize();
		static void release();

		template <typename ... args_t>
		static void log_debug(const log_string_t& fmt, args_t&&... args);

		template <typename ... args_t>
		static void log_warning(const log_string_t& fmt, args_t&&... args);

		template <typename ... args_t>
		static void log_info(const log_string_t& fmt, args_t&&... args);

		template <typename ... args_t>
		static bool log_error(const log_string_t& fmt, args_t&&... args);

		template <class ... args_t>
		static bool log_critical(const log_string_t& fmt, args_t&& ... args);

		static void clear_console();
		static void append_timestamp(const bool append) { should_append_timestamp = append; }

		// Starts logging with default filename: Log_{day}_{month}_{year}_{hours}_{minutes}_{seconds}.log
		static void start_file_logging();
		static void start_file_logging(const std::string& filename);
		static void stop_file_logging();

	private:
		logger_t() = default;

		static bool process_log(log_level level, const log_string_t& fmt, std::format_args args);

		static bool should_append_timestamp;

		static console_logger_t* p_console_logger;
		static file_logger_t* p_file_logger;

		static std::map<log_level, std::string> level_strings;
		static std::map<log_level, std::string> level_color_codes;
	};

	template <typename ...args_t>
	void logger_t::log_info(const log_string_t& fmt, args_t&&... args)
	{
		process_log(ll_info, fmt, std::make_format_args(args...));
	}

	template <typename ... args_t>
	void logger_t::log_debug(const log_string_t& fmt, args_t&&... args)
	{
		process_log(ll_debug, fmt, std::make_format_args(args...));
	}

	template <typename ... args_t>
	void logger_t::log_warning(const log_string_t& fmt, args_t&&... args)
	{
		process_log(ll_warning, fmt, std::make_format_args(args...));
	}

	template <typename ... args_t>
	bool logger_t::log_error(const log_string_t& fmt, args_t&&... args)
	{
		return process_log(ll_error, fmt, std::make_format_args(args...));
	}

	template <typename ... args_t>
	bool logger_t::log_critical(const log_string_t& fmt, args_t&&... args)
	{
		return process_log(ll_critical, fmt, std::make_format_args(args...));
	}

}