#include <iostream>   // Для работы с потоками ввода-вывода (cin, cout)
#include <fstream>    // Для работы с файлами (ifstream, ofstream)
#include <vector>     // Для использования динамических массивов (vector)
#include <array>      // Для использования массивов фиксированного размера (array)
#include <string>     // Для работы с строками (wstring)
#include <cstdlib>    // Для функций общего назначения (rand, srand)
#include <ctime>      // Для работы со временем (time)
#include <wchar.h>    // Для работы с широкими строками и символами (wchar_t)
#include <thread>     // Для управления потоками выполнения (this_thread::sleep_for)
#include <chrono>     // Для работы с временем и задержками (chrono::milliseconds)
#include <iomanip>    // Для настройки форматирования ввода-вывода (setprecision)
#include <set>        // Для работы с множествами уникальных элементов (set)
#include <windows.h>  // Для использования платформозависимых функций Windows (SetConsoleCP, SetConsoleOutputCP)
using namespace std;


class Game
{
private:
	wstring word{ L"" };					// слово, которое нужно отгадать
	wstring table{ L"" };					// таблица с отгаданными буквами
	size_t countError{ 0 };					// кол-во ошибок
	size_t err{ 6 };						// кол-во допустимых ошибок 
	set<wchar_t> letters;         			// массив введенных букв
	time_t start, end;						// подсчет времени на одну игру 
	size_t attempt{ 0 };					// кол-во попыток 

	// Глобальная статистика
	static size_t totalGames;				// Общее количество сыгранных игр
	static size_t totalWins;			    // Общее количество побед
	static size_t totalLosses;			    // Общее количество поражений
	static size_t totalErrors;				// Общее количество ошибок пользователя (неверных попыток угадать букву)
	static double totalTime;				// Общее время, затраченное на игры (в секундах)

	const array<wstring, 7> pictureArr		// массив с псевдографикой    
	{
		 L"\t___________________\n\t  ||     |     ||\n\t  ||     @     ||\n"
		  "\t  ||    |O|    ||\n"
		  "\t  ||     П     ||\n     _____||___________||_____\n",
		 L"\t___________________\n\t  ||     |     ||\n\t  ||     @     ||\n"
		  "\t  ||    /O\\    ||\n"
		  "\t  ||    _Л_    ||\n     _____||_____П_____||_____\n",
		 L"\t___________________\n\t  ||     |     ||\n\t  ||     Q     ||\n"
		  "\t  ||           ||\n"
		  "\t  ||    ___    ||\n     _____||_____П_____||_____\n",
		 L"\t___________________\n\t  ||           ||\n\t  ||           ||\n"
		  "\t  ||           ||\n"
		  "\t  ||    ___    ||\n     _____||_____П_____||_____\n",
		 L"\t___________________\n\t  ||           ||\n\t  ||           ||\n"
		  "\t  ||           ||\n"
		  "\t  ||           ||\n     _____||___________||_____\n",
		 L"\t  ||           ||\n\t  ||           ||\n\t  ||           ||\n"
		  "\t  ||           ||\n     _____||___________||_____\n",
		 L"\t  ||\n\t  ||\n\t  ||\n\t  ||\n     _____||__________________\n"
	};

public:
	Game()
	{
		playGame();
	};

	// Функция для получения рандомного слова из конфига
	wstring getRandomWord() {
		wifstream file(L"words.txt");
		file.imbue(locale("ru_RU.UTF-8"));

		vector<wstring> words;
		wstring word;

		while (getline(file, word)) {
			if (!word.empty()) {
				words.push_back(word);
			}
		}
		file.close();

		// Проверяем, есть ли слова в массиве
		if (words.empty()) {
			wcerr << L"Ошибка: Файл words.txt пуст или недоступен.\n";
			exit(1); 
		}

		int randomIndex = rand() % words.size();
		return words[randomIndex];
	}

	//  Метод для отрисовки линии в таблице
	void drawLine(int n) {
		for (int i = 0; i < n; i++) {
			wcout << "+---";
		}
		wcout << "+\n";
	}

	// Метод для отрисовки таблицы с отгаданными буквами в слове
	void showTable(wstring word) {
		drawLine(word.size());
		wcout << "| ";
		for (int i = 0; i < word.size(); i++) {
			wcout << word[i] << " | ";
		}
		wcout << "\n";
		drawLine(word.size());
	}

	// Функция для изменения слова в таблице
	void checkLetter(wchar_t letter) {
		bool found = false;
		for (size_t i = 0; i < word.size(); ++i) {
			if (word[i] == letter) {
				table[i] = letter;
				found = true;
			}
		}

		if (!found) {
			wcout << L"Неправильная буква!\n";
			countError++;
			err--;
		}
		else {
			wcout << L"Правильная буква!\n";
		}
	}


	// Проверка на ввод буквы
	wchar_t getValidLetter() {
		wchar_t checkLetter = 'a';
		while (true) {
			wcout << L"Введите букву: ";
			wcin >> checkLetter;
			wcin.ignore(32767, '\n');

			checkLetter = towupper(checkLetter);
			if (iswalpha(checkLetter)) {
				return checkLetter;
			}
			wcout << L"Ошибка: введите только букву.\n";
		}
	}

	// Функция для рассчета процента угадывания
	double percentageOfLetters() {
		// Подсчет угаданных букв
		size_t guessedCount = 0;
		for (size_t i = 0; i < word.size(); ++i) {
			if (table[i] == word[i]) {
				++guessedCount;
			}
		}

		// Подсчет всех попыток (угаданные + ошибки)
		size_t totalAttempts = guessedCount + countError;

		// Расчет процента угадывания
		double successRate = (totalAttempts > 0)
			? (static_cast<double>(guessedCount) / totalAttempts) * 100.0
			: 0.0;

		return successRate;
	}

	// Очистить консоль
	void clearConsole() {
		system("cls");
	}

	// Метод вывода картинки и сообщения
	void sentence()
	{
		wcout << '\n' << pictureArr[err] << '\n';  // печать картинки
		wcout << L"Ошибок до виселицы осталось: " << err << "\n" << endl;
		//wcout << L"Случайное слово: " << word << endl;
		wcout << L"Отгаданное слово на данный момент: " << endl;
		showTable(table);
		wcout << L"Вы уже предлагали следующие буквы:" << endl;
		for (const auto& l : letters) {
			wcout << L"[ " << l << L" ]" << L" ";  // вывод каждого символа
		}
		wcout << endl;
	}

	//Вывод статистики
	void displayStatistics() {
		double seconds = difftime(end, start);
		// Проверка победы или поражения
		wstring result = (table == word)
			? L"Поздравляем! Вы угадали слово!\n" 
			: L"Увы, вы проиграли.\n";
		wstring resultAttempt = (table == word)
			? L"Слово было угадано с " + to_wstring(attempt) + L" попытки\n" 
			: L"Использовано " + to_wstring(attempt) + L" попыток\n";

		// вывод с задержкой
		auto print = [](const wstring& text, int delayMs = 100) {
			for (const wchar_t& ch : text) {
				wcout << ch;
				this_thread::sleep_for(chrono::milliseconds(delayMs));
			}
			wcout << endl;
			};
		
		wcout << pictureArr[err];  // печать картинки

		print(L"\n========= СТАТИСТИКА ИГРЫ =========\n", 50);
		print(result, 50);
		print(L"Загаданное слово: " + word, 50);
		print(L"Ваше слово:       " + table, 50);
		print(L"\nУгаданные буквы:", 50);

		wstring lettersStr;

		for (size_t i = 0; i < word.size(); ++i) {
			if (table[i] == word[i])
				lettersStr += L"[" + wstring(1, word[i]) + L"] ";
		}

		print(lettersStr, 50);
		print(L"\nНеугаданные буквы:", 50);
		wstring missedLettersStr;
		for (const wchar_t& letter : letters) {
			if (word.find(letter) == wstring::npos)
				missedLettersStr += L"[" + wstring(1, letter) + L"] ";
		}
		print(missedLettersStr, 50);
		print(L"\nПроцент угадывания: " + to_wstring(percentageOfLetters()) + L"%", 50);
		print(L"Время игры: " + to_wstring(static_cast<int>(seconds / 60)) +
			L" минут(ы) и " + to_wstring(static_cast<int>(seconds) % 60) + L" секунд", 50);
		print(L"Количество ошибок: " + to_wstring(countError), 50);
		print(resultAttempt, 50);
		print(L"\n===================================\n", 50);
	}

	void playGame()
	{
		letters.clear();
		word = getRandomWord();
		table = wstring(word.size(), '_');
		countError = 0;
		err = 6;
		attempt = 0;

		time(&start);


		wcout << L"Добро пожаловать в игру \"Виселица\"!\n\n";
		wcout << L"Угадайте слово по буквам. У вас есть " << err << L" попыток.\n";
		wcout << L"Категория: Фрукты на английском языке.\n";

		system("PAUSE");
		clearConsole();

		while (table != word && countError < 6)
		{
			sentence();
			wchar_t letter = getValidLetter();;

			if (letters.count(letter)) {
				clearConsole();
				wcout << L"Вы уже вводили эту букву. Попробуйте другую.\n";
				continue;
			}

			letters.insert(letter);
			checkLetter(letter);
			attempt++;

			clearConsole();
		}

		time(&end);
		double seconds = difftime(end, start);

		if (table == word) {
			totalWins++;
		}
		else {
			totalLosses++;
		}

		totalGames++;
		totalErrors += countError;
		totalTime += seconds;

		displayStatistics();
	}


	static void displayGlobalStats() {
		// вывод с задержкой
		auto print = [](const wstring& text, int delayMs = 100) {
			for (const wchar_t& ch : text) {
				wcout << ch;
				this_thread::sleep_for(chrono::milliseconds(delayMs));
			}
			wcout << endl;
			};

		print(L"\n========= СТАТИСТИКА ВСЕХ ИГР =========\n", 50);
		print(L"Всего игр: " + to_wstring(totalGames), 50);
		print(L"Побед: " + to_wstring(totalWins), 50);
		print(L"Поражений: " + to_wstring(totalLosses), 50);
		print(L"Всего ошибок: " + to_wstring(totalErrors), 50);

		double totalMinutes = totalTime / 60.0;
		int minutes = static_cast<int>(totalMinutes);               // Целые минуты
		int seconds = static_cast<int>(totalTime) % 60;             // Оставшиеся секунды

		print(L"Общее время игр: " + to_wstring(minutes) + L" минут " +
			to_wstring(seconds) + L" секунд\n", 50);
		print(L"\n=======================================\n", 50);
	}
};

size_t Game::totalGames = 0;
size_t Game::totalWins = 0;
size_t Game::totalLosses = 0;
size_t Game::totalErrors = 0;
double Game::totalTime = 0;

int main() {
	//setlocale(LC_ALL, "UTF8");
	locale::global(locale("ru_RU.UTF-8"));
	wcin.imbue(locale("ru_RU.UTF-8"));
	wcout.imbue(locale("ru_RU.UTF-8"));

	SetConsoleCP(65001);       // Кодировка ввода UTF-8
	SetConsoleOutputCP(65001); // Кодировка вывода UTF-8

	srand(static_cast<unsigned>(time(0)));

	wchar_t key{ L'Д' };		// Ключ для прерывания цикла;
	while (key == L'Д') {
		Game game;                 
		wcout << L"Желаете сыграть ещё раз ( Д / Н )?\n";
		wcin >> key;  
		wcin.ignore(32767, '\n');
		key = towupper(key);           
		game.clearConsole();
	}
	Game::displayGlobalStats();
	return 0;
}