#pragma once
#include "QTextEdit.h"
#include <QWidget>
#include <QFile>
#include <QString>
#include <mutex>

template<typename Mutex>
class EditorSink;

class Console : public QTextEdit
{
	Q_OBJECT
		/* snip */
public:
	explicit Console(QWidget* p_parent = nullptr);


	~Console();
	void appendMessage(const QString& text);
	void Clear();

	EditorSink<std::mutex>* m_coreSink;
	EditorSink<std::mutex>* m_clientSink;
};