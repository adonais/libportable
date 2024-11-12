/* Bypass gcc's internal stack-allocation routines.  */
.global ___chkstk_ms
___chkstk_ms:
    ret
