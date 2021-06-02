
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <boost/filesystem.hpp>
#include <Windows.h>
#include <conio.h>
using std::cout;
using namespace boost::filesystem;


void working_function();

int main(int argc, char* argv[])
{
	working_function();

	return 0;
}


void working_function() {

	std::string my_path = "F:/";

	//std::cin >> my_path;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColor;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(h, &csbi);
	wOldColor = csbi.wAttributes;
	system("cls");

	std::string last_path;

	int i = 1;
	int ch = 0;
	try
	{
		while (true) {
			path p(my_path);
			if (exists(p))
			{
				if (is_regular_file(p)) {
					cout << p << " size is " << file_size(p) << '\n';
					cout << "Do you want to open it? Y/N" << '\n';
					char ans;
					std::cin >> ans;
					if (ans == 'Y' || ans == 'y') {
						system(my_path.c_str());
					}
					int start = my_path.find_last_of('/');
					my_path.replace(start + 1, last_path.size(), "");
				}
				else if (is_directory(p))
				{
					cout << p << " is a directory containing:\n";

					std::vector<path> v;
					v.push_back("..");
					for (auto&& x : directory_iterator(p))
						v.push_back(x.path());

					std::sort(v.begin(), v.end());

					for (auto&& x : v) {
						if (i < 1)
							i = v.size()+1;
						if (i == v.size()+1)
							i = 1;
						if (i - 1 == (&x - &v[0])) {
							SetConsoleTextAttribute(h, (FOREGROUND_BLUE << 4));
							cout << "    " << x.filename() << '\n';
							SetConsoleTextAttribute(h, wOldColor);
						}
						else
							cout << "    " << x.filename() << '\n';
						if (ch == 13) {
							last_path = x.filename().string();
							if (i == 1) {
								int start = my_path.find_last_of('/');
								my_path.replace(start + 1, last_path.size(), "");
								break;
							}
							else {
								my_path += v[i - 1].filename().string();
								if (is_directory(v[i - 1]))
									my_path += "/";
								i = 1;
								break;
							}
						}
					}

				}
				else
					cout << p << " exists, but is not a regular file or directory\n";
			}
			else
				cout << p << " does not exist\n";
			if (ch == 13) {
				ch++;
				system("cls");
				continue;
			}
			ch = _getch();
			if (ch == 224)
				ch = _getch();
			if (ch == 72)
				i--;
			if (ch == 80)
				i++;
			system("cls");
		}
	}

	catch (const filesystem_error& ex)
	{
		cout << ex.what() << '\n';
	}

	return;
}
