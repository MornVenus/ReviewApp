#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_reviewapp.h"
#include "reviewcardlistmodel.h"
#include "reviewcard.h"
#include "cardstyledelegate.h"
#include "reviewfilter.h"
#include <QActionGroup>
#include "editdialog.h"
#include <QMessageBox>

class reviewapp : public QMainWindow
{
    Q_OBJECT

public:
    reviewapp(QWidget *parent = nullptr);
    ~reviewapp();

private:
    Ui::reviewappClass ui;
    ReviewCardListModel* m_model;
    ReviewFilter* m_filter;
	CardStyleDelegate* m_delegate;

private:
	void setShowListType(ReviewCard::ShowListType type);

private slots:
	//void on_actionAll_triggered() { setShowListType(ReviewCard::All); }
    void on_actionAdd_triggered();
	void on_filterTextBox_textChanged(const QString& text) { m_filter->setFilterText(text); }
};

