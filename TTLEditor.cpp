#include "TTLEditor.h"

#include <QSettings>
#include <QFile>
#include <fstream>

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

    QFile file("/proc/sys/net/ipv4/ip_default_ttl");
    file.open(QIODevice::ReadOnly);
    QString tmpData = file.readLine();
    int stockTTL = tmpData.toInt();

    std::ofstream files("/sdcard/settings.ini", std::ofstream::app);
    if(!files.is_open())
    {
        logText->append("Error!");
    }

    QSettings sett("/sdcard/settings.ini", QSettings::IniFormat);

    int getStockTTL;
    getStockTTL = sett.value("General/STOCK").toInt();
    if(getStockTTL == 0)
    {
        sett.setValue("General/STOCK", stockTTL);
    }

    mainLayout->addWidget(btnCurrent, 1, 0, 1, 2);
    mainLayout->addWidget(numTTL, 2, 0);
    mainLayout->addWidget(btnSet, 2, 1);
    mainLayout->addWidget(logText, 3, 0, 3, 2);
    mainLayout->addWidget(btnRestore, 4, 0, 4, 2);

    setLayout(mainLayout);

    QObject::connect(btnCurrent, SIGNAL(clicked()), this, SLOT(currentTTL()));
    QObject::connect(btnSet, SIGNAL(clicked()), this, SLOT(setTTL()));
    QObject::connect(btnRestore, SIGNAL(clicked()), this, SLOT(restoreTTL()));
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
    QString tmpData = file.readLine();
    int currentTTL = tmpData.toInt();

    QString str = "Current TTL: " + QString::number(currentTTL);
    logText->append(str);
}

void TTLEditor::setTTL()
{
    int newTTL;

    newTTL = numTTL->text().toInt();

    if (newTTL > 255 or newTTL <= 0)
    {
        logText->append("Max TTL = 255, Min TTL = 1");
    }
    else if (numTTL->text() == NULL)
    {
        logText->append("Enter TTL");
    }
    else
    {

        QSettings sett("/sdcard/settings.ini", QSettings::IniFormat);
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
}

void TTLEditor::restoreTTL()
{
    int stockTTL;
    QSettings sett("/sdcard/settings.ini", QSettings::IniFormat);
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
