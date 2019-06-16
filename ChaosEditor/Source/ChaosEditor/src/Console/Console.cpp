#include "ChaosEditor/Console/Console.h"
#include <QScrollBar>
#include <QFile>
#include "ChaosEditor/Console/EditorSink.h"

Console::Console(QWidget* p_parent) : QTextEdit(p_parent)
{
	m_coreSink		= new EditorSinkMutex(this, Chaos::Debug::Log::Get().GetCoreLogger());
	m_clientSink	= new EditorSinkMutex(this, Chaos::Debug::Log::Get().GetClientLogger());
}

void Console::appendMessage(const QString& text)
{
	moveCursor(QTextCursor::End);

	setTextBackgroundColor(QColor(0, 0, 0, 0));
	if(text.contains("trace"))
		setTextColor(QColor(0xcc, 0xcc, 0xcc));
	else if (text.contains("info"))
		setTextColor(QColor(0, 255, 0));
	else if (text.contains("warn"))
		setTextColor(QColor(206, 128, 33));
	else if (text.contains("error"))
		setTextColor(QColor(255,0,0));
	else if (text.contains("critical"))
	{
		setTextColor(QColor(0xcc, 0xcc, 0xcc));
		setTextBackgroundColor(QColor(255, 0, 0));
	}

	insertPlainText(text); 
}

Console::~Console()
{
	m_coreSink->SetConsole(nullptr);
	m_clientSink->SetConsole(nullptr);
}

void Console::Clear()
{
	clear();
}
