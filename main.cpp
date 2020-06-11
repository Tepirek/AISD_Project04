#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <limits.h>
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
void printString(char* string);
bool contains(vector<int> list, int value);
vector<worker*> countWorkers(vector<department*> listOfDepartments);
vector<int> countDays(vector<taskGroup*> listOfTaskGroups);
vector<int> countTimeSlots(vector<taskGroup*> listOfTaskGroups);
vector<task*> countTasks(vector<taskGroup*> listOfTaskGroups);
vector<int> createDeleteWorkers(vector<worker*> listOfWorkers, vector<worker*> workersToCheck);
vector<worker*> connectVectors(vector<worker*> v1, vector<worker*> v2);
vector<vector<worker*>> getMatrixOfCombinations(vector<department*> listOfDepartments);

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
	~Graph() {
		this->graph.clear();
		this->listOfDepartments.clear();
		this->listOfTaskGroups.clear();
		this->listOfWorkers.clear();
		this->listOfTasks.clear();
		this->listOfTimeSlots.clear();
	}
	void createEmptyGraph() {
		this->graph.clear();
		this->vertexCounter = 0;
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
	void connectSourceToWorkers() {
		for (int i = 0; i < this->numberOfWorkers; i++) {
			this->graph[this->vertexCounter][i + 1]->capacity = this->listOfWorkers[i]->totalHours;
		}
		this->vertexCounter++;
	}
	void connectWorkersToDays() {
		for (int i = 1; i <= this->numberOfWorkers; i++) {
			for (int j = 0; j < this->numberOfDays; j++) {
				this->graph[i][this->numberOfWorkers + j + i]->capacity = this->listOfWorkers[i - 1]->dailyHours;
			}
			this->vertexCounter++;
		}
	}
	void connectDaysToTimeSlots() {
		for (int i = 0; i < this->numberOfWorkers * this->numberOfDays; i++) {
			for (int j = 0; j < this->numberOfTimeSlots; j++) {
				this->graph[this->vertexCounter][this->vertexCounter + (this->numberOfWorkers * this->numberOfDays - i) + (i * this->numberOfTimeSlots) + j]->capacity = 1;
			}
			this->vertexCounter++;
		}
	}
	void connectTimeSlotsToTasks() {
		for (int i = 0; i < this->numberOfWorkers * this->numberOfDays * this->numberOfTimeSlots; i++) {
			for (int j = 0; j < this->numberOfTasks; j++) {
				if (listOfTimeSlots[i % this->numberOfTimeSlots] == listOfTasks[j]->timeSlot) {
					this->graph[this->vertexCounter][this->vertexCounter + (this->numberOfWorkers * this->numberOfDays * this->numberOfTimeSlots - i) + j]->capacity = 1;
				}
			}
			this->vertexCounter++;
		}
	}
	void connectTasksToTarget() {
		for (int i = 0; i < this->numberOfTasks; i++) {
			this->graph[this->vertexCounter][this->numberOfVertex - 1]->capacity = this->listOfTasks[i]->minHours;
			this->vertexCounter++;
		}
	}
	void connectTasksToTaskGroups() {
		this->vertexCounter -= this->numberOfTasks;
		for (int i = 0; i < this->numberOfTasks; i++) {
			for (int j = 0; j < this->numberOfTaskGroups; j++) {
				for (int k = 0; k < this->listOfTaskGroups[j]->listOfTasks.size(); k++) {
					if (this->listOfTaskGroups[j]->listOfTasks[k] == this->listOfTasks[i]) {
						this->graph[this->vertexCounter][this->vertexCounter + this->numberOfTasks - i + j]->capacity = this->listOfTasks[i]->maxHours - this->listOfTasks[i]->minHours;
					}
				}
			}
			this->vertexCounter++;
		}
	}
	void connectTaskGroupsToTarget() {
		for (int i = 0; i < this->numberOfTaskGroups; i++) {
			int sumOfTasks = 0;
			for (int j = 0; j < this->listOfTaskGroups[i]->listOfTasks.size(); j++) {
				sumOfTasks += this->listOfTaskGroups[i]->listOfTasks[j]->minHours;
			}
			this->graph[this->vertexCounter][this->numberOfVertex - 1]->capacity = this->listOfTaskGroups[i]->numberOfHours - sumOfTasks;
			this->vertexCounter++;
		}
	}
	void createGraph(vector<int> workersToDelete) {
		this->createEmptyGraph();
		this->connectSourceToWorkers();
		this->connectWorkersToDays();
		this->connectDaysToTimeSlots();
		this->connectTimeSlotsToTasks();
		this->connectTasksToTarget();
		this->connectTasksToTaskGroups();
		this->connectTaskGroupsToTarget();
		this->deleteWorkers(workersToDelete);
		// this->printGraphsFlow();
	}
	void printGraphsCapacity() {
		for (int i = 0; i < this->numberOfVertex; i++) {
			for (int j = 0; j < this->numberOfVertex; j++) {
				printf("%3d", this->graph[i][j]->capacity);
			}
			printf("\n");
		}
		printf("\n");
	}
	void printGraphsFlow() {
		for (int i = 0; i < this->numberOfVertex; i++) {
			for (int j = 0; j < this->numberOfVertex; j++) {
				printf("%3d", this->graph[i][j]->flow);
			}
			printf("\n");
		}
		printf("\n");
	}
	void printStatistics() {
		printf("Number of departments: %d\n", this->listOfDepartments.size());
		for (int i = 0; i < this->listOfDepartments.size(); i++) {
			printString(this->listOfDepartments[i]->name);
			printf("\t%d\n", this->listOfDepartments[i]->numberOfWorkers);
			for (int j = 0; j < this->listOfDepartments[i]->listOfWorkers.size(); j++) {
				printf("\tname: "); 
				printString(this->listOfDepartments[i]->listOfWorkers[j]->name);
				printf("\tvalue: %d\ttotal: %d\tdaily: %d\n", this->listOfDepartments[i]->listOfWorkers[j]->value, this->listOfDepartments[i]->listOfWorkers[j]->totalHours, this->listOfDepartments[i]->listOfWorkers[j]->dailyHours);
			}
		}
		printf("\nNumber of task groups: %d\n", this->numberOfTaskGroups);
		for (int i = 0; i < this->numberOfTaskGroups; i++) {
			printString(this->listOfTaskGroups[i]->name);
			printf("\t%d\n", this->listOfTaskGroups[i]->numberOfTasks);
			for (int j = 0; j < this->listOfTaskGroups[i]->listOfTasks.size(); j++) {
				printf("\t");
				printf("day: %d\ttimeslot: %d\tmin: %d\tmax: %d\n", this->listOfTaskGroups[i]->listOfTasks[j]->day, this->listOfTaskGroups[i]->listOfTasks[j]->timeSlot, this->listOfTaskGroups[i]->listOfTasks[j]->minHours, this->listOfTaskGroups[i]->listOfTasks[j]->maxHours);
			}
		}
	}
	void deleteWorkers(vector<int> workersToDelete) {
		for (int i = 0; i < this->numberOfWorkers; i++) {
			if (workersToDelete[i] == 0) {
				this->graph[0][i + 1]->capacity = 0;
			}
		}
	}
	int createCombinations() {
		int goal = 0;
		for (int i = 0; i < this->listOfTaskGroups.size(); i++) {
			goal += this->listOfTaskGroups[i]->numberOfHours;
		}
		int bestCost = INT_MAX, tmpCost = 0, maxFlow = 0;
		vector<worker*> tmpCombinationOfWorkers;
		vector<worker*> bestCombinationOfWorkers;
		vector<int> workersToDelete;
		for (int i = 0; i < this->listOfWorkers.size(); i++) {
			for (int j = 0; j < this->listOfWorkers.size() - i; j++) {
				for (int k = 0; k < i + 1; k++) {
					// printf("%d", this->listOfWorkers[k + j]->value);
					tmpCombinationOfWorkers.push_back(this->listOfWorkers[k + j]);
				}
				/*
				printf("\n");
				printf("LOW => ");
				for (int x = 0; x < combinationOfWorkers.size(); x++) {
					printf("%d ", combinationOfWorkers[x]);
				}
				printf("\n");
				*/
				tmpCost = 0;
				workersToDelete = createDeleteWorkers(this->listOfWorkers, tmpCombinationOfWorkers);
				this->createGraph(workersToDelete);
				for (int i = 0; i < tmpCombinationOfWorkers.size(); i++) {
					tmpCost += tmpCombinationOfWorkers[i]->value;
				}
				maxFlow = this->calculateMaxFlow();
				printf("goal = %d, maxFlow = %d, tmpCost = %d, minCost = %d\n", goal, maxFlow, tmpCost, bestCost);
				if (maxFlow == goal) {
					if (bestCost > tmpCost) {
						bestCost = tmpCost;
						bestCombinationOfWorkers = tmpCombinationOfWorkers;
					}
				}
				printf("goal = %d, maxFlow = %d, tmpCost = %d, minCost = %d\n", goal, maxFlow, tmpCost, bestCost);
				tmpCombinationOfWorkers.clear();
			}
		}
		return 0;
	}
	int calculateMaxFlow() {
		queue <int> queue;
		int* predecessors = new int[this->numberOfVertex];
		int* residualCapacities = new int[this->numberOfVertex];
		int source = 0;
		int target = this->numberOfVertex - 1;
		int maxFlow = 0;
		int residualCapacity = 0;
		int colVertex = 0;
		int rowVertex = 0;
		int i, j;
		bool escape;
		while (true) {
			for (int i = 0; i < this->numberOfVertex; i++) {
				predecessors[i] = -1;
			}
			predecessors[source] = -2;
			residualCapacities[source] = INT_MAX;
			while (!queue.empty()) {
				queue.pop();
			}
			queue.push(source);
			escape = false;
			while (!queue.empty()) {
				colVertex = queue.front();
				queue.pop();
				for (rowVertex = 0; rowVertex < this->numberOfVertex; rowVertex++) {
					residualCapacity = this->graph[colVertex][rowVertex]->capacity - this->graph[colVertex][rowVertex]->flow;
					if (residualCapacity && (predecessors[rowVertex] == -1)) {
						predecessors[rowVertex] = colVertex;
						residualCapacities[rowVertex] = residualCapacities[colVertex] > residualCapacity ? residualCapacity : residualCapacities[colVertex];
						if (rowVertex == target) {
							maxFlow += residualCapacities[target];
							i = rowVertex;
							while (i != source) {
								colVertex = predecessors[i];
								this->graph[colVertex][i]->flow += residualCapacities[target];
								this->graph[i][colVertex]->flow -= residualCapacities[target];
								i = colVertex;
							}
							escape = true; break;
						}
						queue.push(rowVertex);
					}
				}
				if (escape) break;
			}
			if (!escape) break;
		}
		/*
		cout << endl << "fmax = " << maxFlow << endl << endl;
		for (colVertex = 0; colVertex < this->numberOfVertex; colVertex++)
			for (rowVertex = 0; rowVertex < this->numberOfVertex; rowVertex++)
				if (this->graph[colVertex][rowVertex]->capacity) cout << colVertex << " -> " << rowVertex << " " << this->graph[colVertex][rowVertex]->flow << ":" << this->graph[colVertex][rowVertex]->capacity << endl;
		cout << endl;
		*/
		delete[] predecessors;
		delete[] residualCapacities;
		this->printGraphsFlow();
		return maxFlow;
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
	g->printStatistics();
	// g->createCombinations();
	g->~Graph();
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
	// while ((c == getchar()) == ' ' || (c == getchar()) == '\n');
	while ((c = getchar()) != ' ') {
		if (c == '\n') continue;
		// if (counter == stringSize) reallocateString();
		nameOfDepartment[counter] = c;
		counter++;
	}
	return nameOfDepartment;
}

void printString(char* string) {
	int counter = 0;
	while (string[counter] != '\0') {
		printf("%c", string[counter]);
		counter++;
	}
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

vector<int> createDeleteWorkers(vector<worker*> listOfWorkers, vector<worker*> workersToCheck) {
	vector<int> toDelete;
	for (int i = 0; i < listOfWorkers.size(); i++) {
		toDelete.push_back(0);
	}
	for (int i = 0; i < listOfWorkers.size(); i++) {
		for (int j = 0; j < workersToCheck.size(); j++) {
			if (listOfWorkers[i] == workersToCheck[j]) {
				toDelete[i] = 1;
			}
		}
	}
	/*
	for (int i = 0; i < toDelete.size(); i++) {
		printf("%d", toDelete[i]);
	}
	*/
	return toDelete;
}

vector<worker*> connectVectors(vector<worker*> v1, vector<worker*> v2) {
	vector<worker*> ret;
	for (int i = 0; i < v1.size(); i++) {
		ret.push_back(v1[i]);
	}
	for (int i = 0; i < v2.size(); i++) {
		ret.push_back(v2[i]);
	}
	return ret;
}

vector<vector<worker*>> getMatrixOfCombinations(vector<department*> listOfDepartments) {
	vector<vector<worker*>> test;
	for (int i = 0; i < listOfDepartments.size(); i++) {
		for (int j = 0; j < listOfDepartments[i]->numberOfWorkers; j++) {
			vector<worker*> tmp;
			for (int k = 0; k < j + 1; k++) {
				tmp.push_back(listOfDepartments[i]->listOfWorkers[k]);
			}
			test.push_back(tmp);
		}
	}

	for (int i = 0; i < listOfDepartments[0]->numberOfWorkers; i++) {
		for (int j = 0; j < listOfDepartments[1]->numberOfWorkers; j++) {
			test.push_back(connectVectors(test[i], test[j + listOfDepartments[0]->numberOfWorkers]));
		}
	}

	for (int i = 0; i < test.size(); i++) {
		for (int j = 0; j < test[i].size(); j++) {
			printf("%d ", test[i][j]->value);
		}
		printf("\n");
	}
	return test;
}