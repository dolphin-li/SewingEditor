
#include "SvgManager.h"
#include "sewingeditor.h"
#include "global_data_holder.h"
SewingEditor::SewingEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.squareWidget->setChildWidget(ui.widget);
}

SewingEditor::~SewingEditor()
{

}

void SewingEditor::on_actionLoad_svg_triggered()
{
	try
	{
		QString name = QFileDialog::getOpenFileName(this, "load svg", "data", "*.svg");
		if (name.isEmpty())
			return;
		ui.widget->getSvgManager()->load(name.toStdString().c_str());
		float asp = ui.widget->getSvgManager()->width() / (float)ui.widget->getSvgManager()->height();
		ui.squareWidget->setAspect(asp);
		ui.squareWidget->resize(ui.squareWidget->size()-QSize(1,1));
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void SewingEditor::on_actionSave_svg_triggered()
{
	try
	{
		QString name = QFileDialog::getSaveFileName(this, "save svg", "data", "*.svg");
		if (name.isEmpty())
			return;
		if (!name.endsWith(".svg"))
			name.append(".svg");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}
