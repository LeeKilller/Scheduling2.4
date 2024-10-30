#pragma once
#include "BLIFParser.h"
#include <map>

#define ANDLIMIT 2
#define ORLIMIT 3
#define NOTLIMIT 1

enum TaskType
{
	AND,NOT,OR,INIT
};


struct Task {
	std::string idName;
	TaskType type;
	std::vector<std::string> nextTaskName;
	int restFrontTaskNum;
	bool isOutPut;
};

struct PrintScheuleStruct {
	int count;
	std::vector<std::string> ANDRuns;
	std::vector<std::string> ORRuns;
	std::vector<std::string> NOTRuns;
};


class ML_RCS
{
public:

	int Schedule(BLIFParser parser) {

		std::vector<std::string> Inputs = parser.GetInputs();
		std::vector<std::string> Outputs = parser.GetOutputs();
		
		std::vector<Gate> Gates = parser.GetGates();
		std::map<std::string, Task*> TaskMap;

		std::vector <Task*> DoQue;

		/*std::cout << "in"  ;*/
		

		/*Task *curTask = &Task();*/

		for (const auto input : Inputs) {
			Task *curTask = new Task();
			/*std::vector<std::string> curNextName;*/
			curTask->restFrontTaskNum = 0;
			curTask->isOutPut = false;
			curTask->type = INIT;
			std::string tStr = input.c_str();
			
			curTask->idName = tStr;
			
			/*free(tStr);*/
			/*curTask.nextTaskName = curNextName;*/
			TaskMap[input] = curTask;
			DoQue.push_back(curTask);
		}

		// 初始化map，初始化doque
		for (const auto& gate : Gates) {
			/*std::cout << "output " << gate.output << " ";*/
			for (const auto& input : gate.inputs) {
				// 如果input已经完成初始化了
				if (TaskMap.find(input) != TaskMap.end()) {
					Task* curTask = TaskMap[input];
					/*std::cout << input << " output " << gate.output << " ";*/
					curTask->nextTaskName.push_back(gate.output);
					/*std::cout << "name " << curTask->idName << " " 
						<< "nextSize " << curTask->nextTaskName.size() << " "
						<< "restnum " << curTask->restFrontTaskNum
						<< std::endl;*/
					continue;
				}
				// 应该不会有没完成初始化的input

				Task temp;
				Task* curTask = &temp;
				/*std::vector<std::string> curNextName;
				curTask.nextTaskName = curNextName;*/
				curTask->nextTaskName.push_back(gate.output);
				TaskMap[input] = curTask;
			}

			if (TaskMap.find(gate.output) == TaskMap.end()) {
				Task temp;
				Task* curTask = new Task();
				/*std::vector<std::string> curNextName;
				curTask.nextTaskName = curNextName;*/
				curTask->idName = (gate.output);
				curTask->type = getTaskType(gate.truth_table);
				curTask->restFrontTaskNum = gate.inputs.size();
				curTask->isOutPut = false;
				TaskMap[gate.output] = curTask;
				continue;
			}

			// 应该不会有完成初始话的output

			Task* curTask = TaskMap[gate.output];
			curTask->idName = gate.output;
			curTask->type = getTaskType(gate.truth_table);
			curTask->restFrontTaskNum = gate.inputs.size();
			curTask->isOutPut = false;
		}

		/*std::cout << std::endl;*/

		/*for (const auto& item : TaskMap) {
			std::cout << item.second->idName << " :" << item.second->nextTaskName.size() << " ";
			for (const auto& name : item.second->nextTaskName) {
				std::cout << name << " ";
			}
			std::cout << std::endl;
		}*/

		for (const auto& output : Outputs) {
			Task *curTask = TaskMap[output];
			curTask->isOutPut = true;
		}

		std::cout << "Inputs: ";
		for (const auto& input : Inputs) {
			std::cout << input << " ";
		}

		std::cout << "Outputs: ";
		for (const auto& output : Outputs) {
			std::cout << output << " ";
		}
		std::cout << std::endl;
		
		

		int OutPutNumber = Outputs.size();
		// 
		int count = 0;
		while (OutPutNumber != 0 && (DoQue.size() != 0 || ANDQue.size() + NOTQue.size() + ORQue.size() != 0)) {
			// 遍历就绪队列，并往三类任务队列中添加任务
			// 如何判断任务是否就绪？
			// 遍历就绪队列中的下一任务列表，给对应哈希值减一， 哈希值为零这添加进任务队列
			// 
			/*std::cout << "start ";
			std::cout << "count " << count << " ";
			std::cout << DoQue.size() << " d, ";
			std::cout << ANDQue.size() << " a, ";
			std::cout << NOTQue.size() << " n, ";
			std::cout << ORQue.size() << " o, ";
			std::cout << OutPutNumber << " on, " << std::endl;*/
			PrintScheuleStruct temp;

			PrintScheuleStruct* curPrint = new PrintScheuleStruct();
			PrintVec.push_back(curPrint);
			curPrint->count = count;
			for (const auto& doTask : DoQue) {
				for (const auto& next : doTask->nextTaskName) {
					/*std::cout << TaskMap[next]->restFrontTaskNum << " ";*/
					TaskMap[next]->restFrontTaskNum--;
					if (TaskMap[next]->restFrontTaskNum == 0) addToQue(TaskMap[next]);
				}
			}

			DoQue.clear();
			/*std::cout << "mid ";
			std::cout << DoQue.size() << " d, ";
			std::cout << ANDQue.size() << " a, ";
			std::cout << NOTQue.size() << " n, ";
			std::cout << ORQue.size() << " o, ";
			std::cout << OutPutNumber << " on, " << std::endl;*/


			// 从三类任务队列中取出限制数量内的任务，放入就绪队列（此时打印）
			// AND
			int andNum = 0;
			while (!ANDQue.empty() && andNum < ANDLIMIT) {
				Task* front = ANDQue.front();
				if (front->isOutPut) {
					OutPutNumber--;
					//if (OutPutNumber == 0) return 1;
				}
				DoQue.push_back(front);
				curPrint->ANDRuns.push_back(front->idName);
				ANDQue.erase(ANDQue.begin());
				andNum++;
			}

			// OR
			int orNum = 0;
			while (!ORQue.empty() && orNum < ORLIMIT) {
				Task* front = ORQue.front();
				if (front->isOutPut) {
					OutPutNumber--;
					//if (OutPutNumber == 0) return 1;
				}
				DoQue.push_back(front);
				curPrint->ORRuns.push_back((front->idName));
				ORQue.erase(ORQue.begin());
				orNum++;
			}


			// NOT
			int notNum = 0;
			while (!NOTQue.empty() && notNum < NOTLIMIT){
				Task* front = NOTQue.front();
				if (front->isOutPut) {
					OutPutNumber--;
					//if (OutPutNumber == 0) return 1;
				}
				DoQue.push_back(front);
				curPrint->NOTRuns.push_back((front->idName));
				NOTQue.erase(NOTQue.begin());
				notNum++;
			}

			

			count++;
			
			/*std::cout << "end ";
			std::cout << DoQue.size() << " d, ";
			std::cout << ANDQue.size() << " a, ";
			std::cout << NOTQue.size() << " n, ";
			std::cout << ORQue.size() << " o, ";
			std::cout << OutPutNumber << " on, " << std::endl;*/
		}


		return 0;
	}


	void Print() {
		std::cout<< "Total " << PrintVec.size() << " Cycles" << std::endl;


		for (const auto& printItem : PrintVec) {
			std::cout << "Cycle " << printItem->count << " :{ ";
			for (const auto& ANDItem : printItem->ANDRuns) {
				std::cout << ANDItem << " ";
			}

			std::cout << "},{ ";

			for (const auto& ORItem : printItem->ORRuns) {
				std::cout << ORItem << " ";
			}

			std::cout << "},{ ";

			for (const auto& NOTItem : printItem->NOTRuns) {
				std::cout << NOTItem << " ";
			}

			std::cout << "}" << std::endl;
		}

		std::cout << std::endl;
	}

	~ML_RCS() {};





private:
	std::vector<Task*> ANDQue;
	std::vector<Task*> NOTQue;
	std::vector<Task*> ORQue;
	std::vector<PrintScheuleStruct*> PrintVec;
	/*Task curTask;*/


	TaskType getTaskType(std::vector<std::string> truth_table) {
		if (truth_table.size() != 1) return OR;

		if (truth_table[0][0] == '0') return NOT;
		else return AND;
	}

	void addToQue(Task* curTask) {
		switch (curTask->type)
		{
		case OR:
			ORQue.push_back(curTask);
			break;
		case AND:
			ANDQue.push_back(curTask);
			break;
		case NOT:
			NOTQue.push_back(curTask);
			break;
		default:
			break;
		}
	}

};
