#include "LastFM.h"
#include <Windows.h>
#include <WinInet.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

using namespace std;

LastFM::LastFM(string adres, string adres_zrodla)
{
	adres_ = adres;
	adres_zrodla_ = adres_zrodla;
	path = "strona.txt";
	database = "database.txt";
}

LastFM::~LastFM()
{
}

void LastFM::odczytaj_zrodlo_strony()
{
	
	HINTERNET hInternet = InternetOpenA("InetURL/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	HINTERNET hConnection = InternetConnectA(hInternet, adres_.c_str() , 80, " ", " ", INTERNET_SERVICE_HTTP, 0, 0);

	HINTERNET hData = HttpOpenRequestA(hConnection, "GET", adres_zrodla_.c_str(), NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0);

	char buf[2048];

	HttpSendRequestA(hData, NULL, 0, NULL, 0);

	DWORD bytesRead = 0;
	DWORD totalBytesRead = 0;

	while (InternetReadFile(hData, buf, 2000, &bytesRead) && bytesRead != 0)
	{
		buf[bytesRead] = 0; // insert the null terminator.
		zrodlo_strony_cale = zrodlo_strony_cale + buf;
	}
	


	
	//-----WYSWIETLANIE DATY I GODZINY AKTUALIZACJI

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::cout << std::put_time(&tm, "Ostatnia aktualizacja: %d-%m-%Y %H:%M:%S") << std::endl;
	
	//-----KONIEC WYSWIETLANIA DATY I GODZINY AKTUALIZACJI
	

	InternetCloseHandle(hData);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hInternet);
}

void LastFM::zapisz_zrodlo_do_pliku()
{
	ofstream fout;
	fout.open(path, ios_base::out | ios_base::trunc);
	fout << zrodlo_strony_cale;
}

void LastFM::szukaj_piosenek()
{
	string szukanaFraza = "<td class=\"subjectCell \">";
	string fraza_przed_nazwa_zespolu = "\">";

	size_t znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza);
	if (znalezionaPozycja == std::string::npos)
	{
		std::cout << "Wystapil jakis problem. Sprawdz jeszcze raz czy poprawnie wpisales nazwe uzytkownika" << std::endl;
		return;
	}

	do
	{
		znalezionaPozycja = zrodlo_strony_cale.find(fraza_przed_nazwa_zespolu, znalezionaPozycja + szukanaFraza.size());

		//przekazywanie do funkcji pozycji od ktorej zaczyna sie nazwa zespolu
		zapisz_do_bazy(znalezionaPozycja + fraza_przed_nazwa_zespolu.size()); 


		//std::cout << "Fraza zostala odnaleziona na pozycji " << znalezionaPozycja << std::endl;
		znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza, znalezionaPozycja + szukanaFraza.size());
	} while (znalezionaPozycja != std::string::npos);
}

void LastFM::zapisz_do_bazy(size_t pozycja)
{
	ifstream fin;
	fin.open(path, ios_base::in);

	string szukanaFraza = "</a>";
	size_t znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza, pozycja);

	char* nazwa_zespolu;

	size_t dlugosc_nazwy_zespolu = znalezionaPozycja - pozycja;
	//cout << "Dl: " << dlugosc_nazwy_zespolu << endl;

	nazwa_zespolu = new char[dlugosc_nazwy_zespolu];

	int i = 0;
	while(zrodlo_strony_cale[pozycja] != '<')
	{
		nazwa_zespolu[i] = zrodlo_strony_cale[pozycja];
		i++;
		pozycja++;
	}

	int j = 0;
	while (j < i)
	{
		cout << nazwa_zespolu[j];
		j++;
	}
	cout << endl;

	ofstream fout;
	fout.open(database, ios_base::out | ios_base::app);

	j = 0;
	while (j < i)
	{
		fout << nazwa_zespolu[j];
		j++;
	}
	fout << endl;
}