#include "reviewapp.h"

reviewapp::reviewapp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_model = new ReviewCardListModel(this);
	ui.cardListView->setModel(m_model);

	setShowListType(ReviewCard::Review); //  default is review list

	m_delegate = new CardStyleDelegate(this);
	ui.cardListView->setItemDelegate(m_delegate);
}

reviewapp::~reviewapp()
{}

void reviewapp::setShowListType(ReviewCard::ShowListType type)
{
	m_model->setShowListType(type);
}

