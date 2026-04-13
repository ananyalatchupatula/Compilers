**PROCEDURE: func1_
**BEGIN: Three Address Code Statements
    i2_ = 5
    f2_ = 5.00
Label6:
    temp0 = i2_ < 7
    temp2 = ! temp0
    if(temp2) goto Label7
    write i2_
    temp1 = i2_ + 1
    i2_ = temp1
    goto Label6
Label7:
    stemp0 = i2_
    goto Label8
**END: Three Address Code Statements
**PROCEDURE: func11_
**BEGIN: Three Address Code Statements
    i1_ = 5
Label9:
    temp0 = i1_ < 7
    temp2 = ! temp0
    if(temp2) goto Label10
    write i1_
    temp1 = i1_ + 1
    i1_ = temp1
    goto Label9
Label10:
    stemp0 = 1
    goto Label2
Label2:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: func2_
**BEGIN: Three Address Code Statements
    temp0 = a_ + 10
    a_ = temp0
    i1_ = 5
    temp1 = a_ + i1_
    temp2 = temp1 + i2_
    temp3 = temp2 + 2
    stemp0 = temp3
    goto Label3
Label3:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: func3_
**BEGIN: Three Address Code Statements
    temp0 = b_ + f2_
    b_ = temp0
    temp1 = c_ + 10.50
    c_ = temp1
    temp2 = b_ + c_
    stemp0 = temp2
    goto Label4
Label4:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: func4_
**BEGIN: Three Address Code Statements
    i1_ = 4
    temp0 = i1_ + i1_
    stemp0 = temp0
    goto Label5
Label5:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: apple_
**BEGIN: Three Address Code Statements
    i1_ = 5
    stemp0 = i1_
    goto Label1
Label1:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
    temp0 = func1_()
    a_ = temp0
    temp1 = func4_()
    i2_ = temp1
    temp2 = func2_(i1_, f2_, i2_)
    i2_ = temp2
    temp3 = func3_(i1_, f1_, f2_)
    f2_ = temp3
    temp4 = apple_()
    st_ = temp4
    temp5 = func11_()
    stg_ = temp5
    write f1_
    write f2_
    write a_
    write st_
    write stg_
**END: Three Address Code Statements
