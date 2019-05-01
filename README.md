Dies ist die Referenzimplementierung des Simulator Exchange Protocol (SEP). Die Spezifikation befindet sich im Ordner `spec/`. Sie ist in C++11 geschrieben und sollte leicht einzubinden und zu nutzen sein. Die Benutzung erfolgt über statische Methoden der Klasse `SimulatorExchangeSender`:

- `static bool init(std::string &filename)` initialisiert die Bibliothek mit dem Pfad zur Beschreibungsdatei und gibt `true` im Erfolgsfall zurück.
- `static bool allowSending(bool allowed)` schaltet das tatsächliche Senden von Telegrammen ein(aus), wenn `allowed` `true`(`false`) ist und gibt `true` im Erfolgsfall zurück.
- `static void updateValue(const std::string& name, .... )` setzt einen Wert mit dem Namen `name` auf den Wert im zweiten Parameter. Diese Funktion hat Überladungen für die entsprechenden Datentypen.

Das war es auch schon!

Diese Referenzimplementierung benötigt Boost, mindestens in Version 1.63.0. Sie kann sowohl über cmake als auch als Visual Studio Projekt eingebunden werden. Für letzteres sind Projektdateien für Visual Studio 2013 und Visual Studio 2019 bereitgestellt.

Die gesetzten Werte werden intern in einer Map abgelegt und in einem seperaten Thread zum richtigen Zeitpunkt übers Netzwerk gesendet.

Die Lizenz für die Referenzimplementierung ist MIT, kommerzieller Einsatz ist erlaubt!

English:

This is the reference implementation of the Simulator Exchange Protocol (SEP). The spec is located in `spec`. It is written in C++11 and should be easy to use. The interface consists of the following static methods of the `SimulatorExchangeSender` class:

- `static bool init(std::string &filename)` initialises the library with the path to the telegram describer file and returns `true` in case of success.
- `static bool allowSending(bool allowed)` turns the actual sending of telegrams on(off), if `allowed` is `true`(`false`). Returns `true` in case of success.
- `static void updateValue(const std::string& name, .... )` sets the value with name `name` to the value in the second parameter. This function has overloads for the various data types.

That's it!

This library requires Boost, at least version 1.63.0. It can be built/included both using cmake and Visual Studio. For the latter we provide project files for Visual Studio 2013 and Visual Studio 2019.

The values are stored internally in a map. A seperate thread sends the values at the right time via the network.

The license for this lib is MIT, so commercial usage is allowed!