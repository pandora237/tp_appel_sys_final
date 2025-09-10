#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QTableView>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>
#include <signal.h>

using namespace std;

class SystemCall : public QMainWindow
{
    Q_OBJECT

public:
    SystemCall(QWidget *parent = nullptr);
    // QString getMemorySegments(const QString &pid);
public slots:
    void selectProgram();
    void startTracing();
    int stopTracing(bool force = false);
    void readStraceOutput();
    void initBtn(int action, string message = "");
    void resetBtnInit();
    void setClearBtn();

private:
    QPushButton *selectProgrammBtn;
    QLineEdit *iptProgramm;
    QLabel *statusLabel;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *playButton;
    QTableWidget *syscallTable;
    QProcess *process;
    QPushButton *clearBtn;
    int child_pid;
    QLineEdit *iptPidRunning;
    QCheckBox *isTracingRunning;
    QComboBox *traceTypeComboBox;
};

#endif // SYSTEMCALL_H
