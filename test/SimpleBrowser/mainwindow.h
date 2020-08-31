#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "customcefview.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();


private:
	virtual void keyPressEvent(QKeyEvent *event);

protected slots:

private:
	Ui::QCefView ui;
	CustomCefView* cefview;
};

#endif // MAINWINDOW_H
