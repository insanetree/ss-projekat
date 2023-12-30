# file: main.s
#.equ tim_cfg, 0xFFFFFF10
#.equ init_sp, 0xFFFFFF00
.extern handler
.section my_code_main
	ld $0xFFFFFF00, %sp
	ld $handler, %r1
	csrwr %r1, %handler
	ld $0x1, %r1
	st %r1, 0xFFFFFF10
wait:
	ld my_counter, %r1
	ld $5, %r2
	bne %r1, %r2, wait
	halt
.global my_counter
.section my_data
my_counter:
	.word 0
.section my_test_data
tri:
.word 0x33333333
cetiri:
.word 0x44444444
	ld tri, %r1
	ld cetiri, %r1
.end