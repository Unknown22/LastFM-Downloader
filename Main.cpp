#include "LastFM.h"
#include <iostream>
#include <fstream>
#include <conio.h>

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
	cout << pelny_adres << endl;

	LastFM strona = { adres, adres_zrodla };

	//-----DO PÊTLI
	strona.odczytaj_zrodlo_strony();
	strona.zapisz_zrodlo_do_pliku();
	strona.szukaj_piosenek();
	//-----KONIEC DO PÊTLI

	_getch();
	return 0;
}