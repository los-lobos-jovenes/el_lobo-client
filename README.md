# el-lobo-client

Klient czatu el-lobo

Oparty na Qt5

## Deweloperska kompilacja i uruchomienie

**Qt Creator**

Uwaga: stosować do debugowania i rozwijania klienta

## Produkcyjna kompilacja i uruchomienie (terminal)

### Instalacja Qt5 (Ubuntu)
> sudo apt-get install qt5-default

### Instalacja Qt5 (Fedora)
> sudo dnf install qt5-devel

### Kompilacja
> cd lobo-client

> qmake-qt5 lobo-client.pro && make

### Uruchomienie
> ./lobo-client

## Zaimplementowane funkcje:
* Łączenie i rozłączanie z serwerem.
* Tworzenie i zmiana użytkownika (**CREA**).
* Wchodzenie w konwersację z podanym użytkownikiem.
* Wysyłanie wiadomości (**SEND**).
* Pobieranie wszystkich wiadomości z serwera na żądanie (**APLL**) + ich sortowanie wg timestamp-u i wyświetlanie.
* Cykliczne (co 2 sekundy) żądanie nowych wiadomości z serwera (**PULL**).
* Cykliczne (co 3 sekundy) żądanie powiadomień o nowych wiadomościach (**PEND**).
* Oczekiwanie na wiadomości z serwera przez readyRead (**RETN**, **ENDT**).
* Informowanie użytkownika o większości błędów.
