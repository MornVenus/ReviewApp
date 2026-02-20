#pragma once

#include <QDialog>
#include "ui_editdialog.h"
#include "reviewcard.h"
#include "codehighlighter.h"

class EditDialog : public QDialog
{
	Q_OBJECT

public:

	EditDialog() :  ui(new Ui::Dialog)
	{
		ui->setupUi(this);

		new CodeHighlighter(ui->markdownTextBox->document());
	}

	EditDialog(ReviewCard* card) : ui(new Ui::Dialog)
	{
		ui->setupUi(this);
		ui->questionTextBox->setPlainText(card->question);
		ui->answerTextBox->setPlainText(card->answer);
		ui->categoryTextBox->setText(card->category);
		ui->tagTextBox->setText(card->tags);

		new CodeHighlighter(ui->markdownTextBox->document());
	}

	~EditDialog() {}

	PlainCard getResult()
	{
		return {ui->questionTextBox->toPlainText(), ui->answerTextBox->toPlainText(), ui->categoryTextBox->text(), ui->tagTextBox->text()};
	}

private:
	Ui::Dialog* ui;
private slots:
	void on_answerTextBox_textChanged()
	{
		ui->markdownTextBox->setMarkdown(ui->answerTextBox->toPlainText());
	}
};
