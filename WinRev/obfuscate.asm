.code
Obfus proc
	call fake1
fake1:
	pop rax
	; mov p_stack,eax // TODO:add a offset variable to check
	call true1
	db 0eah
	jmp fake2
true1:
	pop rbx
	inc rbx
	push rbx
	mov rax, 54536566h
	ret
fake2:
	call true2
	mov rbx, 55694855h
	jmp ed
true2:
	mov rbx, 51525354h
	pop rbx
	mov rbx, offset ed
	push rbx
	ret
ed:
	mov rbx, 43454647h
Obfus endp
end