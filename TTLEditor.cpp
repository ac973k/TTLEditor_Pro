#include "TTLEditor.h"

#include <QSettings>
#include <QFile>
#include <fstream>

#include <QtAndroidExtras/QtAndroid>
#include <QtTest/QTest>

bool is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

bool requestAndroidPermissions(){
//Request requiered permissions at runtime

const QVector<QString> permissions({
                                    "android.permission.WRITE_EXTERNAL_STORAGE",
                                    "android.permission.READ_EXTERNAL_STORAGE"});

for(const QString &permission : permissions){
    auto result = QtAndroid::checkPermission(permission);
    if(result == QtAndroid::PermissionResult::Denied){
        auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
        if(resultHash[permission] == QtAndroid::PermissionResult::Denied)
            return false;
    }
}

return true;
}

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

    std::ifstream files("/data/data/org.TTLEditor.pro/settings.ini", std::ios_base::app);
    if (!files.is_open())
    {
        logText->append("Error! File not found. Create...");
        requestAndroidPermissions();
        QTest::qSleep(2000);

        QFile files("/data/data/org.TTLEditor.pro/settings.ini");
        files.open(QIODevice::WriteOnly);
        files.close();
    }

    else
    {
        if (is_empty(files))
        {
            QSettings sett("/data/data/org.TTLEditor.pro/settings.ini", QSettings::IniFormat);
            sett.beginGroup("General");
            sett.sync();

            int getStockTTL;
            getStockTTL = sett.value("STOCK").toInt();
            if(getStockTTL == 0)
            {
                sett.setValue("STOCK", stockTTL);
            }
        }
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

    if (newTTL > 255 || newTTL <= 0)
    {
        logText->append("Max TTL = 255, Min TTL = 1");
    }
    else if (numTTL->text() == NULL)
    {
        logText->append("Enter TTL");
    }
    else
    {

        QSettings sett("/data/data/org.TTLEditor.pro/settings.ini", QSettings::IniFormat);
        sett.beginGroup("General");


        QFile file("/proc/sys/net/ipv4/ip_default_ttl");
        file.open(QIODevice::ReadOnly);
        QString tmpData = file.readLine();
        int stockTTL = tmpData.toInt();

        sett.setValue("STOCK", stockTTL);
        sett.setValue("NEW", newTTL);

        QObject::connect(procTTL, SIGNAL(readyReadStandardOutput()), this, SLOT(getLog()));

        procTTL->start("su", QStringList() << "-c" << "echo" << QString::number(newTTL) << ">>" << "/proc/sys/net/ipv4/ip_default_ttl");
        procTTL->setProcessChannelMode(QProcess::MergedChannels);
        procTTL->waitForStarted();

        QString str = "New TTL: " + QString::number(newTTL);
        logText->append(str);
    }
}

void TTLEditor::restoreTTL()
{
    int stockTTL;
    QSettings sett("/data/data/org.TTLEditor.pro/settings.ini", QSettings::IniFormat);
    sett.beginGroup("General");
    stockTTL = sett.value("STOCK", 64).toInt();
    sett.setValue("NEW", stockTTL);

    QObject::connect(procTTL, SIGNAL(readyReadStandardOutput()), this, SLOT(getLog()));

    procTTL->start("su", QStringList() << "-c" << "echo" << QString::number(stockTTL) << ">>" << "/proc/sys/net/ipv4/ip_default_ttl");
    procTTL->setProcessChannelMode(QProcess::MergedChannels);
    procTTL->waitForStarted();

    QString str = "Current TTL: " + QString::number(stockTTL);
    logText->append(tr("Stock TTL has been restored."));
    logText->append(str);
}

void TTLEditor::getLog() {
    logText->append(QString::fromUtf8(procTTL->readAllStandardOutput()));
}
