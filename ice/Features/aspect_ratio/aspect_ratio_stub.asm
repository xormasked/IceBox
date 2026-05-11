

OPTION CASEMAP:NONE

.data
PUBLIC aspect_ratio_live
aspect_ratio_live REAL4 1.5

PUBLIC aspect_mid_resume_storage
aspect_mid_resume_storage DQ 0

.code
PUBLIC aspect_mid_stub
aspect_mid_stub PROC
    movss   xmm6, dword ptr [aspect_ratio_live]
    movss   dword ptr [rsi + 128h], xmm6
    jmp     QWORD PTR [aspect_mid_resume_storage]
aspect_mid_stub ENDP

END
