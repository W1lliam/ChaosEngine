#pragma once

#include "spdlog/sinks/base_sink.h"
#include <vector>
#include "ChaosEditor/Console/Console.h"

#include <ChaosEngine.h>
#include <iostream>

template<typename Mutex>
class EditorSink : public spdlog::sinks::base_sink <Mutex>
{
public:

	EditorSink(Console* p_console, std::shared_ptr<spdlog::logger> p_logger) : m_console(p_console)
	{
		p_logger->sinks().emplace_back(this);
	}

	void SetConsole(Console* p_console)
	{
		m_console = p_console;
	}

protected:

	void sink_it_(const spdlog::details::log_msg& msg) override
	{

		// log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
		// msg.raw contains pre formatted log

		// If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
		fmt::memory_buffer formatted;
		sink::formatter_->format(msg, formatted);
		m_msgs.emplace_back(fmt::to_string(formatted).c_str());

		flush_();
	}

	void flush_() override
	{
		if (!m_console) return;
		for (QString& msg : m_msgs)
		{
			m_console->appendMessage(msg);
		}

		m_msgs.clear();
	}

	std::vector<QString> m_msgs;
	Console* m_console;
};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using EditorSinkMutex	= EditorSink<std::mutex>;
using EditorSinkST		= EditorSink<spdlog::details::null_mutex>;