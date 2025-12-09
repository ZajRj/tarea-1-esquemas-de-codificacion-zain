#ifndef ANALYSIS_H
#define ANALYSIS_H

/**
 * @file analysis.h
 * @brief Análisis cuantitativo de esquemas de codificación (Parte B)
 */

// ============================================
// Funciones de análisis
// ============================================

/**
 * @brief Ejecuta todos los análisis cuantitativos
 * @param ber Bit Error Rate a usar (ej: 0.08)
 */
void run_all_analysis(double ber);

/**
 * @brief Calcula el overhead de codificación para 1000 bits
 */
void run_analysis_overhead(void);

/**
 * @brief Ejecuta análisis estadístico de errores (100 simulaciones)
 * @param ber Bit Error Rate
 */
void run_analysis_error_stats(double ber);

/**
 * @brief Genera curva BER vs Tasa de Error Efectiva
 */
void run_analysis_ber_curve(void);

/**
 * @brief Analiza resistencia a ráfagas de errores
 */
void run_analysis_burst_noise(void);

#endif // ANALYSIS_H
