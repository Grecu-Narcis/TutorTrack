#include "AppGUI.h"
#include "../domain/ValidatorException.h"
#include "../infrastructure/RepositoryException.h"
#include "../utility/StringOperations.h"

#include <QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QMessageBox>


void AppGUI::setMainMenuLayout()
{
	if (adminWindow != nullptr)
		adminWindow->hide();

	if (userWindow != nullptr)
		userWindow->hide();

	mainMenuLabel = new QLabel{ "Welcome to Tutorial Manager\nSelect your mode:" };
	mainMenuLabel->setAlignment(Qt::AlignCenter);
	userModeButton = new QPushButton{ "User" };
	adminModeButton = new QPushButton{ "Admin" };

	connect(userModeButton, &QPushButton::clicked, this, &AppGUI::setUserLayout);
	connect(adminModeButton, &QPushButton::clicked, this, &AppGUI::setAdminLayout);

	watchListType = new QCheckBox{ "HTML Watchlist" };

	QWidget* centralWidget = new QWidget{};

	QVBoxLayout* mainLayout = new QVBoxLayout{centralWidget};
	QHBoxLayout* buttonsLayout = new QHBoxLayout{};
	
	mainLayout->addWidget(mainMenuLabel);

	buttonsLayout->addWidget(adminModeButton);
	buttonsLayout->addWidget(userModeButton);
	buttonsLayout->addWidget(watchListType);

	buttonsLayout->setAlignment(Qt::AlignHCenter);

	mainLayout->addLayout(buttonsLayout);

	this->setCentralWidget(centralWidget);
	this->show();
}


void AppGUI::loadDataToList(QListWidget* listToLoad, vector<Tutorial> dataToLoad)
{
	listToLoad->clear();

	for (const auto& tutorial : dataToLoad)
		listToLoad->addItem(QString::fromStdString(tutorial.toString()));
}


void AppGUI::setAdminLayout()
{
	this->hide();

	adminWindow = new QWidget{};
	adminWindow->setWindowTitle("Admin Mode");
	adminWindow->resize(600, 400);

	tutorialsList = new QListWidget{ adminWindow };
	this->loadDataToList(tutorialsList, this->adminService.getAllTutorials());

	QHBoxLayout* tutorialsListLayout = new QHBoxLayout{};
	tutorialsListLayout->addWidget(tutorialsList);

	// create form layout for tutorial input
	QFormLayout* tutorialForm = new QFormLayout{};

	QLabel* tutorialTitleLabel = new QLabel{ "Title:" };
	tutorialTitleInput = new QLineEdit{};

	tutorialForm->addRow(tutorialTitleLabel, tutorialTitleInput);


	QLabel* tutorialPresenterLabel = new QLabel{ "Presenter:" };
	tutorialPresenterInput = new QLineEdit{};

	tutorialForm->addRow(tutorialPresenterLabel, tutorialPresenterInput);


	QLabel* tutorialLinkLabel = new QLabel{ "Link:" };
	tutorialLinkInput = new QLineEdit{};

	tutorialForm->addRow(tutorialLinkLabel, tutorialLinkInput);


	QLabel* tutorialNumberOfLikesLabel = new QLabel{ "Number of likes:" };
	tutorialNumberOfLikesInput = new QLineEdit{};

	tutorialForm->addRow(tutorialNumberOfLikesLabel, tutorialNumberOfLikesInput);


	QLabel* tutorialDurationMinutesLabel = new QLabel{ "Minutes:" };
	//QRegExp 
	tutorialDurationMinutesInput = new QLineEdit{};

	QLabel* tutorialDurationSecondsLabel = new QLabel{ "Seconds:" };
	tutorialDurationSecondsInput = new QLineEdit{};

	QHBoxLayout* durationLayout = new QHBoxLayout{};
	durationLayout->addWidget(tutorialDurationMinutesLabel);
	durationLayout->addWidget(tutorialDurationMinutesInput);
	durationLayout->addWidget(tutorialDurationSecondsLabel);
	durationLayout->addWidget(tutorialDurationSecondsInput);

	tutorialForm->addRow(durationLayout);

	QVBoxLayout* editTutorialsLayout = new QVBoxLayout{};
	editTutorialsLayout->addLayout(tutorialForm);

	// create buttons for add, remove and update
	QPushButton* addTutorialButton, * updateTutorialButton, * deleteTutorialButton, *backButton;
	addTutorialButton = new QPushButton{ "Add" };
	updateTutorialButton = new QPushButton{ "Update" };
	deleteTutorialButton = new QPushButton{ "Delete" };
	backButton = new QPushButton{ "Back" };

	connect(addTutorialButton, &QPushButton::clicked, this, &AppGUI::addTutorialToDatabase);
	connect(updateTutorialButton, &QPushButton::clicked, this, &AppGUI::updateTutorialFromDatabase);
	connect(deleteTutorialButton, &QPushButton::clicked, this, &AppGUI::removeTutorialFromDatabase);
	connect(backButton, &QPushButton::clicked, this, &AppGUI::setMainMenuLayout);

	QHBoxLayout* tutorialOptionsLayout = new QHBoxLayout{};
	tutorialOptionsLayout->addWidget(addTutorialButton);
	tutorialOptionsLayout->addWidget(updateTutorialButton);
	tutorialOptionsLayout->addWidget(deleteTutorialButton);
	tutorialOptionsLayout->addWidget(backButton);

	editTutorialsLayout->addLayout(tutorialOptionsLayout);

	QHBoxLayout* mainLayout = new QHBoxLayout{ adminWindow };

	mainLayout->addLayout(tutorialsListLayout);
	mainLayout->addLayout(editTutorialsLayout);

	adminWindow->setLayout(mainLayout);
	adminWindow->show();
}


void AppGUI::setUserLayout()
{
	this->hide();

	if (watchListType->isChecked())
		this->userService.setHTMLFileWatchList();
	else
		this->userService.setCSVFileWatchList();

	userWindow = new QWidget{};
	userWindow->setWindowTitle("User mode");
	userWindow->resize(600, 400);

	// set up watchlist layout
	tutorialsList = new QListWidget{userWindow};
	this->loadDataToList(tutorialsList, this->userService.getWatchList());

	QLabel* tutorialsListLabel = new QLabel{ "Watchlist:" };
	QVBoxLayout* watchListDisplayLayout = new QVBoxLayout{};

	watchListDisplayLayout->addWidget(tutorialsListLabel);
	watchListDisplayLayout->addWidget(tutorialsList);


	// set up current tutorial and options layout	
	Tutorial firstTutorialInDatabase = userService.getAllTutorialsFromDatabase()[0];
	indexOfCurrentTutorial = 0;

	QLabel* currentTutorialIndicatorLabel = new QLabel{ "Current tutorial: " };
	currentTutorial = new QLabel{ QString::fromStdString(stripString(firstTutorialInDatabase.toString())) };

	currentTutorialIndicatorLabel->setAlignment(Qt::AlignCenter);
	currentTutorial->setAlignment(Qt::AlignCenter);

	// layout for current tutorial
	QHBoxLayout* currentTutorialLayout = new QHBoxLayout{};
	currentTutorialLayout->addWidget(currentTutorialIndicatorLabel);
	currentTutorialLayout->addWidget(currentTutorial);

	// currentTutorialLayout->setAlignment(Qt::AlignTop);

	QPushButton* addToWatchListButton = new QPushButton{ "Add" };
	QPushButton* nextTutorialButton = new QPushButton{ "Next" };
	QPushButton* deleteFromWatchListButton = new QPushButton{ "Delete" };
	QPushButton* backButton = new QPushButton{ "Back" };
	QPushButton* openWatchListButton = new QPushButton{ "Open" };

	connect(addToWatchListButton, &QPushButton::clicked, this, &AppGUI::addTutorialToWatchList);
	connect(nextTutorialButton, &QPushButton::clicked, this, &AppGUI::getNextTutorialFromDatabase);
	connect(deleteFromWatchListButton, &QPushButton::clicked, this, &AppGUI::deleteTutorialFromWatchList);
	connect(backButton, &QPushButton::clicked, this, &AppGUI::setMainMenuLayout);
	connect(openWatchListButton, &QPushButton::clicked, this, &AppGUI::openWatchList);

	// set the checkbox for like option
	likeCheckBox = new QCheckBox{ "Like the tutorial?" };

	// set up the layout of the options
	QGridLayout* optionsLayout = new QGridLayout{};
	optionsLayout->addWidget(addToWatchListButton, 0, 0);
	optionsLayout->addWidget(nextTutorialButton, 1, 0);
	optionsLayout->addWidget(deleteFromWatchListButton, 0, 1);
	optionsLayout->addWidget(likeCheckBox, 0, 2);
	optionsLayout->addWidget(openWatchListButton, 1, 1);
	optionsLayout->addWidget(backButton, 1, 2);

	// set up watch list management layout
	QVBoxLayout* watchListManagementLayout = new QVBoxLayout{};
	watchListManagementLayout->addLayout(currentTutorialLayout);
	watchListManagementLayout->addLayout(optionsLayout);

	QHBoxLayout* mainLayout = new QHBoxLayout{ userWindow };
	mainLayout->addLayout(watchListDisplayLayout);
	mainLayout->addLayout(watchListManagementLayout);

	userWindow->show();
}


AppGUI::AppGUI(AdministratorService& adminService, UserService& userService, QWidget* parentWidget) :
	adminService{ adminService }, userService{ userService }
{
	adminWindow = nullptr;
	userWindow = nullptr;

	setMainMenuLayout();
}


void AppGUI::addTutorialToDatabase()
{
	string tutorialTitle = tutorialTitleInput->text().toStdString();
	string tutorialPresenter = tutorialPresenterInput->text().toStdString();
	string tutorialLink = tutorialLinkInput->text().toStdString();

	int tutorialNumberOfLikes = stoi(tutorialNumberOfLikesInput->text().toStdString());
	int tutorialMinutesDuration = stoi(tutorialDurationMinutesInput->text().toStdString());
	int tutorialSecondsDuration = stoi(tutorialDurationSecondsInput->text().toStdString());
	
	try
	{
		this->adminService.addTutorial(tutorialTitle, tutorialPresenter, tutorialLink, tutorialNumberOfLikes,
			tutorialMinutesDuration, tutorialSecondsDuration);

		this->loadDataToList(tutorialsList, this->adminService.getAllTutorials());
	}

	catch (ValidatorException& ve)
	{
		QMessageBox errorDisplay;
		errorDisplay.setText(QString::fromStdString(ve.what()));
		errorDisplay.setWindowTitle("Validator Exception");
		errorDisplay.exec();
	}

	catch (RepositoryException& re)
	{
		QMessageBox errorDisplay;
		errorDisplay.setText(QString::fromStdString(re.what()));
		errorDisplay.setWindowTitle("Repository Exception");
		errorDisplay.exec();
	}
}


void AppGUI::updateTutorialFromDatabase()
{
	string tutorialTitle = tutorialTitleInput->text().toStdString();
	string tutorialPresenter = tutorialPresenterInput->text().toStdString();
	string tutorialLink = tutorialLinkInput->text().toStdString();

	int tutorialNumberOfLikes = stoi(tutorialNumberOfLikesInput->text().toStdString());
	int tutorialMinutesDuration = stoi(tutorialDurationMinutesInput->text().toStdString());
	int tutorialSecondsDuration = stoi(tutorialDurationSecondsInput->text().toStdString());

	try
	{
		this->adminService.updateTutorial(tutorialTitle, tutorialPresenter, tutorialLink, tutorialNumberOfLikes,
			tutorialMinutesDuration, tutorialSecondsDuration);

		this->loadDataToList(tutorialsList, this->adminService.getAllTutorials());
	}

	catch (ValidatorException& ve)
	{
		QMessageBox errorDisplay;
		errorDisplay.setText(QString::fromStdString(ve.what()));
		errorDisplay.setWindowTitle("Validator Exception");
		errorDisplay.exec();
	}

	catch (RepositoryException& re)
	{
		QMessageBox errorDisplay;
		errorDisplay.setText(QString::fromStdString(re.what()));
		errorDisplay.setWindowTitle("Repository Exception");
		errorDisplay.exec();
	}
}


void AppGUI::removeTutorialFromDatabase()
{
	string tutorialTitle = tutorialTitleInput->text().toStdString();
	string tutorialPresenter = tutorialPresenterInput->text().toStdString();

	try
	{
		this->adminService.deleteTutorial(tutorialTitle, tutorialPresenter);

		this->loadDataToList(tutorialsList, this->adminService.getAllTutorials());
	}

	catch (ValidatorException& ve)
	{
		QMessageBox errorDisplay;
		errorDisplay.setText(QString::fromStdString(ve.what()));
		errorDisplay.setWindowTitle("Validator Exception");
		errorDisplay.exec();
	}

	catch (RepositoryException& re)
	{
		QMessageBox errorDisplay;
		errorDisplay.setText(QString::fromStdString(re.what()));
		errorDisplay.setWindowTitle("Repository Exception");
		errorDisplay.exec();
	}
}


void AppGUI::addTutorialToWatchList()
{
	Tutorial tutorialToAdd = this->userService.getAllTutorialsFromDatabase()[indexOfCurrentTutorial];

	try
	{
		this->userService.addTutorialToWatchList(tutorialToAdd);
		this->loadDataToList(tutorialsList, this->userService.getWatchList());
	}
	catch (RepositoryException& re)
	{
		QMessageBox errorDisplay;
		errorDisplay.setText(QString::fromStdString(re.what()));
		errorDisplay.setWindowTitle("Repository Exception");
		errorDisplay.exec();
	}
}


void AppGUI::getNextTutorialFromDatabase()
{
	indexOfCurrentTutorial = (indexOfCurrentTutorial + 1) % this->userService.getAllTutorialsFromDatabase().size();
	Tutorial currentTutorial = userService.getAllTutorialsFromDatabase()[indexOfCurrentTutorial];

	this->currentTutorial->setText(QString::fromStdString(stripString(currentTutorial.toString())));
}


void AppGUI::deleteTutorialFromWatchList()
{
	QListWidgetItem* selectedTutorial = tutorialsList->currentItem();

	if (selectedTutorial == nullptr)
	{
		QMessageBox selectTutorialAlert;
		selectTutorialAlert.setText("Please select a tutorial from watch list!");
		selectTutorialAlert.exec();
		return;
	}

	string tutorialTitle = splitString(selectedTutorial->text().toStdString(), ',')[0];
	string tutorialPresenter = splitString(selectedTutorial->text().toStdString(), ',')[1];

	bool isTutorialLiked = likeCheckBox->isChecked();

	tutorialsList->takeItem(tutorialsList->row(selectedTutorial));
	this->userService.deleteTutorialFromWatchList(tutorialTitle, tutorialPresenter, isTutorialLiked);

	loadDataToList(tutorialsList, userService.getWatchList());

	Tutorial currentTutorial = userService.getAllTutorialsFromDatabase()[indexOfCurrentTutorial];
	this->currentTutorial->setText(QString::fromStdString(stripString(currentTutorial.toString())));
}


void AppGUI::openWatchList()
{
	this->userService.openWatchList();
}
