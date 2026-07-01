#include <iostream>
#include <string>
#include <windows.h>
#include "Commands.h"

void enableANSIColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

int main() {
    enableANSIColors();

    std::string username = "admin";
    std::string hostname = "magnolia";
    std::string current_dir = "~";

    std::cout << "Starting Magnolia SUS-system...\n";
    std::cout << "Type 'exit' to shutdown.\n\n";

    while (true) {

        std::cout << "\033[1;32m" << username << "@" << hostname << "\033[0m:"
            << "\033[1;34m" << current_dir << "\033[0m$ ";

        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) continue;

        size_t space_pos = input.find(' ');
        std::string command = input.substr(0, space_pos);
        std::string args = "";

        if (space_pos != std::string::npos) {
            args = input.substr(space_pos + 1);
        }

        if (command == "exit") {
            std::cout << "Shutting down Magnolia SUS-system...\n";
            break;
        }
        else if (command == "echo") {
            MagnoliaOS::executeEcho(args);
        }
		else if (command == "help") {
            MagnoliaOS::executeHelp();
        }
        else if (command == "clear") {
            MagnoliaOS::executeClear();
		}
		else if (command == "calculator") {
			MagnoliaOS::calculator();
		}
        else {
            std::cout << command << ": command not found\n";
        }
    }

    return 0;
}