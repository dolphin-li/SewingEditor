#include "MeshWindow.h"

MeshWindow::MeshWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);	
	new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(showFullScreen()));
	new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(showNormal()));
	setAcceptDrops(true);
	initLeftDockActions();

	startTimer(200);
}

MeshWindow::~MeshWindow()
{

}

BaseMeshViewer* MeshWindow::getViewer()
{
	return ui.widget;
}

const BaseMeshViewer* MeshWindow::getViewer()const
{
	return ui.widget;
}

void MeshWindow::timerEvent(QTimerEvent* ev)
{
	float fps = ui.widget->getFps();
	setWindowTitle(QString().sprintf("fps = %.1f", fps));
}

void MeshWindow::dragEnterEvent(QDragEnterEvent* event)
{
	//if (event->mimeData()->hasUrls())
	//{
	//	QList<QUrl> urls = event->mimeData()->urls();
	//	if (urls[0].fileName().endsWith(".svg"))
	//		event->acceptProposedAction();
	//}
}

void MeshWindow::dropEvent(QDropEvent* event)
{
	//QUrl url = event->mimeData()->urls()[0];
	//QString name = url.toLocalFile();
	//try
	//{
	//	if (ui.widget->getSvgManager() == nullptr)
	//		throw std::exception("svgManger: nullptr");
	//	ui.widget->getSvgManager()->load(name.toStdString().c_str());
	//	pushHistory("load svg");
	//	ui.widget->updateGL();
	//}
	//catch (std::exception e)
	//{
	//	std::cout << e.what() << std::endl;
	//}
	//catch (...)
	//{
	//	std::cout << "unknown error" << std::endl;
	//}

	//event->acceptProposedAction();
}

void MeshWindow::initLeftDockActions()
{

}
