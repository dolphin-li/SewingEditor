#include "MeshWindow.h"
#include "global_data_holder.h"

MeshWindow::MeshWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

MeshWindow::~MeshWindow()
{

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
