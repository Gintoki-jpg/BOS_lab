
.section .text
.global switch_to

switch_to:
	push %ebp;
	mov %esp,%ebp;

	push %edi
	push %esi
	push %ebx
	push %edx
	push %ecx
	push %eax
	pushfl

	mov cur,%eax
	mov %esp,task(,%eax,4)
	mov 8(%ebp),%eax
	mov %eax,cur
	mov task(,%eax,4),%esp

	popfl
	popl %eax
	popl %edx
	popl %ecx
	popl %ebx
	popl %esi
	popl %edi

	popl %ebp
	ret

