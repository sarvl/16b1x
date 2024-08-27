#include <fstream>
#include <iostream>

#include <string>
#include <vector>

struct Instruction{
	int type;
	int R0;
	int R1;
	int imm8;
	
	Instruction() {}
	Instruction(int nType, int nR0, int nR1, int nImm8)
		: type(nType), R0(nR0), R1(nR1), imm8(nImm8) {}
};
struct Label{
	std::string name;
	unsigned int instructionNum;

	Label(){}
	Label(const std::string& nName, unsigned int nInstructionNum)
		: name(nName), instructionNum(nInstructionNum) {} 
};

struct JmpToLabel{
	std::string labelName;
	unsigned int instructionNum;
	
	JmpToLabel(){}
	JmpToLabel(const std::string& nLabelName, unsigned int nInstructionNum)
		: labelName(nLabelName), instructionNum(nInstructionNum) {} 
};

int findValue(std::vector<Label>& v, const std::string& name)
{
	for(auto& e : v)
	{
		if(e.name == name) return static_cast<int>(e.instructionNum);
	}

	return -1;
}

void parseLine(std::vector<std::string>& vec, const std::string& line)
{
	vec.clear();

	std::string toInsert = "";
	for(auto& ch : line)
	{
		if(' ' == ch || '\t' == ch || '\n' == ch)
		{
			if(0 != toInsert.size())
			{
				vec.emplace_back(toInsert);
				toInsert.clear();
			}

			continue;
		}

		toInsert += ch;
	}
	if(0 != toInsert.size())
	{
		vec.emplace_back(toInsert);
		toInsert.clear();
	}

}

int hexToDec(const std::string& hex)
{
	int result = 0;
	for(unsigned int i = 2; i < hex.size(); i++)
	{
		int val = 0;
		if('F' >= hex[i] && 'A' <= hex[i])
			val = hex[i] - 'A' + 10;
		else if('9' >= hex[i] && '0' <= hex[i])
			val = hex[i] - '0';
		else
			return -1;

		result <<= 4;
		result += val;
	}

	return result;
}
int main(int argc, char* argv[])
{
	if(argc < 2) 
	{
		std::cout << "not enough arguments\n";
		return 1;
	}
	std::ifstream read(argv[1]);
	std::string line;

	if(!read.good())
	{
		std::cout << "this file does not exit\n";
		return 2;
	}

	std::vector<Instruction> instructions;
	std::vector<Label> labels;
	std::vector<JmpToLabel> tableToFill;

	std::vector<std::string> content;
	int lineNum = 0;
	while(std::getline(read, line))
	{
		lineNum++;

		parseLine(content, line);
		
		if(0 == content.size()) continue;

		if('#' == (content[0])[0]) continue;
	

		if("LABEL" == content[0])
		{
			if(2 != content.size())
			{
				std::cout << "\033[0;31mERROR: incorrect definition of label on line " << lineNum << " \033[0m\n"; 
				std::cout << "\tcorrect format is LABEL name\n";

				continue;
			}

			labels.emplace_back(content[1], instructions.size());
			
			continue;
		}
		if(3 != content.size())
		{
			std::cout << "\033[0;31mERROR: invalid instruction format on line " << lineNum << " \033[0m\n"; 
			std::cout << "\tcorrect format is INSTRUCTION R0 R1/imm8/labelName\n";

			continue;
		}
		else
		{
			//get R0
			int R0;
			switch((content[1])[0])
			{
			case 'Z':
				R0 = 0;
				break;
			case 'A':
				R0 = 1;
				break;
			case 'B':
				R0 = 2;
				break;
			case 'C':
				R0 = 3;
				break;
			case 'D':
				R0 = 4;
				break;
			case 'E':
				R0 = 5;
				break;
			case 'F':
				R0 = 6;
				break;
			case 'S':
				R0 = 7;
				break;
			default:
				std::cout << "\033[0;31mERROR: invalid register identifier on line " << lineNum << " \033[0m\n"; 
				std::cout << "\tcorrect registers: ZERO A B C D E F G SP\n";
				continue;
			}

			if("mvi" == content[0])
			{
				int imm8 = hexToDec(content[2]);
				if(-1 == imm8)
				{
					std::cout << "\033[0;31mERROR: invalid hex number on line " << lineNum << " \033[0m\n"; 
					continue;
				}

				instructions.emplace_back(1, R0, 0, imm8);
			
				continue;
			}
			else if("jnz" == content[0])
			{
				//it is marked to be resolved later
				instructions.emplace_back(12, R0, 0, 0);
				
				tableToFill.emplace_back(content[2], instructions.size() - 1);

				continue;
			}

			//get R0
			int R1;
			switch((content[2])[0])
			{
			case 'Z':
				R1 = 0;
				break;
			case 'A':
				R1 = 1;
				break;
			case 'B':
				R1 = 2;
				break;
			case 'C':
				R1 = 3;
				break;
			case 'D':
				R1 = 4;
				break;
			case 'E':
				R1 = 5;
				break;
			case 'F':
				R1 = 6;
				break;
			case 'S':
				R1 = 7;
				break;
			default:
				std::cout << "\033[0;31mERROR: invalid register identifier on line " << lineNum << " \033[0m\n"; 
				std::cout << "\tcorrect registers: ZERO A B C D E F G SP\n";
				continue;
			}
			

			if("mvr" == content[0])
			{
				instructions.emplace_back(0, R0, R1, 0);
				continue;
			}
			else if("lod" == content[0])
			{
				instructions.emplace_back(2, R0, R1, 0);
				continue;
			}
			else if("str" == content[0])
			{
				instructions.emplace_back(3, R0, R1, 0);
				continue;
			}
			else if("add" == content[0])
			{
				instructions.emplace_back(4, R0, R1, 0);
				continue;
			}
			else if("sub" == content[0])
			{
				instructions.emplace_back(5, R0, R1, 0);
				continue;
			}
			else if("and" == content[0])
			{
				instructions.emplace_back(6, R0, R1, 0);
				continue;
			}
			else if("or" == content[0])
			{
				instructions.emplace_back(7, R0, R1, 0);
				continue;
			}
			else if("xor" == content[0])
			{
				instructions.emplace_back(8, R0, R1, 0);
				continue;
			}
			else if("not" == content[0])
			{
				instructions.emplace_back(9, R0, R1, 0);
				continue;
			}
			else if("sll" == content[0])
			{
				instructions.emplace_back(10, R0, R1, 0);
				continue;
			}
			else if("srl" == content[0])
			{
				instructions.emplace_back(11, R0, R1, 0);
				continue;
			}
			else
			{
				std::cout << "\033[0;31mERROR: invalid instruction on line " << lineNum << " \033[0m\n"; 
				continue;

			}
		}
	}

	//replace labels with num
	for(auto& toFill : tableToFill)
	{
		int value = findValue(labels, toFill.labelName);

		if(-1 == value)
		{
			std::cout << "\033[0;31mERROR: LABEL " << toFill.labelName << " on line " << lineNum << " does not exist \033[0m\n"; 
			return 3;
		}
		instructions[toFill.instructionNum].imm8 = value;
	}

	std::ofstream write("output.bin");

	for(auto& ins : instructions)
	{
		long long value = 0;
		if(1 == ins.type || 12 == ins.type)
		{
			value = (ins.type << 12) + (ins.R0 << 9) + (ins.imm8 << 1);
		}
		else
		{
			value = (ins.type << 12) + (ins.R0 << 9) + (ins.R1 << 6);
		}
		write << std::hex << value << '\n';
	}

	return 0;
}
