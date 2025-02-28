; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=armv6-unknown-linux-gnu | FileCheck %s --check-prefixes=ARMV6
; RUN: llc < %s -mtriple=armv7-unknown-linux-gnu | FileCheck %s --check-prefixes=ARMV7

define { i128, i8 } @muloti_test(i128 %l, i128 %r) unnamed_addr #0 {
; ARMV6-LABEL: muloti_test:
; ARMV6:       @ %bb.0: @ %start
; ARMV6-NEXT:    push {r4, r5, r6, r7, r8, r9, r10, r11, lr}
; ARMV6-NEXT:    sub sp, sp, #28
; ARMV6-NEXT:    ldr r4, [sp, #72]
; ARMV6-NEXT:    mov r7, r0
; ARMV6-NEXT:    str r0, [sp, #4] @ 4-byte Spill
; ARMV6-NEXT:    ldr r12, [sp, #64]
; ARMV6-NEXT:    umull r1, r0, r2, r4
; ARMV6-NEXT:    ldr r5, [sp, #68]
; ARMV6-NEXT:    str r1, [r7]
; ARMV6-NEXT:    ldr r1, [sp, #76]
; ARMV6-NEXT:    umull r7, r6, r1, r12
; ARMV6-NEXT:    str r6, [sp, #8] @ 4-byte Spill
; ARMV6-NEXT:    umull r6, r9, r5, r4
; ARMV6-NEXT:    add r7, r6, r7
; ARMV6-NEXT:    umull r4, r6, r12, r4
; ARMV6-NEXT:    str r4, [sp, #16] @ 4-byte Spill
; ARMV6-NEXT:    mov r4, #0
; ARMV6-NEXT:    adds r8, r6, r7
; ARMV6-NEXT:    ldr r6, [sp, #80]
; ARMV6-NEXT:    adc r7, r4, #0
; ARMV6-NEXT:    ldr r4, [sp, #84]
; ARMV6-NEXT:    str r7, [sp, #24] @ 4-byte Spill
; ARMV6-NEXT:    umull r12, lr, r3, r6
; ARMV6-NEXT:    umull r11, r7, r4, r2
; ARMV6-NEXT:    add r12, r11, r12
; ARMV6-NEXT:    umull r11, r10, r6, r2
; ARMV6-NEXT:    adds r12, r10, r12
; ARMV6-NEXT:    mov r10, #0
; ARMV6-NEXT:    adc r6, r10, #0
; ARMV6-NEXT:    str r6, [sp, #20] @ 4-byte Spill
; ARMV6-NEXT:    ldr r6, [sp, #16] @ 4-byte Reload
; ARMV6-NEXT:    adds r6, r6, r11
; ARMV6-NEXT:    str r6, [sp, #12] @ 4-byte Spill
; ARMV6-NEXT:    adc r6, r8, r12
; ARMV6-NEXT:    str r6, [sp, #16] @ 4-byte Spill
; ARMV6-NEXT:    ldr r6, [sp, #72]
; ARMV6-NEXT:    mov r12, #0
; ARMV6-NEXT:    umull r2, r8, r2, r1
; ARMV6-NEXT:    umlal r0, r12, r3, r6
; ARMV6-NEXT:    adds r0, r2, r0
; ARMV6-NEXT:    ldr r2, [sp, #4] @ 4-byte Reload
; ARMV6-NEXT:    adcs r8, r12, r8
; ARMV6-NEXT:    adc r12, r10, #0
; ARMV6-NEXT:    cmp lr, #0
; ARMV6-NEXT:    str r0, [r2, #4]
; ARMV6-NEXT:    movne lr, #1
; ARMV6-NEXT:    ldr r11, [sp, #8] @ 4-byte Reload
; ARMV6-NEXT:    cmp r7, #0
; ARMV6-NEXT:    movne r7, #1
; ARMV6-NEXT:    ldr r0, [sp, #64]
; ARMV6-NEXT:    cmp r11, #0
; ARMV6-NEXT:    umlal r8, r12, r3, r1
; ARMV6-NEXT:    movne r11, #1
; ARMV6-NEXT:    cmp r9, #0
; ARMV6-NEXT:    movne r9, #1
; ARMV6-NEXT:    orrs r10, r0, r5
; ARMV6-NEXT:    ldr r0, [sp, #80]
; ARMV6-NEXT:    movne r10, #1
; ARMV6-NEXT:    ldr r6, [sp, #12] @ 4-byte Reload
; ARMV6-NEXT:    orrs r0, r0, r4
; ARMV6-NEXT:    movne r0, #1
; ARMV6-NEXT:    cmp r4, #0
; ARMV6-NEXT:    movne r4, #1
; ARMV6-NEXT:    cmp r3, #0
; ARMV6-NEXT:    movne r3, #1
; ARMV6-NEXT:    cmp r5, #0
; ARMV6-NEXT:    movne r5, #1
; ARMV6-NEXT:    cmp r1, #0
; ARMV6-NEXT:    movne r1, #1
; ARMV6-NEXT:    adds r6, r8, r6
; ARMV6-NEXT:    str r6, [r2, #8]
; ARMV6-NEXT:    and r1, r5, r1
; ARMV6-NEXT:    ldr r6, [sp, #16] @ 4-byte Reload
; ARMV6-NEXT:    orr r1, r1, r9
; ARMV6-NEXT:    orr r1, r1, r11
; ARMV6-NEXT:    and r0, r10, r0
; ARMV6-NEXT:    adcs r6, r12, r6
; ARMV6-NEXT:    str r6, [r2, #12]
; ARMV6-NEXT:    ldr r6, [sp, #24] @ 4-byte Reload
; ARMV6-NEXT:    orr r1, r1, r6
; ARMV6-NEXT:    orr r0, r0, r1
; ARMV6-NEXT:    and r1, r4, r3
; ARMV6-NEXT:    orr r1, r1, r7
; ARMV6-NEXT:    ldr r3, [sp, #20] @ 4-byte Reload
; ARMV6-NEXT:    orr r1, r1, lr
; ARMV6-NEXT:    orr r1, r1, r3
; ARMV6-NEXT:    orr r0, r0, r1
; ARMV6-NEXT:    mov r1, #0
; ARMV6-NEXT:    adc r1, r1, #0
; ARMV6-NEXT:    orr r0, r0, r1
; ARMV6-NEXT:    and r0, r0, #1
; ARMV6-NEXT:    strb r0, [r2, #16]
; ARMV6-NEXT:    add sp, sp, #28
; ARMV6-NEXT:    pop {r4, r5, r6, r7, r8, r9, r10, r11, pc}
;
; ARMV7-LABEL: muloti_test:
; ARMV7:       @ %bb.0: @ %start
; ARMV7-NEXT:    push {r4, r5, r6, r7, r8, r9, r10, r11, lr}
; ARMV7-NEXT:    sub sp, sp, #44
; ARMV7-NEXT:    ldr r8, [sp, #88]
; ARMV7-NEXT:    mov r9, r0
; ARMV7-NEXT:    ldr r7, [sp, #96]
; ARMV7-NEXT:    ldr lr, [sp, #100]
; ARMV7-NEXT:    umull r0, r5, r2, r8
; ARMV7-NEXT:    ldr r4, [sp, #80]
; ARMV7-NEXT:    str r0, [sp, #32] @ 4-byte Spill
; ARMV7-NEXT:    umull r1, r0, r3, r7
; ARMV7-NEXT:    str r0, [sp, #4] @ 4-byte Spill
; ARMV7-NEXT:    umull r0, r11, lr, r2
; ARMV7-NEXT:    str r1, [sp, #20] @ 4-byte Spill
; ARMV7-NEXT:    ldr r1, [sp, #92]
; ARMV7-NEXT:    str r0, [sp] @ 4-byte Spill
; ARMV7-NEXT:    umull r0, r10, r7, r2
; ARMV7-NEXT:    mov r7, r1
; ARMV7-NEXT:    umull r6, r12, r1, r4
; ARMV7-NEXT:    str r0, [sp, #40] @ 4-byte Spill
; ARMV7-NEXT:    ldr r0, [sp, #84]
; ARMV7-NEXT:    str r6, [sp, #24] @ 4-byte Spill
; ARMV7-NEXT:    umull r6, r1, r0, r8
; ARMV7-NEXT:    str r6, [sp, #16] @ 4-byte Spill
; ARMV7-NEXT:    umull r6, r2, r2, r7
; ARMV7-NEXT:    mov r7, r4
; ARMV7-NEXT:    str r6, [sp, #8] @ 4-byte Spill
; ARMV7-NEXT:    str r2, [sp, #12] @ 4-byte Spill
; ARMV7-NEXT:    umull r2, r6, r4, r8
; ARMV7-NEXT:    str r2, [sp, #36] @ 4-byte Spill
; ARMV7-NEXT:    ldr r2, [sp, #32] @ 4-byte Reload
; ARMV7-NEXT:    str r6, [sp, #28] @ 4-byte Spill
; ARMV7-NEXT:    mov r6, #0
; ARMV7-NEXT:    str r2, [r9]
; ARMV7-NEXT:    umlal r5, r6, r3, r8
; ARMV7-NEXT:    ldr r2, [sp, #20] @ 4-byte Reload
; ARMV7-NEXT:    ldr r4, [sp] @ 4-byte Reload
; ARMV7-NEXT:    add r4, r4, r2
; ARMV7-NEXT:    adds r2, r10, r4
; ARMV7-NEXT:    str r2, [sp, #20] @ 4-byte Spill
; ARMV7-NEXT:    mov r2, #0
; ARMV7-NEXT:    adc r2, r2, #0
; ARMV7-NEXT:    cmp r12, #0
; ARMV7-NEXT:    str r2, [sp, #32] @ 4-byte Spill
; ARMV7-NEXT:    movwne r12, #1
; ARMV7-NEXT:    cmp r1, #0
; ARMV7-NEXT:    ldr r2, [sp, #96]
; ARMV7-NEXT:    movwne r1, #1
; ARMV7-NEXT:    orrs r10, r7, r0
; ARMV7-NEXT:    movwne r10, #1
; ARMV7-NEXT:    orrs r7, r2, lr
; ARMV7-NEXT:    ldr r2, [sp, #92]
; ARMV7-NEXT:    movwne r7, #1
; ARMV7-NEXT:    cmp r0, #0
; ARMV7-NEXT:    movwne r0, #1
; ARMV7-NEXT:    cmp r2, #0
; ARMV7-NEXT:    mov r4, r2
; ARMV7-NEXT:    mov r8, r2
; ARMV7-NEXT:    ldr r2, [sp, #8] @ 4-byte Reload
; ARMV7-NEXT:    movwne r4, #1
; ARMV7-NEXT:    and r0, r0, r4
; ARMV7-NEXT:    mov r4, #0
; ARMV7-NEXT:    adds r5, r2, r5
; ARMV7-NEXT:    str r5, [r9, #4]
; ARMV7-NEXT:    orr r0, r0, r1
; ARMV7-NEXT:    ldr r1, [sp, #24] @ 4-byte Reload
; ARMV7-NEXT:    ldr r2, [sp, #16] @ 4-byte Reload
; ARMV7-NEXT:    and r5, r10, r7
; ARMV7-NEXT:    orr r0, r0, r12
; ARMV7-NEXT:    mov r12, #0
; ARMV7-NEXT:    add r1, r2, r1
; ARMV7-NEXT:    ldr r2, [sp, #12] @ 4-byte Reload
; ARMV7-NEXT:    adcs r2, r6, r2
; ARMV7-NEXT:    ldr r6, [sp, #28] @ 4-byte Reload
; ARMV7-NEXT:    adc r7, r4, #0
; ARMV7-NEXT:    adds r1, r6, r1
; ARMV7-NEXT:    umlal r2, r7, r3, r8
; ARMV7-NEXT:    adc r4, r4, #0
; ARMV7-NEXT:    orr r0, r0, r4
; ARMV7-NEXT:    orr r0, r5, r0
; ARMV7-NEXT:    ldr r4, [sp, #40] @ 4-byte Reload
; ARMV7-NEXT:    ldr r5, [sp, #36] @ 4-byte Reload
; ARMV7-NEXT:    adds r5, r5, r4
; ARMV7-NEXT:    ldr r4, [sp, #20] @ 4-byte Reload
; ARMV7-NEXT:    adc r1, r1, r4
; ARMV7-NEXT:    ldr r4, [sp, #4] @ 4-byte Reload
; ARMV7-NEXT:    cmp r4, #0
; ARMV7-NEXT:    movwne r4, #1
; ARMV7-NEXT:    cmp r3, #0
; ARMV7-NEXT:    movwne r3, #1
; ARMV7-NEXT:    cmp lr, #0
; ARMV7-NEXT:    movwne lr, #1
; ARMV7-NEXT:    cmp r11, #0
; ARMV7-NEXT:    movwne r11, #1
; ARMV7-NEXT:    adds r2, r2, r5
; ARMV7-NEXT:    and r3, lr, r3
; ARMV7-NEXT:    str r2, [r9, #8]
; ARMV7-NEXT:    adcs r1, r7, r1
; ARMV7-NEXT:    str r1, [r9, #12]
; ARMV7-NEXT:    orr r1, r3, r11
; ARMV7-NEXT:    ldr r2, [sp, #32] @ 4-byte Reload
; ARMV7-NEXT:    orr r1, r1, r4
; ARMV7-NEXT:    orr r1, r1, r2
; ARMV7-NEXT:    orr r0, r0, r1
; ARMV7-NEXT:    adc r1, r12, #0
; ARMV7-NEXT:    orr r0, r0, r1
; ARMV7-NEXT:    and r0, r0, #1
; ARMV7-NEXT:    strb r0, [r9, #16]
; ARMV7-NEXT:    add sp, sp, #44
; ARMV7-NEXT:    pop {r4, r5, r6, r7, r8, r9, r10, r11, pc}
start:
  %0 = tail call { i128, i1 } @llvm.umul.with.overflow.i128(i128 %l, i128 %r) #2
  %1 = extractvalue { i128, i1 } %0, 0
  %2 = extractvalue { i128, i1 } %0, 1
  %3 = zext i1 %2 to i8
  %4 = insertvalue { i128, i8 } undef, i128 %1, 0
  %5 = insertvalue { i128, i8 } %4, i8 %3, 1
  ret { i128, i8 } %5
}

; Function Attrs: nounwind readnone speculatable
declare { i128, i1 } @llvm.umul.with.overflow.i128(i128, i128) #1

attributes #0 = { nounwind readnone uwtable }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind }
