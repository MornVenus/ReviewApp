#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_reviewapp.h"
#include "reviewcardlistmodel.h"
#include "reviewcard.h"

class reviewapp : public QMainWindow
{
    Q_OBJECT

public:
    reviewapp(QWidget *parent = nullptr);
    ~reviewapp();

private:
    Ui::reviewappClass ui;
    ReviewCardListModel* m_model;

private:
	void setShowListType(ReviewCard::ShowListType type);
};

