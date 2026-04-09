**PROCEDURE: f_
**BEGIN: Three Address Code Statements
temp0 = n_ == 0
temp1 = ! temp0
if(temp1) goto Label1
ret_ = 1
stemp0 = ret_
goto Label3
Label3:
return stemp0
goto Label1
Label1:
temp2 = n_ - 1
k_ = temp2
ret_ = f_(k_)
temp4 = n_ * ret_
ret_ = temp4
stemp1 = ret_
goto Label4
Label4:
return stemp1
**END: Three Address Code Statements
**PROCEDURE: main
**BEGIN: Three Address Code Statements
b_ = 5
a_ = f_(b_)
write a_
m_ = 2
write m_
**END: Three Address Code Statements
