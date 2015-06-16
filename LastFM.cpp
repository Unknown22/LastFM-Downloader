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
}


LastFM::~LastFM()
{
}

void LastFM::odczytaj_zrodlo_strony()
{
	
	cout << "Cstr: " << adres_.c_str() << endl;

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
	

	//cout << "Odswiezono: " << czas << endl;

	InternetCloseHandle(hData);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hInternet);
}

void LastFM::zapisz_zrodlo_do_pliku()
{
	ofstream fout;
	fout.open("strona.html", ios_base::out | ios_base::trunc);
	fout << zrodlo_strony_cale;
}