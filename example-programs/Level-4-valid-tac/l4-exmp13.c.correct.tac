**PROCEDURE: main
**BEGIN: Three Address Code Statements
    a_ = 2.30
    b_ = 3.40
    c_ = 4.50
    d_ = 5.60
    e_ = 6.70
    temp0 = b_ + c_
    temp1 = a_ >= temp0
    temp2 = e_ * d_
    temp3 = temp2 / c_
    temp4 = d_ > temp3
    temp5 = temp1 && temp4
    temp8 = ! temp5
    if(temp8) goto Label1
    temp6 = a_ * d_
    temp7 = temp6 / e_
    a_ = temp7
    goto Label0
Label1:
    temp9 = e_ + d_
    e_ = temp9
Label0:
**END: Three Address Code Statements