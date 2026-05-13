OPTION CASEMAP:NONE

.data
PUBLIC unlock_all_mid_resume
unlock_all_mid_resume DQ 0

.code
PUBLIC unlock_all_mid_stub
unlock_all_mid_stub PROC
	mov	r12b, 0
	or	r15b, r12b
	jmp	QWORD PTR [unlock_all_mid_resume]
unlock_all_mid_stub ENDP

END
