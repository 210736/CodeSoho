#include "stdafx.h"
#include <QtCore/qglobal.h>
#include <QApplication>
//#include <QTextCodec>
#include "mconsole_global.h"
#include "mframe.h"

int MCONSOLE_API run_QTUI(int argc, char *argv[])
{
    QApplication a(argc,argv);
//	QTextCodec *codec = QTextCodec::codecForName("GBK");//情况1
//	QTextCodec::setCodecForTr(codec);
//	QTextCodec::setCodecForLocale(codec);
//	QTextCodec::setCodecForCStrings(codec);
	const wchar_t* temp_logon = L"你好";
    MFrame* dialog = new MFrame(temp_logon);
    dialog->show();
    return a.exec();
}

int MCONSOLE_API Trun_QTUI(UINT64 argc,CONST LPTSTR argv)
{
	int testArgc = 0;
	QApplication a(testArgc, NULL);
	//	QTextCodec *codec = QTextCodec::codecForName("GBK");//情况1
	//	QTextCodec::setCodecForTr(codec);
	//	QTextCodec::setCodecForLocale(codec);
	//	QTextCodec::setCodecForCStrings(codec);

	const wchar_t* temp_logon = ECore::SS::ToW(argc);
	MFrame* dialog = new MFrame(temp_logon, argv);
	dialog->show();
	return a.exec();
}

int MCONSOLE_API Trun_QTUI2(INT32 argc, CONST WCHAR** argv)
{
	int testArgc = 0;
	QApplication a(testArgc, NULL);

	const LPTSTR temp_logon = L"你好";
	MFrame* dialog = new MFrame(temp_logon, argc, argv);
	dialog->show();
	return a.exec();
}
