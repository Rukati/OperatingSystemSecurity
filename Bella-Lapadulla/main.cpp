#include <iostream>
#include "Class.h"
#include "ColorsText.h"


int start_panel()
{
	std::cout << BRIGHT_CYAN_COLOR;
	std::cout << "\n Select the command:\n";
	std::cout << " 1. read_subject_object\n";
	std::cout << " 2. write_subject_object\n";
	std::cout << " 3. exit\n";

	int choice;
	std::cout << " >> ";
	std::cout << RESET_COLOR;
	std::cin >> choice;

	return choice;
}

int main()
{
	setlocale(LC_ALL, "ru");
	enum Commands {
		read = 1,
		write = 2,
		exit = 3
	};

	while (true) {
		Access db;

		switch (start_panel())
		{

		case read:
		{
			std::string name_subject;
			std::cout << BRIGHT_MAGENTA_COLOR << "\nSubject >> " << RESET_COLOR;
			std::cin >> name_subject;
			
			std::string name_object;
			std::cout << BRIGHT_MAGENTA_COLOR << "\nObject >> " << RESET_COLOR;
			std::cin >> name_object;
			
			if (db.ChekSubj(name_subject))//проверка есть ли такие субьекты/обьекты
				if (db.ChekObj(name_object)) {
					db.AddRight(name_subject, name_object, "read");
					db.SaveToFile();
				}
			break;
		}


		case write:
		{
			std::string name_subject;
			std::cout << BRIGHT_MAGENTA_COLOR << "\nSubject >> " << RESET_COLOR;
			std::cin >> name_subject;

			std::string name_object;
			std::cout << BRIGHT_MAGENTA_COLOR << "\nObject >> " << RESET_COLOR;
			std::cin >> name_object;

			if (db.ChekSubj(name_subject))//проверка есть ли такие субьекты/обьекты
				if (db.ChekObj(name_object)) {
					db.AddRight(name_subject, name_object, "write");
					db.SaveToFile();
				}

			break;
		}


		case exit:
			return 1;


		default:
			break;


		}
	}


	return 0;
}