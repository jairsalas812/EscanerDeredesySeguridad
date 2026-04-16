# 📡 Escáner de Redes y Seguridad Wi-Fi

> Dispositivo portátil basado en ESP32 para el análisis en tiempo real del entorno inalámbrico. Detecta redes Wi-Fi cercanas, evalúa la intensidad de señal y muestra los resultados en una pantalla TFT sin necesidad de software externo.

---

## 📋 Tabla de contenido

- [¿Qué hace el proyecto?](#-qué-hace-el-proyecto)
- [Arquitectura del sistema](#-arquitectura-del-sistema)
- [Tecnologías utilizadas](#-tecnologías-utilizadas)
- [Estructura de carpetas](#-estructura-de-carpetas)
- [Requisitos previos](#-requisitos-previos)
- [Instalación y configuración](#-instalación-y-configuración)
- [Uso](#-uso)
- [Pruebas](#-pruebas)
- [Contribuir](#-contribuir)

---

## 🔍 ¿Qué hace el proyecto?

El **Escáner de Redes y Seguridad Wi-Fi** es una herramienta de diagnóstico portátil que permite:

- **Detectar** todas las redes Wi-Fi disponibles en el entorno
- **Visualizar** nombre de red (SSID), canal y intensidad de señal (RSSI) en pantalla TFT
- **Identificar** posibles congestiones, interferencias o dispositivos desconocidos
- **Operar** de forma autónoma sin computadora ni software especializado

> ⚠️ El dispositivo realiza **análisis pasivo** únicamente. No interviene, vulnera ni ataca redes. Solo observa y muestra información del espectro Wi-Fi.

---

## 🏗️ Arquitectura del sistema

El sistema se organiza en tres capas:

```
┌─────────────────────────────────────────────┐
│           VISUALIZACIÓN                     │
│         Pantalla TFT ILI9341                │
│     (SPI → tabla, colores, animación)       │
├─────────────────────────────────────────────┤
│           PROCESAMIENTO                     │
│               ESP32                         │
│  (lógica, filtrado, colores por RSSI,       │
│   recorte de texto, organización de datos)  │
├─────────────────────────────────────────────┤
│        ADQUISICIÓN DE DATOS                 │
│       Módulo Wi-Fi interno ESP32            │
│    (WiFi.scanNetworks → SSID, canal, RSSI)  │
└─────────────────────────────────────────────┘
```

### Flujo de operación

```
Inicialización → Escaneo Wi-Fi → Procesamiento → Visualización TFT → Espera → (repite)
```

El ciclo se repite aproximadamente cada **9.5 segundos**, manteniendo la información actualizada sin sobrecargar el sistema.

---

## 🛠️ Tecnologías utilizadas

### Hardware

| Componente | Función |
|---|---|
| **ESP32** | Microcontrolador principal. Procesamiento, Wi-Fi integrado |
| **Pantalla TFT ILI9341** (2.4") | Visualización gráfica de los datos |
| **Protocolo SPI** | Comunicación de alta velocidad entre ESP32 y pantalla |

### Software

| Tecnología | Uso |
|---|---|
| **Arduino IDE** | Entorno de desarrollo (C/C++) |
| `WiFi.h` | Escaneo de redes y obtención de parámetros Wi-Fi |
| `SPI.h` | Comunicación con la pantalla TFT |
| `Adafruit_GFX` | Motor gráfico base (texto, formas, colores) |
| `Adafruit_ILI9341` | Driver específico para la pantalla ILI9341 |
| `Serial` | Depuración y monitoreo durante desarrollo |

---

## 📁 Estructura de carpetas

```
wifi-scanner/
│
├── wifi_scanner.ino        # Archivo principal del programa
│
├─

├── README.md               # Este archivo
├── .gitignore
└── LICENSE
```

---

## ✅ Requisitos previos

- [ ] Microcontrolador **ESP32**
- [ ] Pantalla **TFT ILI9341** (2.4 pulgadas)
- [ ] Cables de conexión (jumpers)
- [ ] Cable USB para programación
- [ ] Computadora con Windows, Linux o macOS
- [ ] **Arduino IDE** instalado

---

## 🚀 Instalación y configuración

### 1. Agregar soporte para ESP32 en Arduino IDE

En Arduino IDE ve a **Archivo → Preferencias** y agrega la siguiente URL en "Gestor de URLs adicionales de tarjetas":

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Luego ve a **Herramientas → Placa → Gestor de tarjetas**, busca `esp32` e instálalo.

### 2. Instalar librerías

Desde **Herramientas → Administrar bibliotecas**, busca e instala:

- `Adafruit GFX Library`
- `Adafruit ILI9341`

> `WiFi.h` y `SPI.h` vienen incluidas con el soporte ESP32.

### 3. Conectar el hardware

Verifica que los pines definidos en el código coincidan con tu cableado físico:

| Pin ESP32 | Pin TFT ILI9341 |
|---|---|
| MOSI | SDI/MOSI |
| MISO | SDO/MISO |
| SCK | SCK |
| CS | CS |
| DC | DC/RS |
| RST | RESET |
| 3.3V | VCC |
| GND | GND |

### 4. Cargar el programa

1. Abre `wifi_scanner.ino` en Arduino IDE
2. Selecciona la placa: **Herramientas → Placa → ESP32 Dev Module**
3. Selecciona el puerto correspondiente
4. Haz clic en **Subir** (→)

El ESP32 se reiniciará automáticamente y comenzará el escaneo.

---

## 📲 Uso

Una vez encendido, el dispositivo opera de forma completamente automática:

1. **Pantalla de bienvenida** — indica que el sistema está listo
2. **Animación de escaneo** — muestra que el proceso está en curso
3. **Lista de redes detectadas** — tabla con SSID, canal y RSSI

La intensidad de señal se indica mediante colores:

| Color | Calidad de señal |
|---|---|
| 🟢 Verde | Fuerte |
| 🟡 Amarillo | Media |
| 🔴 Rojo | Débil |

La pantalla muestra hasta **8 redes simultáneamente** y se actualiza cada ~9.5 segundos.

### Monitoreo serial (opcional)

Para ver información adicional, abre el **Monitor Serial** en Arduino IDE. Los datos de cada red también se imprimen ahí en formato texto, útil para depuración o análisis detallado.

---

## 🧪 Pruebas

Al tratarse de un sistema embebido, la validación se realiza de forma funcional:

- [ ] El dispositivo enciende correctamente y muestra la pantalla de bienvenida
- [ ] La animación de escaneo se despliega sin errores
- [ ] Se detectan redes Wi-Fi reales del entorno
- [ ] Los valores de RSSI y canal son coherentes
- [ ] Los colores de señal se asignan correctamente
- [ ] La información se actualiza de forma periódica y continua

Se recomienda probar en distintos entornos (hogar, oficina, espacio público) para validar el comportamiento bajo diferentes condiciones de red.

---

## 🤝 Contribuir

¡Las contribuciones son bienvenidas! Sigue estos pasos:

```bash
# 1. Clona el repositorio
git clone https://github.com/tu-usuario/wifi-scanner.git

# 2. Crea una rama para tu feature o fix
git checkout -b feature/nueva-funcionalidad

# 3. Realiza tus cambios y haz commit
git commit -m "feat: descripción del cambio"

# 4. Sube tu rama
git push origin feature/nueva-funcionalidad

# 5. Abre un Pull Request
```

### Áreas donde puedes contribuir

- ⚡ Optimización del rendimiento del escaneo
- 🎨 Mejoras en la interfaz gráfica TFT
- 💾 Integración con almacenamiento de datos (SD card)
- ☁️ Conexión a plataformas en la nube (MQTT, HTTP)
- 📊 Nuevas métricas de análisis (densidad de canales, historial)

Por favor, mantén el código limpio, documentado y respeta la estructura del proyecto.

---

## 📄 Licencia

Este proyecto está bajo la licencia especificada en el archivo [LICENSE](./LICENSE).
