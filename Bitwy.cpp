
#include <stdio.h>
#include <tchar.h>
#include <string>
#include<iostream>
#include<conio.h>
#include<fstream>

//do wycieków pamięci
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

/////Łucznik////
const float LUCZNICY_ATAK = 30;
const float LUCZNICY_OBRONA = 5;
const int LUCZNICY_WYTRZYMALOSC = 20;
const int LUCZNICY_LICZEBNOSC = 200;
const int LUCZNICY_ZASIEG=5;
const float KARA_LUCZNICY_OBRONA = 0.5;
const float KARA_LUCZNICY_ATAK = 0.5;
const float LUCZNICY_STRATA_MORALI = 1;
//////Kusznik//////
const float KUSZNICY_ATAK = 50;
const float KUSZNICY_OBRONA = 8;
const int KUSZNICY_WYTRZYMALOSC = 30;
const int KUSZNICY_LICZEBNOSC = 150;
const int KUSZNICY_ZASIEG = 3;
const float KARA_KUSZNICY_OBRONA = 0.3;
const float KARA_KUSZNICY_ATAK = 0;
const float KUSZNICY_STRATA_MORALI = 1;
//////Miecznik//////
const float MIECZNICY_ATAK = 30;
const float MIECZNICY_OBRONA = 20;
const int MIECZNICY_WYTRZYMALOSC = 50;
const int MIECZNICY_LICZEBNOSC = 200;
const int MIECZNICY_ZASIEG = 1;
const float MIECZNICY_STRATA_MORALI = 1;
//////Halabardzista//////
const float HALABARDZISCI_ATAK = 40;
const float HALABARDZISCI_OBRONA = 20;
const int HALABARDZISCI_WYTRZYMALOSC = 40;
const int HALABARDZISCI_LICZEBNOSC = 150;
const int HALABARDZISCI_ZASIEG = 2;
const float HALABARDZISCI_STRATA_MORALI = 1;
//////Konny//////
const float KONNI_ATAK = 50;
const float KONNI_OBRONA = 10;
const int KONNI_WYTRZYMALOSC = 70;
const int KONNI_LICZEBNOSC = 100;
const int KONNI_ZASIEG = 2;
const float KONNI_STRATA_MORALI = 1;
//////Bębniarz//////
const float BEBNIARZE_ATAK = 0;
const float BEBNIARZE_OBRONA = 1;
const int BEBNIARZE_WYTRZYMALOSC = 50;
const int BEBNIARZE_LICZEBNOSC = 40;
const int BEBNIARZE_ZASIEG = 4;
const float BEBNIARZE_STRATA_MORALI = 1;
//////Tarczownik//////
const float TARCZOWNICY_ATAK = 0;
const float TARCZOWNICY_OBRONA = 20;
const int TARCZOWNICY_WYTRZYMALOSC = 100;
const int TARCZOWNICY_LICZEBNOSC = 100;
const int TARCZOWNICY_ZASIEG = 2;
const float TARCZOWNICY_STRATA_MORALI = 1;
//dodatkowe stałe///
const int NAJWIEKSZY_ZASIEG = fmax(LUCZNICY_ZASIEG, KUSZNICY_ZASIEG); //UWAGA:jeśli ma być więcej jednostek to trzeba tu dopisać (są tylko zasięgowe, bo mają największy zasięg)

class Oddzial;

class Armia {
	int ileOddzialow; 
public:
	Armia() :ileOddzialow(0) {}
	void operator++(int){
		++ileOddzialow;
	}
	void operator--(int) {
		--ileOddzialow;
	}
	bool czyistnieje() {
		if (ileOddzialow > 0)return true;
		return false;
	}
};


class Bebniarz;
class Tarczownik;

class Oddzial {
protected:
	int nrArmii;
	int atak;
	int obrona;
	int wytrzymalosc;
	int liczebnosc;
	int maxLiczebnosc;
	int zasieg;
	char oznaczenie;
	double morale;
	float strataMorali;  //mnożnik traconych morali przez oddział (potrzebny dla bębniarzy)
	int dodatkoweMorale;
	int otrzymaneObrazenia;  //otrzymane obrażenia w danej rundzie

	int pierwszaLiniaWroga() const {   //do znajdzCel
		if (nrArmii == 1) return 4;
		return 2;
	}
	virtual Oddzial* znajdzCel(int rzad, Oddzial ***pole) const {
		int i = 0;
		Oddzial* cel=nullptr;
		while (i < zasieg) {
			if (rzad - i >= 0) {  //sprawdzanie czy nie wychodzimy poza pole bitwy
				cel = pole[rzad - i][pierwszaLiniaWroga()];
				if (cel != nullptr)return cel;
			}
			//dla rząd + i nie trzeba sprawdzać wychodzenia poza tablicę, ponieważ umyślnie jest ona większa
			cel = pole[rzad + i][pierwszaLiniaWroga()];
			if (cel != nullptr)return cel;
			++i;
		}
		return cel;
	}
	int Obrazenia() {
		return (1 + atak)*liczebnosc;
	}
	void zadajObrazenia(Oddzial *j) {
		j->otrzymaneObrazenia += Obrazenia();
	}
public:
	bool czyZyjecieJeszcze() {
		if (liczebnosc > 0)return true;
		return false;
	}
	virtual void zaatakuj(int rzad, int linia, Oddzial ***pole) = 0; //[rzad] i [linia] to położenie jednostki atakującej
	virtual ~Oddzial() = default;
	virtual bool czyWsparcie() {
		return false;
	}
	virtual bool czyZasiegowa() = 0;
	virtual char jakieOznaczenie() = 0;  //potrzebne przy wyliczaniu bonusów niektórych jednostek
	virtual void ustawStatystyki() = 0;  //ustawia domyślny atak, obronę itp. (nie zmienia liczebności i morali)
	void straty() {
		int straty = otrzymaneObrazenia*(1 - ((morale+dodatkoweMorale) / (1 - (morale+dodatkoweMorale)))) / (wytrzymalosc*(1 + obrona));
		morale -= straty / (float)liczebnosc *strataMorali;
		liczebnosc -= straty;
	}
	void stracMorale() {
		morale -= std::fmax(0.25, -0.25*(morale+dodatkoweMorale)*strataMorali);   //morale są zawsze <=0, więc -morale >=0
	}
	std::string wypiszSie() {
		std::string napis;
		napis += oznaczenie;
		napis += ':';
		if (liczebnosc == maxLiczebnosc) { napis += "00"; }
		else {
			int temp = static_cast<int>((liczebnosc / (float)maxLiczebnosc) * 100)+1;
			if (temp < 10)napis += '0';
			napis += std::to_string(temp);
		}
		return napis;
	}
	int ktoraArmia() { return nrArmii; }
	friend Bebniarz;  //żeby mógł dawać bufy na protectowane zmienne
	friend Tarczownik;  //to samo co wyżej^^

};


//////////rodzaje jednostek///////////

class Zasiegowa:public Oddzial {
protected:
	virtual void karaPierwszyRzad() = 0;
public:
	bool czyZasiegowa() {
		return true;
	}
	virtual void zaatakuj(int rzad, int linia,Oddzial*** pole){
		if (linia == 2||linia==4)karaPierwszyRzad();
		Oddzial *cel = znajdzCel(rzad, pole);
		if(cel!=nullptr)
		zadajObrazenia(cel);
	}
	virtual ~Zasiegowa() =default;
};

class Zwarcie :public Oddzial {
protected:
	virtual void bonus(Oddzial *o) {}; //domyślnie nie ma bonusu przy ataku
public:
	void zaatakuj(int rzad, int linia,Oddzial*** pole) {
		if (linia == 2||linia==4) {
			Oddzial *cel = znajdzCel(rzad,pole);
			if (cel != nullptr) {
				bonus(cel);
				zadajObrazenia(cel);
			}
		}
	}
	bool czyZasiegowa() {
		return false;
	}
	virtual ~Zwarcie() = default;
};

class Wsparcie :public Oddzial {
protected:
	virtual void buf(Oddzial *o) = 0;
	int pierwszaLinia() const {
		if (nrArmii == 1)return 2;
		return 4;
	}
	Oddzial* znajdzCel(int rzad, Oddzial ***pole) const override {
		Oddzial* cel;
		for (int i = 0; i<zasieg; ++i) {
			if (rzad - i >= 0) {  //sprawdzanie czy nie wychodzimy poza pole bitwy
				cel = pole[rzad - i][pierwszaLinia()];
				if (cel != nullptr&&cel != (Oddzial*)this)return cel;
			}
			//dla (rząd + i) nie trzeba sprawdzać wychodzenia poza tablicę, ponieważ umyślnie jest ona większa
			cel = pole[rzad + i][pierwszaLinia()];
			if (cel != nullptr&&cel != (Oddzial*)this)return cel;
		}
		return (Oddzial*)this;
	}
public:
	bool czyWsparcie() {
		return true;
	}
	void zaatakuj(int rzad, int linia, Oddzial*** pole) override {
		Oddzial *cel = znajdzCel(rzad, pole);  //cel nie może być nullptr (zawsze można wspierać siebie)
		buf(cel);
	}
};


////////////jednostki//////////////////
class Lucznik :public Zasiegowa {
	int drugaLiniaWroga() const {
		if (nrArmii == 1)return 5;
		return 1;
	}
	void karaPierwszyRzad() {
		obrona *= (1-KARA_LUCZNICY_OBRONA);
		atak *= (1 - KARA_LUCZNICY_ATAK);
	}
	Oddzial* znajdzCel(int rzad,Oddzial ***pole) const override{
		int i = 0; 
		Oddzial* cel=nullptr;
		while (i < zasieg) {
			if (rzad - i >= 0) {
				cel = pole[rzad - i][drugaLiniaWroga()];
				if (cel != nullptr)return cel;
				cel = pole[rzad - i][pierwszaLiniaWroga()];
				if (cel != nullptr)return cel;
			}
			cel = pole[rzad + i][drugaLiniaWroga()];
			if (cel != nullptr)return cel;
			cel = pole[rzad + i][pierwszaLiniaWroga()];
			if (cel != nullptr)return cel;
			++i;
		}
		return cel;
	}   
public:
	void ustawStatystyki() {
		atak = LUCZNICY_ATAK;
		obrona = LUCZNICY_OBRONA;
		strataMorali = LUCZNICY_STRATA_MORALI;
		otrzymaneObrazenia = 0;
	}
	Lucznik(int nr) {
		nrArmii = nr;
		ustawStatystyki();
		wytrzymalosc = LUCZNICY_WYTRZYMALOSC;
		liczebnosc = LUCZNICY_LICZEBNOSC;
		maxLiczebnosc = LUCZNICY_LICZEBNOSC;
		oznaczenie = 'L';
		zasieg = LUCZNICY_ZASIEG;
		morale = 0;
		dodatkoweMorale = 0;
	}
	char jakieOznaczenie() {
		return 'L';
	}
	Lucznik(const Lucznik&) = delete;
	~Lucznik() = default;
};

class Kusznik :public Zasiegowa {
	void karaPierwszyRzad() {
		obrona *= (1 - KARA_KUSZNICY_OBRONA);
		atak *= (1 - KARA_KUSZNICY_ATAK);
	}
public:
	void ustawStatystyki() {
		atak = KUSZNICY_ATAK;
		obrona = KUSZNICY_OBRONA;
		strataMorali = KUSZNICY_STRATA_MORALI;
		otrzymaneObrazenia = 0;
		dodatkoweMorale = 0;
	}
	Kusznik(int nr) {
		nrArmii = nr;
		ustawStatystyki();
		wytrzymalosc = KUSZNICY_WYTRZYMALOSC;
		liczebnosc = KUSZNICY_LICZEBNOSC;
		maxLiczebnosc = KUSZNICY_LICZEBNOSC;
		oznaczenie = 'k';
		zasieg = KUSZNICY_ZASIEG;
		morale = 0;
		dodatkoweMorale = 0;
	}
	char jakieOznaczenie() {
		return 'k';
	}
	Kusznik(const Kusznik&) = delete;
	~Kusznik() = default;
};

class Miecznik :public Zwarcie {
public:
	Miecznik(int nr) {
		nrArmii = nr;
		ustawStatystyki();
		wytrzymalosc = MIECZNICY_WYTRZYMALOSC;
		liczebnosc = MIECZNICY_LICZEBNOSC;
		maxLiczebnosc = MIECZNICY_LICZEBNOSC;
		oznaczenie = 'M';
		zasieg = MIECZNICY_ZASIEG;
		morale = 0;
	}
	void ustawStatystyki() {
		atak = MIECZNICY_ATAK;
		obrona = MIECZNICY_OBRONA;
		strataMorali = MIECZNICY_STRATA_MORALI;
		otrzymaneObrazenia = 0;
		dodatkoweMorale = 0;
	}
	char jakieOznaczenie() {
		return 'M';
	}
	Miecznik(const Miecznik&) = delete;
	~Miecznik()=default;
};

class Halabardzista :public Zwarcie {
	void bonus(Oddzial *o) override {
		if (o->jakieOznaczenie() == 'K') atak += 0.5*HALABARDZISCI_ATAK;
	}
public:
	Halabardzista(int nr) {
		nrArmii = nr;
		ustawStatystyki();
		wytrzymalosc = HALABARDZISCI_WYTRZYMALOSC;
		liczebnosc = HALABARDZISCI_LICZEBNOSC;
		maxLiczebnosc = HALABARDZISCI_LICZEBNOSC;
		oznaczenie = 'H';
		zasieg = HALABARDZISCI_ZASIEG;
		morale = 0;
	}
	void ustawStatystyki() {
		atak = HALABARDZISCI_ATAK;
		obrona = HALABARDZISCI_OBRONA;
		strataMorali = HALABARDZISCI_STRATA_MORALI;
		otrzymaneObrazenia = 0;
		dodatkoweMorale = 0;
	}
	char jakieOznaczenie() {
		return 'H';
	}
	Halabardzista(const Halabardzista&) = delete;
	~Halabardzista() = default;
};

class Konny :public Zwarcie {
	void bonus(Oddzial *o) override {
		if (o->czyZasiegowa()) atak += 0.5*KONNI_ATAK;
	}
public:
	Konny(int nr) {
		nrArmii = nr;
		ustawStatystyki();
		wytrzymalosc = KONNI_WYTRZYMALOSC;
		liczebnosc = KONNI_LICZEBNOSC;
		maxLiczebnosc = KONNI_LICZEBNOSC;
		oznaczenie = 'K';
		zasieg = KONNI_ZASIEG;
		morale = 0;
	}
	void ustawStatystyki() {
		atak = KONNI_ATAK;
		obrona = KONNI_OBRONA;
		strataMorali = KONNI_STRATA_MORALI;
		otrzymaneObrazenia = 0;
		dodatkoweMorale = 0;
	}
	char jakieOznaczenie() {
		return 'K';
	}
	Konny(const Konny&) = delete;
	~Konny() = default;
};

class Bebniarz :public Wsparcie {
	void buf(Oddzial *o) {
		o->strataMorali *= 0.5;
		o->dodatkoweMorale = -morale*0.5;   //dodatkoweMorale powinny byc dodatnie, a morale<=0, więc -
	}
public:
	bool czyZasiegowa() {
		return true;
	}
	Bebniarz(int nr) {
		nrArmii = nr;
		ustawStatystyki();
		wytrzymalosc = BEBNIARZE_WYTRZYMALOSC;
		liczebnosc = BEBNIARZE_LICZEBNOSC;
		maxLiczebnosc = BEBNIARZE_LICZEBNOSC;
		oznaczenie = 'B';
		zasieg = BEBNIARZE_ZASIEG;
		morale = 0;
	}
	void ustawStatystyki() {
		atak = BEBNIARZE_ATAK;
		obrona = BEBNIARZE_OBRONA;
		strataMorali = BEBNIARZE_STRATA_MORALI;
		otrzymaneObrazenia = 0;
		dodatkoweMorale = 0;
	}
	char jakieOznaczenie() {
		return 'B';
	}
	Bebniarz(const Bebniarz&) = delete;
	~Bebniarz() = default;
};

class Tarczownik :public Wsparcie {
	void buf(Oddzial *o) {
		o->obrona *= 1.5;
	}
public:
	Tarczownik(int nr) {
		nrArmii = nr;
		ustawStatystyki();
		wytrzymalosc = TARCZOWNICY_WYTRZYMALOSC;
		liczebnosc = TARCZOWNICY_LICZEBNOSC;
		maxLiczebnosc = TARCZOWNICY_LICZEBNOSC;
		oznaczenie = 'T';
		zasieg = TARCZOWNICY_ZASIEG;
		morale = 0;
	}
	void ustawStatystyki() {
		atak = TARCZOWNICY_ATAK;
		obrona = TARCZOWNICY_OBRONA;
		strataMorali = TARCZOWNICY_STRATA_MORALI;
		otrzymaneObrazenia = 0;
		dodatkoweMorale = 0;
	}
	bool czyZasiegowa() {
		return false;
	}
	char jakieOznaczenie() {
		return 'T';
	}
	Tarczownik(const Tarczownik&) = delete;
	~Tarczownik() = default;
};


Oddzial* StworzOddzial(char typ, int nr) {
	switch (typ) {
	case 'L':
		return new Lucznik(nr);
	case 'k':
		return new Kusznik(nr);
	case 'M':
		return new Miecznik(nr);
	case 'H':
		return new Halabardzista(nr);
	case 'K':
		return new Konny(nr);
	case 'B':
		return new Bebniarz(nr);
	case 'T':
		return new Tarczownik(nr);
	case 'X':
		return nullptr;
	default:
		std::cout << "Blad! Nie ma takiego oddzialu!";
		throw "nie ma takiego oddzialu";
	}
}

///////pole bitwy///////////
class PoleBitwy {
	int rzedy;
	const int linie = 7;
	const int limitRund;
	Oddzial ***pole;
	std::string **wynikiRund;  //przechowuje stany kolejnych rund
	Armia armia1;
	Armia armia2;
	int runda;

	void uzupelnijLinieArmia1() {    //UŻYWAĆ PO KONSOLIDACJI SZEREGÓW
			for (int i = 0; i<rzedy; ++i) {
				//pole[i][0]-wsparcie
				//pole[i][1]-druga linia
				//pole[i][2]-pierwsza linia

				if (pole[i][2] == nullptr) {
					if (pole[i][0] != nullptr && pole[i][0]->jakieOznaczenie() == 'T') {  //leniwe sprawdzanie, warunek ok
						pole[i][2] = pole[i][0];
						pole[i][0] = nullptr;
					}
					else {
						if (pole[i][1] != nullptr) {
							pole[i][2] = pole[i][1];
							pole[i][1] = pole[i][0];
							pole[i][0] = nullptr;
						}
						else {
							pole[i][2] = pole[i][0];
							pole[i][0] = nullptr;
						}
					}
				}
				if (pole[i][1] == nullptr) {
					pole[i][1] = pole[i][0];
					pole[i][0] = nullptr;
				}
			}
		}
	void uzupelnijLinieArmia2() {   //UŻYWAĆ PO KONSOLIDACJI SZEREGÓW
		for (int i = 0; i<rzedy; ++i) {
			//pole[i][6]-wsparcie
			//pole[i][5]-druga linia
			//pole[i][4]-pierwsza linia

			if (pole[i][4] == nullptr) {
				if (pole[i][6] != nullptr && pole[i][6]->jakieOznaczenie() == 'T') {  //leniwe sprawdzanie, warunek ok
					pole[i][4] = pole[i][6];
					pole[i][6] = nullptr;
				}
				else {
					if (pole[i][5] != nullptr) {
							pole[i][4] = pole[i][5];
							pole[i][5] = pole[i][6];
							pole[i][6] = nullptr;
					}
					else {
							pole[i][4] = pole[i][6];
							pole[i][6] = nullptr;
						}
					}
				}
			if (pole[i][5] == nullptr){
				pole[i][5] = pole[i][6];
				pole[i][6] = nullptr;
			}
		}
	}

	void konsolidujSzeregiGoraArmia1() {  //przesuwa oddziały, żeby zapełnić puste rzędy (w górnej połowie pola, dla armii 1)
		int ilePustych=0;
		for (int i = rzedy / 2; i+ilePustych < rzedy; ++i) {
			while (pole[i+ilePustych][0] == nullptr&&pole[i+ ilePustych][1] == nullptr&&pole[i+ ilePustych][2] == nullptr) {
				++ilePustych;
				if (i + ilePustych >= rzedy)break;
			}
			pole[i][0] = pole[i + ilePustych][0];
			pole[i][1] = pole[i + ilePustych][1];
			pole[i][2] = pole[i + ilePustych][2];
		}
		for (int i = rzedy - ilePustych; i < rzedy; ++i) {
			pole[i][0] = nullptr;
			pole[i][1] = nullptr;
			pole[i][2] = nullptr;
		}
	}
	void konsolidujSzeregiGoraArmia2() {
		int ilePustych = 0;
		for (int i = rzedy / 2; i +ilePustych< rzedy; ++i) {
			while (pole[i + ilePustych][4] == nullptr&&pole[i + ilePustych][5] == nullptr&&pole[i + ilePustych][6] == nullptr) {
				++ilePustych;
				if (i + ilePustych >= rzedy)break;
			}
			pole[i][4] = pole[i + ilePustych][4];
			pole[i][5] = pole[i + ilePustych][5];
			pole[i][6] = pole[i + ilePustych][6];
		}
		for (int i = rzedy - ilePustych; i < rzedy; ++i) {
			pole[i][4] = nullptr;
			pole[i][5] = nullptr;
			pole[i][6] = nullptr;
		}
	}
	void konsolidujSzeregiDolArmia1() {
		int ilePustych = 0;
		for (int i = rzedy / 2 - 1; i-ilePustych >= 0; --i) {
			while (pole[i - ilePustych][0] == nullptr&&pole[i - ilePustych][1] == nullptr&&pole[i - ilePustych][2] == nullptr) {
				++ilePustych;
				if (i - ilePustych < 0) { --ilePustych; break; }
			}
			pole[i][0] = pole[i - ilePustych][0];
			pole[i][1] = pole[i - ilePustych][1];
			pole[i][2] = pole[i - ilePustych][2];
		}
		for (int i = ilePustych-1; i >=0; --i) { //wszystkie odzziały z tej części zostały już przekopiowane i trzeba je wyzerować
			pole[i][0] = nullptr;
			pole[i][1] = nullptr;
			pole[i][2] = nullptr;
		}
	}
	void konsolidujSzeregiDolArmia2() {
		int ilePustych = 0;
		for (int i = rzedy / 2 - 1; i -ilePustych>=0; --i) {
			while (pole[i - ilePustych][4] == nullptr&&pole[i - ilePustych][5] == nullptr&&pole[i - ilePustych][6] == nullptr) {
				++ilePustych;
				if (i - ilePustych < 0) { --ilePustych; break; }
			}
			pole[i][4] = pole[i - ilePustych][4];
			pole[i][5] = pole[i - ilePustych][5];
			pole[i][6] = pole[i - ilePustych][6];
		}
		for (int i = ilePustych - 1; i >= 0; --i) {
			pole[i][4] = nullptr;
			pole[i][5] = nullptr;
			pole[i][6] = nullptr;
		}
	}

	void konsolidujSzeregi() {
		konsolidujSzeregiGoraArmia1();
		konsolidujSzeregiGoraArmia2();
		konsolidujSzeregiDolArmia1();
		konsolidujSzeregiDolArmia2();

		uzupelnijLinieArmia1();
		uzupelnijLinieArmia2();
	}

	void zapiszStan() {  //rysuje i zapisuje aktualny stan pola walki do wynikiRund[runda] w postaci stringa (każdy rząd to osobny string)
		for (int i = 0; i < linie / 2; ++i) {  //pierwsze 3 linie
			for (int j = 0; j < rzedy; ++j) {
				wynikiRund[runda][i] += "  ";
				if (pole[j][i] == nullptr) {
					wynikiRund[runda][i] += "  X ";
				}
				else {
					wynikiRund[runda][i] += pole[j][i]->wypiszSie();
				}
			}
		}

		for (int i = 0; i < rzedy; ++i) {  //pole niczyje
			wynikiRund[runda][linie / 2] += "------";
		}

		for (int i = linie / 2 + 1; i < linie; ++i) { //ostatnie 3 linie 
			for (int j = 0; j < rzedy; ++j) {
				wynikiRund[runda][i] += "  ";
				if (pole[j][i] == nullptr) {
					wynikiRund[runda][i] += "  X ";
				}
				else {
					wynikiRund[runda][i] += pole[j][i]->wypiszSie(); 
				}
			}
		}

	}
	void wyswietlWynikBitwy() {//wyświetla kolejne rundy zapisane w wynikiRund, dodaje napis "runda n z [runda]" przed n-tą rundą
		for (int i = 0; i <= runda; ++i) {
			for (int j = 0; j < rzedy; ++j) {
				std::cout << "======";
			}
			std::cout << std::endl;
			std::cout << "Runda " << i << " z " << runda;
			std::cout << std::endl;
			for (int j = 0; j < linie; ++j) {
				std::cout << wynikiRund[i][j];
				std::cout << std::endl;
			}
		}

	}

	void obliczWsparcie() {
		int i = 0;
		Oddzial *oddzial;
		while (i < rzedy) {
			for (int j = 0; j < linie; ++j) {
				oddzial = pole[i][j];
				if (oddzial != nullptr && oddzial->czyWsparcie()) {  //leniwe sprawdzanie, warunek jest ok
					oddzial->zaatakuj(i, j, pole);
				}
			}
			++i;
		}
	}
	void obliczObrazenia() {
		int i = 0;
		Oddzial *oddzial;
		while (i < rzedy) {
			for (int j = 1; j < linie-1; ++j) { //nie jest obliczane dla lini wsparcia
				oddzial = pole[i][j];
				if (oddzial != nullptr && !oddzial->czyWsparcie()) {
					oddzial->zaatakuj(i, j, pole);
				}
			}
			++i;
		}
	}
	void obliczStraty() {
		int i = 0;
		Oddzial *oddzial;
		while (i < rzedy) {
			for (int j = 1; j < linie-1; ++j) {
				oddzial = pole[i][j];
				if (oddzial != nullptr) {
					oddzial->straty();  //zmniejsaza liczebność oraz morale oddziału spowodowane atakiem
				}
			}
			++i;
		}
	}
	void obliczMorale() {
		int i = 0;
		Oddzial *oddzial;
		while (i < rzedy) {
			for (int j = 1; j < linie-1; ++j) {
				oddzial = pole[i][j];
				if (oddzial != nullptr && !oddzial->czyZyjecieJeszcze()) {
					if (oddzial->ktoraArmia() == 1) { armia1--; }
					else { armia2--; }
					delete oddzial;
					pole[i][j] = nullptr;
					int h; //linia
					if (j == 1 || j == 2) { h = 2; }
					else { h = 5; }
					for (int k = -1; k <= 1; ++k) {
						if (i - k >= 0) {//sprawdzenie czy nie wypadamy pod tablicę
							oddzial = pole[i - k][h];
							if (oddzial != nullptr)
								oddzial->stracMorale();
							oddzial = pole[i - k][h - 1];
							if (oddzial != nullptr)
								oddzial->stracMorale();
						}
					}
				}
			}
			++i;
		}
	}
	void zresetujStatystyki() {    //zdejmuje buffy oraz zmienia otrzymaneObrazenia na 0
		int i = 0;
		Oddzial *oddzial;
		while (i < rzedy) {
			for (int j = 0; j < linie; ++j) {
				oddzial = pole[i][j];
				if (oddzial != nullptr)
					oddzial->ustawStatystyki();
			}
			++i;
		}
	}
	void naprawSzeregi() {
		konsolidujSzeregiGoraArmia1();
		konsolidujSzeregiGoraArmia2();
		konsolidujSzeregiDolArmia1();
		konsolidujSzeregiDolArmia2();

		uzupelnijLinieArmia1();
		uzupelnijLinieArmia2();
	}
	bool czyKtosWygral() {
		if (!(armia1.czyistnieje() && armia2.czyistnieje()))return true;
		return false;
	}
public:
	
	PoleBitwy(int iloscRzedy,int maxRund,char **tabelaJednostek):rzedy(iloscRzedy),limitRund(maxRund),runda(0),armia1(),armia2() {
		
		wynikiRund = new std::string*[limitRund+1];
		for (int i = 0; i < limitRund+1; ++i) {
			wynikiRund[i] = new std::string[linie];
			for (int j = 0; j < linie; ++j) {
			}
		}
		
		//zasadnicze pole bitwy
		pole = new Oddzial**[rzedy + NAJWIEKSZY_ZASIEG + 1];//większe pole bitwy, żeby przy wyszukiwaniu jednostek do zaatakowania nie wyjść za tablicę (+1 dla pewności)
		for (int i = 0; i < rzedy; ++i) {
			pole[i] = new Oddzial*[linie];
			for (int j = 0; j < linie / 2; ++j) {
				pole[i][j]=StworzOddzial(tabelaJednostek[i][j], 1);
				if (pole[i][j] != nullptr)armia1++;
			}

			pole[i][linie / 2] = nullptr;

			for (int j = linie/2+1; j < linie; ++j) {
				pole[i][j] = StworzOddzial(tabelaJednostek[i][j-1], 2);
				if (pole[i][j] != nullptr)armia2++;
			}
		}
		//dodatkowe miejsce na polu bitwy ustawiane na nullptr
		for (int i = rzedy; i < rzedy+NAJWIEKSZY_ZASIEG + 1; ++i) {
			pole[i] = new Oddzial*[linie];
			for (int j = 0; j < linie; ++j) {
				pole[i][j] = nullptr;
			}
		}
	}
	
	void WALKA() {
		while (runda < limitRund) {
			if (!czyKtosWygral()) {
 				zapiszStan();
				++runda;
				obliczWsparcie();
				obliczObrazenia();
				obliczStraty();
				obliczMorale();
				zresetujStatystyki();
				konsolidujSzeregi();
			}
			else {
				break;
			}
		}
		zapiszStan();
		wyswietlWynikBitwy();
	}
	~PoleBitwy() {
		//usuwanie wynikiRund
		for (int i = 0; i < limitRund+1; ++i) {
			delete[] wynikiRund[i];
		}
		delete wynikiRund;

		//usuwanie pole
		for (int i = 0; i < rzedy+NAJWIEKSZY_ZASIEG+1; ++i) {
			for (int j = 0; j < linie; ++j) {
				delete pole[i][j];
			}
			delete pole[i];
		}
		delete pole;
	}
};




int main()
{
	//wczytywanie danych

	char **poleBitwy;
	int rzedy=0, maxrund=0;
	std::fstream plik;
	plik.open("dane.txt",std::ios::in|std::ios::out);
	if (plik.good()) {
		plik >> rzedy >> maxrund;
	}

	poleBitwy = new char*[rzedy];
	for (int i = 0; i < rzedy; ++i) {
		poleBitwy[i] = new char[6];
	}

	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < rzedy; ++j) {
			plik >> poleBitwy[j][i];
		}
	}
	//tworzenie PolaBitwy

	PoleBitwy *symulacja=new PoleBitwy(rzedy,maxrund,poleBitwy);
	symulacja->WALKA();
	delete symulacja;
	//test co się wczytało
	std::cout << rzedy<<" "<<maxrund;
	for (int i = 0; i < 6; ++i) {
		std::cout << std::endl;
		for (int j = 0; j < rzedy; ++j) {
			std::cout<< poleBitwy[j][i]<<" ";
		}
	}

		for (int j = 0; j < rzedy; ++j) {
			delete poleBitwy[j];
		}
		delete poleBitwy;

		_getch();
//	_CrtDumpMemoryLeaks();
	return 0;
}