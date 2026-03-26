**PROCEDURE: main
**BEGIN: Three Address Code Statements
    temp0 = a_ > 20
    temp1 = ! temp0
    temp2 = ! temp1
    if(temp2) goto Label1
    a_ = 30
    goto Label0
Label1:
    a_ = 40
Label0:
Label2:
    temp3 = a_ > 30
    temp4 = ! temp3
    if(temp4) goto Label3
    a_ = 40
    goto Label2
Label3:
    temp5 = - 30
    a_ = temp5
    temp6 = - a_
    temp7 = temp6 > 30
    temp8 = ! temp7
    if(temp8) goto Label4
    a_ = 50
    goto Label4
Label4:
**END: Three Address Code Statements