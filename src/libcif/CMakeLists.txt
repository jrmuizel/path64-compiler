

INCLUDE_DIRECTORIES(.)

ADD_LIBRARY(cif
            cifbinread.c
            cifclose.c
            cifdata.c
            cifdup.c
            ciferrstr.c
            ciffiledir.c
            ciffree.c
            cifgetpos.c
            cifgetrec.c
            cifmode.c
            cifmsg.c
            cifopen.c
            cifprint.c
            cifputrec.c
            cifrecgroup.c
            cifrel.c
            cifsetpos.c
            cifspace.c
            cifunitdir.c
            cif_conv.c
            cif_lines.c)

