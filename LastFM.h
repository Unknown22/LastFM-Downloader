#pragma once
#include <string>
#include <map>

using namespace std;

class LastFM
{
private:
	//zmienne
	string adres_;
	string adres_zrodla_;
	string zrodlo_strony_cale;
	string path;
	string database;
	string nazwa_klucza;

	size_t dlugosc_nazwy_zespolu;
	size_t dlugosc_tytulu;

	map<string, bool> baza;

	char* nazwa_zespolu;
	char* tytul;

	//metody
	size_t znajdz_wykonawce(size_t pozycja);
	size_t znajdz_tytul(size_t pozycja);
	void zapisz_wykonawce();
	void zapisz_tytul();
	bool czy_jest_w_bazie();

public:
	string path_slownik;
	LastFM(string adres, string adres_zrodla);
	~LastFM();
	void odczytaj_zrodlo_strony();
	void zapisz_zrodlo_do_pliku();
	void szukaj_piosenek();
	void zapisz_slownik();
	void wczytaj_slownik();
};

