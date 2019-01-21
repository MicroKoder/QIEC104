#ifndef EDITCMDDIALOG_H
#define EDITCMDDIALOG_H

#include <QDialog>

namespace Ui {
class EditCMDdialog;
}

class EditCMDdialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditCMDdialog(QWidget *parent = 0);
    ~EditCMDdialog();

private:
    Ui::EditCMDdialog *ui;
};

#endif // EDITCMDDIALOG_H
