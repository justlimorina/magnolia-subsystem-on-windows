#include "Commands.h"
#include <iostream>
#include <sstream>
#include "Evaluator.h"
#include "History.h"
#include <iomanip>
#include <algorithm>
#include <cctype>

static History g_history(200);
static int g_precision = 6;

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
			std::cout << "Enter an expression (e.g., 2 + 2), commands: 'history', '!n', 'prec N', or 'exit' to return:\n";
			std::string line;
			if (!std::getline(std::cin, line)) return;
			if (line.empty()) continue;
			// trim trailing and leading whitespace
			line.erase(line.find_last_not_of(" \t\n\r") + 1);
			size_t start = line.find_first_not_of(" \t\n\r");
			if (start != std::string::npos) line = line.substr(start);
			if (line == "exit") return;

			// history
			if (line == "history") {
				auto entries = g_history.list();
				if (entries.empty()) std::cout << "History is empty.\n";
				else {
					for (size_t i = 0; i < entries.size(); ++i) std::cout << (i+1) << ": " << entries[i] << "\n";
				}
				continue;
			}

			// recall !n
			if (!line.empty() && line[0] == '!') {
				try {
					int idx = std::stoi(line.substr(1));
					if (idx <= 0 || static_cast<size_t>(idx) > g_history.size()) { std::cout << "Invalid history index.\n"; continue; }
					line = g_history.get(static_cast<size_t>(idx-1));
					std::cout << "Recalling: " << line << "\n";
				} catch (...) { std::cout << "Invalid recall format. Use !n\n"; continue; }
			}

			// precision command
			{
				std::string tmp = line;
				std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
				if (tmp.rfind("prec ",0) == 0 || tmp.rfind("precision ",0) == 0) {
					size_t pos = tmp.find(' ');
					if (pos != std::string::npos) {
						std::string val = tmp.substr(pos+1);
						try { int p = std::stoi(val); if (p >= 0 && p <= 15) { g_precision = p; std::cout << "Precision set to " << g_precision << "\n"; } else std::cout << "Precision out of range (0-15).\n"; }
						catch (...) { std::cout << "Invalid precision value.\n"; }
					}
					continue;
				}
			}

			// evaluate
			double value = 0.0;
			std::string err;
			if (!evaluateExpression(line, value, err)) { std::cout << "Error: " << err << "\n"; continue; }
			g_history.add(line);
			std::cout << "Result: " << std::fixed << std::setprecision(g_precision) << value << "\n";

			// continue prompt
			std::string resp;
			while (true) {
				std::cout << "Do you want to perform another calculation? (y/n): ";
				if (!std::getline(std::cin, resp)) return;
				if (resp.empty()) continue;
				char c = resp[0];
				if (c == 'y' || c == 'Y') break;
				if (c == 'n' || c == 'N') return;
				if (resp == "exit") return;
				std::cout << "Please enter 'y' or 'n'.\n";
			}
		}
	}
}