/* =================================================================
 * UNIFIED HYPER-MODULAR LIBRARY (SAHM v4.1)
 * Integracion CRT (Teorema Chino del Resto) + ALU Completa
 * ================================================================= */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_COPRIMES 3
// Nuestro universo base: {2, 3, 5}
const uint32_t COPRIMES[NUM_COPRIMES] = {2, 3, 5};
const uint32_t D_MASTER = 30; // 2 * 3 * 5

typedef struct {
    uint32_t q;                         // Ciclos Maestros
    uint32_t c_rem[NUM_COPRIMES];       // Espectro de Residuos
    uint32_t theta;                     // Direccion: 0 = Positivo, 0x80000000 = Negativo
    uint8_t lazy_sqrt;                  
} UnifiedModZ;


/* =================================================================
 * 1. DECODIFICADOR HARDWARE (Teorema Chino del Resto)
 * ================================================================= */
// Reconstruye el residuo maestro a partir del espectro aislado.
// Pesos ortogonales para {2, 3, 5}: 15, 10, 6.
uint32_t decode_crt(uint32_t r2, uint32_t r3, uint32_t r5) {
    return (r2 * 15 + r3 * 10 + r5 * 6) % D_MASTER;
}


/* =================================================================
 * 2. PUERTOS I/O: TRANSFORMACIÓN Y ANTITRANSFORMACIÓN
 * ================================================================= */

// TRANSFORMADOR: Entero Tradicional -> Espécimen SAHM
UnifiedModZ encode_from_int(int32_t numero_clasico) {
    UnifiedModZ z;
    uint32_t magnitud = abs(numero_clasico);
    
    // Asignacion de rotacion (Euler BAM)
    z.theta = (numero_clasico < 0) ? 0x80000000 : 0x00000000;
    
    // Extraccion de ciclos y restos
    z.q = magnitud / D_MASTER;
    uint32_t base_rem = magnitud % D_MASTER;
    
    // Dispersión en el prisma coprimo
    for(int i = 0; i < NUM_COPRIMES; i++) {
        z.c_rem[i] = base_rem % COPRIMES[i];
    }
    z.lazy_sqrt = 0;
    return z;
}

// ANTITRANSFORMADOR: Espécimen SAHM -> Entero Tradicional
int32_t decode_to_int(UnifiedModZ z) {
    // 1. Reconstruir magnitud
    uint32_t base_rem = decode_crt(z.c_rem[0], z.c_rem[1], z.c_rem[2]);
    uint32_t magnitud = (z.q * D_MASTER) + base_rem;
    
    // 2. Aplicar proyeccion de Euler
    if (z.theta == 0x80000000) {
        return -(int32_t)magnitud;
    }
    return (int32_t)magnitud;
}


/* =================================================================
 * 3. ALU MATEMÁTICA: INSTRUCCIONES DE HARDWARE
 * ================================================================= */

// INSTRUCCIÓN ADD (Suma con parche CRT)
UnifiedModZ add_unified(UnifiedModZ A, UnifiedModZ B) {
    UnifiedModZ Res;
    Res.theta = A.theta; // Asumimos misma direccion para simplificar la ALU basica
    Res.lazy_sqrt = 0;
    
    uint32_t base_A = decode_crt(A.c_rem[0], A.c_rem[1], A.c_rem[2]);
    uint32_t base_B = decode_crt(B.c_rem[0], B.c_rem[1], B.c_rem[2]);
    
    // Deteccion de Acarreo Maestro
    uint32_t carry = ((base_A + base_B) >= D_MASTER) ? 1 : 0;
    Res.q = A.q + B.q + carry;
    
    // Suma paralela puramente discreta
    for(int i = 0; i < NUM_COPRIMES; i++) {
        Res.c_rem[i] = (A.c_rem[i] + B.c_rem[i]) % COPRIMES[i];
    }
    return Res;
}

// INSTRUCCIÓN SUB PARCHEADA (Prevención de Underflow y Rotación Geométrica)
UnifiedModZ sub_unified(UnifiedModZ A, UnifiedModZ B) {
    UnifiedModZ Res;
    Res.lazy_sqrt = 0;
    
    // 1. Reconstruimos los valores base para control de ALU
    uint32_t base_A = decode_crt(A.c_rem[0], A.c_rem[1], A.c_rem[2]);
    uint32_t base_B = decode_crt(B.c_rem[0], B.c_rem[1], B.c_rem[2]);
    
    // Calculamos magnitudes crudas para comparar tamaño
    uint32_t mag_A = (A.q * D_MASTER) + base_A;
    uint32_t mag_B = (B.q * D_MASTER) + base_B;
    
    // 2. DETECCIÓN DE UNDERFLOW (La magia de Euler al rescate)
    if (mag_A < mag_B) {
        // El resultado cruzará el cero. Invertimos la operacion (B - A)
        // Llama a la misma funcion pero al revés.
        Res = sub_unified(B, A); 
        
        // Rotamos el vector 180 grados (Invertimos el signo con XOR)
        Res.theta ^= 0x80000000; 
        return Res;
    }

    // 3. RESTA NORMAL (Garantizado que A >= B)
    Res.theta = A.theta; // Mantiene la direccion original
    
    // Deteccion de Préstamo Maestro
    uint32_t borrow = 0;
    if (base_A < base_B) borrow = 1;
    
    Res.q = A.q - B.q - borrow;
    
    // Resta paralela segura
    for(int i = 0; i < NUM_COPRIMES; i++) {
        if (A.c_rem[i] >= B.c_rem[i]) {
            Res.c_rem[i] = A.c_rem[i] - B.c_rem[i];
        } else {
            Res.c_rem[i] = (A.c_rem[i] + COPRIMES[i]) - B.c_rem[i];
        }
    }
    
    return Res;
}
// INSTRUCCIÓN MUL (Multiplicación Paralela)
UnifiedModZ mul_unified(UnifiedModZ A, UnifiedModZ B) {
    UnifiedModZ Res;
    
    // Álgebra de ángulos: La rotacion se suma
    // Si ambos son negativos (PI + PI = 2PI = 0), da positivo.
    Res.theta = A.theta ^ B.theta; // Un XOR a nivel de bits resuelve la magia de signos
    Res.lazy_sqrt = 0;
    
    uint32_t base_A = decode_crt(A.c_rem[0], A.c_rem[1], A.c_rem[2]);
    uint32_t base_B = decode_crt(B.c_rem[0], B.c_rem[1], B.c_rem[2]);
    
    // El desbordamiento de multiplicacion es enorme, la ALU calcula los ciclos extra
    uint32_t raw_mul = base_A * base_B;
    uint32_t extra_q = raw_mul / D_MASTER;
    
    // Ecuacion completa de ciclos
    Res.q = (A.q * B.q * D_MASTER) + (A.q * base_B) + (B.q * base_A) + extra_q;
    
    // Multiplicacion paralela pura en 1 ciclo de reloj
    for(int i = 0; i < NUM_COPRIMES; i++) {
        Res.c_rem[i] = (A.c_rem[i] * B.c_rem[i]) % COPRIMES[i];
    }
    return Res;
}

// UTILIDAD: Mostrar en pantalla
void print_unified_z(UnifiedModZ z) {
    printf("Z = ( %u + #c[ %u/2 %u/3 %u/5 ] ) * #s(0x%08X) --> [Magnitud Clasica: %d]\n", 
           z.q, z.c_rem[0], z.c_rem[1], z.c_rem[2], z.theta, decode_to_int(z));
}
// INSTRUCCIÓN DIV (Enfoque de ALU Híbrida)
UnifiedModZ div_unified(UnifiedModZ A, UnifiedModZ B) {
    UnifiedModZ Res;
    
    // 1. Ley de Signos por Hardware (XOR)
    // Igual que en la multiplicación: si los signos son iguales, da positivo.
    Res.theta = A.theta ^ B.theta; 
    Res.lazy_sqrt = 0;
    
    // 2. Extraer magnitudes puras mediante el CRT
    uint32_t mag_A = (A.q * D_MASTER) + decode_crt(A.c_rem[0], A.c_rem[1], A.c_rem[2]);
    uint32_t mag_B = (B.q * D_MASTER) + decode_crt(B.c_rem[0], B.c_rem[1], B.c_rem[2]);
    
    // Prevención de división por cero (Protección contra Kernel Panic)
    if (mag_B == 0) {
        printf("[FATAL ERROR ALU] Intento de division por cero.\n");
        return A; // Devuelve el dividendo intacto como control de fallos
    }
    
    // 3. División estricta en el núcleo híbrido
    uint32_t mag_Res = mag_A / mag_B;
    
    // 4. Re-empaquetado a través del prisma de coprimos
    Res.q = mag_Res / D_MASTER;
    uint32_t base_rem = mag_Res % D_MASTER;
    
    for(int i = 0; i < NUM_COPRIMES; i++) {
        Res.c_rem[i] = base_rem % COPRIMES[i];
    }
    
    return Res;
}
