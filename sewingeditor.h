#ifndef SEWINGEDITOR_H
#define SEWINGEDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_sewingeditor.h"

class SewingEditor : public QMainWindow
{
	Q_OBJECT

public:
	SewingEditor(QWidget *parent = 0);
	~SewingEditor();

private:
	Ui::SewingEditorClass ui;
};

#endif // SEWINGEDITOR_H
