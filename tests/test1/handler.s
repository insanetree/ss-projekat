# file: handler.s
#.equ term_out, 0xFFFFFF00
#.equ term_in, 0xFFFFFF04
#.equ ascii_code, 84 # ascii(’T’)
.extern my_counter
.global handler
.section my_code_handler
handler:
	push %r1
	push %r2
	csrrd %cause, %r1
	ld $2, %r2
	beq %r1, %r2, my_isr_timer
	ld $3, %r2
	beq %r1, %r2, my_isr_terminal
# obrada prekida od tajmera
my_isr_timer:
	ld $84 , %r1
	st %r1, 0xFFFFFF00
	jmp finish
# obrada prekida od terminala
my_isr_terminal:
	ld 0xFFFFFF04, %r1
	st %r1, 0xFFFFFF00
	ld my_counter, %r1
	ld $1, %r2
	add %r2, %r1
	st %r1, my_counter
finish:
	pop %r2
	pop %r1
	iret
.section my_test_data
jedan:
.word 0x11111111
dva:
.word 0x22222222
	ld jedan, %r1
	ld dva, %r1
.end