#include "sewingeditor.h"

SewingEditor::SewingEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.squareWidget->setChildWidget(ui.widget);
}

SewingEditor::~SewingEditor()
{

}
