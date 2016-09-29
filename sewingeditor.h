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
	public slots:
	void on_actionLoad_svg_triggered();
	void on_actionSave_svg_triggered();
	void on_actionSelect_all_triggered();
	void on_actionSelect_none_triggered();
	void on_actionSelect_inverse_triggered();
	void on_actionDelete_selected_triggered();
	void on_actionGroup_triggered();
	void on_actionUngroup_triggered();
	void leftDocButtonsClicked(int i);
public:
	void initLeftDockActions();
private:
	Ui::SewingEditorClass ui;
	QMap<AbstractEventHandle::ProcessorType, QSharedPointer<QPushButton>> m_leftDockButtons;
	QSharedPointer<QSignalMapper> m_ldbSignalMapper;
protected:
	void addLeftDockWidgetButton(AbstractEventHandle::ProcessorType type, QString iconImage, QString toolTip="");
};

#endif // SEWINGEDITOR_H
