# Documentación del Esquema de Codificación Propio

> **Estudiante**: Zain Rondon  
> **Cédula**: 30.601.248

---

## Información del Esquema

**Nombre del esquema**: ZR-Code (Zain Rondon Code)

---

## Mi Restricción Asignada

Mi cédula termina en: **8**

Mi restricción es: **Debe funcionar correctamente con inversión de polaridad**

---

## Descripción del Algoritmo

### Idea general

ZR-Code es un esquema de codificación 2B/3B que codifica pares de bits en bloques de 3 símbolos. La característica principal es que utiliza **codificación diferencial basada en el nivel anterior**, lo que lo hace inherentemente resistente a la inversión de polaridad.

La información no se codifica en los niveles absolutos (H o L), sino en los **patrones relativos** al último nivel transmitido. Esto significa que si se invierte toda la señal (H↔L), el patrón relativo se mantiene y la decodificación sigue siendo correcta.

Además, ZR-Code incluye **detección de errores integrada** mediante la validación de patrones, y proporciona **autosincronización parcial** al garantizar transiciones regulares cada 3 símbolos.

### Regla de codificación

Codificación por bloques de 2 bits → 3 símbolos:

**Tabla cuando el nivel anterior es LOW:**

| Entrada | Salida | Transiciones |
|---------|--------|--------------|
| 00      | LLL    | 0            |
| 01      | LLH    | 1            |
| 10      | LHL    | 2            |
| 11      | LHH    | 1            |

**Tabla cuando el nivel anterior es HIGH:**

| Entrada | Salida | Transiciones |
|---------|--------|--------------|
| 00      | HHH    | 0            |
| 01      | HHL    | 1            |
| 10      | HLH    | 2            |
| 11      | HLL    | 1            |

**Algoritmo:**
1. Iniciar con nivel de referencia = LOW
2. Para cada par de bits:
   - Convertir a índice (00→0, 01→1, 10→2, 11→3)
   - Seleccionar tabla según nivel anterior
   - Emitir patrón de 3 símbolos
   - Actualizar nivel de referencia al último símbolo emitido

### Regla de decodificación

1. Iniciar con nivel de referencia = LOW
2. Para cada bloque de 3 símbolos:
   - Leer los 3 símbolos
   - Buscar patrón en la tabla correspondiente al nivel anterior
   - Convertir índice encontrado a par de bits
   - Actualizar nivel de referencia al último símbolo leído
3. Si el patrón no existe en la tabla → **Error detectado**

---

## Demostración de Propiedades

### Cumplimiento de la restricción asignada

**Mi restricción**: "Debe funcionar correctamente con inversión de polaridad"

**Demostración formal:**

La codificación es **diferencial** - depende del nivel anterior, no de niveles absolutos.

**Ejemplo 1: Bitstream "00"**
- Normal: Nivel anterior=L → Patrón=LLL
- Invertido: Nivel anterior=H → Patrón=HHH
- Ambos decodifican a "00" ✅

**Ejemplo 2: Bitstream "11010010"**

Normal (nivel inicial L):
```
11 → LHH (nivel anterior L, índice 3)
01 → HHL (nivel anterior H, índice 1)
00 → LLL (nivel anterior L, índice 0)
10 → LHL (nivel anterior L, índice 2)
Señal: LHHHHLLLLLHL
```

Invertido (nivel inicial H):
```
11 → HLL (nivel anterior H, índice 3)
01 → LLH (nivel anterior L, índice 1)
00 → HHH (nivel anterior H, índice 0)
10 → HLH (nivel anterior H, índice 2)
Señal: HLLLLHHHHHLH
```

Ambas señales decodifican correctamente a "11010010" ✅

**Ejemplo 3: Bitstream "101100"**

Normal:
```
10 → LHL
11 → HLL
00 → LLL
Señal: LHLHLLLLL
```

Invertido:
```
10 → HLH
11 → LHH
00 → HHH
Señal: HLHLHHHHH
```

Ambas decodifican a "101100" ✅

### Análisis de eficiencia

- Bits de entrada: N
- Símbolos de salida: (N/2) × 3 = 1.5N
- **Eficiencia**: N / 1.5N × 100 = **66.67%**

Para 1000 bits:
- Bits útiles: 1000
- Símbolos transmitidos: 1500
- Overhead: 50%

### Autosincronización

**Parcialmente autosincronizante:**
- Máximo 3 símbolos consecutivos iguales (patrón "000" o "111")
- Garantiza al menos una transición cada 3 símbolos
- Suficiente para enlaces de baja tasa de error
- No tan robusto como Manchester (transición por bit)

---

## Ejemplo Completo

### Entrada
```
Bitstream: 11010010
```

### Proceso de codificación

```
Paso 1: Dividir en pares
  11 | 01 | 00 | 10

Paso 2: Codificar cada par
  Nivel inicial: L
  
  Par "11" (índice 3):
    Nivel anterior = L → Tabla LOW
    Patrón = LHH
    Nuevo nivel = H
  
  Par "01" (índice 1):
    Nivel anterior = H → Tabla HIGH
    Patrón = HHL
    Nuevo nivel = L
  
  Par "00" (índice 0):
    Nivel anterior = L → Tabla LOW
    Patrón = LLL
    Nuevo nivel = L
  
  Par "10" (índice 2):
    Nivel anterior = L → Tabla LOW
    Patrón = LHL
    Nuevo nivel = L

Paso 3: Concatenar
  LHH + HHL + LLL + LHL = LHHHHLLLLLHL
```

### Salida codificada
```
LHHHHLLLLLHL
```

### Diagrama de señal
```
Tiempo:    0   1   2   3   4   5   6   7   8   9  10  11
Entrada:   1   1   0   1   0   0   1   0
Salida:    L   H   H   H   H   L   L   L   L   L   H   L

       H   ─────────────
           │           │
       L   ─           ─────────────────   ─
           └───────────┘                 └─┘
```

---

## Comparación con Otros Esquemas

| Propiedad | ZR-Code | NRZ | Manchester | 4B/5B |
|-----------|---------|-----|------------|-------|
| Eficiencia | **66.67%** | 100% | 50% | 80% |
| Autosincronizante | Parcial | No | Sí | Parcial |
| Max. símbolos iguales consecutivos | **3** | ∞ | 2 | 3 |
| Resistente a inversión de polaridad | **Sí** | No | Sí | No |
| Detección de errores | **Sí** | No | Sí | Sí |
| Complejidad de implementación | Media | Baja | Media | Alta |

---

## Implementación

### Firma de funciones

```c
char *encode_custom(const char *bitstream);
char *decode_custom(const char *encoded);
```

### Notas de implementación

- **Tablas de lookup**: Uso de arrays estáticos para codificación rápida
- **Validación**: Decodificación verifica patrones válidos
- **Padding**: Bitstreams impares se rellenan con '0'
- **Memoria dinámica**: Uso de `malloc_string()` para asignación segura
- **Manejo de errores**: Retorna NULL si detecta patrones inválidos

**Complejidad:**
- Codificación: O(n) donde n = longitud del bitstream
- Decodificación: O(m) donde m = longitud de la señal codificada
- Espacio: O(1.5n) para almacenar la señal codificada

---

## Conclusiones

### Ventajas

1. **Resistencia a inversión de polaridad** - Cumple la restricción asignada
2. **Eficiencia aceptable** - 66.67%, mejor que Manchester
3. **Detección de errores** - Patrones inválidos son detectables
4. **Autosincronización parcial** - Máximo 3 símbolos iguales
5. **Implementación simple** - Tablas de lookup eficientes

### Desventajas

1. **Menor eficiencia que 4B/5B** - 66.67% vs 80%
2. **No es estándar** - No hay soporte de hardware existente
3. **Overhead significativo** - 50% más símbolos que bits

### Escenarios de uso

ZR-Code sería útil en:
- **Sistemas sin referencia de polaridad** - Donde la señal puede invertirse
- **Enlaces ópticos bidireccionales** - Donde la polaridad puede cambiar
- **Sistemas legacy** - Que requieren compatibilidad con inversión
- **Aplicaciones educativas** - Para demostrar codificación diferencial

### Comparación con alternativas

- **vs Manchester**: Más eficiente (66.67% vs 50%) pero menos autosincronizante
- **vs 4B/5B**: Menos eficiente (66.67% vs 80%) pero resistente a inversión
- **vs NRZ/NRZI**: Menos eficiente pero con características adicionales valiosas

---

**Implementado por:** Zain Rondon  
**Fecha:** 2025-12-08  
**Versión:** 1.0
