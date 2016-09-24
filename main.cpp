#include "sewingeditor.h"
#include <QtWidgets/QApplication>
#include "glut.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	glutInit(&argc, argv);
	SewingEditor w;
	w.show();
	return a.exec();
}
