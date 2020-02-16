#include "TTLEditor.h"

#include <QDir>
#include <QSettings>
#include <QVariant>
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

    QDir dir("/sdcard/Android/data/org.TTLEditor.pro");
    if (!dir.exists())
    {
        dir.mkdir("/sdcard/Android/data/org.TTLEditor.pro");
    }

    mainLayout->addWidget(btnCurrent, 0, 0, 0, 2);
    mainLayout->addWidget(numTTL, 1, 0);
    mainLayout->addWidget(btnSet, 1, 1);
    mainLayout->addWidget(logText, 2, 0, 2, 2);
    mainLayout->addWidget(btnRestore, 3, 0, 3, 2);

    setLayout(mainLayout);
}

TTLEditor::~TTLEditor()
{
    delete mainLayout;

    delete btnCurrent;
    delete btnSet;
    delete btnRestore;

    delete numTTL;

    delete logText;

    delete procTTL;
}

void TTLEditor::currentTTL()
{
    QFile file("/proc/sys/net/ipv4/ip_default_ttl");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    int stockTTL;
    in >> stockTTL;

    QSettings sett("/sdcard/Android/data/org/TTLEditor.pro/settings.ini", QSettings::IniFormat);
    sett.setValue("General/STOCK", stockTTL);

    QString str = "Current TTL: " + QString::number(stockTTL);
    logText->append(str);
}

void TTLEditor::setTTL()
{
    int newTTL;

    newTTL = numTTL->text().toInt();

    QSettings sett("/sdcard/Android/data/org/TTLEditor.pro/settings.ini", QSettings::IniFormat);
    sett.setValue("General/NEW", newTTL);

    procTTL->setProcessChannelMode(QProcess::SeparateChannels);
    procTTL->start("su", QStringList() << "-c" << "echo" << QString::number(newTTL) << ">>" << "/proc/sys/net/ipv4/ip_default_ttl");

        if(!procTTL->waitForFinished())
        {
            logText->append(procTTL->errorString());
        }
        else
        {
            QString str = "New TTL: " + QString::number(newTTL);
            logText->append(str);
            logText->append(procTTL->readAll());
            logText->append(tr("Ok!"));
        }
}

void TTLEditor::restoreTTL()
{
    int stockTTL;
    QSettings sett("/sdcard/Android/data/org/TTLEditor.pro/settings.ini", QSettings::IniFormat);
    stockTTL = sett.value("General/STOCK", 64).toInt();

    procTTL->setProcessChannelMode(QProcess::SeparateChannels);
    procTTL->start("su", QStringList() << "-c" << "echo" << QString::number(stockTTL) << ">>" << "/proc/sys/net/ipv4/ip_default_ttl");

        if(!procTTL->waitForFinished())
        {
            logText->append(procTTL->errorString());
        }
        else
        {
            QString str = "Current TTL: " + QString::number(stockTTL);
            logText->append(tr("Stock TTL has been restored."));
            logText->append(str);
            logText->append(procTTL->readAll());
        }
}
