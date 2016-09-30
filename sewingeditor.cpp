#include "SvgManager.h"
#include "sewingeditor.h"
#include "global_data_holder.h"
SewingEditor::SewingEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.squareWidget->setChildWidget(ui.widget);

	initLeftDockActions();
}

SewingEditor::~SewingEditor()
{

}

void SewingEditor::initLeftDockActions()
{
	m_ldbSignalMapper.reset(new QSignalMapper(this));
	connect(m_ldbSignalMapper.data(), SIGNAL(mapped(int)), this, SLOT(leftDocButtonsClicked(int)));
	ui.dockWidgetLeftContents->setLayout(new QGridLayout(ui.dockWidgetLeftContents));
	ui.dockWidgetLeftContents->layout()->setAlignment(Qt::AlignTop);

	// add buttons
	for (size_t i = (size_t)AbstractEventHandle::ProcessorTypeShape;
		i < (size_t)AbstractEventHandle::ProcessorTypeEnd; i++)
	{
		auto type = AbstractEventHandle::ProcessorType(i);
		addLeftDockWidgetButton(type, ui.widget->getEventHandle(type)->iconFile(), 
			ui.widget->getEventHandle(type)->toolTips());
	}

	// do connections
	for (auto it : m_leftDockButtons.toStdMap())
	{
		m_ldbSignalMapper->setMapping(it.second.data(), it.first);
		connect(it.second.data(), SIGNAL(clicked()), m_ldbSignalMapper.data(), SLOT(map()));
	}
}

void SewingEditor::leftDocButtonsClicked(int btnId)
{
	AbstractEventHandle::ProcessorType type = (AbstractEventHandle::ProcessorType)btnId;
	ui.widget->setEventHandleType(type);
}

void SewingEditor::addLeftDockWidgetButton(AbstractEventHandle::ProcessorType type, QString iconImage, QString toolTip)
{
	auto colorStr = QString("background-color: rgb(73, 73, 73)");
	QSharedPointer<QPushButton> btn(new QPushButton(QIcon(iconImage), "", ui.dockWidgetLeftContents));
	btn->setAutoFillBackground(true);
	btn->setCheckable(true);
	btn->setStyleSheet(colorStr);
	btn->setAutoExclusive(true);
	btn->setToolTip(toolTip);
	m_leftDockButtons.insert(type, btn);
	ui.dockWidgetLeftContents->layout()->addWidget(btn.data());
}

void SewingEditor::on_actionLoad_svg_triggered()
{
	try
	{
		QString name = QFileDialog::getOpenFileName(this, "load svg", "data", "*.svg");
		if (name.isEmpty())
			return;
		ui.widget->getSvgManager()->load(name.toStdString().c_str());
		float asp = ui.widget->getSvgManager()->width() / (float)ui.widget->getSvgManager()->height();
		ui.squareWidget->setAspect(asp);
		ui.squareWidget->resize(ui.squareWidget->size()-QSize(1,1));
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionSave_svg_triggered()
{
	try
	{
		QString name = QFileDialog::getSaveFileName(this, "save svg", "data", "*.svg");
		if (name.isEmpty())
			return;
		if (!name.endsWith(".svg"))
			name.append(".svg");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionSelect_all_triggered()
{
	try
	{
		ui.widget->getSvgManager()->selectShapeByIndex(0, svg::SvgManager::SelectAll);
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionSelect_none_triggered()
{
	try
	{
		ui.widget->getSvgManager()->selectShapeByIndex(0, svg::SvgManager::SelectNone);
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionSelect_inverse_triggered()
{
	try
	{
		ui.widget->getSvgManager()->selectShapeByIndex(0, svg::SvgManager::SelectInverse);
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionGroup_triggered()
{
	try
	{
		ui.widget->getSvgManager()->groupSelected();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionFix_grouping_triggered()
{
	try
	{
		ui.widget->getSvgManager()->removeSingleNodeAndEmptyNode();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionUngroup_triggered()
{
	try
	{
		ui.widget->getSvgManager()->ungroupSelected();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionDelete_selected_triggered()
{
	try
	{
		ui.widget->getSvgManager()->removeSelected();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionSplit_selected_path_triggered()
{
	try
	{
		ui.widget->getSvgManager()->splitSelectedPath();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionMerge_selected_path_triggered()
{
	try
	{
		ui.widget->getSvgManager()->mergeSelectedPath();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}
