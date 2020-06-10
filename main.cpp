#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct task {
	int day;
	int timeSlot;
	int minHours;
	int maxHours;
};

struct taskGroup {
	char* name;
	int numberOfTasks;
	int numberOfHours;
	vector<task*> listOfTasks;
};

struct worker {
	char* name;
	int value;
	int totalHours;
	int dailyHours;
	vector<task*> tasksPerformed;
};

struct department {
	char* name;
	int numberOfWorkers;
	vector<worker*> listOfWorkers;
};

struct edge {
	int data;
	bool active;
	int flow;
	int capacity;
};

char* readString();
bool contains(vector<int> list, int value);
vector<worker*> countWorkers(vector<department*> listOfDepartments);
vector<int> countDays(vector<taskGroup*> listOfTaskGroups);
vector<int> countTimeSlots(vector<taskGroup*> listOfTaskGroups);
vector<task*> countTasks(vector<taskGroup*> listOfTaskGroups);
int countTaskGroups(vector<taskGroup*> listOfTaskGroups);

class Graph {
	vector<vector<edge*>> graph;
	vector<department*> listOfDepartments;
	vector<taskGroup*> listOfTaskGroups;
	vector<worker*> listOfWorkers;
	vector<task*> listOfTasks;
	vector<int> listOfTimeSlots;
	int numberOfWorkers, numberOfDays, numberOfTaskGroups, numberOfTimeSlots, numberOfTasks, numberOfVertex, vertexCounter;
public:
	Graph(vector<department*> listOfDepartments, vector<taskGroup*> listOfTaskGroups) {
		this->listOfDepartments = listOfDepartments;
		this->listOfTaskGroups = listOfTaskGroups;
		this->listOfWorkers = countWorkers(this->listOfDepartments);
		this->listOfTasks = countTasks(this->listOfTaskGroups);
		this->listOfTimeSlots = countTimeSlots(this->listOfTaskGroups);
		this->numberOfWorkers = this->listOfWorkers.size();
		this->numberOfDays = countDays(this->listOfTaskGroups).size();
		this->numberOfTaskGroups = this->listOfTaskGroups.size();
		this->numberOfTimeSlots = this->listOfTimeSlots.size();
		this->numberOfTasks = this->listOfTasks.size();
		this->numberOfVertex = 2 + this->numberOfWorkers + (this->numberOfWorkers * this->numberOfDays) + (this->numberOfWorkers * this->numberOfDays * this->numberOfTimeSlots) + this->numberOfTasks + this->numberOfTaskGroups;
		this->vertexCounter = 0;
	}
	~Graph() = default;
	void createEmptyGraph() {
		for (int i = 0; i < numberOfVertex; i++) {
			vector<edge*> v;
			for (int j = 0; j < numberOfVertex; j++) {
				edge* e = new edge;
				e->data = -1;
				e->active = true;
				e->capacity = 0;
				e->flow = 0;
				v.push_back(e);
			}
			this->graph.push_back(v);
		}
	}
	void createGraph() {
		vector<vector<edge*>> graph;
		int sum = 2;
		vector<worker*> listOfWorkers = countWorkers(listOfDepartments);
		int w = listOfWorkers.size();
		int d = countDays(listOfTaskGroups).size();
		vector<int> listOfTimeSlots = countTimeSlots(listOfTaskGroups);
		vector<task*> listOfTasks = countTasks(listOfTaskGroups);
		int g = countTaskGroups(listOfTaskGroups);
		sum += w + (w * d) + (w * d * listOfTimeSlots.size()) + listOfTasks.size() + g;
		int numberOfVertex = sum;



		this->createEmptyGraph();


		graph = this->graph;

		int vertexCounter = 0;


		// po³¹czenie source z pracownikami
		for (int i = 0; i < listOfWorkers.size(); i++) {
			graph[vertexCounter][i + 1]->flow = listOfWorkers[i]->totalHours;
			// graph[vertexCounter][i + 1]->flow = 1;
		}
		vertexCounter++;



		// po³¹czenie pracowników z dniami
		for (int i = 1; i <= w; i++) {
			for (int j = 0; j < d; j++) {
				graph[i][w + j + i]->flow = listOfWorkers[i - 1]->dailyHours;
				// graph[vertexCounter][w + j + i]->flow = 1;
			}
			vertexCounter++;
		}



		// po³¹czenie dni z czasem
		for (int i = 0; i < w * d; i++) {
			for (int j = 0; j < listOfTimeSlots.size(); j++) {
				graph[vertexCounter][vertexCounter + (w * d - i) + (i * listOfTimeSlots.size()) + j]->flow = 1;
			}
			vertexCounter++;
		}



		// po³¹czenie czasu z zadaniami
		for (int i = 0; i < w * d * listOfTimeSlots.size(); i++) {
			for (int j = 0; j < listOfTasks.size(); j++) {
				if (listOfTimeSlots[i % listOfTimeSlots.size()] == listOfTasks[j]->timeSlot) {
					graph[vertexCounter][vertexCounter + (w * d * listOfTimeSlots.size() - i) + j]->flow = 1;
				}
			}
			vertexCounter++;
		}



		// po³¹czenie zadañ z ujœciem
		for (int i = 0; i < listOfTasks.size(); i++) {
			graph[vertexCounter][numberOfVertex - 1]->flow = listOfTasks[i]->minHours;
			vertexCounter++;
		}



		// po³¹czenie zadañ z grupami
		vertexCounter -= listOfTasks.size();
		for (int i = 0; i < listOfTasks.size(); i++) {
			for (int j = 0; j < listOfTaskGroups.size(); j++) {
				for (int k = 0; k < listOfTaskGroups[j]->listOfTasks.size(); k++) {
					if (listOfTaskGroups[j]->listOfTasks[k] == listOfTasks[i]) {
						graph[vertexCounter][vertexCounter + listOfTasks.size() - i + j]->flow = listOfTasks[i]->maxHours - listOfTasks[i]->minHours;
					}
				}
			}
			vertexCounter++;
		}
		// po³¹czenie grup z ujœciem
		for (int i = 0; i < listOfTaskGroups.size(); i++) {
			int sumOfTasks = 0;
			for (int j = 0; j < listOfTaskGroups[i]->listOfTasks.size(); j++) {
				sumOfTasks += listOfTaskGroups[i]->listOfTasks[j]->minHours;
			}
			graph[vertexCounter][numberOfVertex - 1]->flow = listOfTaskGroups[i]->numberOfHours - sumOfTasks;
			// graph[vertexCounter][numberOfVertex - 1]->flow = -1;
			vertexCounter++;
		}

		this->graph = graph;
		this->printGraph();
	}
	void printGraph() {
		// TEST START
		printf("%d\n", this->vertexCounter);
		for (int i = 0; i < this->numberOfVertex; i++) {
			for (int j = 0; j < this->numberOfVertex; j++) {
				printf("%d  ", this->graph[i][j]->flow);
			}
			printf("\n");
		}
		printf("\n");
		// TEST END
	}
};

int main(void) {
	// tryb szeregowania i tryb wyjœcia
	int schedulingMode = 0, outputMode = 0;
	scanf_s("%d %d", &schedulingMode, &outputMode);
	// iloœæ katedr
	int numberOfDepartments = 0;
	scanf_s("%d", &numberOfDepartments);
	vector<department*> listOfDepartments;
	// tworzenie katedr
	for (int i = 0; i < numberOfDepartments; i++) {
		department* d = new department;
		d->name = readString();
		scanf_s("%d", &d->numberOfWorkers);
		listOfDepartments.push_back(d);
		// tworzenie pracowników dla ka¿dej katedry
		for (int j = 0; j < d->numberOfWorkers; j++) {
			worker* w = new worker;
			w->name = readString();
			scanf_s("%d %d %d", &w->value, &w->totalHours, &w->dailyHours);
			listOfDepartments.back()->listOfWorkers.push_back(w);
		}
	}
	// iloœæ grup
	int numberOfGroups = 0;
	scanf_s("%d", &numberOfGroups);
	vector<taskGroup*> listOfTaskGroups;
	// tworzenie grup zadañ
	for (int i = 0; i < numberOfGroups; i++) {
		taskGroup* tg = new taskGroup;
		tg->name = readString();
		scanf_s("%d %d", &tg->numberOfTasks, &tg->numberOfHours);
		listOfTaskGroups.push_back(tg);
		// tworzenie zadañ dla ka¿dej grupy zadañ
		for (int j = 0; j < tg->numberOfTasks; j++) {
			task* t = new task;
			scanf_s("%d %d %d %d", &t->day, &t->timeSlot, &t->minHours, &t->maxHours);
			listOfTaskGroups.back()->listOfTasks.push_back(t);
		}
	}
	Graph* g = new Graph(listOfDepartments, listOfTaskGroups);
	g->createGraph();

	return 0;
}

char* readString() {
	int stringSize = 16;
	char* nameOfDepartment = new char[stringSize];
	for (int i = 0; i < stringSize; i++) {
		nameOfDepartment[i] = '\0';
	}
	char c = '\0';
	int counter = 0;
	// czyszczenie buffora bo testy s¹ wredne i maj¹ dodatkowe spacje...
	while ((c == getchar()) == ' ' || (c == getchar()) == '\n');
	while ((c = getchar()) != ' ') {
		if (c == '\n') continue;
		// if (counter == stringSize) reallocateString();
		nameOfDepartment[counter] = c;
		counter++;
	}
	return nameOfDepartment;
}

bool contains(vector<int> list, int value) {
	for (int i = 0; i < list.size(); i++) {
		if (list[i] == value) return true;
	}
	return false;
}

vector<worker*> countWorkers(vector<department*> listOfDepartments) {
	vector<worker*> listOfWorkers;
	for (int i = 0; i < listOfDepartments.size(); i++) {
		for (int j = 0; j < listOfDepartments[i]->listOfWorkers.size(); j++) {
			listOfWorkers.push_back(listOfDepartments[i]->listOfWorkers[j]);
		}
	}
	return listOfWorkers;
}

vector<int> countDays(vector<taskGroup*> listOfTaskGroups) {
	vector<int> listOfDays;
	for (int i = 0; i < listOfTaskGroups.size(); i++) {
		for (int j = 0; j < listOfTaskGroups[i]->listOfTasks.size(); j++) {
			if (!contains(listOfDays, listOfTaskGroups[i]->listOfTasks[j]->day)) {
				listOfDays.push_back(listOfTaskGroups[i]->listOfTasks[j]->day);
			}
		}
	}
	return listOfDays;
}

vector<int> countTimeSlots(vector<taskGroup*> listOfTaskGroups) {
	vector<int> listOfTimeSlots;
	for (int i = 0; i < listOfTaskGroups.size(); i++) {
		for (int j = 0; j < listOfTaskGroups[i]->listOfTasks.size(); j++) {
			if (!contains(listOfTimeSlots, listOfTaskGroups[i]->listOfTasks[j]->timeSlot)) {
				listOfTimeSlots.push_back(listOfTaskGroups[i]->listOfTasks[j]->timeSlot);
			}
		}
	}
	return listOfTimeSlots;
}

vector<task*> countTasks(vector<taskGroup*> listOfTaskGroups) {
	vector<task*> listOfTasks;
	for (int i = 0; i < listOfTaskGroups.size(); i++) {
		for (int j = 0; j < listOfTaskGroups[i]->listOfTasks.size(); j++) {
			listOfTasks.push_back(listOfTaskGroups[i]->listOfTasks[j]);
		}
	}
	return listOfTasks;
}

int countTaskGroups(vector<taskGroup*> listOfTaskGroups) {
	return listOfTaskGroups.size();
}