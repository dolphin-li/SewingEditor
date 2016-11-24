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
	void updateParamUI();
	public slots:
	void leftDocButtonsClicked(int i);
	void on_cbThickness_currentTextChanged(const QString& s);
	void on_pbInverseCylinder_clicked();
	void on_dbStiffBend_valueChanged(double v);
	void on_dbStiffMyu_valueChanged(double v);
	void on_dbStiffLambda_valueChanged(double v);
	void on_dbRho_valueChanged(double v);
	void on_dbMyuK_valueChanged(double v);
	void on_dbMyuS_valueChanged(double v);
	void on_dbStiffN_valueChanged(double v);
	void on_dbStiffF_valueChanged(double v);
	void on_dbOffset_valueChanged(double v);
	void on_sbCoarseMesh_valueChanged(int v);
	void on_sbDetailMesh_valueChanged(int v);
	void on_pbReInitMesh_clicked();

	void on_actionSave_triggered();
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
