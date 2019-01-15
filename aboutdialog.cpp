#include "aboutdialog.h"
#include "ui_aboutdialog.h"
aboutDialog::aboutDialog():
QDialog(0),
ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
}
