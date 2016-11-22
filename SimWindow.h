#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_simwindow.h"

class SimWindow : public QMainWindow
{
	Q_OBJECT

public:
	SimWindow(QWidget *parent = 0);
	~SimWindow();

	SimViewer* getViewer();
	const SimViewer* getViewer()const;
	void updateParamUI();
	public slots:
	void leftDocButtonsClicked(int i);
protected:
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);
	virtual void timerEvent(QTimerEvent* ev);
private:
	Ui_SimWindow ui;
//////////////////////////////////////////////////////////////////////////
protected:
	QMap<AbstractSimEventHandle::ProcessorType, QSharedPointer<QPushButton>> m_leftDockButtons;
	QSharedPointer<QSignalMapper> m_ldbSignalMapper;
	void initLeftDockActions();
	void addLeftDockWidgetButton(AbstractSimEventHandle::ProcessorType type);
};

