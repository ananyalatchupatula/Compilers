**PROCEDURE: main
**BEGIN: Three Address Code Statements
local_ = 2
global_ = 1
test_ = 1
temp_ = 1
write local_
write global_
read temp_
temp0 = local_ * temp_
temp1 = global_ + temp0
test_ = temp1
write test_
read temp_
temp2 = global_ * local_
temp3 = temp2 / temp_
test_ = temp3
write test_
**END: Three Address Code Statements
