#include "Input.h"
#include "Utility.h"
#include <fstream> 

std::string Input::next_line()
{
    std::string line = "";
    std::getline(std::cin, line);
    return line;
}

void Input::parse_and_store(std::istream& input) {
    std::string line;
    std::vector<std::shared_ptr<Command>>* command_list = &start;
    bool parsing_world = false;
    bool asserts_reached = false;

    while (std::getline(input, line)) {
      

        if (line.empty()) {
            continue;
        }
        strip(line);

        if (line.rfind("+", 0) == 0) {
            // command block reached(e.g.: World, Infrastructure, etc...)
            std::string name = line.substr(1);
            parsing_world = false;
            if (name == Command::WORLD) {
                parsing_world = true;
            } else if (name == Command::START) {
                command_list = &start;
            } else if (name == Command::INPUT) {
                command_list = &steps;
            } else if (name == Command::ASSERTS) {
                asserts_reached = true;
            } else {
                throw std::runtime_error(
                    "Unknown Input Command found: " + name
                );
            }
        } else {
            if (asserts_reached) {
                asserts.push_back(line);
            } else if (parsing_world) {
                auto row = split(line);
                world->data.push_back(row);
                auto command = parse_command(line);
                command_list->push_back(command);
            } else {
                auto command = parse_command(line);
                command_list->push_back(command);
            }
        }
    }
}

std::shared_ptr<Command> Input::parse_command(std::string line)
{
    auto strings = split(line);
    std::shared_ptr<Command> command(new Command(strings[0]));
    command->arguments.insert(
        command->arguments.end(), strings.begin() + 1, strings.end()
    );
    return command;
}

