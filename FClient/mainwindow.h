#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fftp.h"
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QTextCodec>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	 ~MainWindow();

	QString getPath(QModelIndex index, bool &isFolder);
	QString nameNewFolder(QString currentPath);

private slots:
    void on_uploadButton_clicked();
	void on_downloadButton_clicked();
    void on_chooseToolButton_clicked();
    void on_passwordLineEdit_returnPressed();
    void on_ipComboBox_enterFinished();
    void on_userNameComboBox_enterFinished();
	void on_actionExit_triggered();
	void on_actionType_triggered();
	void on_actionPASV_triggered();
	void on_actionPOSI_2_triggered();
    void on_responseAction(int code, QString caption, QString msg);
	void updateTransferProgress(qint64 completeBytes, qint64 totalBytes);
	void on_changeButton_clicked();
	void rename();
	void showDirectoy();
	void updateSelection();
	void on_addButton_clicked();
	void on_deleteButton_clicked();

private:
	Ui::MainWindow *ui;
    FFtp *ftp;
	QString ip, userName, password;
	quint16 port;
    QLabel *loginStatus;
    QFont font;
};

#endif // MAINWINDOW_H
