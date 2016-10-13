#include "SvgManager.h"
#include "sewingeditor.h"
#include "global_data_holder.h"
#include "svgpp\SvgAbstractObject.h"
SewingEditor::SewingEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.squareWidget->setChildWidget(ui.widget);

	new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(showFullScreen()));
	new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(showNormal()));

	setAcceptDrops(true);

	initLeftDockActions();

	connect(ui.listLayers->itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), 
		this, SLOT(on_listWidgetEditEnd(QWidget*, QAbstractItemDelegate::EndEditHint)));
	initLayerList();

	resetRoll();
	initHistoryList();
}

SewingEditor::~SewingEditor()
{

}

void SewingEditor::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		QList<QUrl> urls = event->mimeData()->urls();
		if (urls[0].fileName().endsWith(".svg"))
			event->acceptProposedAction();
	}
}

void SewingEditor::dropEvent(QDropEvent* event)
{
	QUrl url = event->mimeData()->urls()[0];
	QString name = url.toLocalFile();
	try
	{
		ui.widget->loadSvg(name);
		pushHistory("load svg");
		float asp = ui.widget->getSvgManager()->width() / (float)ui.widget->getSvgManager()->height();
		ui.squareWidget->setAspect(asp);
		ui.squareWidget->adjustChildWidget();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}

	event->acceptProposedAction();
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
		ui.squareWidget->adjustChildWidget();
		ui.widget->updateGL();
		initLayerList();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
		ui.widget->getSvgManager()->save(name.toStdString().c_str());
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbSelectSimilarShape_clicked()
{
	try
	{
		ui.widget->getSvgManager()->selectPathSimilarShape();
		ui.widget->updateGL();
		pushHistory("select path connected");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbSelectClosed_clicked()
{
	try
	{
		ui.widget->getSvgManager()->selectPathClosed();
		ui.widget->updateGL();
		pushHistory("select path closed");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbSplitByShape_clicked()
{
	try
	{
		ui.widget->getSvgManager()->splitSelectedPathByShape();
		ui.widget->updateGL();
		pushHistory("split path by shape");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
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
	m_disableCurrentRowChanged = false;
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
	try
	{
		m_rollPos = (m_rollHead + pos) % MAX_ROLLBACK_STEP;
		ldp::Float4 lastBound = ui.widget->getSvgManager()->getBound();
		ui.widget->setSvgManager(m_rollBackControls[m_rollPos].data->clone());
		ldp::Float4 thisBound = ui.widget->getSvgManager()->getBound();
		if (lastBound != thisBound)
		{
			float asp = ui.widget->getSvgManager()->width() / (float)ui.widget->getSvgManager()->height();
			ui.squareWidget->setAspect(asp);
			ui.squareWidget->adjustChildWidget();
			ui.widget->resetCamera();
		}
		ui.widget->updateGL();

		updateHistoryList();

		initLayerList();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
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
	try
	{
		m_rollPos = (m_rollPos + 1) % MAX_ROLLBACK_STEP;
		m_rollBackControls[m_rollPos].name = name;
		m_rollBackControls[m_rollPos].data = ui.widget->getSvgManager()->clone();

		m_rollTail = (m_rollPos + 1) % MAX_ROLLBACK_STEP;
		if (m_rollTail == m_rollHead)
			m_rollHead = (m_rollHead + 1) % MAX_ROLLBACK_STEP;
		m_disableCurrentRowChanged = true;
		ui.listHistory->setCurrentRow(rollPos());
		ui.listHistory->scrollToBottom();
		m_disableCurrentRowChanged = false;
		updateHistoryList();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
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
	ui.listHistory->setCurrentRow(MAX_ROLLBACK_STEP - 1);
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
	if (!m_disableCurrentRowChanged)
		rollBackTo(r);
}

//////////////////////////////////////////////////////////////////////////
// layer control
void SewingEditor::initLayerList()
{
	m_disableCurrentRowChanged = true;
	ui.listLayers->setUpdatesEnabled(false);
	ui.listLayers->clear();
	ui.listLayers->setEditTriggers(QAbstractItemView::DoubleClicked
		| QAbstractItemView::EditKeyPressed);
	int currentRow = 0;
	for (auto iter : ui.widget->getSvgManager()->layers())
	{
		ui.listLayers->addItem(iter.first.c_str());
		auto item = ui.listLayers->item(ui.listLayers->count() - 1);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		item->setSelected(iter.second->selected);
		auto currentLayer = ui.widget->getSvgManager()->getCurrentLayer();
		if (currentLayer == nullptr)
		{
			ui.widget->getSvgManager()->setCurrentLayer(iter.first);
			currentLayer = ui.widget->getSvgManager()->getCurrentLayer();
		}
		if (currentLayer->name == iter.first)
			currentRow = ui.listLayers->count() - 1;
	}
	ui.listLayers->setCurrentRow(currentRow);
	ui.listLayers->setUpdatesEnabled(true);
	m_disableCurrentRowChanged = false;
}

void SewingEditor::on_listLayers_itemSelectionChanged()
{
	try
	{
		if (m_disableCurrentRowChanged)
			return;
		for (int i = 0; i < ui.listLayers->count(); i++)
		{
			auto item = ui.listLayers->item(i);
			auto name = item->text().toStdString();
			auto layer = ui.widget->getSvgManager()->getLayer(name);
			if (layer == nullptr)
				throw std::exception("FATAL: nullptr in itemSelectionChanged!");
			layer->selected = item->isSelected();
		}
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_listLayers_currentRowChanged(int rowId)
{
	try
	{
		if (m_disableCurrentRowChanged)
			return;
		if (ui.listLayers->count() <= rowId || rowId < 0)
			return;
		ui.widget->getSvgManager()->setCurrentLayer(ui.listLayers->item(rowId)->text().toStdString());
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_listWidgetEditEnd(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
{
	try
	{
		if (m_disableCurrentRowChanged)
			return;
		QString newName = ui.listLayers->currentItem()->text();
		ui.widget->getSvgManager()->renameLayer(
			ui.widget->getSvgManager()->getCurrentLayer()->name, newName.toStdString());
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbNewLayer_clicked()
{
	try
	{
		auto layer = ui.widget->getSvgManager()->selectedToNewLayer();
		pushHistory(QString("add layer: ") + layer->name.c_str());
		initLayerList();
		ui.widget->updateGL();
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbMergeLayers_clicked()
{
	try
	{
		ui.widget->getSvgManager()->mergeSelectedLayers();		
		initLayerList();
		ui.widget->updateGL();
		pushHistory("merge selected layers");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbRemoveLayers_clicked()
{
	try
	{
		QString info("remove layers: ");
		auto items = ui.listLayers->selectedItems();
		for (auto item : items)
		{
			ui.widget->getSvgManager()->removeLayer(item->text().toStdString());
			info += item->text() + "; ";
		}

		initLayerList();

		ui.widget->updateGL();
		pushHistory(info);
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

