C $Id: gpypars.F,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
      INTEGER*4 FUNCTION PYPARS_ADDRESS() 
      IMPLICIT NONE

      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      EXTERNAL PYDATA ! All that is required to force loading of BLOCK DATA
      INTEGER*4 GETADDR

      PYPARS_ADDRESS = GETADDR(MSTP)
      RETURN
      END
