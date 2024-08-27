	mvi 	A, 0x0
	mvi 	B, 0x1

#(n + 2)th fib number
	mvi 	E, 0x5
#to be able to subtract one
	mvi 	F, 0x1

LABEL loop
	#C = A + B
	mvr 	C, A
	add 	C, B

	#A = B
	mvr 	A, B
	#B = C
	mvr 	B, C

	#M[0] = C
	str 	C, ZERO

	#E -= F
	sub 	E, F

	jnz 	E, loop
