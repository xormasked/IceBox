
OPTION CASEMAP:NONE

.data
ALIGN 16
PUBLIC world_edit_params
world_edit_params REAL4 1.10, 1.10, 1.10, 4.0

PUBLIC world_edit_mid_resume_storage
world_edit_mid_resume_storage DQ 0

.code
PUBLIC world_edit_mid_stub
world_edit_mid_stub PROC
	movdqa	xmm1, xmmword ptr [world_edit_params]
	jmp	QWORD PTR [world_edit_mid_resume_storage]
world_edit_mid_stub ENDP

END
