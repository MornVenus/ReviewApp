#include "reviewapp.h"

ReviewApp::ReviewApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	setupThemeMenu();
	ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ui.answerTextBox->setVisible(false);
	ui.answerLabel->setVisible(false);

	QActionGroup* group = new QActionGroup(this);
	group->setExclusive(true);
	group->addAction(ui.actionAll);
	group->addAction(ui.actionReview);
	group->addAction(ui.actionFav);
	group->addAction(ui.actionTest);

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

	QString appDir = QCoreApplication::applicationDirPath();
	QString datebasePath = QDir(appDir).filePath("review.db");

	bool dbFileExist = QFile(datebasePath).exists();
	db.setDatabaseName(datebasePath);
	if (db.open())
	{
		qDebug() << "Database opened successfully.";

		if (!dbFileExist)
		{
			// create table
			QSqlQuery query;
			QString createTableSQL = R"(
				CREATE TABLE records(
					id integer primary key autoincrement,
					question text not null,
					answer text,
					category text,
					tags text,
					level integer not null default(0),
					create_time datetime not null default(datetime('now', 'localtime')),
					review_time datetime not null default(datetime('now', 'localtime')),
					fav integer default 0
				)
			)";
			if (!query.exec(createTableSQL))
			{
				qCritical() << "Failed to create table records:" << query.lastError().text();
			}
			else
			{
				qDebug() << "Table records created successfully.";
			}
		}
	}
	else
	{
		qDebug() << "Failed to open database:" << db.lastError().text();
	}

	m_model = new ReviewCardListModel(this);
	m_model->setShowListType(ReviewCard::Review); //  default is review list
	m_filter = new ReviewFilter(this);
	
	m_filter->setSourceModel(m_model);

	ui.cardListView->setModel(m_filter);

	m_selectionModel = new QItemSelectionModel(m_filter, this);
	ui.cardListView->setSelectionModel(m_selectionModel);

	m_delegate = new CardStyleDelegate(this);
	ui.cardListView->setItemDelegate(m_delegate);

	connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &ReviewApp::on_currentIndex_changed);

	new CodeHighlighter(ui.answerTextBox->document());
}

ReviewApp::~ReviewApp()
{}

void ReviewApp::updateLevel(int increment)
{
	if (increment == 0) return;

	ReviewCard* record = m_selectionModel->currentIndex().data(Qt::UserRole + 1).value<ReviewCard*>();
	if (!record) return;
	int level = record->level + increment;
	if (level < 0) level = 0;
	if (level > 8) level = 8;
	record->level = static_cast<ReviewCard::ReviewLevel>(level);
	QSqlQuery query;
	query.prepare("update records set level = :level, review_time = :time where id = :id");
	query.bindValue(":level", level);
	query.bindValue(":time", QDateTime::currentDateTime());
	query.bindValue(":id", record->id);
	if (!query.exec())
	{
		QMessageBox::critical(this, "Error", query.lastError().text());
		return;
	}
	setShowListType(-1);
}

void ReviewApp::setShowListType(int level)
{
	ui.favBtn->setChecked(false);
	ui.viewAnswerBtn->setChecked(false);
	ui.answerTextBox->setVisible(false);
	ui.answerLabel->setVisible(false);
	ui.questionTextBox->clear();
	enableControls(false);
	m_model->setShowListType(level);
}

void ReviewApp::enableControls(bool enable)
{
	ui.editBtn->setEnabled(enable);
	ui.deleteBtn->setEnabled(enable);
	ui.favBtn->setEnabled(enable);
	ui.viewAnswerBtn->setEnabled(enable);
	ui.hardBtn->setEnabled(enable);
	ui.normalBtn->setEnabled(enable);
	ui.easyBtn->setEnabled(enable);
}

void ReviewApp::updateFavBtnIcon(bool checked)
{
	QIcon icon;
	if (checked)
	{
		icon.addFile(":/images/fav_selected.png");
	}
	else
	{
		icon.addFile(":/images/fav.png");
	}
	ui.favBtn->setIcon(icon);
}

void ReviewApp::setupThemeMenu()
{
	QMenu* themeMenu = new QMenu("主题", this);

	ThemeManager* tm = ThemeManager::instance();
	QStringList themeNames = tm->getThemeNames();

	QActionGroup* themeGroup = new QActionGroup(this);
	themeGroup->setExclusive(true);

	Theme currentTheme = tm->getCurrentTheme();

	for (const QString& name : themeNames)
	{
		Theme theme = tm->getTheme(name);
		QAction* action = new QAction(theme.displayName, this);
		action->setCheckable(true);
		action->setData(name);
		action->setToolTip(theme.description);

		if (name == currentTheme.name)
		{
			action->setChecked(true);
		}

		connect(action, &QAction::triggered, this, [this, name]() {
			ThemeManager::instance()->applyTheme(name);
			});

		themeGroup->addAction(action);
		themeMenu->addAction(action);
	}

	// Add theme menu to toolbar
	QToolButton* themeBtn = new QToolButton(this);
	QIcon icon;
	icon.addFile(":/images/theme.png");
	themeBtn->setIcon(icon);
	themeBtn->setText("主题");
	themeBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	themeBtn->setMenu(themeMenu);
	themeBtn->setPopupMode(QToolButton::InstantPopup);

	ui.mainToolBar->addWidget(themeBtn);

	// Connect theme change signal
	connect(tm, &ThemeManager::themeChanged, this, &ReviewApp::onThemeChanged);
}

void ReviewApp::on_actionAdd_triggered()
{
	EditDialog dialog;
	dialog.setMinimumWidth(600);
	dialog.setMinimumHeight(400);
	if (dialog.exec())
	{
		auto result = dialog.getResult();
		m_model->append(result);
	}
}

void ReviewApp::on_currentIndex_changed(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	ui.viewAnswerBtn->setChecked(false);
	ui.answerTextBox->setVisible(false);
	ui.answerLabel->setVisible(false);
	
	if (!current.isValid())
	{
		return;
	}
	ReviewCard* card = current.data(Qt::UserRole + 1).value<ReviewCard*>();
	if (!card) return;
	
	enableControls(true);

	ui.questionTextBox->setPlainText(card->question);
	ui.answerTextBox->setMarkdown(card->answer);
	ui.favBtn->setChecked(card->fav);
	updateFavBtnIcon(card->fav);
}

void ReviewApp::on_editBtn_clicked()
{
	auto index = ui.cardListView->currentIndex();
	if (!index.isValid()) return;

	ReviewCard* card = index.data(Qt::UserRole + 1).value<ReviewCard*>();
	if (!card) return;
	EditDialog dialog(card);
	dialog.setMinimumWidth(600);
	dialog.setMinimumHeight(400);
	if (dialog.exec())
	{
		auto result = dialog.getResult();
		// update database
		QSqlQuery query;
		query.prepare(R"(update records set question = :question, answer = :answer, category = :category, tags = :tags where id = :id)");
		query.bindValue(":question", result.question);
		query.bindValue(":answer", result.answer);
		query.bindValue(":category", result.category);
		query.bindValue(":tags", result.tags);
		query.bindValue(":id", card->id);

		if (!query.exec())
		{
			qCritical() << "Failed to update record:" << query.lastError().text();
		}
		else
		{
			qDebug() << "Record updated successfully.";
			card->question = result.question;
			card->answer = result.answer;
			card->category = result.category;
			card->tags = result.tags;
			ui.questionTextBox->setPlainText(card->question);
			ui.answerTextBox->setMarkdown(card->answer);
		}
	}
}

void ReviewApp::on_deleteBtn_clicked()
{
	auto index = ui.cardListView->currentIndex();
	if (!index.isValid()) return;
	QModelIndex sourceIndex = m_filter->mapToSource(index);
	if (!sourceIndex.isValid()) return;

	if (QMessageBox::Ok == QMessageBox::warning(this, "Warning", "Are you sure you want to delete?"))
	{
		QString error = m_model->remove(sourceIndex);
		if (!error.isEmpty())
		{
			QMessageBox::critical(this, "Error", error);
		}
	}
}

void ReviewApp::on_favBtn_clicked(bool checked)
{
	auto index = ui.cardListView->currentIndex();
	if (!index.isValid()) return;
	QModelIndex sourceIndex = m_filter->mapToSource(index);
	if (!sourceIndex.isValid()) return;

	updateFavBtnIcon(checked);

	ReviewCard* card = sourceIndex.data(Qt::UserRole + 1).value<ReviewCard*>();
	if (!card) return;
	
	QSqlQuery query;
	query.prepare("update records set fav = :fav where id = :id");
	query.bindValue(":fav", checked ? 1 : 0);
	query.bindValue(":id", card->id);
	if (!query.exec())
	{
		QMessageBox::critical(this, "Error", query.lastError().text());
	}
	else
	{
		card->fav = checked;
	}
}

void ReviewApp::on_viewAnswerBtn_clicked(bool checked)
{
	ui.answerTextBox->setVisible(checked);
	ui.answerLabel->setVisible(checked);
}

void ReviewApp::onThemeChanged()
{
	Theme theme = ThemeManager::instance()->getCurrentTheme();
	ui.statusBar->showMessage("Theme changed: " + theme.displayName, 2000);
}