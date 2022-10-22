#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "credentials.h"

#define POWER_PIN_1 5
#define POWER_PIN_2 4


// Disconnected: Black: HIGH, Red: LOW
// Connected:    Black: LOW,  Red: HIGH

ESP8266WebServer server(80);

bool status_led = true;

void setup() {
    Serial.begin(115200);
    Serial.println();

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(POWER_PIN_1, OUTPUT);
    pinMode(POWER_PIN_2, OUTPUT);

    set_power_pins_connected(false);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        digitalWrite(LED_BUILTIN, HIGH);
    }
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, handle_root);
    server.on("/press", HTTP_POST, handle_press);
    server.on("/long_press", HTTP_POST, handle_long_press);

    server.begin();
}

void loop() {
    status_led = true;
    digitalWrite(LED_BUILTIN, status_led);

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Disconnected");

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(250);
            digitalWrite(LED_BUILTIN, LOW);
            delay(250);
            digitalWrite(LED_BUILTIN, HIGH);
        }

        Serial.print("Connected, IP address: ");
        Serial.println(WiFi.localIP());
    }

    delay(1000);

    server.handleClient();
}

void handle_root() {
    String html =
        "<html>"
        "<head>"
        "    <meta charset=\"UTF-8\">"
        "    <title>Elias Computer Startup</title>"
        "</head>"
        "<body>"
        "    <h1>Elias Computer Startup</h1>"
        "    <form action=\"/press\" method=\"POST\">"
        "        <input type=\"submit\" value=\"Press Power Button\"><br>"
        "    </form>"
        "    <form action=\"/long_press\" method=\"POST\">"
        "        <input type=\"submit\" value=\"(LONG - 12s) Press Power Button\"><br>"
        "    </form>"
        "</body>"
        "</html>";

    server.send(200, "text/html", html);
}

void handle_press() {
    press_power();
    delay(500);

    server.send(200, "text/html", "<html><script>location.pathname = \"/\"</script></html>");
}

void handle_long_press() {
    long_press_power();
    delay(500);

    server.send(200, "text/html", "<html><script>location.pathname = \"/\"</script></html>");
}

void press_power() {
    set_power_pins_connected(true);
    for (int i = 0; i < 5; i++) {
        status_led = !status_led;
        digitalWrite(LED_BUILTIN, status_led);
        delay(100);
    }
    set_power_pins_connected(false);
}

void long_press_power() {
    set_power_pins_connected(true);
    for (int i = 0; i < 120; i++) {
        status_led = !status_led;
        digitalWrite(LED_BUILTIN, status_led);
        delay(100);
    }
    set_power_pins_connected(false);
}

void set_power_pins_connected(bool connected) {
    if (connected) {
        digitalWrite(POWER_PIN_1, LOW);
        digitalWrite(POWER_PIN_2, HIGH);
    } else {
        digitalWrite(POWER_PIN_1, HIGH);
        digitalWrite(POWER_PIN_2, LOW);
    }
}
