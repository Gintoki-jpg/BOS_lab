/*void switch_to(struct task_struct* next)*/

.section .text
.global switch_to
switch_to:
	call closealarm
	push %ebp
	mov %esp,%ebp /* in order to find flag */

	/*store the status*/
	push %edi
	push %esi
	push %ebx
	push %edx
	push %ecx
	push %eax
	pushfl

	/*prepare to change stack*/
	mov current,%eax /*store the esp*/
	mov %esp,8(%eax) /*store the origin position of esp*/
	mov 8(%ebp),%eax /*get the next thread id*/
	mov %eax,current /*set current the next thread*/
	mov 8(%eax),%esp /*switch_to the stack of next thread*/

	popfl
	popl %eax
	popl %edx
	popl %ecx
	popl %ebx
	popl %esi
	popl %edi

	popl %ebp
	call openalarm
	ret
