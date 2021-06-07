
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

void disclaimer();

std::wstring s2ws(const std::string& s)                              //ïåðåâîä èç string â wstring
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
	setlocale(0, "");
	disclaimer();

	vector<string> v;

	working_function(v);

	return 0;
}


void working_function(vector<string>& vector) {

	

	std::string my_path = "C:/";

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
	try {
		while (true) {
			path paths(my_path);
			string chosen_file;
			if (exists(paths)) {
				
				if (is_regular_file(paths)) {
					cout << paths << " size is " << file_size(paths) << '\n';
					cout << "Do you want to open it? Y/N" << '\n';
					char ans;
					std::cin >> ans;
					wstring tmp = s2ws(my_path);                          
					LPCWSTR yot = tmp.c_str();							  
					if (ans == 'Y' || ans == 'y') {
						ShellExecute(0, 0, yot, 0, 0, SW_SHOW);			  
					}
					int start = my_path.find_last_of('/');
					my_path.replace(start + 1, 100, "");				  
					cout << "Press any arrow to continue";
					_getch();
				}
				
				else if (is_directory(paths))
				{
					cout << paths << " is a directory containing:\n";

					std::vector<path> all_paths;
					all_paths.push_back("..");

					GetConsoleScreenBufferInfo(h, &csbi);                        
					size_t rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
					int line_in_console = 0;
					all_paths.reserve(rows - 2);

					for (auto&& element : directory_iterator(paths)) {             
						if (line_in_console < rows - 3)
							all_paths.push_back(element.path());
						line_in_console++;
					}
					std::sort(all_paths.begin(), all_paths.end());

					for (auto&& element : all_paths) {

						arrow = ((arrow + all_paths.size()) % all_paths.size());

						if (arrow == (&element - &all_paths[0])) {
							SetConsoleTextAttribute(h, (FOREGROUND_BLUE << 4));		
							cout << "    " << element.filename() << '\n';
							chosen_file = element.filename().string();
							SetConsoleTextAttribute(h, wOldColor);
						}
						else
							cout << "    " << element.filename() << '\n';                 
						if (ch == 13) {
							if (arrow == 0) {
								if (my_path == "C:/")
									continue;										
								int start = my_path.find_last_of('/');				
								if (!my_path[start + 1]) my_path.replace(start, 1, "");
								start = my_path.find_last_of('/');
								my_path.replace(start + 1, 100, "");
								break;
							}
							else {
								my_path += all_paths[arrow].filename().string();           
								if (is_directory(all_paths[arrow]))
									my_path += "/";
								arrow = 0;
								break;
							}
						}

					}
					while (line_in_console < rows - 4) {
						std::cout << std::endl;										
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
				cout << paths << " does not exist\n";									
				cout << "Return to root?";
				char a;
				cin >> a;
				if (a == 'Y' || a == 'y')
					my_path = "C:/";
			}
			if (ch == 13) {
				ch++;
				std::system("cls");														
				continue;
			}
			ch = _getch();                                    //input handling
			if (ch == 224)
				ch = _getch();
			switch (ch) {
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

	catch (const filesystem_error& ex) {
		cout << ex.what() << '\n';								
	}

	return;
}

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
		std::system("cls");
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
	catch (const filesystem_error& ex) {
		std::system("cls");
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
	try {
		string temp = my_path + chosen_path;
		remove(path(temp));
		return;
	}
	catch (const filesystem_error& ex) {
		std::system("cls");
		cout << "You can only delete empty folders" << '\n';
		_getch();
	}
}


void disclaimer() {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColor;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(h, &csbi);
	size_t middle = (csbi.srWindow.Right - csbi.srWindow.Left + 1)/2;
	for (int i = 0; i < middle - 5; i++)
		cout << ' ';
	wOldColor = csbi.wAttributes;

	SetConsoleTextAttribute(h, (FOREGROUND_RED) << 4);
	std::cout << "ВНИМАНИЕ!!!\n";
	SetConsoleTextAttribute(h, wOldColor);
	std::cout << "		Запуская эту программу вы полностью согласны, с тем, \
что если какой-либо файл будет случайно\nбезвозвратно удален вашими неосторожными действиями \
всю вину вы берете на себя, \nв связи с этим рекоммендуется прочтение предостережения и правил использования!\
\n\n\n 1.Все файлы удаляются безвозвратно (не в корзину летят)!!!\n\
2. Создание/удаление файлов возможно только без пробелов.\n\
3. Все файлы открываются дефолтными приложениями, что может занять время + постоянно грузятся символы (в vs)\n\
4. При быстром удалении не будет вопроса удалить ли файл, он просто будет удален моментально.\n\n\n\n\n";
	for (int i = 0; i < middle - 5; i++)
		cout << ' ';
	SetConsoleTextAttribute(h, (FOREGROUND_RED) << 4);
	std::cout << "WARNING!!!\n";
	SetConsoleTextAttribute(h, wOldColor);
	std::cout << "		By launching this program you agree that in case of accidental removal of any file it is\n\
completely your fault and there is no way to restore it. So it is highly recommended to first read the rules\n\n\n\n\
1. All files are removed forever(they don't go to bin)\n\
2. Creation/deletion of entities is only possible without spaces.\n\
3. All files are opened by the default app so it may take up some time before it's loaded.\n\
4. If deleting quickly there will not be any confirmation message so it will be deleted instanteniously.\n\n\n\n\n\n\n\n\n\
Нажмите любые кнопки 2 раза (или стрелочку 1 раз).\n\
Press any buttons twice (or arrow once).";

	_getch();
	_getch();
}