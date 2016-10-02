#include "SvgManager.h"
#include "sewingeditor.h"
#include "global_data_holder.h"
SewingEditor::SewingEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.squareWidget->setChildWidget(ui.widget);

	initLeftDockActions();

	resetRoll();
	initHistoryList();
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

	// relate to main ui
	for (size_t i = (size_t)AbstractEventHandle::ProcessorTypeGeneral;
		i < (size_t)AbstractEventHandle::ProcessorTypeEnd; i++)
	{
		auto type = AbstractEventHandle::ProcessorType(i);
		ui.widget->getEventHandle(type)->setMainUI(this);
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
		ui.widget->loadSvg(name);
		pushHistory("load svg");
		float asp = ui.widget->getSvgManager()->width() / (float)ui.widget->getSvgManager()->height();
		ui.squareWidget->setAspect(asp);
		ui.squareWidget->resize(ui.squareWidget->size() - QSize(1, 1));
		ui.widget->updateGL();
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
		pushHistory("select all");
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
		pushHistory("select none");
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
		pushHistory("select inverse");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionSelect_similar_width_triggered()
{
	try
	{
		ui.widget->getSvgManager()->selectPathBySimilarSelectedWidth();
		ui.widget->updateGL();
		pushHistory("select similar width");
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
		pushHistory("group");
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
		pushHistory("fix grouping");
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
		pushHistory("ungroup");
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
		pushHistory("delete selected");
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
		pushHistory("split selected path");
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
		pushHistory("merge selected path");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionUndo_triggered()
{
	try
	{
		rollBackward();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionRedo_triggered()
{
	try
	{
		rollForward();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_pbSelectConnected_clicked()
{
	try
	{
		ui.widget->getSvgManager()->selectPathConnected();
		ui.widget->updateGL();
		pushHistory("select path connected");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

/////////////////////////////////////////////////////////////////
// roll back control
void SewingEditor::resetRoll()
{
	m_rollBackControls.clear();
	m_rollBackControls.resize(MAX_ROLLBACK_STEP);
	m_rollHead = 0;
	m_rollTail = 0;
	m_rollPos = m_rollHead - 1;
}

int SewingEditor::rollPos()const
{
	return (m_rollPos - m_rollHead + MAX_ROLLBACK_STEP) % MAX_ROLLBACK_STEP;
}

int SewingEditor::rollNum()const
{
	return (m_rollTail - m_rollHead + MAX_ROLLBACK_STEP) % MAX_ROLLBACK_STEP;
}

void SewingEditor::rollBackTo(int pos)
{
	if (pos < 0 || pos >= rollNum())
		return;

	m_rollPos = (m_rollHead + pos) % MAX_ROLLBACK_STEP;
	ui.widget->setSvgManager(m_rollBackControls[m_rollPos].data->clone());
	ui.widget->updateGL();
	updateHistoryList();
}

void SewingEditor::rollBackward()
{
	rollBackTo(rollPos() - 1);
}

void SewingEditor::rollForward()
{
	rollBackTo(rollPos() + 1);
}

void SewingEditor::pushHistory(QString name)
{
	m_rollPos = (m_rollPos + 1) % MAX_ROLLBACK_STEP;
	m_rollBackControls[m_rollPos].name = name;
	m_rollBackControls[m_rollPos].data = ui.widget->getSvgManager()->clone();

	m_rollTail = (m_rollPos + 1) % MAX_ROLLBACK_STEP;
	if (m_rollTail == m_rollHead)
		m_rollHead = (m_rollHead + 1) % MAX_ROLLBACK_STEP;

	updateHistoryList();
}

void SewingEditor::initHistoryList()
{
	ui.listHistory->clear();
	for (int i = 0; i < MAX_ROLLBACK_STEP; i++)
	{
		ui.listHistory->addItem("");
		ui.listHistory->item(i)->setHidden(true);
		ui.listHistory->item(i)->setTextColor(Qt::white);
	}
}

void SewingEditor::updateHistoryList()
{
	for (int i = 0; i < rollNum(); i++)
	{
		if (i <= rollPos())
			ui.listHistory->item(i)->setTextColor(Qt::white);
		else
			ui.listHistory->item(i)->setTextColor(Qt::gray);
		ui.listHistory->item(i)->setText(QString().sprintf("%d: ", m_rollHead+i) 
			+ m_rollBackControls[(m_rollHead+i)%MAX_ROLLBACK_STEP].name);
		ui.listHistory->item(i)->setHidden(false);
	}
	for (int i = rollNum(); i < MAX_ROLLBACK_STEP; i++)
		ui.listHistory->item(i)->setHidden(true);
}

void SewingEditor::on_listHistory_currentRowChanged(int r)
{
	rollBackTo(r);
}
