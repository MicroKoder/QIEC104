#ifndef ADDSIGNALDIALOG_H
#define ADDSIGNALDIALOG_H

#include <QDialog>
#include <ciecsignal.h>

namespace Ui {
class addSignalDialog;
}

class addSignalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addSignalDialog(QWidget *parent = 0);
    ~addSignalDialog();
    CIECSignal tag;
    QString description;
private:
    Ui::addSignalDialog *ui;
public slots:
    void accept();
};

#endif // ADDSIGNALDIALOG_H
