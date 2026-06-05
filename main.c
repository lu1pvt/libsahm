/* ==============================================================================
 * PROJECT: SAHM v4.1 (Hyper-Modular Arithmetic System)
 * FILE:    main.c
 * * DESCRIPTION: 
 * Hardware instruction set stress benchmark. Validates zero-crossings, underflow 
 * evasion, sign asymmetry (XOR logic gates), and Kernel Panic protections.
 * * AUTHOR:
 * Volney Torres Almendro
 * Low-Level Computing Laboratory
 * San Juan, Argentina
 * DATE: June 2026
 * ============================================================================== */
#include <stdio.h>
#include "unified_sahm.h"

int main() {
    printf("=================================================================\n");
    printf("   INICIANDO BENCHMARK DE ESTRÉS: SAHM v4.1 (ALU UNIFICADA)\n");
    printf("=================================================================\n\n");

    // =================================================================
    // TEST 1: RESTA QUE CRUZA EL CERO (Evadiendo el Underflow)
    // =================================================================
    printf("--- TEST 1: Resta que cruza el cero (5 - 10) ---\n");
    UnifiedModZ A = encode_from_int(5);
    UnifiedModZ B = encode_from_int(10);
    
    printf("Minuendo  (A) : "); print_unified_z(A);
    printf("Sustraendo(B) : "); print_unified_z(B);
    
    UnifiedModZ Res_Sub = sub_unified(A, B);
    printf("--> RESULTADO : "); print_unified_z(Res_Sub);
    printf("[!] Expectativa: Magnitud 5, Angulo 0x80000000 (Clasico: -5)\n\n");


    // =================================================================
    // TEST 2: MULTIPLICACIÓN PESADA CON SIGNOS (Positivo * Negativo)
    // =================================================================
    printf("--- TEST 2: Multiplicacion pesada y asimetrica (14 * -12) ---\n");
    UnifiedModZ C = encode_from_int(14);
    UnifiedModZ D = encode_from_int(-12);
    
    printf("Factor 1  (C) : "); print_unified_z(C);
    printf("Factor 2  (D) : "); print_unified_z(D);
    
    UnifiedModZ Res_Mul1 = mul_unified(C, D);
    printf("--> RESULTADO : "); print_unified_z(Res_Mul1);
    printf("[!] Expectativa: Magnitud 168, Angulo 0x80000000 (Clasico: -168)\n\n");


    // =================================================================
    // TEST 3: EL DEMONIO DE LOS SIGNOS (Negativo * Negativo)
    // =================================================================
    printf("--- TEST 3: Ley de signos por hardware XOR (-15 * -7) ---\n");
    UnifiedModZ E = encode_from_int(-15);
    UnifiedModZ F = encode_from_int(-7);
    
    printf("Factor 1  (E) : "); print_unified_z(E);
    printf("Factor 2  (F) : "); print_unified_z(F);
    
    UnifiedModZ Res_Mul2 = mul_unified(E, F);
    printf("--> RESULTADO : "); print_unified_z(Res_Mul2);
    printf("[!] Expectativa: Magnitud 105, Angulo 0x00000000 (Clasico: +105)\n\n");


    // =================================================================
    // TEST 4: DIVISIÓN EXACTA (ALU Híbrida)
    // =================================================================
    printf("--- TEST 4: Division Hibrida con signos cruzados (84 / -6) ---\n");
    UnifiedModZ G = encode_from_int(84);
    UnifiedModZ H = encode_from_int(-6);
    
    printf("Dividendo (G) : "); print_unified_z(G);
    printf("Divisor   (H) : "); print_unified_z(H);
    
    UnifiedModZ Res_Div = div_unified(G, H);
    printf("--> RESULTADO : "); print_unified_z(Res_Div);
    printf("[!] Expectativa: Magnitud 14, Angulo 0x80000000 (Clasico: -14)\n\n");


    // =================================================================
    // TEST 5: KERNEL PANIC (División por cero)
    // =================================================================
    printf("--- TEST 5: Resistencia a fallos (50 / 0) ---\n");
    UnifiedModZ I = encode_from_int(50);
    UnifiedModZ J = encode_from_int(0);
    
    printf("Dividendo (I) : "); print_unified_z(I);
    printf("Divisor   (J) : "); print_unified_z(J);
    
    printf("--> EJECUTANDO INSTRUCCIÓN:\n");
    UnifiedModZ Res_Zero = div_unified(I, J);
    printf("--> RESULTADO : "); print_unified_z(Res_Zero);
    printf("[!] Expectativa: Mensaje de error y devolucion segura del dividendo.\n\n");


    printf("=================================================================\n");
    printf("   FIN DEL BENCHMARK. SISTEMA ESTABLE Y OPERATIVO.\n");
    printf("=================================================================\n");

    return 0;
}
