#include "SvgManager.h"
#include "MeshWindow.h"
#include "sewingeditor.h"
#include "global_data_holder.h"
#include "svgpp\SvgAbstractObject.h"
#include "FreeFormDeform.h"
SewingEditor::SewingEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(showFullScreen()));
	new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(showNormal()));
	setAcceptDrops(true);
	initLeftDockActions();
	connect(ui.listLayers->itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),
		this, SLOT(on_listWidgetEditEnd(QWidget*, QAbstractItemDelegate::EndEditHint)));
	m_meshWindow.reset(new MeshWindow);
	m_meshWindow->setWindowTitle("mesh window");
	try
	{
		g_dataholder.init();
		ui.widget->setSvgManager(g_dataholder.m_svgManager.get());
		ui.widget->setSvgDeformer(g_dataholder.m_svgDefomer.get());
		m_meshWindow->getViewer()->initCloth(g_dataholder.m_clothManger.get(), g_dataholder.m_clothUiListener.get());
		initLayerList();
		resetRoll();
		initHistoryList();
	}
	catch (std::exception e){
		std::cout << e.what() << std::endl;
	}
}

SewingEditor::~SewingEditor()
{

}

void SewingEditor::closeEvent(QCloseEvent* ev)
{
	m_meshWindow->close();
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
		loadSvg(name);
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

void SewingEditor::loadSvg(QString name)
{
	m_meshWindow->hide();
	if (ui.widget->getSvgManager() == nullptr)
	{
		g_dataholder.m_svgManager.reset(new svg::SvgManager);
		ui.widget->setSvgManager(g_dataholder.m_svgManager.get());
	}
	ui.widget->getSvgManager()->load(name.toStdString().c_str());
	pushHistory("load svg");
	ui.sbPixelToMeter->setValue(1.0/ui.widget->getSvgManager()->getPixelToMeters());
	ui.widget->resetCamera();
	ui.widget->updateGL();
	initLayerList();
	setWindowTitle(name);
	ui.dbSmoothPolyThre->setValue(g_dataholder.m_param.m_smoothPolyThre);
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
		QString name = QFileDialog::getOpenFileName(this, "load svg", "", "*.svg");
		if (name.isEmpty())
			return;
		loadSvg(name);
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
		QString name = QFileDialog::getSaveFileName(this, "save svg", "", "*.svg");
		if (name.isEmpty())
			return;
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		if (!name.endsWith(".svg"))
			name.append(".svg");
		ui.widget->getSvgManager()->save(name.toStdString().c_str());
		setWindowTitle(name);
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
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

void SewingEditor::on_pbToConnectedGroups_clicked()
{
	try
	{
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
		ui.widget->getSvgManager()->convertSelectedPathToConnectedGroups();
		ui.widget->updateGL();
		pushHistory("path to connected groups");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_rollPos = (m_rollHead + pos) % MAX_ROLLBACK_STEP;
		ldp::Float4 lastBound = ui.widget->getSvgManager()->getBound();
		g_dataholder.m_svgManager = m_rollBackControls[m_rollPos].data->clone();
		g_dataholder.m_svgDefomer = m_rollBackControls[m_rollPos].deformer->clone(g_dataholder.m_svgManager.get());
		ui.widget->setSvgManager(g_dataholder.m_svgManager.get());
		ui.widget->setSvgDeformer(g_dataholder.m_svgDefomer.get());
		ldp::Float4 thisBound = ui.widget->getSvgManager()->getBound();
		if (lastBound != thisBound)
			ui.widget->resetCamera();
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_rollPos = (m_rollPos + 1) % MAX_ROLLBACK_STEP;
		m_rollBackControls[m_rollPos].name = name;
		m_rollBackControls[m_rollPos].data = ui.widget->getSvgManager()->clone();
		m_rollBackControls[m_rollPos].deformer = ui.widget->getSvgDeformer()->clone(g_dataholder.m_svgManager.get());

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
	if (ui.widget->getSvgManager() == nullptr)
		throw std::exception("svgManger: nullptr");
	m_disableCurrentRowChanged = true;
	ui.listLayers->setUpdatesEnabled(false);
	ui.listLayers->clear();
	ui.listLayers->setEditTriggers(QAbstractItemView::DoubleClicked
		| QAbstractItemView::EditKeyPressed);
	int currentRow = 0;
	bool hasSelection = false;
	for (auto iter : ui.widget->getSvgManager()->layers())
	{
		ui.listLayers->addItem(iter.first.c_str());
		auto item = ui.listLayers->item(ui.listLayers->count() - 1);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		item->setSelected(iter.second->selected);
		if (iter.second->selected)
			hasSelection = true;
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
	if (!hasSelection && ui.listLayers->count()){
		ui.listLayers->item(0)->setSelected(true);
		ui.widget->getSvgManager()->getLayer(ui.listLayers->item(0)->text().toStdString())->selected = true;
	}
	ui.listLayers->setUpdatesEnabled(true);
	m_disableCurrentRowChanged = false;
}

void SewingEditor::on_listLayers_itemSelectionChanged()
{
	try
	{
		if (m_disableCurrentRowChanged)
			return;
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		QString info("remove layers: ");
		m_meshWindow->hide();
		auto items = ui.listLayers->selectedItems();
		for (auto item : items)
		{
			if (ui.widget->getSvgManager()->getLayer(item->text().toStdString()))
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

void SewingEditor::on_pbMakePair_clicked()
{
	try
	{
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		ui.widget->getSvgManager()->makeSelectedToPair();
		ui.widget->updateGL();
		ui.widget->updateGL();
		pushHistory("make pair");
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

void SewingEditor::on_pbRemovePairs_clicked()
{
	try
	{
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		ui.widget->getSvgManager()->removeSelectedPairs();
		ui.widget->updateGL();
		pushHistory("remove pairs");
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

void SewingEditor::on_pbClosePolygon_clicked()
{
	try
	{
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
		ui.widget->getSvgManager()->closeSelectedPolygons();
		ui.widget->updateGL();
		pushHistory("close selected polygons");
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
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		ui.widget->getSvgManager()->selectClosedPolygons();
		ui.widget->updateGL();
		pushHistory("select closed polygons");
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

void SewingEditor::on_pbSymmetricCopy_clicked()
{
	try
	{
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
		ui.widget->getSvgManager()->symmetryCopySelectedPoly();
		ui.widget->updateGL();
		pushHistory("symmetric copy");
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

void SewingEditor::on_pbPixelToMeter_clicked()
{
	try
	{
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		m_meshWindow->hide();
		float one_pixel_is_how_many_meters = ui.widget->getSvgManager()->estimatePixelToMetersFromSelected();
		ui.sbPixelToMeter->setValue(1.0 / one_pixel_is_how_many_meters);
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

void SewingEditor::on_sbPixelToMeter_valueChanged(double v)
{
	try
	{
		if (ui.widget->getSvgManager() == nullptr)
			throw std::exception("svgManger: nullptr");
		double p1 = 1.0 / v;
		double p2 = ui.widget->getSvgManager()->getPixelToMeters();
		if (abs(p1 - p2) > std::numeric_limits<double>::epsilon())
		{
			m_meshWindow->hide();
			ui.widget->getSvgManager()->setPixelToMeters(p1);
			pushHistory("pixel2meter");
		}
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

void SewingEditor::on_pbGenerateMesh_clicked()
{
	try
	{
		g_dataholder.svgToCloth();
		m_meshWindow->updateParamUI();
		m_meshWindow->show();
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

void SewingEditor::on_actionRemove_selected_corner_triggered()
{
	try
	{
		ui.widget->getSvgManager()->removeSelectedPolyCorners();
		ui.widget->updateGL();
		pushHistory("remove selected corners");
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

void SewingEditor::on_actionSplit_selected_edge_triggered()
{
	try
	{
		ui.widget->getSvgManager()->splitSelectedPolyEdgeByMidPoint();
		ui.widget->updateGL();
		pushHistory("remove selected edges");
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbSmoothPoly_clicked()
{
	try
	{
		ui.widget->getSvgManager()->smoothSelectedPoly(g_dataholder.m_param.m_smoothPolyThre);
		ui.widget->updateGL();
		pushHistory("smooth poly");
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_dbSmoothPolyThre_valueChanged(double v)
{
	try
	{
		g_dataholder.m_param.m_smoothPolyThre = v;
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_pbPathSplitByIntersect_clicked()
{
	try
	{
		ui.widget->getSvgManager()->selectedPathsSplitByIntersect();
		ui.widget->updateGL();
		pushHistory("split path by intersect");
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void SewingEditor::on_actionDouble_path_width_triggered()
{
	try
	{
		ui.widget->getSvgManager()->doublePathWidth();
		ui.widget->updateGL();
		pushHistory("double path width");
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

