#include "sewingeditor.h"
#include <QtWidgets/QApplication>
#include "glut.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	glutInit(&argc, argv);

	QFile f(":qdarkstyle/style.qss");
	if (!f.exists())
	{
		printf("Unable to set stylesheet, file not found\n");
	}
	else
	{
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}

	SewingEditor w;
	w.show();
	return a.exec();
}
