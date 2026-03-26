**PROCEDURE: main
**BEGIN: Three Address Code Statements
    year_ = 1990
    temp0 = year_ / 400
    temp1 = temp0 * 400
    temp2 = temp1 == year_
    temp3 = ! temp2
    if(temp3) goto Label1
    cntr_ = 1
    goto Label0
Label1:
    temp4 = year_ / 100
    temp5 = temp4 * 100
    temp6 = temp5 == year_
    temp7 = ! temp6
    if(temp7) goto Label3
    cntr_ = 0
    goto Label2
Label3:
    temp8 = year_ / 4
    temp9 = temp8 * 4
    temp10 = temp9 == year_
    temp11 = ! temp10
    if(temp11) goto Label5
    cntr_ = 1
    goto Label4
Label5:
    cntr_ = 0
Label4:
Label2:
Label0:
**END: Three Address Code Statements