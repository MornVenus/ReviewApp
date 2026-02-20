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
#include <QItemSelectionModel>
#include "codehighlighter.h"
#include <QtSql>
#include <QDebug>

class ReviewApp : public QMainWindow
{
    Q_OBJECT

public:
    ReviewApp(QWidget *parent = nullptr);
    ~ReviewApp();

private:
    Ui::reviewappClass ui;
    ReviewCardListModel* m_model;
    ReviewFilter* m_filter;
	QItemSelectionModel* m_selectionModel;
	CardStyleDelegate* m_delegate;

private:
	void setShowListType(ReviewCard::ShowListType type);

private slots:
	void on_actionAll_triggered() { setShowListType(ReviewCard::All); }
    void on_actionReview_triggered() { setShowListType(ReviewCard::Review); }
    void on_actionTest_triggered() { setShowListType(ReviewCard::Test); }
    void on_actionFav_triggered() { setShowListType(ReviewCard::Fav); }
    void on_actionAdd_triggered();
	void on_filterTextBox_textChanged(const QString& text) { m_filter->setFilterText(text); }
    void on_currentIndex_changed(const QModelIndex& current, const QModelIndex& previous);
};

