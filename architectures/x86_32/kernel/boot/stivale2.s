;; --- Stival2 Header ------------------------------------------------------- ;;

 extern __stack_top
 extern _kstart_stivale2

 __stivale2_framebuffer_tag:
     dq 0x3ecc1bc43d0f7971
     dq __stivale2_smp_tag
     dw 0
     dw 0
     dw 32

__stivale2_smp_tag:
     dq 0x1ab015085f3273df
     dq 0
     dq 0

 section .stivale2hdr
 align 4
 __stivale2_header:
     dq _kstart_stivale2
     dq __stack_top
     dq 0
     dq __stivale2_framebuffer_tag
