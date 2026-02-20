#pragma once

#include <QDialog>
#include "ui_editdialog.h"
#include "reviewcard.h"

class EditDialog : public QDialog
{
	Q_OBJECT

public:

	EditDialog() :  ui(new Ui::Dialog)
	{
		ui->setupUi(this);
	}

	~EditDialog() {}

	PlainCard getResult()
	{
		return {ui->questionTextBox->toPlainText(), ui->answerTextBox->toPlainText(), ui->categoryTextBox->text(), ui->tagTextBox->text()};
	}

private:
	Ui::Dialog* ui;
};
