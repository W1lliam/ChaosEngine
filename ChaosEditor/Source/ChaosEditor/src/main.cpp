#include "ChaosAssetsManager.h"
#include <qapplication.h>
#include "ChaosEditor/editor.h"
#include <ChaosEngine.h>
#include "ChaosEditor/InputHandler.h"

int main(int arg, char* args[])
{
	Chaos::Initialize();
    InputHandler::Init();
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
	QApplication app(arg, args);
    
	QSurfaceFormat format;
	format.setVersion(4, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	QSurfaceFormat::setDefaultFormat(format);

	Editor* editor = new Editor();
	editor->showMaximized();
	int status = app.exec();
	delete editor;
	Chaos::Shutdown();
	return status;
}