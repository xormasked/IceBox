OPTION CASEMAP:NONE

.data
PUBLIC better_light_resume_normal
better_light_resume_normal DQ 0

PUBLIC better_light_branch_taken
better_light_branch_taken DQ 0

.code
PUBLIC better_light_mid_stub
better_light_mid_stub PROC
	cmp	dword ptr [rax+5Ch], 0
	jne	taken
	jmp	QWORD PTR [better_light_resume_normal]
taken:
	jmp	QWORD PTR [better_light_branch_taken]
better_light_mid_stub ENDP

END
