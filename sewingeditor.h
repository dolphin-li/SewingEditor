#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_sewingeditor.h"
#include "event_handles\AbstractEventHandle.h"
#include <QMap>
#include <QSharedPointer>
class FreeFormDeform;
class MeshWindow;
class SewingEditor : public QMainWindow
{
	Q_OBJECT

public:
	SewingEditor(QWidget *parent = 0);
	~SewingEditor();

	void loadSvg(QString name);
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
	void on_pbGenerateMesh_clicked();
	void on_actionRemove_selected_corner_triggered();
	void on_actionSplit_selected_edge_triggered();
protected:
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);
	void closeEvent(QCloseEvent* ev);
private:
	Ui::SewingEditorClass ui;
	QSharedPointer<MeshWindow> m_meshWindow;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
protected:
	QMap<AbstractEventHandle::ProcessorType, QSharedPointer<QPushButton>> m_leftDockButtons;
	QSharedPointer<QSignalMapper> m_ldbSignalMapper;
	void initLeftDockActions();
	void addLeftDockWidgetButton(AbstractEventHandle::ProcessorType type, QString iconImage, QString toolTip = "");
//////////////////////////////////////////////////////////////////////////
protected:
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
	void on_pbPixelToMeter_clicked();
	void on_sbPixelToMeter_valueChanged(double v);
	void on_listWidgetEditEnd(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
	void on_pbSmoothPoly_clicked();
	void on_dbSmoothPolyThre_valueChanged(double v);
protected:
//////////////////////////////////////////////////////////////////////////
	// roll back control
	struct RollBackControl{
		QString name;
		std::shared_ptr<svg::SvgManager> data;
		std::shared_ptr<FreeFormDeform> deformer;
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

