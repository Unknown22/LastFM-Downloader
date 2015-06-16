#pragma once
#include <string>

using namespace std;

class LastFM
{
private:
	string adres_;
	string adres_zrodla_;
	string zrodlo_strony_cale;
	string path;
	string database;
public:
	LastFM(string adres, string adres_zrodla);
	~LastFM();
	void odczytaj_zrodlo_strony();
	void zapisz_zrodlo_do_pliku();
	void szukaj_piosenek();
	void zapisz_do_bazy(size_t pozycja);
};

