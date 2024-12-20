#include <iostream>   // ��� ������ � �������� �����-������ (cin, cout)
#include <fstream>    // ��� ������ � ������� (ifstream, ofstream)
#include <vector>     // ��� ������������� ������������ �������� (vector)
#include <array>      // ��� ������������� �������� �������������� ������� (array)
#include <string>     // ��� ������ � �������� (wstring)
#include <cstdlib>    // ��� ������� ������ ���������� (rand, srand)
#include <ctime>      // ��� ������ �� �������� (time)
#include <wchar.h>    // ��� ������ � �������� �������� � ��������� (wchar_t)
#include <thread>     // ��� ���������� �������� ���������� (this_thread::sleep_for)
#include <chrono>     // ��� ������ � �������� � ���������� (chrono::milliseconds)
#include <iomanip>    // ��� ��������� �������������� �����-������ (setprecision)
#include <set>        // ��� ������ � ����������� ���������� ��������� (set)
#include <windows.h>  // ��� ������������� ������������������ ������� Windows (SetConsoleCP, SetConsoleOutputCP)
using namespace std;


class Game
{
private:
	wstring word{ L"" };					// �����, ������� ����� ��������
	wstring table{ L"" };					// ������� � ����������� �������
	size_t countError{ 0 };					// ���-�� ������
	size_t err{ 6 };						// ���-�� ���������� ������ 
	set<wchar_t> letters;         			// ������ ��������� ����
	time_t start, end;						// ������� ������� �� ���� ���� 
	size_t attempt{ 0 };					// ���-�� ������� 

	// ���������� ����������
	static size_t totalGames;				// ����� ���������� ��������� ���
	static size_t totalWins;			    // ����� ���������� �����
	static size_t totalLosses;			    // ����� ���������� ���������
	static size_t totalErrors;				// ����� ���������� ������ ������������ (�������� ������� ������� �����)
	static double totalTime;				// ����� �����, ����������� �� ���� (� ��������)

	const array<wstring, 7> pictureArr		// ������ � ��������������    
	{
		 L"\t___________________\n\t  ||     |     ||\n\t  ||     @     ||\n"
		  "\t  ||    |O|    ||\n"
		  "\t  ||     �     ||\n     _____||___________||_____\n",
		 L"\t___________________\n\t  ||     |     ||\n\t  ||     @     ||\n"
		  "\t  ||    /O\\    ||\n"
		  "\t  ||    _�_    ||\n     _____||_____�_____||_____\n",
		 L"\t___________________\n\t  ||     |     ||\n\t  ||     Q     ||\n"
		  "\t  ||           ||\n"
		  "\t  ||    ___    ||\n     _____||_____�_____||_____\n",
		 L"\t___________________\n\t  ||           ||\n\t  ||           ||\n"
		  "\t  ||           ||\n"
		  "\t  ||    ___    ||\n     _____||_____�_____||_____\n",
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

	// ������� ��� ��������� ���������� ����� �� �������
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

		// ���������, ���� �� ����� � �������
		if (words.empty()) {
			wcerr << L"������: ���� words.txt ���� ��� ����������.\n";
			exit(1); 
		}

		int randomIndex = rand() % words.size();
		return words[randomIndex];
	}

	//  ����� ��� ��������� ����� � �������
	void drawLine(int n) {
		for (int i = 0; i < n; i++) {
			wcout << "+---";
		}
		wcout << "+\n";
	}

	// ����� ��� ��������� ������� � ����������� ������� � �����
	void showTable(wstring word) {
		drawLine(word.size());
		wcout << "| ";
		for (int i = 0; i < word.size(); i++) {
			wcout << word[i] << " | ";
		}
		wcout << "\n";
		drawLine(word.size());
	}

	// ������� ��� ��������� ����� � �������
	void checkLetter(wchar_t letter) {
		bool found = false;
		for (size_t i = 0; i < word.size(); ++i) {
			if (word[i] == letter) {
				table[i] = letter;
				found = true;
			}
		}

		if (!found) {
			wcout << L"������������ �����!\n";
			countError++;
			err--;
		}
		else {
			wcout << L"���������� �����!\n";
		}
	}


	// �������� �� ���� �����
	wchar_t getValidLetter() {
		wchar_t checkLetter = 'a';
		while (true) {
			wcout << L"������� �����: ";
			wcin >> checkLetter;
			wcin.ignore(32767, '\n');

			checkLetter = towupper(checkLetter);
			if (iswalpha(checkLetter)) {
				return checkLetter;
			}
			wcout << L"������: ������� ������ �����.\n";
		}
	}

	// ������� ��� �������� �������� ����������
	double percentageOfLetters() {
		// ������� ��������� ����
		size_t guessedCount = 0;
		for (size_t i = 0; i < word.size(); ++i) {
			if (table[i] == word[i]) {
				++guessedCount;
			}
		}

		// ������� ���� ������� (��������� + ������)
		size_t totalAttempts = guessedCount + countError;

		// ������ �������� ����������
		double successRate = (totalAttempts > 0)
			? (static_cast<double>(guessedCount) / totalAttempts) * 100.0
			: 0.0;

		return successRate;
	}

	// �������� �������
	void clearConsole() {
		system("cls");
	}

	// ����� ������ �������� � ���������
	void sentence()
	{
		wcout << '\n' << pictureArr[err] << '\n';  // ������ ��������
		wcout << L"������ �� �������� ��������: " << err << "\n" << endl;
		//wcout << L"��������� �����: " << word << endl;
		wcout << L"���������� ����� �� ������ ������: " << endl;
		showTable(table);
		wcout << L"�� ��� ���������� ��������� �����:" << endl;
		for (const auto& l : letters) {
			wcout << L"[ " << l << L" ]" << L" ";  // ����� ������� �������
		}
		wcout << endl;
	}

	//����� ����������
	void displayStatistics() {
		double seconds = difftime(end, start);
		// �������� ������ ��� ���������
		wstring result = (table == word)
			? L"�����������! �� ������� �����!\n" 
			: L"���, �� ���������.\n";
		wstring resultAttempt = (table == word)
			? L"����� ���� ������� � " + to_wstring(attempt) + L" �������\n" 
			: L"������������ " + to_wstring(attempt) + L" �������\n";

		// ����� � ���������
		auto print = [](const wstring& text, int delayMs = 100) {
			for (const wchar_t& ch : text) {
				wcout << ch;
				this_thread::sleep_for(chrono::milliseconds(delayMs));
			}
			wcout << endl;
			};
		
		wcout << pictureArr[err];  // ������ ��������

		print(L"\n========= ���������� ���� =========\n", 50);
		print(result, 50);
		print(L"���������� �����: " + word, 50);
		print(L"���� �����:       " + table, 50);
		print(L"\n��������� �����:", 50);

		wstring lettersStr;

		for (size_t i = 0; i < word.size(); ++i) {
			if (table[i] == word[i])
				lettersStr += L"[" + wstring(1, word[i]) + L"] ";
		}

		print(lettersStr, 50);
		print(L"\n����������� �����:", 50);
		wstring missedLettersStr;
		for (const wchar_t& letter : letters) {
			if (word.find(letter) == wstring::npos)
				missedLettersStr += L"[" + wstring(1, letter) + L"] ";
		}
		print(missedLettersStr, 50);
		print(L"\n������� ����������: " + to_wstring(percentageOfLetters()) + L"%", 50);
		print(L"����� ����: " + to_wstring(static_cast<int>(seconds / 60)) +
			L" �����(�) � " + to_wstring(static_cast<int>(seconds) % 60) + L" ������", 50);
		print(L"���������� ������: " + to_wstring(countError), 50);
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


		wcout << L"����� ���������� � ���� \"��������\"!\n\n";
		wcout << L"�������� ����� �� ������. � ��� ���� " << err << L" �������.\n";
		wcout << L"���������: ������ �� ���������� �����.\n";

		system("PAUSE");
		clearConsole();

		while (table != word && countError < 6)
		{
			sentence();
			wchar_t letter = getValidLetter();;

			if (letters.count(letter)) {
				clearConsole();
				wcout << L"�� ��� ������� ��� �����. ���������� ������.\n";
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
		// ����� � ���������
		auto print = [](const wstring& text, int delayMs = 100) {
			for (const wchar_t& ch : text) {
				wcout << ch;
				this_thread::sleep_for(chrono::milliseconds(delayMs));
			}
			wcout << endl;
			};

		print(L"\n========= ���������� ���� ��� =========\n", 50);
		print(L"����� ���: " + to_wstring(totalGames), 50);
		print(L"�����: " + to_wstring(totalWins), 50);
		print(L"���������: " + to_wstring(totalLosses), 50);
		print(L"����� ������: " + to_wstring(totalErrors), 50);

		double totalMinutes = totalTime / 60.0;
		int minutes = static_cast<int>(totalMinutes);               // ����� ������
		int seconds = static_cast<int>(totalTime) % 60;             // ���������� �������

		print(L"����� ����� ���: " + to_wstring(minutes) + L" ����� " +
			to_wstring(seconds) + L" ������\n", 50);
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

	SetConsoleCP(65001);       // ��������� ����� UTF-8
	SetConsoleOutputCP(65001); // ��������� ������ UTF-8

	srand(static_cast<unsigned>(time(0)));

	wchar_t key{ L'�' };		// ���� ��� ���������� �����;
	while (key == L'�') {
		Game game;                 
		wcout << L"������� ������� ��� ��� ( � / � )?\n";
		wcin >> key;  
		wcin.ignore(32767, '\n');
		key = towupper(key);           
		game.clearConsole();
	}
	Game::displayGlobalStats();
	return 0;
}