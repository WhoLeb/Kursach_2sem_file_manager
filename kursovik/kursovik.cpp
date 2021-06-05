
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <boost/filesystem.hpp>
#include <Windows.h>
#include <conio.h>
#include <fstream>

using namespace std;
using namespace boost::filesystem;


void working_function(vector<string>& vector);
void create_file(string&);
void create_dir(string&);
void rm_file(string&);
void rm_dir(string&);
void my_rename(string&);
void rm_this(string&, string&);

std::wstring s2ws(const std::string& s)                              //перевод из string в wstring
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int main(int argc, char* argv[])
{

	vector<string> v;

	working_function(v);

	return 0;
}


void working_function(vector<string>& vector) {

	setlocale(0, "");

	std::string my_path = "F:/";

	//std::cin >> my_path;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColor;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(h, &csbi);

	wOldColor = csbi.wAttributes;
	system("cls");

	std::string prev_path;

	int arrow = 0;
	int ch = 0;
	try{
		while (true) {
			path paths(my_path);
			string chosen_file;
			if (exists(paths)){
				//если файл, то предлагает его исполнить с дефолтной программой
				if (is_regular_file(paths)) { 
					cout << paths << " size is " << file_size(paths) << '\n';
					cout << "Do you want to open it? Y/N" << '\n';
					char ans;
					std::cin >> ans;
					wstring tmp = s2ws(my_path);                          //дл€ того, чтобы можно было открывать файлы
					LPCWSTR yot = tmp.c_str();							  //с русскими названи€ми, скобочками и тд
					if (ans == 'Y' || ans == 'y') {
						ShellExecute(0, 0, yot, 0, 0, SW_SHOW);			  //исполнение файла
					}
					int start = my_path.find_last_of('/');
					my_path.replace(start + 1, 100, "");				  //возвращение к директории
					cout << "Press any key to continue";
				}
				//если директори€, то выводит ее + обработка ввода (стрелочки/кнопочки)
				else if (is_directory(paths))
				{
					cout << paths << " is a directory containing:\n";

					std::vector<path> all_paths;
					all_paths.push_back("..");

					GetConsoleScreenBufferInfo(h, &csbi);                        //обновление буфера, чтобы помещалось в консоль
					size_t rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1; 
					int line_in_console = 0;
					
					for (auto&& element : directory_iterator(paths)) {             //заполнение вектора пут€ми
						if (line_in_console < rows - 3)
							all_paths.push_back(element.path());
						line_in_console++;
					}
					std::sort(all_paths.begin(), all_paths.end());

					for (auto&& element : all_paths) {

						arrow = ((arrow + all_paths.size()) % all_paths.size());

						if (arrow == (&element - &all_paths[0])) {
							SetConsoleTextAttribute(h, (FOREGROUND_BLUE << 4));		//"стрелочка"
							cout << "    " << element.filename() << '\n';
							chosen_file = element.filename().string();
							SetConsoleTextAttribute(h, wOldColor);
						}
						else
							cout << "    " << element.filename() << '\n';                 //вывод всех остальных, кроме стрелочки
						if (ch == 13) {
							if (arrow == 0) {
								if (my_path == "F:/")
									continue;										//если выбран .. (предыдуща€ директори€) открывает ее, 
								int start = my_path.find_last_of('/');				//удал€€ последний путь
								if (!my_path[start + 1]) my_path.replace(start, 1, "");
								start = my_path.find_last_of('/');
								my_path.replace(start + 1, 100, "");
								break;
							}
							else {
								my_path += all_paths[arrow].filename().string();            //иначе открывает директорию/файл
								if (is_directory(all_paths[arrow]))
									my_path += "/";
								arrow = 0;
								break;
							}
						}

					}
					while (line_in_console < rows - 4) {
						std::cout << std::endl;										//дл€ вывода менюшки снизу
						line_in_console++;
					}
					if (line_in_console >= rows - 5) {
						SetConsoleTextAttribute(h, (FOREGROUND_BLUE << 4));
						cout << "1 - create file;     2 - create directory;    3 - delete file;     \
4 - delete empty directory;     5 - rename file/dir;    6 - delete highlighted file/empty dir";
						SetConsoleTextAttribute(h, wOldColor);
					}
				}
				else
					cout << paths << " exists, but is not a regular file or directory\n";
			}
			else {
				cout << paths << " does not exist\n";									//в случае отсутстви€ пути выдает "ошибку" и предлагает вернутьс€
				cout << "Return to root?";
				char a;
				cin >> a;
				if (a == 'Y' || a == 'y')
					my_path = "F:/";
			}
			if (ch == 13) {
				ch++;
				std::system("cls");														//если был нажат ввод, собственно исполн€ет
				continue;
			}
			ch = _getch();                                    //input handling
			if (ch == 224)
				ch = _getch();
			switch (ch){
			case 72:
				arrow--;
				break;
			case 80:
				arrow++;
				break;
			case '1':
				create_file(my_path);
				break;
			case '2':
				create_dir(my_path);
				break;
			case '3':
				rm_file(my_path);
				break;
			case '4':
				rm_dir(my_path);
				break;
			case '5':
				my_rename(my_path);
				break;
			case '6':
				rm_this(my_path, chosen_file);
				break;
			}
				
			std::system("cls");
		}
	}

	catch (const filesystem_error& ex){
		cout << ex.what() << '\n';								//дефолтна€ штука буста
	}

	return;
}
//дальше функции названием исчерпываютс€
void create_file(string& my_path) {
	system("cls");
	cout << "Type in the name of the file:" << endl;
	string file_name;
	cin >> file_name;
	std::ofstream(my_path + file_name);
}

void create_dir(string& my_path) {
	system("cls");
	cout << "Type in the name of the directory:" << endl;
	string dir_name;
	cin >> dir_name;
	string res_name = my_path + dir_name;
	path p(res_name);
	create_directory(p);
}

void rm_file(string& my_path) {
	try {
		system("cls");
		cout << "Type in the name of the file:" << endl;
		string file_name;
		cin >> file_name;
		string res_name = my_path + file_name;
		path p(res_name);
		remove(p);
	}
	catch (const filesystem_error& ex) {
		cout << "This function is for deleting files but well done you tried to break even it!" << '\n';
		_getch();
	}
}

void rm_dir(string& my_path) {
	try {
		system("cls");
		cout << "Type in the name of the directory:" << endl;
		string dir_name;
		cin >> dir_name;
		string res_name = my_path + dir_name;
		path p(res_name);
		remove(p);
	}
	catch (const filesystem_error& ex){
		cout << "You can only delete empty folders" << '\n';
		_getch();
	}
}

void my_rename(string& my_path) {
	system("cls");
	cout << "Type in the old name of the thing:" << endl;
	string old_dir_name;
	cin >> old_dir_name;
	string res_name = my_path + old_dir_name;
	path p(res_name);
	cout << std::endl << "Type in the new name of the thing:" << endl;
	string new_dir_name;
	cin >> new_dir_name;
	string res_name2 = my_path + new_dir_name;
	path p2(res_name2);
	rename(p, p2);
}

void rm_this(string& my_path, string& chosen_path) {
	try{
		string temp = my_path + chosen_path;
		remove(path(temp));
	}
	catch (const filesystem_error& ex){
		cout << "You can only delete empty folders" << '\n';
		_getch();
	}
}