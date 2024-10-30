#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <sstream>

struct Gate
{
    std::vector<std::string> inputs;//����
    std::string output;//���
    std::vector<std::string> truth_table;
};

class BLIFParser
{
public:


    bool Parse(const std::string& fileName) {
        std::fstream file(fileName);
        if (!file.is_open()) {
            std::cerr << "Failed to open the file:" << fileName << std::endl;
            return false;
        }

        initVarities();

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;//�����հ��к�ע����
            if (line.find(".model") == 0) current_model = Trim(line.substr(6));
            else if (line.find(".inputs")==0) ParseInputs(line);
            else if (line.find(".outputs") == 0) ParseOutputs(line);
            else if (line.find(".names") == 0) ParseGateDefinition(line);
            else if (line.find(".end") == 0) break;
            else {
                //��ֵ�����
                if (!current_gate.output.empty()) {
                    current_gate.truth_table.push_back(line);
                }
            }
        }
        gates.push_back(current_gate);
        return true;
    }
    void Print() const {
        std::cout << "Model: " << current_model << std::endl;
       /* std::cout << "Inputs: ";
        for (const auto& input : inputs) {
            std::cout << input << " ";
        }
        std::cout << std::endl;

        std::cout << "Outputs: ";
        for (const auto& output : outputs) {
            std::cout << output << " ";
        }
        std::cout << std::endl;

        std::cout << "Gates:" << std::endl;
        for (const auto& gate : gates) {
            std::cout << "  Inputs: ";
            for (const auto& input : gate.inputs) {
                std::cout << input << " ";
            }
            std::cout << ", Output: " << gate.output << std::endl;

            std::cout << "  Truth Table:" << std::endl;
            for (const auto& row : gate.truth_table) {
                std::cout << "    " << row << std::endl;
            }
        }*/
    }
    std::vector<std::string> GetInputs() {
        if (inputs.size() <= 0) std::cerr << "Inputs contains nothing." << std::endl;
        return inputs;
    }
    std::vector<std::string> GetOutputs() {
        if (outputs.size() <= 0) std::cerr << "Outputs contains nothing." << std::endl;
        return outputs;
    }
    std::vector<Gate> GetGates() {
        if (gates.size() <= 0)std::cerr << "Gates contains nothing." << std::endl;
        return gates;
    }
    ~BLIFParser() {};

private:
    std::string current_model;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<Gate> gates;
    Gate current_gate;


    //ȥ���ո�
    std::string Trim(const std::string& str) {
        if (str.empty()) {
            return str;
        }

        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) {
            return ""; // ȫ���ǿո�
        }

        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    void ParseInputs(const std::string& line) {
        std::istringstream iss(line.substr(8)); // �ӵ� 9 ���ַ���ʼ��ȡ���ַ���
        std::string input;
        while (iss >> input) { //�����ȡ����
            inputs.push_back(input); //�������ź�������ӵ� inputs ������
        }
    }

    void ParseOutputs(const std::string& line) {
        std::istringstream iss(line.substr(9)); //�ӵ� 10 ���ַ���ʼ��ȡ���ַ���
        std::string output;
        while (iss >> output) { //ͬ��
            outputs.push_back(output); //������ź�������ӵ� outputs ������
        }
    }

    void ParseGateDefinition(const std::string& line) {
        if (!current_gate.output.empty()) {
            gates.push_back(current_gate);
        }

        current_gate = Gate();
        std::istringstream iss(line.substr(6)); //�ӵ� 7 ���ַ���ʼ��ȡ���ַ���
        std::string signal;
        while (iss >> signal) {
            current_gate.inputs.push_back(signal);
        }
        current_gate.output = current_gate.inputs.back(); //���һ���ź������
        current_gate.inputs.pop_back(); // ��inputs ���Ƴ����
    }

    void initVarities()
    {
        inputs.clear();
        outputs.clear();
        gates.clear();
    }
    

};