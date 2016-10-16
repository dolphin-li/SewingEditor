#ifndef SEWINGEDITOR_H
#define SEWINGEDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_sewingeditor.h"
#include "event_handles\AbstractEventHandle.h"
#include <QMap>
#include <QSharedPointer>
class SewingEditor : public QMainWindow
{
	Q_OBJECT

public:
	SewingEditor(QWidget *parent = 0);
	~SewingEditor();
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);
	public slots:
	void on_actionLoad_svg_triggered();
	void on_actionSave_svg_triggered();
	void on_actionSelect_all_triggered();
	void on_actionSelect_none_triggered();
	void on_actionSelect_inverse_triggered();
	void on_actionSelect_similar_width_triggered();
	void on_actionDelete_selected_triggered();
	void on_actionGroup_triggered();
	void on_actionUngroup_triggered();
	void on_actionFix_grouping_triggered();
	void on_actionSplit_selected_path_triggered();
	void on_actionMerge_selected_path_triggered();
	void on_actionUndo_triggered();
	void on_actionRedo_triggered();
	void on_pbToConnectedGroups_clicked();
	void on_pbSymmetricCopy_clicked();
	void leftDocButtonsClicked(int i);
public:
	void initLeftDockActions();
private:
	Ui::SewingEditorClass ui;
	QMap<AbstractEventHandle::ProcessorType, QSharedPointer<QPushButton>> m_leftDockButtons;
	QSharedPointer<QSignalMapper> m_ldbSignalMapper;
protected:
	void addLeftDockWidgetButton(AbstractEventHandle::ProcessorType type, QString iconImage, QString toolTip="");

//////////////////////////////////////////////////////////////////////////
public:
	void initLayerList();
	void updateLayerList();
	void on_listLayers_rename();
	public slots:
	void on_listLayers_itemSelectionChanged();
	void on_listLayers_currentRowChanged(int row);
	void on_pbNewLayer_clicked();
	void on_pbMergeLayers_clicked();
	void on_pbRemoveLayers_clicked(); 
	void on_pbMakePair_clicked();
	void on_pbRemovePairs_clicked();
	void on_pbClosePolygon_clicked();
	void on_pbSelectClosed_clicked();
	void on_listWidgetEditEnd(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
protected:
//////////////////////////////////////////////////////////////////////////
	// roll back control
	struct RollBackControl{
		QString name;
		std::shared_ptr<svg::SvgManager> data;
	};
	enum {
		MAX_ROLLBACK_STEP = 100,
	};
	std::vector<RollBackControl> m_rollBackControls;
	int m_rollHead;
	int m_rollPos;
	int m_rollTail;
	bool m_disableCurrentRowChanged;
public:
	void resetRoll();
	void rollBackTo(int pos);
	void rollBackward();
	void rollForward();
	void pushHistory(QString name);
	int rollPos()const;
	int rollNum()const;

	void initHistoryList();
	void updateHistoryList();

	public slots:
	void on_listHistory_currentRowChanged(int r);
};

#endif // SEWINGEDITOR_H
