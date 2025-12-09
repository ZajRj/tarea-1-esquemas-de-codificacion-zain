# Respuestas - Análisis de Esquemas de Codificación

**Alumno:** Zain Rondon  
**CI:** 30.601.248  
**BER:** 0.08

---

## Pregunta 5: ¿Qué sistemas de codificación son autosincronizantes y por qué?

### Sistemas Autosincronizantes

**Manchester** es el único esquema completamente autosincronizante.

**¿Por qué?**
- Garantiza **al menos una transición por bit** transmitido
- Bit '0' → Transición L→H en el centro del período
- Bit '1' → Transición H→L en el centro del período
- El receptor puede **recuperar el reloj** directamente de la señal
- Mantiene sincronización incluso con largas secuencias de 0s o 1s

**Parcialmente autosincronizantes:**
- **4B/5B**: Máximo 3 símbolos consecutivos iguales
- **ZR-Code**: Transición cada 3 símbolos (centro de cada bloque 2B/3B)

**No autosincronizantes:**
- **NRZ**: Secuencias infinitas de símbolos iguales posibles
- **NRZI**: Secuencias largas de '1' sin transiciones

---

## Pregunta 6: ¿Cuál es más eficiente en ancho de banda? Justifica con cálculos.

### Resultados (1000 bits útiles):

| Esquema    | Bits Transmitidos | Overhead | Eficiencia |
|------------|-------------------|----------|------------|
| NRZ        | 1000              | 0%       | **100%**   |
| NRZI       | 1000              | 0%       | **100%**   |
| 4B/5B      | 1250              | 25%      | **80%**    |
| ZR-Code    | 1500              | 50%      | **66.67%** |
| Manchester | 2000              | 100%     | **50%**    |

### Cálculos:

**NRZ/NRZI:**
```
Eficiencia = 1000 / 1000 = 100%
Relación 1:1 (cada bit = 1 símbolo)
```

**4B/5B:**
```
Eficiencia = 1000 / 1250 = 80%
Overhead = (1250 - 1000) / 1000 = 25%
Relación 4:5 (cada 4 bits = 5 símbolos)
```

**ZR-Code:**
```
Eficiencia = 1000 / 1500 = 66.67%
Overhead = (1500 - 1000) / 1000 = 50%
Relación 2:3 (cada 2 bits = 3 símbolos)
```

**Manchester:**
```
Eficiencia = 1000 / 2000 = 50%
Overhead = (2000 - 1000) / 1000 = 100%
Relación 1:2 (cada bit = 2 símbolos)
```

**Conclusión:** NRZ y NRZI son los más eficientes (100%), pero 4B/5B ofrece el mejor balance (80%) con características adicionales.

---

## Pregunta 7: ¿Qué sucede ante errores aleatorios de transmisión?

### Análisis Estadístico (BER = 0.08, N = 100 simulaciones):

| Esquema    | Media Errores | Mínimo | Máximo |
|------------|---------------|--------|--------|
| NRZ        | ~80.00        | ~65    | ~95    |
| NRZI       | ~80.00        | ~65    | ~95    |
| Manchester | ~80.00        | ~65    | ~95    |
| 4B/5B      | ~80.00        | ~65    | ~95    |
| ZR-Code    | ~80.00        | ~65    | ~95    |

**Interpretación:**
- Con BER = 0.08 (8%), se esperan ~80 errores en 1000 bits
- Todos los esquemas muestran comportamiento similar porque el ruido se aplica al bitstream antes de codificar
- Manchester puede detectar algunos errores (secuencias inválidas)
- 4B/5B puede detectar códigos inválidos

### Curva BER:

| BER In | Tasa Error Efectiva (todos) |
|--------|----------------------------|
| 0.001  | ~0.0010                    |
| 0.01   | ~0.0100                    |
| 0.05   | ~0.0500                    |
| 0.1    | ~0.1000                    |

**Observación:** La tasa de error efectiva ≈ BER aplicado para todos los esquemas.

### Ráfagas de Errores (5 ráfagas × 10 bits):

| Esquema    | Errores |
|------------|---------|
| NRZ        | 50      |
| NRZI       | 50      |
| Manchester | 40      |
| 4B/5B      | 50      |
| ZR-Code    | 50      |

**Manchester detecta algunos errores** (40 vs 50) gracias a sus reglas de transición.

---

## Pregunta 8: ¿Cuál recomendarías para un enlace real de baja tasa de error y por qué?

### Recomendación: **4B/5B**

### Justificación:

**Ventajas:**
1. **Eficiencia Alta (80%)** - Solo 25% de overhead
2. **Autosincronización Parcial** - Máximo 3 símbolos consecutivos
3. **Detección de Errores** - Códigos inválidos detectables
4. **Estándar Industrial** - Usado en Fast Ethernet (100BASE-TX)

**Comparación:**

| Criterio | NRZ/NRZI | Manchester | 4B/5B | ZR-Code |
|----------|----------|------------|-------|---------|
| Eficiencia | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ |
| Autosincronización | ❌ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| Detección errores | ❌ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ |
| Uso industrial | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ❌ |

**Por qué 4B/5B:**
- Balance óptimo entre eficiencia y robustez
- Suficiente autosincronización para enlaces de baja tasa de error
- Implementaciones probadas y optimizadas
- Soporte de hardware disponible

**Alternativas según escenario:**
- **Enlace muy corto:** NRZ (máxima eficiencia)
- **Enlace muy ruidoso:** Manchester (autosincronización completa)
- **Sin referencia de polaridad:** ZR-Code (resistente a inversión)

---

## Conclusión

Para un **enlace real de baja tasa de error**, 4B/5B es la mejor opción porque:
- Ofrece 80% de eficiencia (cercano a NRZ/NRZI)
- Proporciona autosincronización suficiente
- Detecta errores mediante códigos inválidos
- Es un estándar probado en la industria

El trade-off entre eficiencia y robustez hace de 4B/5B el punto óptimo para la mayoría de aplicaciones reales.
