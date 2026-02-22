#pragma once

#include <QDialog>
#include "ui_editdialog.h"
#include "reviewcard.h"
#include "codehighlighter.h"
#include "theme.h"

class EditDialog : public QDialog
{
	Q_OBJECT

public:

	EditDialog() :  ui(new Ui::Dialog)
	{
		ui->setupUi(this);

		new CodeHighlighter(ui->markdownTextBox->document());

		// Set monospace font - using display font
		Theme theme = THEME_MANAGER->getCurrentTheme();
		QFont font;
		font.setFamily(theme.fontDisplay.isEmpty() ? "Consolas" : theme.fontDisplay);
		font.setPointSize(12);
		ui->questionTextBox->setFont(font);
		ui->answerTextBox->setFont(font);
		ui->markdownTextBox->setFont(font);

		// Set Tab width
		const int tabStop = 4;
		QFontMetrics metrics(font);
		ui->questionTextBox->setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
		ui->answerTextBox->setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
	}

	EditDialog(ReviewCard* card) : ui(new Ui::Dialog)
	{
		ui->setupUi(this);
		ui->questionTextBox->setPlainText(card->question);
		ui->answerTextBox->setPlainText(card->answer);
		ui->categoryTextBox->setText(card->category);
		ui->tagTextBox->setText(card->tags);

		new CodeHighlighter(ui->markdownTextBox->document());

		// Set monospace font - using display font
		Theme theme = THEME_MANAGER->getCurrentTheme();
		QFont font;
		font.setFamily(theme.fontDisplay.isEmpty() ? "Consolas" : theme.fontDisplay);
		font.setPointSize(12);
		ui->questionTextBox->setFont(font);
		ui->answerTextBox->setFont(font);
		ui->markdownTextBox->setFont(font);

		// Set Tab width
		const int tabStop = 4;
		QFontMetrics metrics(font);
		ui->questionTextBox->setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
		ui->answerTextBox->setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
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
