# 📡 Escáner de Redes y Seguridad Wi-Fi

> Dispositivo portátil basado en **ESP32** que analiza el entorno inalámbrico en tiempo real. Detecta redes Wi-Fi cercanas, evalúa la intensidad de señal y muestra los resultados en una pantalla TFT, sin necesidad de computadora ni software externo.

---

## 📋 Tabla de contenido

- [Descripción del proyecto](#-descripción-del-proyecto)
- [Arquitectura del sistema](#-arquitectura-del-sistema)
- [Diagrama de secuencia](#-diagrama-de-secuencia)
- [Tecnologías utilizadas](#-tecnologías-utilizadas)
- [Estructura del proyecto](#-estructura-del-proyecto)
- [Requisitos e instalación](#-requisitos-e-instalación)
- [Cómo usarlo](#-cómo-usarlo)
- [Cómo correr tests](#-cómo-correr-tests)
- [FAQ — Dudas frecuentes](#-faq--dudas-frecuentes)
- [Cómo contribuir](#-cómo-contribuir)

---

## 📖 Descripción del proyecto

El **Escáner de Redes y Seguridad Wi-Fi** es una herramienta de diagnóstico portátil orientada a usuarios que necesitan visibilidad sobre el entorno inalámbrico que los rodea. En muchos contextos —hogares, oficinas o espacios públicos— las personas desconocen cuántas redes existen cerca, cuáles están saturadas o si hay dispositivos desconocidos que representen un riesgo.

### ¿Para qué sirve?

- Detectar todas las redes Wi-Fi disponibles en el área
- Visualizar SSID, canal y nivel de señal (RSSI) en pantalla TFT
- Identificar congestión de canales o interferencias
- Operar de forma autónoma sin necesidad de computadora

### ¿Qué NO hace?

> ⚠️ El dispositivo realiza **análisis pasivo** únicamente. No intercepta tráfico, no vulnera redes ni realiza ningún tipo de ataque. Solo observa y muestra información pública del espectro Wi-Fi.

---

## 🏗️ Arquitectura del sistema

El sistema sigue una arquitectura de **tres capas** alojadas en un único microcontrolador ESP32:

```
┌──────────────────────────────────────────────────────┐
│                   VISUALIZACIÓN                      │
│              Pantalla TFT ILI9341 (2.4")             │
│        Protocolo SPI · Adafruit_GFX · colores        │
├──────────────────────────────────────────────────────┤
│                  PROCESAMIENTO                       │
│                      ESP32                           │
│   Lógica principal · filtrado · colores por RSSI     │
│   recorte de texto · organización de tabla visual    │
├──────────────────────────────────────────────────────┤
│              ADQUISICIÓN DE DATOS                    │
│          Módulo Wi-Fi interno del ESP32              │
│       WiFi.scanNetworks() → SSID · Canal · RSSI      │
└──────────────────────────────────────────────────────┘
```

### Flujo general del sistema

```
  ┌─────────────┐
  │ Inicializar │  SPI + TFT + Wi-Fi modo estación
  └──────┬──────┘
         │
  ┌──────▼──────┐
  │  Animación  │  Pantalla "Escaneando..."
  └──────┬──────┘
         │
  ┌──────▼──────────┐
  │ Escaneo Wi-Fi   │  WiFi.scanNetworks()
  └──────┬──────────┘
         │
  ┌──────▼──────────────┐
  │ Procesar resultados │  SSID · canal · RSSI · color
  └──────┬──────────────┘
         │
  ┌──────▼──────────┐
  │ Mostrar en TFT  │  Tabla + colores dinámicos
  └──────┬──────────┘
         │
  ┌──────▼──────┐
  │   Esperar   │  ~9.5 segundos
  └──────┬──────┘
         │
         └──────────────────► (repite el ciclo)
```

---

## 🔄 Diagrama de secuencia

Flujo de comunicación entre los componentes del sistema en cada ciclo:

```
   ESP32 (CPU)          Módulo Wi-Fi          Pantalla TFT
       │                    │                     │
       │── setup() ────────►│                     │
       │   configura SPI ───────────────────────►│
       │   muestra bienvenida ──────────────────►│
       │                    │                     │
       │── scanNetworks() ─►│                     │
       │                    │── escanea redes ───►(espectro)
       │◄── lista de redes ─│                     │
       │                    │                     │
       │── procesa datos ───┤                     │
       │   (RSSI→color,     │                     │
       │    corte de texto) │                     │
       │                    │                     │
       │── envía por SPI ───────────────────────►│
       │                    │     muestra tabla   │
       │                    │                     │
       │── delay(9500ms) ───┤                     │
       │                    │                     │
       └── (repite loop) ───┘                     │
```

---

## 🛠️ Tecnologías utilizadas

### Hardware

| Componente | Descripción |
|---|---|
| **ESP32** | Microcontrolador principal con Wi-Fi integrado |
| **Pantalla TFT ILI9341** (2.4") | Visualización gráfica en color |
| **Protocolo SPI** | Comunicación de alta velocidad ESP32 ↔ TFT |
| **Cables jumper + USB** | Conexión y programación |

### Software

| Librería / Herramienta | Uso |
|---|---|
| **Arduino IDE** | Entorno de desarrollo (C/C++) |
| `WiFi.h` | Escaneo de redes, SSID, canal, RSSI |
| `SPI.h` | Comunicación con la pantalla TFT |
| `Adafruit_GFX` | Motor gráfico: texto, formas, colores |
| `Adafruit_ILI9341` | Driver específico para la pantalla |
| `Serial` | Depuración por monitor serial |

---

## 📁 Estructura del proyecto

```
wifi-scanner/
│
├── wifi_scanner.ino        # Punto de entrada — lógica principal del sistema
│
├── src/                    # Módulos separados para escalabilidad futura
│   ├── display.h/.cpp      # Funciones de visualización TFT
│   ├── scanner.h/.cpp      # Lógica de escaneo Wi-Fi
│   └── utils.h/.cpp        # Funciones auxiliares (colores, texto)
│
├── lib/                    # Librerías externas fijadas por versión
├── include/                # Archivos de cabecera globales
│
├── docs/                   # Documentación técnica y diagramas
│   └── arquitectura.png
│
├── assets/                 # Capturas e imágenes del dispositivo físico
│
├── README.md               # Este archivo
├── .gitignore
└── LICENSE
```

> **Funciones principales en `wifi_scanner.ino`:**  
> `setup()` · `loop()` · `mostrarRedes()` · `pantallaEscaneandoAnimada()` · `pantallaBienvenida()` · `colorRSSI()` · `cortarTexto()`

---

## ✅ Requisitos e instalación

### Requisitos de hardware

- [ ] Microcontrolador **ESP32**
- [ ] Pantalla **TFT ILI9341** (2.4 pulgadas)
- [ ] Cables de conexión (jumpers)
- [ ] Cable USB (micro o tipo-C según modelo de ESP32)

### Requisitos de software

- [ ] [Arduino IDE](https://www.arduino.cc/en/software) instalado
- [ ] Soporte para placas ESP32 agregado al IDE
- [ ] Librerías: `Adafruit GFX` y `Adafruit ILI9341`

---

### Paso 1 — Agregar soporte ESP32 en Arduino IDE

1. Abre Arduino IDE → **Archivo → Preferencias**
2. En *"Gestor de URLs adicionales de tarjetas"*, pega:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

3. Ve a **Herramientas → Placa → Gestor de tarjetas**
4. Busca `esp32` e instala el paquete de **Espressif Systems**

---

### Paso 2 — Instalar librerías

En **Herramientas → Administrar bibliotecas**, busca e instala:

```
Adafruit GFX Library
Adafruit ILI9341
```

> `WiFi.h` y `SPI.h` vienen incluidas automáticamente con el soporte ESP32.

---

### Paso 3 — Conectar el hardware

| Pin ESP32 | Pin TFT ILI9341 | Descripción |
|---|---|---|
| GPIO 23 | SDI / MOSI | Datos hacia la pantalla |
| GPIO 19 | SDO / MISO | Datos desde la pantalla |
| GPIO 18 | SCK | Reloj SPI |
| GPIO 5 | CS | Chip Select |
| GPIO 2 | DC / RS | Comando vs Datos |
| GPIO 4 | RESET | Reset de pantalla |
| 3.3V | VCC | Alimentación |
| GND | GND | Tierra |

> ⚠️ Verifica que los `#define` de pines en el código coincidan exactamente con tu cableado físico antes de cargar el programa.

---

### Paso 4 — Clonar y cargar el proyecto

```bash
# Clona el repositorio
git clone https://github.com/tu-usuario/wifi-scanner.git

# Abre el archivo principal en Arduino IDE:
# Archivo → Abrir → wifi_scanner.ino
```

En Arduino IDE:

1. **Herramientas → Placa** → selecciona `ESP32 Dev Module`
2. **Herramientas → Puerto** → selecciona el puerto COM/USB del ESP32
3. Haz clic en **Subir** (`→`)

El ESP32 se reiniciará automáticamente y comenzará a escanear.

---

## 📲 Cómo usarlo

Una vez cargado el programa, el dispositivo opera de forma **completamente automática**. No se necesita ingresar comandos ni interactuar con él.

### Ciclo de operación

| Etapa | Qué sucede en pantalla |
|---|---|
| **1. Encendido** | Pantalla de bienvenida con nombre del proyecto |
| **2. Escaneo** | Animación de "Escaneando..." |
| **3. Resultados** | Tabla con hasta 8 redes: SSID · Canal · RSSI |
| **4. Actualización** | Ciclo se repite automáticamente cada ~9.5 s |

### Lectura de colores de señal

| Color | Calidad RSSI | Interpretación |
|---|---|---|
| 🟢 **Verde** | Señal fuerte | Buena cobertura |
| 🟡 **Amarillo** | Señal media | Cobertura aceptable |
| 🔴 **Rojo** | Señal débil | Cobertura limitada |

### Monitoreo serial (opcional)

Para ver los datos en formato texto durante desarrollo:

1. Conecta el ESP32 a la computadora por USB
2. En Arduino IDE → **Herramientas → Monitor Serial**
3. Configura la velocidad en **`115200 baudios`**

Cada red detectada aparecerá con su SSID, canal y valor RSSI numérico.

> Este proyecto **no requiere servidor**. Toda la lógica corre localmente en el ESP32, sin conexión a servicios externos.

---

## 🧪 Cómo correr tests

Al ser un sistema embebido, no se utilizan frameworks de testing automatizado. La validación se realiza mediante **verificación funcional directa** sobre el dispositivo.

### Checklist de pruebas

```
[ ] El dispositivo enciende y muestra la pantalla de bienvenida
[ ] La animación de escaneo se despliega sin errores
[ ] Se detectan redes Wi-Fi reales del entorno
[ ] Los valores de canal son números entre 1 y 13
[ ] Los valores de RSSI son negativos (ej: -45, -72, -88)
[ ] Los colores corresponden correctamente a la intensidad de señal
[ ] Los SSIDs largos se cortan sin desbordarse en pantalla
[ ] La información se actualiza periódicamente sin congelarse
[ ] El monitor serial imprime los datos en paralelo
```

### Entornos de prueba recomendados

| Entorno | Qué se valida |
|---|---|
| **Hogar** | Pocas redes, señal fuerte, comportamiento base |
| **Oficina** | Múltiples redes, mayor densidad de dispositivos |
| **Espacio público** | Alta densidad de redes, posible congestión de canales |

---

## ❓ FAQ — Dudas frecuentes

**¿El dispositivo necesita conexión a internet?**  
No. Opera completamente offline. Solo escanea redes cercanas sin conectarse a ninguna.

**¿Puede hackear o acceder a las redes que detecta?**  
No. Solo lee información que las redes transmiten públicamente (SSID, canal, potencia de señal). No intercepta tráfico ni realiza ningún ataque.

**¿Cuántas redes puede mostrar a la vez?**  
Hasta 8 redes simultáneamente. Si hay más en el entorno, se muestran las primeras detectadas.

**¿Por qué no aparece ninguna red?**  
Verifica que el ESP32 esté configurado en modo `WIFI_STA` y que la conexión SPI con la pantalla sea correcta. También revisa que no haya interferencia metálica cerca de la antena.

**¿Puedo cambiar el intervalo de actualización?**  
Sí. En el código, modifica el valor del `delay()` al final de `loop()`. Está configurado en `9500` (milisegundos = 9.5 segundos).

**¿Funciona con otras pantallas TFT?**  
El código usa el driver `ILI9341`. Usar otra pantalla requiere cambiar la librería y posiblemente ajustar los pines y resolución.

**¿El proyecto necesita servidor o base de datos?**  
No en su versión actual. A futuro podría expandirse para enviar datos a un servidor MQTT o plataforma en la nube.

---

## 🤝 Cómo contribuir

Este proyecto es open source. Sigue este flujo estándar para colaborar:

```bash
# 1. Haz fork del repositorio en GitHub

# 2. Clona tu fork
git clone https://github.com/tu-usuario/wifi-scanner.git
cd wifi-scanner

# 3. Crea una rama para tu cambio
git checkout -b feature/nombre-de-tu-mejora

# 4. Realiza tus cambios y documenta el código

# 5. Haz commit descriptivo
git add .
git commit -m "feat: descripción clara del cambio"

# 6. Sube tu rama
git push origin feature/nombre-de-tu-mejora

# 7. Abre un Pull Request en GitHub
```

### Buenas prácticas

- Mantén el código limpio y comentado
- Respeta la estructura de carpetas del proyecto
- Prueba los cambios en el hardware antes de abrir el PR
- No modifiques funcionalidades existentes sin documentarlo en el PR

### Áreas abiertas para contribución

| Área | Descripción |
|---|---|
| ⚡ Rendimiento | Optimizar velocidad de escaneo |
| 🎨 Interfaz | Mejorar diseño visual en la pantalla TFT |
| 💾 Almacenamiento | Guardar historial en tarjeta SD |
| ☁️ Nube | Envío de datos vía MQTT o HTTP |
| 📊 Métricas | Historial de señal, densidad por canal |
| 🌐 Idiomas | Soporte multilenguaje en la interfaz |

---

## 📄 Licencia

Este proyecto está bajo la licencia especificada en el archivo [LICENSE](./LICENSE).
