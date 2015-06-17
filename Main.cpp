#include "LastFM.h"
#include <iostream>
#include <fstream>
#include <conio.h>
#include <thread>
#include <chrono>

using namespace std;

int main()
{
	string adres = "www.last.fm";
	string adres_zrodla = "/user/";
	string uzytkownik;

	

	cout << "Podaj nazwe uzytkownika last.fm: ";
	cin >> uzytkownik;
	adres_zrodla = adres_zrodla + uzytkownik;
	string pelny_adres = adres + adres_zrodla;
	cout << pelny_adres << endl << endl;

	LastFM strona = { adres, adres_zrodla };

	strona.wczytaj_slownik();
	//-----DO PÊTLI
	while (true)
	{
		strona.odczytaj_zrodlo_strony();
		//strona.zapisz_zrodlo_do_pliku();
		strona.szukaj_piosenek();
		strona.zapisz_slownik();
		this_thread::sleep_for(chrono::seconds(5));
	}

	//-----KONIEC DO PÊTLI

	_getch();
	return 0;
}