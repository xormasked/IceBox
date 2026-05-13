OPTION CASEMAP:NONE

.data
PUBLIC no_spread_mid_resume
no_spread_mid_resume DQ 0

.code
PUBLIC no_spread_mid_stub
no_spread_mid_stub PROC
	jmp	QWORD PTR [no_spread_mid_resume]
no_spread_mid_stub ENDP

END
