#include "TTLEditor.h"

#include <QDir>
#include <QFile>

TTLEditor::TTLEditor(QWidget *parent)
: QWidget(parent)
{
	mainLayout = new QGridLayout;
	
	btnCurrent = new QPushButton(tr("Current TTL"));
	btnSet = new QPushButton(tr("Set TTL"));
	btnRestore = new QPushButton(tr("Restore TTL"));
	
	numTTL = new QLineEdit;
	
	logText = new QTextEdit;
	logText->setReadOnly(true);
	
	procTTL = new QProcess;
}

TTLEditor::~TTLEditor()
{
	
}

void TTLEditor::currentTTL()
{
	
}

void TTLEditor::setTTL()
{
	
}

void TTLEditor::restoreTTL()
{
	
}