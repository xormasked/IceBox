
OPTION CASEMAP:NONE

.data
PUBLIC run_shoot_mid_resume_a
run_shoot_mid_resume_a DQ 0

PUBLIC run_shoot_mid_resume_b
run_shoot_mid_resume_b DQ 0

.code
PUBLIC run_shoot_mid_stub_a
run_shoot_mid_stub_a PROC
    cmp     byte ptr [rcx+80h], 1
    jmp     QWORD PTR [run_shoot_mid_resume_a]
run_shoot_mid_stub_a ENDP

PUBLIC run_shoot_mid_stub_b
run_shoot_mid_stub_b PROC
    cmp     byte ptr [rcx+80h], 1
    jmp     QWORD PTR [run_shoot_mid_resume_b]
run_shoot_mid_stub_b ENDP

END
