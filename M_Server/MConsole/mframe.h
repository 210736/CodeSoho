#ifndef MFRAME_H
#define MFRAME_H

#include <QDialog>

class QLineEdit;
class QTextEdit;
class MFrame : public QDialog
{
    Q_OBJECT
private:
    QLineEdit* lineEdit;
    QTextEdit* textEdit;

public:
    MFrame(const wchar_t* consoleTitle = L"MConsole", QWidget *parent = 0);
	MFrame(const wchar_t* consoleTitle, const wchar_t* consoleCmd, QWidget *parent = 0);
	MFrame(const wchar_t* consoleTitle, int argc, const wchar_t* argv[], QWidget *parent = 0);
    ~MFrame();

    void Serialize(int level, const QString& v);
	void Serialize(int level, const wchar_t* outStr);

    virtual QSize sizeHint() const;					// ���ô��ڳ�ʼ��С
	virtual void closeEvent(QCloseEvent * event);	// ��д�ر��¼���ֹ����رմ���
	virtual void keyPressEvent(QKeyEvent *event);	// ��д�����¼���ֹ����esc�رմ���
private:
	static bool bCloseAllow;
private slots:
	void ReturnTextEdit();
};

#endif // MFRAME_H
