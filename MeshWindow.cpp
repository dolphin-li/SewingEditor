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
	m_ldbSignalMapper.reset(new QSignalMapper(this));
	connect(m_ldbSignalMapper.data(), SIGNAL(mapped(int)), this, SLOT(leftDocButtonsClicked(int)));
	ui.dockWidgetLeftContents->setLayout(new QGridLayout(ui.dockWidgetLeftContents));
	ui.dockWidgetLeftContents->layout()->setAlignment(Qt::AlignTop);

	// add buttons
	for (size_t i = (size_t)AbstractMeshEventHandle::ProcessorTypeClothSelect;
		i < (size_t)AbstractMeshEventHandle::ProcessorTypeEnd; i++)
	{
		auto type = AbstractMeshEventHandle::ProcessorType(i);
		addLeftDockWidgetButton(type);
	}
	m_leftDockButtons[AbstractMeshEventHandle::ProcessorTypeClothSelect]->setChecked(true);
	m_leftDockButtons[AbstractMeshEventHandle::ProcessorTypeClothTranslate]->setShortcut(Qt::Key_T);
	m_leftDockButtons[AbstractMeshEventHandle::ProcessorTypeClothRotate]->setShortcut(Qt::Key_R);

	// do connections
	for (auto it : m_leftDockButtons.toStdMap())
	{
		m_ldbSignalMapper->setMapping(it.second.data(), it.first);
		connect(it.second.data(), SIGNAL(clicked()), m_ldbSignalMapper.data(), SLOT(map()));
		connect(it.second.data(), SIGNAL(toggled(bool)), m_ldbSignalMapper.data(), SLOT(leftDocButtonsToggled(bool)));
	}
}

void MeshWindow::addLeftDockWidgetButton(AbstractMeshEventHandle::ProcessorType type)
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

void MeshWindow::leftDocButtonsClicked(int i)
{
	AbstractMeshEventHandle::ProcessorType type = (AbstractMeshEventHandle::ProcessorType)i;
	ui.widget->setEventHandleType(type);
	m_leftDockButtons[type]->setChecked(true);
}
