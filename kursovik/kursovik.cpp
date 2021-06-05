
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

	int i = 1;
	int ch = 0;
	try
	{
		while (true) {
			path p(my_path);
			if (exists(p))
			{
				//если файл, то предлагает его исполнить с дефолтной программой
				if (is_regular_file(p)) { 
					cout << p << " size is " << file_size(p) << '\n';
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
				else if (is_directory(p))
				{
					cout << p << " is a directory containing:\n";

					std::vector<path> v;
					v.push_back("..");

					GetConsoleScreenBufferInfo(h, &csbi);                        //обновление буфера, чтобы помещалось в консоль
					size_t rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1; 
					int l = 0;
					
					for (auto&& x : directory_iterator(p)) {             //заполнение вектора пут€ми
						if (l < rows - 3)
							v.push_back(x.path());
						l++;
					}
					std::sort(v.begin(), v.end());

					for (auto&& x : v) {

						if (i < 1)
							i = v.size();											//перемещение "стрелочки"
						else if (i > v.size())
							i = 1;
						if (i - 1 == (&x - &v[0])) {
							SetConsoleTextAttribute(h, (FOREGROUND_BLUE << 4));		//"стрелочка"
							cout << "    " << x.filename() << '\n';
							SetConsoleTextAttribute(h, wOldColor);
						}
						else
							cout << "    " << x.filename() << '\n';                 //вывод всех остальных, кроме стрелочки
						if (ch == 13) {
							if (i == 1) {
								if (my_path == "F:/")
									continue;										//если выбран .. (предыдуща€ директори€) открывает ее, 
								int start = my_path.find_last_of('/');				//удал€€ последний путь
								if (!my_path[start + 1]) my_path.replace(start, 1, "");
								start = my_path.find_last_of('/');
								my_path.replace(start + 1, 100, "");
								break;
							}
							else {
								my_path += v[i - 1].filename().string();            //иначе открывает директорию/файл
								if (is_directory(v[i - 1]))
									my_path += "/";
								i = 1;
								break;
							}
						}

					}
					while (l < rows-4) {
						std::cout << std::endl;										//дл€ вывода менюшки снизу
						l++;
					}
					if (l >= rows - 5) {
						SetConsoleTextAttribute(h, (FOREGROUND_BLUE << 4));
						cout << "1 - create file;     2 - create directory;    3 - delete file;      4 - delete empty directory;     5 - rename file/dir";
						SetConsoleTextAttribute(h, wOldColor);
					}
				}
				else
					cout << p << " exists, but is not a regular file or directory\n";
			}
			else {
				cout << p << " does not exist\n";									//в случае отсутстви€ пути выдает "ошибку" и предлагает вернутьс€
				cout << "Return to root?";
				char a;
				cin >> a;
				if (a == 'Y' || a == 'y')
					my_path = "F:/";
			}
			if (ch == 13) {
				ch++;
				system("cls");														//если был нажат ввод, собственно исполн€ет
				continue;
			}
			ch = _getch();
			if (ch == 224)															//управление вводом
				ch = _getch();
			if (ch == 72)
				i--;
			if (ch == 80)
				i++;
			if (ch == 49)
				create_file(my_path);
			if (ch == 50)
				create_dir(my_path);
			if (ch == 51)
				rm_file(my_path);
			if (ch == 52)
				rm_dir(my_path);
			if (ch == 53)
				my_rename(my_path);
			system("cls");
		}
	}

	catch (const filesystem_error& ex)
	{
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
	catch (const filesystem_error& ex)
	{
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