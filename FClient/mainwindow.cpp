#include "mainwindow.h"
#include "treemodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ftp = new FFtp();

	// 非法操作限制
	ui->menuSetting->setDisabled(true);
	ui->actionPASV->setDisabled(true);
	ui->actionAscii->setDisabled(true);
	ui->groupBox_transfer->setDisabled(true);
	ui->groupBox_directory->setDisabled(true);

	// 状态栏
    loginStatus = new QLabel("Unconnected State. Offline.");
	ui->statusBar->addWidget(loginStatus);
    font.setPointSize(16);
    font.setFamily("Adobe Arabic");
	loginStatus->setFont(font);

	// IP输入框
	QRegExp rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");	
	ui->ipComboBox->setValidator(new QRegExpValidator(rx, this));
	// 密码输入框
	ui->passwordLineEdit->setEchoMode(QLineEdit::Password);	//密码隐藏
	ui->passwordLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);	//密码中文输入失效
	// 绑定password输入框回车键和login按钮：登录事件
    connect(ui->loginButton, SIGNAL(clicked(bool)), ui->passwordLineEdit, SIGNAL(returnPressed()), Qt::UniqueConnection);

	// 按钮图标
    ui->addButton->setIcon(QIcon("D:/Users/Public/Documents/Qt/FClient/icon/green_plus.png"));
    ui->addButton->setIconSize(QSize(36, 36));
    ui->deleteButton->setIcon(QIcon("D:/Users/Public/Documents/Qt/FClient/icon/red_minus.png"));
    ui->deleteButton->setIconSize(QSize(36, 36));

	// TODO 两个下拉框：触发记录输入历史
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_ipComboBox_enterFinished);
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_userNameComboBox_enterFinished);
	
	// 文件传输显示
	connect(ftp, SIGNAL(FFtp::uploading(qint64, qint64)), this, SLOT(MainWindow::updateTransferProgress(qint64, qint64)), Qt::DirectConnection);

	// FTP控制口状态检测
    connect(ftp, &FFtp::responseSignal, this, &MainWindow::on_responseAction);
    connect(ui->actionLogout, &QAction::triggered, ftp, &FFtp::close);
	connect(ui->actionAscii, &QAction::triggered, this, &MainWindow::on_actionType_triggered);
	connect(ui->actionBinary, &QAction::triggered, this, &MainWindow::on_actionType_triggered);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_passwordLineEdit_returnPressed()
{
    QString warningInfo(QString::null);
	port = ui->portLineEdit->text().toUInt();
    if((ip = ui->ipComboBox->currentText()).isEmpty())
        warningInfo.append(" FTP server IP ");
    if((userName = ui->userNameComboBox->currentText()).isEmpty())
        warningInfo.append(" username ");
    if((password = ui->passwordLineEdit->text()).isEmpty() && (userName != "Anonymous"))
        warningInfo.append(" password ");
    if(!warningInfo.isNull())
        emit ftp->responseSignal(590, "login warning", "Please enter " + warningInfo.replace("  ", " and ").trimmed() + "!");
    else
    {
		if (userName == "Anonymous")
			password = "";
		ftp->userInfo(userName, password);
        if(ftp->status() == FSocket::UnconnectedState)
			ftp->open(ip, port);
        else
			ftp->user();
    }
}

void MainWindow::on_chooseToolButton_clicked()
{
	ui->fileDirLineEdit->clear();
	//文件选择; 还有一种可选filter：QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs
	QString path = QFileDialog::getOpenFileName(this, tr("Select one or more files to upload"), QDir::homePath(), "*.*");
	//文件夹选择: QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Choose directory"), QDir::homePath()));
	ui->fileDirLineEdit->setText(path);
}

void MainWindow::on_uploadButton_clicked()
{
	//QTextCodec *codec = QTextCodec::codecForName("GB2312"); //utf8
	//QString name = QString::fromLatin1(codec->fromUnicode(ui->fileDirLineEdit->text())); //文件名转码
	QString uploadFile = ui->fileDirLineEdit->text();
	if (uploadFile.isEmpty())	// 直接点击upload按钮无用
		return;
	QFile *file = new QFile(uploadFile);
	QFileInfo fileInfo = QFileInfo(uploadFile);
	ui->textBrowser->clear();
	if (file->open(QIODevice::ReadOnly))
	{
		ui->groupBox_transfer->setTitle("Transfer Status: Uploading");
		int size = fileInfo.size();
		ui->textBrowser->setText("Name: " + fileInfo.fileName() + "\nSize: " + QString::number(size) + " bytes");
		
		QString filepath = ui->serverWorkingDirectory->text();
		if (!filepath.endsWith("/"))
			filepath = filepath.left(filepath.lastIndexOf("/") + 1);

		QByteArray byteArray = file->readAll();
		ftp->put(filepath, fileInfo, byteArray);
		
		ui->fileDirLineEdit->clear();
		showDirectoy();
		ui->groupBox_transfer->setTitle("Transfer Status: Free");
	}
	file->close();
}

void MainWindow::on_downloadButton_clicked()
{
	QString downloadFile = ui->serverWorkingDirectory->text();
	if (downloadFile.isEmpty() || downloadFile.endsWith("/"))	//文件夹不能直接批量下载
		return;

	QAbstractItemModel *model = ui->treeView->model();
	QModelIndex index = ui->treeView->selectionModel()->currentIndex();
	
	ftp->get(downloadFile, model->data(index.sibling(index.row(), 1)).toInt());
}

void MainWindow::on_addButton_clicked()
{
	QString filepath = ui->serverWorkingDirectory->text();
	// 当前目录路径
	if (filepath.isEmpty())
		return;
	if (!filepath.endsWith("/"))
		filepath = filepath.left(filepath.lastIndexOf("/") + 1);
	ftp->mkdir(filepath + nameNewFolder(filepath));
	showDirectoy();
}

QString MainWindow::nameNewFolder(QString currentPath)
{
	QString  data = ftp->ls(currentPath);
	if (data.contains("new_folder"))
		return "new_folder" + QString::number(qrand());
	return "new_folder";
}

void MainWindow::on_deleteButton_clicked()
{
	QString filepath = ui->serverWorkingDirectory->text();
	if (filepath.isEmpty())
		return;
	if (filepath.endsWith('/'))
		ftp->folderDele(filepath);	//1，删除目录（默认其中的文件也都删除）
	else
		ftp->dele(filepath);	//2，删除文件
	showDirectoy();
}

void MainWindow::showDirectoy()
{
	QStringList directoryList;
	int position = 0;
	ftp->fileResourceManager(position, 0, directoryList);

	//内嵌显示树结构
	QStringList headers;
	headers << tr("Name") << tr("Size") << tr("Date Modified");
	TreeModel *model = new TreeModel(headers, directoryList);
	ui->treeView->setModel(model);

	for (int column = 0; column < model->columnCount(); ++column)
		ui->treeView->resizeColumnToContents(column);
	
	connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateSelection);
	connect(ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::rename);
}

void MainWindow::rename()
{
	QModelIndex index = ui->treeView->selectionModel()->currentIndex();
	QAbstractItemModel *model = ui->treeView->model();

}

void MainWindow::updateSelection()
{
	bool hasSelection = ui->treeView->selectionModel()->currentIndex().isValid();

	if (hasSelection) 
	{
		ui->treeView->closePersistentEditor(ui->treeView->selectionModel()->currentIndex());
		bool isFolder;
		QString filepath = getPath(ui->treeView->selectionModel()->currentIndex(), isFolder);
		if (isFolder)
			filepath += "/";
		ui->serverWorkingDirectory->setText(filepath);
	}
}

QString MainWindow::getPath(QModelIndex index, bool &isFolder)		// isFolder判断当前文件是否是一个文件夹
{
	QAbstractItemModel *model = ui->treeView->model();
	QString current = model->data(index).toString();
	QString parentname = model->data(model->parent(index)).toString();	//父目录名

	if (parentname != "")
		parentname = getPath(model->parent(index), isFolder) + "/";	// 当不处于根目录下时

	isFolder = (model->hasChildren(index)) ? true : false;	// 只关心最后一次的赋值，这才是我们的目标文件（夹）
	
	return parentname + current;	// 返回当前index的完整路径
}

void MainWindow::on_ipComboBox_enterFinished()
{
	//qDebug() << "1 ComboBox finished & Remember!";
}

void MainWindow::on_userNameComboBox_enterFinished()
{
	//qDebug() << "2 ComboBox finished & Remember!";
}

void MainWindow::on_actionExit_triggered()
{
	ftp->close();
	this->close();
}

void MainWindow::on_actionType_triggered()
{
	char t = ui->actionAscii->isEnabled() ? 'A' : 'I';
	ftp->type(t);
	ui->actionAscii->setDisabled(ui->actionAscii->isEnabled());
	ui->actionBinary->setDisabled(ui->actionBinary->isEnabled());
}

void MainWindow::on_actionPASV_triggered()
{
	ftp->setPassive();
	ui->actionPASV->setDisabled(ui->actionPASV->isEnabled());
	ui->actionPOSI_2->setDisabled(ui->actionPOSI_2->isEnabled());
}

void MainWindow::on_actionPOSI_2_triggered()
{
	ftp->setPositive();
	ui->actionPASV->setDisabled(ui->actionPASV->isEnabled());
	ui->actionPOSI_2->setDisabled(ui->actionPOSI_2->isEnabled());
}

void MainWindow::on_responseAction(int code, QString caption, QString msg)
{
	if (code >= 500)
	{
		QMessageBox::warning(this, caption, msg, QMessageBox::Ok);
		return;
	}
	switch (code) {
	case 221:
		ui->menuSetting->setDisabled(true);
		ui->actionLogout->setDisabled(true);
		ui->groupBox_login->setEnabled(true);
		ui->groupBox_transfer->setDisabled(true);
		ui->groupBox_directory->setDisabled(true);
		ui->userNameComboBox->clearEditText(); // TODO History? remember me
		ui->passwordLineEdit->clear();
		loginStatus->setText("Unconnected State. Offline.");
		ui->treeView->setModel(NULL);
		break;
	case 230:
		// 非法操作限制
		ui->menuSetting->setEnabled(true);
		ui->actionLogout->setEnabled(true);
		ui->groupBox_login->setDisabled(true);
		ui->groupBox_transfer->setEnabled(true);
		ui->groupBox_directory->setEnabled(true);
		loginStatus->setText("Hi " + ui->userNameComboBox->currentText() + ", welcome! Connected to ftp://" + ui->ipComboBox->currentText());
		ftp->setPassive();
		showDirectoy();
		break;
	default:
		break;
	}
}

void MainWindow::updateTransferProgress(qint64 completeBytes, qint64 totalBytes)
{
	ui->progressBar->setMaximum(totalBytes);
	ui->progressBar->setValue(completeBytes);
	qDebug() << "setValue: " << completeBytes << "  totalBytes: " << totalBytes;
}

void MainWindow::on_changeButton_clicked()
{
	ftp->rename(ui->serverWorkingDirectory->text(), ui->serverFileName->text());
	showDirectoy();
}

