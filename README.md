# MQTT_Automation
Translation will follow!
Diese Bibliothek verwaltet ein System von Regeln, die abhängig von Eingangsbedin-gungen Aktionen auslösen können.  Sowohl für den Eingang von Messwerten als auch zum Ausführen von Aktionen wird MQTT verwendet. Da die Bibliothek auch die Konfiguration und Speicherung der Regeln unterstützt, macht sie ebenfalls von der TFTForm Bibliothek gebrauch.

## MQTT_Automation(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font)
Das ist der Konstruktor der Klasse. Es wird ein Zeiger auf eine Instanz der Adafruit_ILI9341 Display Klasse, ein weiterer Zeiger auf eine Instanz der TFTForm Klasse und ein Zeiger auf einen GFX Zeichensatz benötigt. Der Zeiger auf den Zeichensatz kann auch auf NULL gesetzt werden, dann wird auch für die Über-schriften der interne 5x7 Zeichensatz benutzt.

## void init() 
Diese Funktion initialisiert die Klasse. Falls bereits Regeln im Filesystem gespeichert wurden, werden diese gelesen. Sonst wird die Anzahl der bekannten Regeln auf 0 gesetzt.

## void showConfig()
Mit dieser Funktion wird die Konfiguration der Regeln gestartet. Das Hauptfenster mit einer Liste der vorhandenen Regeln wird angezeigt. 

## void handleClick(int16_t x, int16_t y)
Damit das Konfigurationsfenster auf Klicks auf den Touch-Screen reagieren kann, müssen bei einem Klick mit dieser Funktion die Koordinaten des Berührungspunktes übergeben werden.

## void registerOnDone(void (\*callback)())
Es wird die Callback-Funktion onDone() registriert. Sie wird dann aufgerufen, wenn das Hauptfenster der Konfiguration mit dem „Fertig“ Knopf beendet wurde.  Sie kann genutzt werden, um auf andere Funktionen zurückzuschalten, wenn die Konfiguration beendet wurde. Ehe die Callback-Funktion aufgerufen wird, werden alle Regeln im Filesystem gespeichert.

## void registerOnPublish(bool (\*callback)(const char * topic, const char * message))
Es wird die Callback-Funktion onPublish(const char * topic, const char * message) registriert. Sie wird dann aufgerufen, wenn eine Aktion eine MQTT-Nachricht senden möchte. Das Thema und die Nachricht werden als Parame-ter übergeben. Die Funktion sollte wahr zurückgeben, wenn die Nachricht erfolg-reich gesendet wurde.

## void endListe()
Diese Funktion sollte immer dann aufgerufen werden, wenn ein Formular oder eine Liste mit „Abbruch“ beendet wurde. Dies erfolgt typisch im Aufruf der Callback-Funktion onCancel() der TFTForm Klasse.

## void endForm(String data)
Diese Funktion sollte immer dann aufgerufen werden, wenn ein Formular mit dem „Speichern“ Knopf beendet wurde. Der Parameter data zeigt auf einen String im JSON Format, der die geänderten Daten des Formulars enthält. Dies erfolgt typisch im Aufruf der Callback-Funktion onConfSave(String data) der TFTForm Klasse.

## void updateTopic(char topic[], const char data[])
Diese Funktion sollte immer dann aufgerufen werden, wenn vom MQTT Broker eine Nachricht mit dem Präfix stat empfangen wurde. Der Parameter topic zeigt auf das Thema ohne Präfix, der Parameter data auf die empfangene Nachricht. Die Nachricht wird im JSON Format erwartet.

## void refresh()
Diese Funktion führt die Auswertung der Regeln durch und löst abhängig vom Er-gebnis Aktionen aus. Die Funktion soll in der Hauptschleife des Sketches aufgerufen werden.

## bool saveRules()
Mit dieser Funktion können die aktuellen Daten aller Regeln im Filesystem gespei-chert werden. Tritt dabei ein Fehler auf wird falsch zurückgegeben. Ein Aufruf die-ser Funktion ist im Normalfall nicht notwendig, da die Regeln beim Beenden der Konfiguration automatisch gespeichert werden.

## bool readRules()
Mit dieser Funktion werden alle Regeln gelöscht und neu aus dem Filesystem gela-den. Auch diese Funktion wird im Normalfall nicht aufgerufen, da die Regeln beim Initialisieren der Klasse automatisch gelesen werden.

## bool deleteEntry(uint8_t index)
Diese Funktion sollte dann aufgerufen werden, wenn ein Formular zum Konfigurie-ren einer Bedingung oder einer Aktion mit dem „Löschen“ Knopf beendet wurde.  Dies erfolgt typisch im Aufruf der Callback-Funktion onDelete(uint8_t index) der TFTForm Klasse.
