# el-lobo-client

Klient czatu el-lobo

[insert-logo-here]

## Kompilacja

> Qt Creator

## Uruchomienie

> Qt Creator

## Zaimplementowane funkcje:
* Łączenie i rozłączanie z serwerem.
* Tworzenie i zmiana użytkownika (**CREA**).
* Wchodzenie w konwersację z podanym użytkownikiem.
* Wysyłanie wiadomości (**SEND**).
* Pobieranie wszystkich wiadomości z serwera na żądanie (**APLL**) + ich sortowanie wg timestamp-u i wyświetlanie.
* Cykliczne (co 2 sekundy) żądanie nowych wiadomości z serwera (**PULL**).
* Oczekiwanie na wiadomości z serwera przez readyRead (**RETN**, **ENDT**).
* Informowanie użytkownika o większości błędów.
