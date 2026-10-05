# Dart Kamera-Test (Quellpaket)

Dieses Programm öffnet bis zu drei USB-Kameras, zeigt Livebilder samt erkannter
Auflösung/FPS und speichert mit `S` Beispielbilder in einen Unterordner.
`ESC` oder `Q` beendet das Programm.

## Wichtig

Das ZIP enthält den C++-Quellcode. Wenn auf keinem eigenen PC genug Speicher für
Visual Studio/OpenCV frei ist, kann GitHub Actions die Windows-Version in der
Cloud bauen. Auf dem Dart-PC muss dafür nichts installiert werden. Die Kameras
müssen nur beim eigentlichen Test am Ziel-PC angeschlossen sein.

## Womit öffnen?

Am einfachsten zum Bauen ist **Visual Studio Community 2022** (kostenlos) mit
den Workloads **Desktopentwicklung mit C++** und **C++-CMake-Tools für Windows**.
VS Code kann den Quellcode anzeigen/bearbeiten, benötigt zum Bauen aber dieselben
C++- und OpenCV-Werkzeuge und ist daher für den ersten Schritt nicht nötig.

## Variante A: ohne lokale Installation bauen (empfohlen)

1. Lege auf GitHub ein neues Repository an (privat ist ausreichend).
2. Lade den Inhalt dieses Projektordners in das Repository hoch. Achte darauf,
   dass `.github/workflows/windows-build.yml` mit hochgeladen wird.
3. Öffne im Repository den Reiter **Actions** und starte **Build Windows camera test**.
   Beim ersten Aufruf kann GitHub Actions eine Aktivierung verlangen.
4. Nach erfolgreichem Lauf den erzeugten `DartCameraTest-Windows-x64`-Artifact
   herunterladen und entpacken. Den ganzen entpackten Ordner auf den USB-Stick
   kopieren. Auf dem Dart-PC `DartCameraTest.exe` aus diesem Ordner starten.

Der Cloud-Build benötigt Internet und ein GitHub-Konto. Auf dem Dart-PC sind
weder Administratorrechte noch Compiler/OpenCV-Installation erforderlich.

## Variante B: lokal unter Windows bauen

Nur nötig, wenn ein PC genügend freien Speicher hat:

1. Installiere Visual Studio Community 2022 und wähle die Workloads oben aus.
2. Installiere Git für Windows.
3. Öffne PowerShell und führe aus:

```powershell
cd $env:USERPROFILE\Downloads
git clone https://github.com/microsoft/vcpkg.git
cd .\vcpkg
.\bootstrap-vcpkg.bat
```

4. Entpacke diesen Ordner z. B. nach `C:\DartCameraTest`.
5. Öffne in Visual Studio: **Datei > Ordner öffnen** und wähle `C:\DartCameraTest`.
   Bei der CMake-Konfiguration gib als Toolchain-Datei an:

```text
C:\Users\DEIN_BENUTZER\Downloads\vcpkg\scripts\buildsystems\vcpkg.cmake
```

Visual Studio installiert dann OpenCV aus der Manifestdatei und baut das Programm.
Beim ersten Mal braucht der PC Internet.

## Starten und Testen

Schließe alle drei Kameras an den Ziel-PC an, beende andere Kamera-Apps und starte
`DartCameraTest.exe`. Falls Windows den Kamerazugriff blockiert, erlaube ihn unter
**Einstellungen > Datenschutz und Sicherheit > Kamera**.

Das Programm sucht Indizes 0–9 ab und öffnet die ersten drei funktionierenden
Geräte. Kameratreiber können die Reihenfolge ändern. Bei fehlender Kamera teste
USB-Ports direkt am PC und schließe Kameras einzeln an. Drei HD-Streams können
einen USB-Controller stark belasten; wenn es ruckelt, verteile die Kameras auf
verschiedene USB-Controller/Ports.

`S` speichert die aktuellen Bilder neben dem Programm in `capture_<Zeitstempel>`.
Diese Bilder brauchen wir anschließend, um Sichtfeld, Schärfe und Boardausschnitt
zu beurteilen. Dieses erste Programm erkennt noch keine Dartspitzen und bucht
keine Punkte.
