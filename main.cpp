#include "sewingeditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SewingEditor w;
	w.show();
	return a.exec();
}
