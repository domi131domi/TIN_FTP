# _**Techniki internetowe**_ ##  
### Projekt dotyczący implementacji klienta i serwera wymieniających pliki za pomocą protokołu FTP.  
Autorzy:
Dominik Szacilowski,
Adam Stec,
Jakub Kowalczyk,
Rafal Surdej  
Środowisko: Linux  
Język programowania: C++  
Gałąź do oceny: main, commit: 8711259  
Sposób uruchomienia serwera:  
```
./ServerProgram <port> <files_directory>  
```
czyli na przykład:  
```
./ServerProgram 8080 ~/home/Desktop/FileSystem/  
```
Sposób uruchomienia klienta:  
```
./ClientProgram <server_ip> <port>  
```
Na przykład:  
```
./ClientProgram 25.99.14.230 8080  
```
Komendy klienta:  
- cd <folder> – służy do poruszania się po systemie plików serwera  
- ls – wyświetla pliki znajdujące się w zadanym kontekście  
- login <username>  – logowanie w celu uzyskania dostępu do plików  
- logout – wylogowuje zadanego użytkownika  
- sign – umożliwia wysłanie prośby o założenie nowego konta  
- send <filename> – wysyłanie zadanego pliku na serwer  
- get <filename> – pobieranie pliku z zadanego kontekstu  

Uwaga: prośbę o założenie konta należy zatwierdzić na serwerze komendą register <username>, wówczas dopiero można się zalogować
