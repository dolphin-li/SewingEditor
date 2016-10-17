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
	void dragEnterEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);
public:
	void initLeftDockActions();
private:
	Ui_MeshWindow ui;
};

#endif // SEWINGEDITOR_H
