#include "Commands.h"
#include <iostream>
#include <sstream>
float a, b, result;
char operation;
char choice;

namespace MagnoliaOS {
    void executeEcho(const std::string& args) {
        std::cout << args << std::endl;
    }

    void executeHelp() {
        std::cout << "Magnolia SUS-system - GNU bash emulation, version 1.0\n";
        std::cout << "These shell commands are defined internally. Type 'help' to see this list.\n\n";

        std::cout << "  echo [arg ...]     Prints the arguments to the standard output.\n";
        std::cout << "  help               Displays information about built-in commands.\n";
        std::cout << "  clear              Clears the terminal screen.\n";
        std::cout << "  exit               Exits the Magnolia SUS-system.\n";
		std::cout << "  calculator         Opens a simple calculator for basic arithmetic operations.\n";
    }
    void executeClear() {
        std::cout << "\033[2J\033[1;1H";
	}
	void calculator() {
		std::cout << "|---------------------|\n";
		std::cout << "| Magnolia Calculator |\n";
		std::cout << "|---------------------|\n";

		while (true) {
			std::cout << "Enter an expression (e.g., 2 + 2) or 'exit' to return: ";
			// Read the whole line so user can type 'exit' or a full expression
			std::string line;
			if (!std::getline(std::cin, line)) return; // EOF or error
			if (line == "exit") return;
			if (line.empty()) continue;

			std::stringstream ss(line);
			if (!(ss >> a >> operation >> b)) {
				std::cout << "Error: Failed to parse expression. Use format: <num> <op> <num>\n";
				continue;
			}

			if (operation == '+') {
				result = a + b;
			}
			else if (operation == '-') {
				result = a - b;
			}
			else if (operation == '*') {
				result = a * b;
			}
			else if (operation == '/') {
				if (b != 0) {
					result = a / b;
				}
				else {
					std::cout << "Error: Division by zero is not allowed.\n";
					continue;
				}
			}
			else {
				std::cout << "Error: Invalid operation. Please use +, -, *, or /.\n";
				continue;
			}

			std::cout << "Result: " << result << "\n";

			// Ask whether to continue
			std::string resp;
			while (true) {
				std::cout << "Do you want to perform another calculation? (y/n): ";
				if (!std::getline(std::cin, resp)) return;
				if (resp.empty()) continue;
				char c = resp[0];
				if (c == 'y' || c == 'Y' || c=='c' || c=='C' || c=='yes' || c=='Yes' || c =='YES') break; // continue outer loop
				if (c == 'n' || c == 'N' || c== 'no' || c=='NO' || c=='No') return; // exit calculator
				if (resp == "exit") return;
				std::cout << "Please enter 'y' or 'n'.\n";
			}
		}
	}
}