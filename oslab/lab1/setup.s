!
!	setup.s		(C) 1991 Linus Torvalds
!
! setup.s is responsible for getting the system data from the BIOS,
! and putting them into the appropriate places in system memory.
! both setup.s and system has been loaded by the bootblock.
!
! This code asks the bios for memory/disk/other parameters, and
! puts them in a "safe" place: 0x90000-0x901FF, ie where the
! boot-block used to be. It is then up to the protected mode
! system to read them from there before the area is overwritten
! for buffer-blocks.
!
INITSEG  = 0x9000	! we move boot here - out of the way
SYSSEG   = 0x1000	! system loaded at 0x10000 (65536).
SETUPSEG = 0x9020	! this is the current segment
! NOTE! These had better be the same as in bootsect.s!


entry start
start:
! set ds
	mov ax,#SETUPSEG
	mov ds,ax
	mov es,ax 

! Print some inane message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#24
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

! set ds
	mov ax,#INITSEG
	mov ds,ax
	mov ax,#SETUPSEG
	mov es,ax 

! posterity.

	mov	ax,#INITSEG	! this is done in bootsect already, but...
	mov	ds,ax
	mov	ah,#0x03	! read cursor pos
	xor	bh,bh
	int	0x10		! save it in known place, con_init fetches
	mov	[0],dx		! it from 0x90000.

! Get memory size (extended mem, kB)

	mov	ah,#0x88
	int	0x15
	mov	[2],ax

! Get hd0 data

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x41]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0080
	mov	cx,#0x10
	rep
	movsb
! set ds
	mov ax,#INITSEG
	mov ds,ax
	mov ax,#SETUPSEG
	mov es,ax 

! Print some curpos message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#0
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#cur
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

	mov ax,[0]
	call print_hex
	call print_nl

! Print some memory message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#11
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#memory
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

	mov ax,[2]
	call print_hex
	call print_nl

! Print some disk message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#10
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#disk
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

	mov ax,[0x80]
    call print_hex
    call print_nl

	!end
	s1:
		jmp s1

print_hex:
    mov cx,#4   ! 4个十六进制数字
    mov dx,ax   ! 将ax所指的值放入dx中，ax作为参数传递寄存器
print_digit:
    rol dx,#4  ! 循环以使低4比特用上 !! 取dx的高4比特移到低4比特处。
    mov ax,#0xe0f  ! ah = 请求的功能值,al = 半字节(4个比特)掩码。
    and al,dl ! 取dl的低4比特值。
    add al,#0x30  ! 给al数字加上十六进制0x30
    cmp al,#0x3a
    jl outp  !是一个不大于十的数字
    add al,#0x07  !是a~f,要多加7
outp:
    int 0x10
    loop print_digit
    ret
print_nl:
    mov ax,#0xe0d
    int 0x10
    mov al,#0xa
    int 0x10
    ret


msg1:
	.byte 13,10
	.ascii "this is setup"
	.byte 13,10,13,10


cur:
    .ascii "pos:"

memory:
    .ascii "memorySize:"

disk:
    .ascii "diskSize:"
