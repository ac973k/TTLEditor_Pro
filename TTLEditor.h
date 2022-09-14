#ifndef TTLEDITOR_H
#define TTLEDITOR_H

#include <QWidget>

#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include <QProcess>

class QGridLayout;

class QLineEdit;
class QPushButton;
class QTextEdit;

class QProcess;

class TTLEditor : public QWidget
{
	Q_OBJECT
	
	private:
	
	QGridLayout *mainLayout;
	
	QPushButton *btnCurrent;
	QPushButton *btnSet;
	QPushButton *btnRestore;
	
	QLineEdit *numTTL;
	
	QTextEdit *logText;
	
	QProcess *procTTL;
	
	public:
	TTLEditor(QWidget *parent = nullptr);
	~TTLEditor();
	
	private slots:
	void setTTL();
	void currentTTL();
	void restoreTTL();
    void getLog();
};

#endif // TTLEDITOR_H
