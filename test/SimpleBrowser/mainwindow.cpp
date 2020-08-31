#include "mainwindow.h"

#include <windows.h>

#include <QDir>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  ui.setupUi(this);

  cefview = new CustomCefView("about:blank", this);
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(3);
  layout->addWidget(cefview);

  ui.cefContainer->setLayout(layout);

  connect(ui.backButton, &QPushButton::pressed, this, [=]() {
	  cefview->goBack();
  });

  connect(ui.goForwardButton, &QPushButton::pressed, this, [=]() {
	  cefview->goForward();
  });

  connect(ui.goButton, &QPushButton::pressed, this, [=]() {
	  QString url = ui.lineEdit->text();
	  if (!url.isEmpty()) {
		  cefview->goUrl(url);
	  }
  });

  connect(ui.cookieButton, &QPushButton::pressed, this, [=]() {
	  QString cookie = cefview->getCookie();
	  QMessageBox::information(NULL, "Get Cookies", cookie,
		  QMessageBox::Yes);
  });
}

MainWindow::~MainWindow() {}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
	if (Qt::Key_Return == event->key()) {
		QString url = ui.lineEdit->text();
		if (!url.isEmpty()) {
			cefview->goUrl(url);
		}
	}
}
