#include "SimWindow.h"
#include "global_data_holder.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgPolyPath.h"
#include "SimulationManager.h"
SimWindow::SimWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);	
	new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(showFullScreen()));
	new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(showNormal()));
	setAcceptDrops(true);
	initLeftDockActions();
	startTimer(200);
}

SimWindow::~SimWindow()
{

}

SimViewer* SimWindow::getViewer()
{
	return ui.widget;
}

const SimViewer* SimWindow::getViewer()const
{
	return ui.widget;
}

void SimWindow::updateParamUI()
{

}

void SimWindow::on_actionSave_triggered()
{
	try
	{
		QString name = QFileDialog::getSaveFileName(this, "save mesh", "", "*.obj");
		if (name.isEmpty())
			return;
		if (!name.toLower().endsWith(".obj"))
			name.append(".obj");
		g_dataholder.m_simManager->saveCurrentCloth(name.toStdString());
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SimWindow::timerEvent(QTimerEvent* ev)
{
	float fps = ui.widget->getFps();
	setWindowTitle(QString().sprintf("fps = %.1f", fps));
}

void SimWindow::dragEnterEvent(QDragEnterEvent* event)
{
	//if (event->mimeData()->hasUrls())
	//{
	//	QList<QUrl> urls = event->mimeData()->urls();
	//	if (urls[0].fileName().endsWith(".svg"))
	//		event->acceptProposedAction();
	//}
}

void SimWindow::dropEvent(QDropEvent* event)
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

void SimWindow::initLeftDockActions()
{
	m_ldbSignalMapper.reset(new QSignalMapper(this));
	connect(m_ldbSignalMapper.data(), SIGNAL(mapped(int)), this, SLOT(leftDocButtonsClicked(int)));
	ui.dockWidgetLeftContents->setLayout(new QGridLayout(ui.dockWidgetLeftContents));
	ui.dockWidgetLeftContents->layout()->setAlignment(Qt::AlignTop);

	// add buttons
	for (size_t i = (size_t)AbstractSimEventHandle::ProcessorTypeGeneral+1;
		i < (size_t)AbstractSimEventHandle::ProcessorTypeEnd; i++)
	{
		auto type = AbstractSimEventHandle::ProcessorType(i);
		addLeftDockWidgetButton(type);
	}

	// do connections
	for (auto it : m_leftDockButtons.toStdMap())
	{
		m_ldbSignalMapper->setMapping(it.second.data(), it.first);
		connect(it.second.data(), SIGNAL(clicked()), m_ldbSignalMapper.data(), SLOT(map()));
	}
}

void SimWindow::addLeftDockWidgetButton(AbstractSimEventHandle::ProcessorType type)
{
	auto handle = ui.widget->getEventHandle(type);
	auto colorStr = QString("background-color: rgb(73, 73, 73)");
	QIcon icon;
	icon.addFile(handle->iconFile(), QSize(), QIcon::Active);
	icon.addFile(handle->iconFile(), QSize(), QIcon::Selected);
	icon.addFile(handle->inactiveIconFile(), QSize(), QIcon::Normal);
	QSharedPointer<QPushButton> btn(new QPushButton(ui.dockWidgetLeft));
	btn->setIconSize(QSize(80, 80));
	btn->setIcon(icon);
	btn->setCheckable(true);
	btn->setStyleSheet(colorStr);
	btn->setAutoExclusive(true);
	btn->setToolTip(handle->toolTips());
	m_leftDockButtons.insert(type, btn);
	ui.dockWidgetLeftContents->layout()->addWidget(btn.data());
}

void SimWindow::leftDocButtonsClicked(int i)
{
	AbstractSimEventHandle::ProcessorType type = (AbstractSimEventHandle::ProcessorType)i;
	ui.widget->setEventHandleType(type);
	m_leftDockButtons[type]->setChecked(true);
}