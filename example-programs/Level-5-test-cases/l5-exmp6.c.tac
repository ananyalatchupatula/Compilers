**PROCEDURE: f2_
**BEGIN: Three Address Code Statements
    stemp0 = p_
    goto Label1
Label1:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: f_
**BEGIN: Three Address Code Statements
    stemp0 = p_
    goto Label0
Label0:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
    r_ = 2.00
    temp0 = 2.00 + 3.00
    temp1 = r_ * temp0
    r_ = temp1
    temp2 = r_ * 2.00
temp3 = f_(temp2, 3)
    r_ = temp3
**END: Three Address Code Statements
