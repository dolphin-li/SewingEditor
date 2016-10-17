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
protected:
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);
	void initLeftDockActions();
	virtual void timerEvent(QTimerEvent* ev);
private:
	Ui_MeshWindow ui;
};

#endif // SEWINGEDITOR_H
