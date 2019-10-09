#include "stdafx.h"
#include "mframe.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeWidgetItem>
#include <qevent.h>
//#include <QTextCodec>
//#include <QScrollBar>
wchar_t tempChar[512] = L"";
bool MFrame::bCloseAllow = false;

//编辑框自动滚屏，每次新增一行或多行字符调用一次
void autoScrollTextEdit(QTextEdit *textEdit,QString &text,QString &add_text)
{
    //添加行字符并刷新编辑框
    text+=add_text+"\n";
    textEdit->setText(text);
    //保持编辑器在光标最后一行
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
    //如果超过一定行数清空一下
    if(textEdit->document()->lineCount()>20)
    {
        text.clear();
        textEdit->clear();
    }

}

QSize MFrame::sizeHint() const
{
    return QSize(1024,720);
}

void MFrame::closeEvent(QCloseEvent * event)
{
	if (!bCloseAllow && event)
	{
		event->ignore();
	}
}

void MFrame::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		break;
	default:
		QDialog::keyPressEvent(event);
	}
}

void MFrame::Serialize(int level, const wchar_t* outStr)
{
	textEdit->append(QString::fromWCharArray(outStr));
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
}

void MFrame::Serialize(int level, const QString& outStr)
{
	textEdit->append(outStr);
	QTextCursor cursor = textEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	textEdit->setTextCursor(cursor);
}

MFrame::MFrame(const wchar_t* consoleTitle, const wchar_t* consoleCmd, QWidget *parent)
	: QDialog(parent)
{
	//	ECore::InitECore();
	memset(tempChar, 0, sizeof(tempChar));

	//	char* a = ECore::SS::To8(consoleTitle);
	//	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QString pQString = QString::fromWCharArray(consoleTitle);
	setWindowTitle(pQString);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

	textEdit = new QTextEdit;

	textEdit->setReadOnly(true);

	for (int i = 0; i < 100; ++i)
	{
		Serialize(0, consoleCmd);
	}

	lineEdit = new QLineEdit;

	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(ReturnTextEdit()));

	QHBoxLayout* mainLayout = new QHBoxLayout;

	QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, NULL);
	mainSplitter->setChildrenCollapsible(false);
	mainSplitter->setOpaqueResize(false); // 窗口线拖动

	QSplitter* leftSplitter = new QSplitter(Qt::Vertical, mainSplitter);
	leftSplitter->setChildrenCollapsible(false);

	leftSplitter->addWidget(textEdit);
	leftSplitter->addWidget(lineEdit);
	lineEdit->setFixedHeight(25);

	/*QTextEdit* textEdit2 = new QTextEdit;
	mainSplitter->addWidget(textEdit2);*/

	QTreeWidget* treeWidget = new QTreeWidget;
	treeWidget->setHeaderLabels(QStringList() << "Category" << "Value");
	QTreeWidgetItem* pCategory = new QTreeWidgetItem(QStringList() << "A");
	for (int i = 0; i<3; ++i)
	{
		QStringList columItemList;
		QTreeWidgetItem *child;
		QString key, value;
		key += "b" + QString::number(i);
		value += QString::number(i);
		columItemList << key << value;
		child = new QTreeWidgetItem(columItemList);
		pCategory->addChild(child);
		QString pString = child->text(1);
		//		Serialize(0, pString);
		child->setText(1, QString::asprintf("%d", pString.toInt() + 10));
		//		Serialize(0, child->text(1));
	}

	QTreeWidgetItem *child = pCategory->child(2);
	child->setText(1, QString::asprintf("%f", child->text(1).toInt() + 10.7));
	//	pCategory->addChild(child);
	//	A->setCheckState(0, Qt::Checked);
	//	returnPressed()
	treeWidget->addTopLevelItem(pCategory);

	mainSplitter->addWidget(treeWidget);

	mainLayout->addWidget(mainSplitter);
	setLayout(mainLayout);

}

MFrame::MFrame(const wchar_t* consoleTitle, QWidget *parent)
	: QDialog( parent )
{
//	ECore::InitECore();
	memset(tempChar, 0, sizeof(tempChar));

//	char* a = ECore::SS::To8(consoleTitle);
//	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QString pQString = QString::fromWCharArray(consoleTitle);
	setWindowTitle(pQString);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

	textEdit = new QTextEdit;

	textEdit->setReadOnly(true);

	for (int i = 0; i < 100; ++i)
	{
		Serialize(0, L"你好");
	}

    lineEdit = new QLineEdit;

	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(ReturnTextEdit()));

	QHBoxLayout* mainLayout = new QHBoxLayout;

	QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, NULL);
	mainSplitter->setChildrenCollapsible(false);
	mainSplitter->setOpaqueResize(false); // 窗口线拖动
	
	QSplitter* leftSplitter = new QSplitter(Qt::Vertical, mainSplitter);
	leftSplitter->setChildrenCollapsible(false);
	
	leftSplitter->addWidget(textEdit);
	leftSplitter->addWidget(lineEdit);
	lineEdit->setFixedHeight(25);
	
	/*QTextEdit* textEdit2 = new QTextEdit;
	mainSplitter->addWidget(textEdit2);*/
	
	QTreeWidget* treeWidget = new QTreeWidget;
	treeWidget->setHeaderLabels(QStringList() << "Category" << "Value");
	QTreeWidgetItem* pCategory = new QTreeWidgetItem(QStringList() << "A" );
	for (int i = 0; i<3; ++i)
	{
		QStringList columItemList;
		QTreeWidgetItem *child;
		QString key, value;
		key += "b" + QString::number(i);
		value += QString::number(i);
		columItemList << key << value;
		child = new QTreeWidgetItem(columItemList);
		pCategory->addChild(child);
		QString pString = child->text(1);
//		Serialize(0, pString);
		child->setText(1, QString::asprintf("%d",pString.toInt() + 10));
//		Serialize(0, child->text(1));
	}

	QTreeWidgetItem *child = pCategory->child(2);
	child->setText(1, QString::asprintf("%f", child->text(1).toInt() + 10.7));
//	pCategory->addChild(child);
//	A->setCheckState(0, Qt::Checked);
//	returnPressed()
	treeWidget->addTopLevelItem(pCategory);

	mainSplitter->addWidget(treeWidget);

	mainLayout->addWidget(mainSplitter);
	setLayout(mainLayout);

}

MFrame::MFrame(const wchar_t* consoleTitle, int argc, const wchar_t* argv[], QWidget *parent)
	: QDialog(parent)
{
	//	ECore::InitECore();
	memset(tempChar, 0, sizeof(tempChar));

	//	char* a = ECore::SS::To8(consoleTitle);
	//	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QString pQString = QString::fromWCharArray(consoleTitle);
	setWindowTitle(pQString);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

	textEdit = new QTextEdit;

	textEdit->setReadOnly(true);

	for (int i = 0; i < argc; ++i)
	{
		Serialize(0, *argv++);
	}

	lineEdit = new QLineEdit;

	connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(ReturnTextEdit()));

	QHBoxLayout* mainLayout = new QHBoxLayout;

	QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, NULL);
	mainSplitter->setChildrenCollapsible(false);
	mainSplitter->setOpaqueResize(false); // 窗口线拖动

	QSplitter* leftSplitter = new QSplitter(Qt::Vertical, mainSplitter);
	leftSplitter->setChildrenCollapsible(false);

	leftSplitter->addWidget(textEdit);
	leftSplitter->addWidget(lineEdit);
	lineEdit->setFixedHeight(25);

	/*QTextEdit* textEdit2 = new QTextEdit;
	mainSplitter->addWidget(textEdit2);*/

	QTreeWidget* treeWidget = new QTreeWidget;
	treeWidget->setHeaderLabels(QStringList() << "Category" << "Value");
	QTreeWidgetItem* pCategory = new QTreeWidgetItem(QStringList() << "A");
	for (int i = 0; i<3; ++i)
	{
		QStringList columItemList;
		QTreeWidgetItem *child;
		QString key, value;
		key += "b" + QString::number(i);
		value += QString::number(i);
		columItemList << key << value;
		child = new QTreeWidgetItem(columItemList);
		pCategory->addChild(child);
		QString pString = child->text(1);
		//		Serialize(0, pString);
		child->setText(1, QString::asprintf("%d", pString.toInt() + 10));
		//		Serialize(0, child->text(1));
	}

	QTreeWidgetItem *child = pCategory->child(2);
	child->setText(1, QString::asprintf("%f", child->text(1).toInt() + 10.7));
	//	pCategory->addChild(child);
	//	A->setCheckState(0, Qt::Checked);
	//	returnPressed()
	treeWidget->addTopLevelItem(pCategory);

	mainSplitter->addWidget(treeWidget);

	mainLayout->addWidget(mainSplitter);
	setLayout(mainLayout);
}

MFrame::~MFrame()
{

}

void MFrame::ReturnTextEdit()
{
	QString textString = lineEdit->text();

	/*memset(tempChar, 0, sizeof(tempChar));
	size_t szSize = textString.toWCharArray(tempChar);
	char* u8Test = ECore::SS::ToA(tempChar);*/

	/*QByteArray ba = textString.toLatin1();
	char *mm = ba.data();
	wchar_t* a = ECore::SS::To16(mm);

	memset(tempChar, 0, sizeof(tempChar));
	size_t szSize2 = QString(mm).toWCharArray(tempChar);
	char* u8Test2 = ECore::SS::ToA(tempChar);*/
//	wcscmp()
//	wcslen()
//	wcscat
	if (textString.isEmpty())
		return;
	Serialize(0, textString);
	lineEdit->clear();
	if (textString == "quit")
	{
		bCloseAllow = true;
		close();
	}
}
