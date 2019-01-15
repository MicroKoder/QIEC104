#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "iec104driver.h"
namespace Ui {
class aboutDialog;
}

class aboutDialog : public QDialog
{
    Q_OBJECT
public:
    Ui::aboutDialog *ui;
    aboutDialog();
};

#endif // ABOUTDIALOG_H
