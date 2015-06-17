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
	path_slownik = "base_dictionary.txt";
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
	string szukanaFrazaObecnieSluchane = "<td class=\"subjectCell highlight\">";
	string fraza_przed_nazwa_zespolu = "\">";
	string fraza_przed_tytulem = "\" >";
	size_t pozycja_za_wykonawca;
	size_t znalezionaPozycja = 0;

//---szukanie obecnie odtwarzanego
	znalezionaPozycja = zrodlo_strony_cale.find(szukanaFrazaObecnieSluchane);
	if (znalezionaPozycja != std::string::npos)
	{
		znalezionaPozycja = zrodlo_strony_cale.find(fraza_przed_nazwa_zespolu, znalezionaPozycja + szukanaFrazaObecnieSluchane.size());

		//przekazywanie do funkcji pozycji od ktorej zaczyna sie nazwa zespolu
		pozycja_za_wykonawca = znajdz_wykonawce(znalezionaPozycja + fraza_przed_nazwa_zespolu.size());
		znalezionaPozycja = zrodlo_strony_cale.find(fraza_przed_tytulem, pozycja_za_wykonawca);
		znalezionaPozycja = znajdz_tytul(znalezionaPozycja + fraza_przed_tytulem.size());

		
		if (czy_jest_w_bazie())
		{
			zapisz_wykonawce();
			zapisz_tytul();
		}
		nazwa_klucza.clear();
		

		//std::cout << "Fraza zostala odnaleziona na pozycji " << znalezionaPozycja << std::endl;
		znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza, znalezionaPozycja + szukanaFraza.size());
	}
	
//--KONIEC szukania obecnie odtwarzanego


//---szukanie ostatnio s³uchanych
	znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza);
	if (znalezionaPozycja == std::string::npos)
	{
		std::cout << "Wystapil jakis problem. Sprawdz jeszcze raz czy poprawnie wpisales nazwe uzytkownika" << std::endl;
		return;
	}

	do
	{
		znalezionaPozycja = zrodlo_strony_cale.find(fraza_przed_nazwa_zespolu, znalezionaPozycja + szukanaFraza.size());

		//przekazywanie do funkcji pozycji od ktorej zaczyna sie nazwa zespolu
		pozycja_za_wykonawca = znajdz_wykonawce(znalezionaPozycja + fraza_przed_nazwa_zespolu.size()); 
		znalezionaPozycja = zrodlo_strony_cale.find(fraza_przed_tytulem, pozycja_za_wykonawca);
		znalezionaPozycja = znajdz_tytul(znalezionaPozycja + fraza_przed_tytulem.size());

		if (czy_jest_w_bazie())
		{
			zapisz_wykonawce();
			zapisz_tytul();
		}
		nazwa_klucza.clear();


		//std::cout << "Fraza zostala odnaleziona na pozycji " << znalezionaPozycja << std::endl;
		znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza, znalezionaPozycja + szukanaFraza.size());
	} while (znalezionaPozycja != std::string::npos);
//---KONIEC szukania ostatnio sluchanych

	zrodlo_strony_cale.clear();
}

size_t LastFM::znajdz_wykonawce(size_t pozycja)
{
	ifstream fin;
	fin.open(path, ios_base::in);

	string szukanaFraza = "</a>";
	size_t znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza, pozycja);

	dlugosc_nazwy_zespolu = znalezionaPozycja - pozycja;
	//cout << "Dl: " << dlugosc_nazwy_zespolu << endl;

	nazwa_zespolu = new char[dlugosc_nazwy_zespolu];

	int i = 0;
	while(zrodlo_strony_cale[pozycja] != '<')
	{
		nazwa_zespolu[i] = zrodlo_strony_cale[pozycja];
		nazwa_klucza = nazwa_klucza + nazwa_zespolu[i];
		i++;
		pozycja++;
	}
	return pozycja;
}

void LastFM::zapisz_wykonawce()
{
	int j = 0;
	/*
	while (j < dlugosc_nazwy_zespolu)
	{
		cout << nazwa_zespolu[j];
		j++;
	}
	cout << " - ";
	*/
	ofstream fout;
	fout.open(database, ios_base::out | ios_base::app);

	j = 0;
	while (j < dlugosc_nazwy_zespolu)
	{
		fout << nazwa_zespolu[j];
		j++;
	}
	fout << " - ";

}

size_t LastFM::znajdz_tytul(size_t pozycja)
{

	ifstream fin;
	fin.open(path, ios_base::in);

	string szukanaFraza = "</a>";
	size_t znalezionaPozycja = zrodlo_strony_cale.find(szukanaFraza, pozycja);

	dlugosc_tytulu = znalezionaPozycja - pozycja;
	//cout << "Dl: " << dlugosc_nazwy_zespolu << endl;

	tytul = new char[dlugosc_tytulu];
	string separator = " - ";
	nazwa_klucza = nazwa_klucza + separator;

	int i = 0;
	while (zrodlo_strony_cale[pozycja] != '<')
	{
		tytul[i] = zrodlo_strony_cale[pozycja];
		nazwa_klucza = nazwa_klucza + tytul[i];
		i++;
		pozycja++;
	}

	return pozycja;
}

void LastFM::zapisz_tytul()
{

	int j = 0;
	/*
	while (j < dlugosc_tytulu)
	{
		cout << tytul[j];
		j++;
	}
	cout << endl;
	*/
	ofstream fout;
	fout.open(database, ios_base::out | ios_base::app);

	while (j < dlugosc_tytulu)
	{
		fout << tytul[j];
		j++;
	}
	fout << endl;

}

bool LastFM::czy_jest_w_bazie()
{
	if (baza.count(nazwa_klucza) == 0)
	{
		baza[nazwa_klucza] = FALSE;
		//cout << nazwa_klucza << endl;
		return TRUE;
	}
	else
		return FALSE;
}

void LastFM::zapisz_slownik()
{
	ofstream fout;
	fout.open(path_slownik, ios_base::out | ios_base::trunc);
	int licznik = 0;
	for (auto i : baza)
	{
		fout << i.first << endl
			<< i.second;
		licznik++;
		if (licznik < baza.size())
		{
			fout << endl;
		}
	}
}

void LastFM::wczytaj_slownik()
{
	ifstream slownik;
	string linia1;
	string linia2;
	bool status;
	slownik.open(path_slownik, ios_base::in);
	if (slownik.good() == true)
	{
			while (!slownik.eof())
			{
				getline(slownik, linia1);
				getline(slownik, linia2);
				if (linia2 == "0")
					status = FALSE;
				else
					status = TRUE;
				baza[linia1] = status;
				//cout << linia << endl; //wyœwietlenie linii
			}
	}
	else
	{

	}
	slownik.close();
}