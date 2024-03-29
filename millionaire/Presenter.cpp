//---------------------------------------------------------------------------


#pragma hdrstop
#include <fstream>
#include "Presenter.h"
#include "WinWindow.h"
#include "LoseWindow.h"
#include "RoomWindow.h"
#include "PhoneWindow.h"

using namespace std;
//---------------------------------------------------------------------------

#pragma package(smart_init)

AnswerOrder::AnswerOrder(bool isMixed){
	order = new string[4];
	if(!isMixed){
		order[0] = "rightAnswer";
		order[1] = "answer2";
		order[2] = "answer3";
		order[3] = "answer4";
	} else{
		vector<string> *tempAnswerList = new vector<string>;
		tempAnswerList->push_back("rightAnswer");
		tempAnswerList->push_back("answer2");
		tempAnswerList->push_back("answer3");
		tempAnswerList->push_back("answer4");
		for (int i = 0; i < 4; i++){
			int j = rand() % (4 - i);
			order[i] = (*tempAnswerList)[j];
			tempAnswerList->erase(tempAnswerList->begin() + j);
		}
	}
}

string AnswerOrder::getAnswerByIndex(Question question, int i){
	return getAnswerByName(question, order[i]);
}

string AnswerOrder::getAnswerByName(Question question, string answerName){
	if(answerName == "rightAnswer")
		return question.getRightAnswer();
	else if(answerName == "answer2")
		return question.getAnswer2();
	else if(answerName == "answer3")
		return question.getAnswer3();
	else if(answerName == "answer4")
		return question.getAnswer4();
	else
		return NULL;
}

int* AnswerOrder::mixRoomAnswers(int* answers){
	for(int i = 0; i < 4; i++){
		if(order[i] == "rightAnswer"){
			int temp = answers[i];
			answers[i] = answers[0];
			answers[0] = temp;
			break;
		}
	}
	return answers;
}

bool AnswerOrder::isAnswerTrue(int index){
	return order[index] == "rightAnswer" ? true : false;
}

BaseGame::BaseGame(Question* questions): isFiftyFiftyAvialable(true), isFriendAvialable(true), isRoomHelpAvialable(true){
	this->questions = questions;
	currentState = -1;
	loadGameWinSums();
}

void BaseGame::loadGameWinSums(){
	ifstream fileStream;
	string* path = new string();
	*path += "resources\\data\\win_sums.dat";
	fileStream.open(path->c_str(), ios::in);
	gameWinSums = new int[15];
	int i = 0;
	while(!fileStream.eof()){
		fileStream>>gameWinSums[i];
		if(++i >= 15)
        	break;
	}
}

void BaseGame::processGameOver(){
	if(!gameOver){
		int winSum = currentState <= 0 ? 0 : gameWinSums[currentState-1];
		FormWinWindow->initComponents(winSum);
		FormWinWindow->ShowModal();
		gameOver=true;
	} else{
		if(currentState < 5){
			FormLoseWindow->ShowModal();
		} else if(currentState < 10){
			FormWinWindow->initComponents(gameWinSums[4]);
			FormWinWindow->ShowModal();
		} else if(currentState < 15){
			FormWinWindow->initComponents(gameWinSums[9]);
			FormWinWindow->ShowModal();
		} else if(currentState == 15){
            FormWinWindow->initComponents(gameWinSums[14]);
			FormWinWindow->ShowModal();
		}
    }
}

void BaseGame::processRoomHelp(){
	int* answers = new int[4];
	if(currentState<5){
		answers[0]=rand()%10 + 85;
		answers[1]=rand()%(100-answers[0] + 1);
		answers[2]=rand()%(100-answers[0]-answers[1]+ 1);
		answers[3]=100-answers[0]-answers[1]-answers[2];
	} else if(currentState>=5 && currentState<10){
		answers[0]=rand()%10 + 45;
		answers[1]=rand()%(100-answers[0] + 1);
		answers[2]=rand()%(100-answers[0]-answers[1] + 1);
		answers[3]=100-answers[0]-answers[1]-answers[2];
	} else if(currentState>=10){
		answers[0]=rand()%10 + 20;
		answers[1]=rand()%(100-answers[0] + 1);
		answers[2]=rand()%(100-answers[0]-answers[1] + 1);
		answers[3]=100-answers[0]-answers[1]-answers[2] ;
	}
	answers = answerOrder->mixRoomAnswers(answers);
	FormHelpRoom->InitComponents(answers, getCurrentQuestion());
}

void BaseGame::processFriendHelp(){
	int c = rand() % 100;
	string* friendAnswer =new string();
	if(currentState<5){
		if(c < 85){
			*friendAnswer=questions[currentState].getRightAnswer();
		} else if((100 - c) % 3 == 0){
			*friendAnswer=questions[currentState].getAnswer2();
		} else if((100 - c) % 3 == 1){
			*friendAnswer=questions[currentState].getAnswer3();
		} else{
			*friendAnswer=questions[currentState].getAnswer4();
		}
	} else if(currentState>=5 && currentState<10){
		if(c < 55){
			*friendAnswer=questions[currentState].getRightAnswer();
		} else if((100 - c) % 3 == 0){
			*friendAnswer=questions[currentState].getAnswer2();
		} else if((100 - c) % 3 == 1){
			*friendAnswer=questions[currentState].getAnswer3();
		} else{
			*friendAnswer=questions[currentState].getAnswer4();
		}
	} else if(currentState>=10){
		if(c < 25){
			*friendAnswer=questions[currentState].getRightAnswer();
		} else if((100 - c) % 3 == 0){
			*friendAnswer=questions[currentState].getAnswer2();
		} else if((100 - c) % 3 == 1){
			*friendAnswer=questions[currentState].getAnswer3();
		} else{
			*friendAnswer=questions[currentState].getAnswer4();
		}
	}
	FormHelpPhone->InitComponents(friendAnswer);
}

void BaseGame::processFiftyFiftyHelp(){
   int c=rand()%10;
   if(c<3){
		questions[currentState].setAnswer2("");
		questions[currentState].setAnswer3("");
   } else if(c>=3 && c<=6){
		questions[currentState].setAnswer2("");
		questions[currentState].setAnswer4("");
   } else if(c>6){
		questions[currentState].setAnswer3("");
		questions[currentState].setAnswer4("");
   }
}

void BaseGame::getMoneyAndLeave(){
	if(gameOver){
		return;
	}
	processGameOver();
}

void BaseGame::getRoomHelp(){
	if(!isRoomHelpAvialable){
		return;
	}
	processRoomHelp();
	FormHelpRoom->ShowModal();
	isRoomHelpAvialable=false;
}

void BaseGame::getFriendHelp(){
	if(!isFriendAvialable){
		return;
	}
	processFriendHelp();
	FormHelpPhone->ShowModal();
	isFriendAvialable=false;
}

void BaseGame::getFiftyFiftyHelp(){
	if(!isFiftyFiftyAvialable){
		return;
	}
	processFiftyFiftyHelp();
	isFiftyFiftyAvialable=false;
}

string* BaseGame::getNextQuestion(){
	answerOrder = new AnswerOrder(true);
	currentState++;
	if(currentState == 15){
		processGameOver();
		gameOver=true;
	}
	string* nextQuestion = new string[5];
	nextQuestion[0] = questions[currentState].getQuestionText();
	for(int i = 0; i < 4; i++){
	   nextQuestion[i+1]=answerOrder->getAnswerByIndex(questions[currentState], i);
	}
	return nextQuestion;
}

string* BaseGame::getCurrentQuestion(){
	if(currentState >= 15){
		return NULL;
	}
	string* currentQuestion = new string[5];
	currentQuestion[0] = questions[currentState].getQuestionText();
	for(int i = 0; i < 4; i++){
	   currentQuestion[i+1]=answerOrder->getAnswerByIndex(questions[currentState], i);
	}
	return currentQuestion;
}

bool BaseGame::isAnswerTrue(int index){
	if(!answerOrder->isAnswerTrue(index)){
		gameOver=true;
		processGameOver();
	}
	return answerOrder->isAnswerTrue(index);
}

Presenter::Presenter(){
	 modelHandler = new ModelHandler();
	 gameLogic = new BaseGame(modelHandler->getGameSession());
}

bool Presenter::OnGameStartClick(){
	try{
		gameLogic = new BaseGame(modelHandler->getGameSession());
	 	return true;
    } catch(exception& e){
		return false;
	}
}

bool Presenter::OnAnswerClick(int answerIndex){
	return gameLogic->isAnswerTrue(answerIndex-1);
}

bool Presenter::OnHelpClick(int helpType){
	switch(helpType){
		case 1: gameLogic->getRoomHelp();
				break;
		case 2: gameLogic->getFriendHelp();
				break;
		case 3: gameLogic->getFiftyFiftyHelp();
				break;
		case 4: gameLogic->getMoneyAndLeave();
				break;
	}
}
