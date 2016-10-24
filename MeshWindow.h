#ifndef MESHWINDOW_H
#define MESHWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_meshwindow.h"

class MeshWindow : public QMainWindow
{
	Q_OBJECT

public:
	MeshWindow(QWidget *parent = 0);
	~MeshWindow();

	BaseMeshViewer* getViewer();
	const BaseMeshViewer* getViewer()const;
	public slots:
	void leftDocButtonsClicked(int i);
	void on_cbThickness_currentTextChanged(const QString& s);
protected:
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);
	virtual void timerEvent(QTimerEvent* ev);
private:
	Ui_MeshWindow ui;
//////////////////////////////////////////////////////////////////////////
protected:
	QMap<AbstractMeshEventHandle::ProcessorType, QSharedPointer<QPushButton>> m_leftDockButtons;
	QSharedPointer<QSignalMapper> m_ldbSignalMapper;
	void initLeftDockActions();
	void addLeftDockWidgetButton(AbstractMeshEventHandle::ProcessorType type);
};

#endif // SEWINGEDITOR_H
