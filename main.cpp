#include "TTLEditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	TTLEditor w;
	w.show();
	
	return app.exec();
}