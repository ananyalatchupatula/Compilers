**PROCEDURE: main
**BEGIN: Three Address Code Statements
    a_ = 5
    i_ = 0
Label0:
    temp0 = i_ < a_
    temp2 = ! temp0
    if(temp2) goto Label1
    temp1 = i_ + 1
    i_ = temp1
    write  i_
    goto Label0
Label1:
**END: Three Address Code Statements