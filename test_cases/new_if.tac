main:
@t0 = 4 INT
- INT a
a = @t0 INT
@t1 = 1 INT
@t2 = a == @t1 INT
if @t2 GOTO #L1 else GOTO #L2
#L1:
@t3 = 1 INT
@t4 = a + @t3 INT
a = @t4 INT
GOTO #L0
#L2:
@t5 = 2 INT
#L0: