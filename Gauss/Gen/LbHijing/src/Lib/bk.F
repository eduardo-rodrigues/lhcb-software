C
C
C
	FUNCTION BK(X)
	IMPLICIT REAL*8(A-H,O-Z)
	COMMON /BESEL/X4
	SAVE   /BESEL/
	BK=EXP(-X)*(X**2-X4**2)**2.50/15.0
	RETURN
	END
