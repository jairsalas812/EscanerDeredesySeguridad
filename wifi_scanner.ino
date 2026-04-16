#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_SCK  18

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// ---------- Colores ----------
#define COLOR_FONDO       ILI9341_BLACK
#define COLOR_MARCO       ILI9341_CYAN
#define COLOR_SUBMARCO    ILI9341_DARKCYAN
#define COLOR_TITULO      ILI9341_WHITE
#define COLOR_SUBTITULO   ILI9341_LIGHTGREY
#define COLOR_DETALLE     ILI9341_CYAN
#define COLOR_LINEA       ILI9341_DARKCYAN
#define COLOR_ALERTA      ILI9341_YELLOW

// ---------- Funciones auxiliares ----------
String cortarTexto(String texto, int maxLen) {
  if (texto.length() <= maxLen) return texto;
  return texto.substring(0, maxLen - 3) + "...";
}

uint16_t colorRSSI(int rssi) {
  if (rssi >= -60) return ILI9341_GREEN;
  if (rssi >= -75) return ILI9341_YELLOW;
  return ILI9341_RED;
}

void textoCentradoPantalla(String texto, int y, int tam, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(tam);
  tft.getTextBounds(texto, 0, y, &x1, &y1, &w, &h);
  int x = (320 - w) / 2;
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.print(texto);
}

void textoCentradoColumna(String texto, int xInicio, int ancho, int y, int tam, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(tam);
  tft.getTextBounds(texto, 0, y, &x1, &y1, &w, &h);
  int x = xInicio + (ancho - w) / 2;
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.print(texto);
}

void dibujarMarcoElegante() {
  tft.drawRect(0, 0, 320, 240, COLOR_MARCO);
  tft.drawRect(2, 2, 316, 236, COLOR_SUBMARCO);
  tft.drawRect(6, 6, 308, 228, ILI9341_NAVY);
}

void dibujarLineaDecorativa(int y) {
  tft.drawFastHLine(40, y, 240, COLOR_LINEA);
  tft.drawFastHLine(60, y + 2, 200, COLOR_LINEA);
}

// ---------- Pantalla de bienvenida ----------
void pantallaBienvenida() {
  tft.fillScreen(COLOR_FONDO);
  dibujarMarcoElegante();

  textoCentradoPantalla("Analizador", 32, 3, COLOR_TITULO);
  textoCentradoPantalla("de Redes WiFi", 68, 3, COLOR_TITULO);

  dibujarLineaDecorativa(110);

  textoCentradoPantalla("Sistema portatil de monitoreo", 135, 1, COLOR_SUBTITULO);
  textoCentradoPantalla("ESP32 + TFT ILI9341", 152, 1, COLOR_DETALLE);

  tft.fillRoundRect(90, 185, 140, 26, 6, ILI9341_NAVY);
  textoCentradoPantalla("Iniciando...", 192, 2, COLOR_ALERTA);
}

// ---------- Animación de escaneo ----------
void pantallaEscaneandoAnimada() {
  tft.fillScreen(COLOR_FONDO);
  dibujarMarcoElegante();

  textoCentradoPantalla("Escaneando", 40, 3, COLOR_TITULO);
  dibujarLineaDecorativa(92);

  textoCentradoPantalla("Buscando redes cercanas", 120, 1, COLOR_SUBTITULO);

  tft.drawRoundRect(85, 155, 150, 34, 8, COLOR_MARCO);
  tft.fillRoundRect(87, 157, 146, 30, 8, ILI9341_NAVY);

  for (int i = 0; i < 4; i++) {
    String puntos = "";
    for (int j = 0; j < i; j++) puntos += ".";
    tft.fillRoundRect(110, 163, 100, 16, 6, ILI9341_NAVY);
    textoCentradoPantalla(puntos, 166, 2, COLOR_ALERTA);
    delay(250);
  }
}

// ---------- Encabezado ----------
void encabezadoLista() {
  tft.fillScreen(COLOR_FONDO);
  dibujarMarcoElegante();

  tft.fillRoundRect(18, 14, 284, 30, 8, ILI9341_NAVY);
  textoCentradoPantalla("Redes WiFi", 21, 2, COLOR_TITULO);

  tft.drawFastHLine(15, 52, 290, COLOR_LINEA);

  // Columnas
  int colSSID_X = 14;
  int colSSID_W = 170;

  int colCH_X = 184;
  int colCH_W = 40;

  int colRSSI_X = 224;
  int colRSSI_W = 80;

  textoCentradoColumna("SSID", colSSID_X, colSSID_W, 60, 1, COLOR_ALERTA);
  textoCentradoColumna("CH", colCH_X, colCH_W, 60, 1, COLOR_ALERTA);
  textoCentradoColumna("RSSI", colRSSI_X, colRSSI_W, 60, 1, COLOR_ALERTA);
}

// ---------- Mostrar redes ----------
void mostrarRedes() {
  pantallaEscaneandoAnimada();

  int n = WiFi.scanNetworks();

  encabezadoLista();

  if (n <= 0) {
    textoCentradoPantalla("No se detectaron redes", 100, 2, ILI9341_RED);
    textoCentradoPantalla("Intente nuevamente", 132, 1, COLOR_SUBTITULO);
    return;
  }

  // Columnas
  int colSSID_X = 14;
  int colSSID_W = 170;

  int colCH_X = 184;
  int colCH_W = 40;

  int colRSSI_X = 224;
  int colRSSI_W = 80;

  int maxMostrar = 8;

  for (int i = 0; i < n && i < maxMostrar; i++) {
    int y = 78 + (i * 18);

    String ssid = WiFi.SSID(i);
    if (ssid.length() == 0) ssid = "(Oculta)";
    ssid = cortarTexto(ssid, 18);

    String canal = String(WiFi.channel(i));
    String rssi = String(WiFi.RSSI(i)) + " dBm";

    textoCentradoColumna(ssid, colSSID_X, colSSID_W, y, 1, ILI9341_WHITE);
    textoCentradoColumna(canal, colCH_X, colCH_W, y, 1, ILI9341_WHITE);
    textoCentradoColumna(rssi, colRSSI_X, colRSSI_W, y, 1, colorRSSI(WiFi.RSSI(i)));
  }

  tft.drawFastHLine(15, 220, 290, COLOR_LINEA);

  String resumen = "Redes encontradas: " + String(n);
  textoCentradoPantalla(resumen, 226, 1, COLOR_DETALLE);

  Serial.println("----- ESCANEO WIFI -----");
  for (int i = 0; i < n; i++) {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" | Canal: ");
    Serial.print(WiFi.channel(i));
    Serial.print(" | RSSI: ");
    Serial.print(WiFi.RSSI(i));
    Serial.println(" dBm");
  }
  Serial.println("------------------------");
}

void setup() {
  Serial.begin(115200);

  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI, TFT_CS);

  tft.begin();
  tft.setRotation(1);

  pantallaBienvenida();
  delay(2500);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(300);
}

void loop() {
  mostrarRedes();
  delay(9500);
}