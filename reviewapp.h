#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_reviewapp.h"

class reviewapp : public QMainWindow
{
    Q_OBJECT

public:
    reviewapp(QWidget *parent = nullptr);
    ~reviewapp();

private:
    Ui::reviewappClass ui;
};

