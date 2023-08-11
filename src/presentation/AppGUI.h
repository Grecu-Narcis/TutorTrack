#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QCheckBox>

#include "../business/AdministratorService.h"
#include "../business/UserService.h"


class AppGUI : public QMainWindow
{
	Q_OBJECT

private:
	QWidget* adminWindow, *userWindow;
	QLabel* mainMenuLabel, * adminMenuLabel, * userMenuLabel, * currentTutorial;
	QPushButton* adminModeButton, * userModeButton;
	
	QLineEdit* tutorialTitleInput, * tutorialPresenterInput, * tutorialLinkInput, 
		* tutorialNumberOfLikesInput, * tutorialDurationMinutesInput, * tutorialDurationSecondsInput;
	
	QListWidget* tutorialsList;
	QCheckBox* likeCheckBox, * watchListType;

	int indexOfCurrentTutorial;

	UserService& userService;
	AdministratorService& adminService;

	void loadDataToList(QListWidget* listToLoad, vector<Tutorial> dataToLoad);
	

private slots:
	void setMainMenuLayout();
	void setAdminLayout();
	void setUserLayout();

	void addTutorialToDatabase();
	void updateTutorialFromDatabase();
	void removeTutorialFromDatabase();

	void addTutorialToWatchList();
	void getNextTutorialFromDatabase();
	void deleteTutorialFromWatchList();
	void openWatchList();

public:
	AppGUI(AdministratorService& adminService, UserService& userService, QWidget* parentWidget = nullptr);
};

