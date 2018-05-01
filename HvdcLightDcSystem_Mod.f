      MODULE HvdcLightDcSystem_Mod
!     Include file for Type DcSystemDataDef
!     To be used with HvdcLightDcSystem.f
!
      INTEGER, PARAMETER :: iErrorCountMaxDcSys=10, iIntStoreMaxDcSys=1, iMaxTerminals=5, iMaxBranches= 7, iRealStoreMaxDcSys=12
!
      TYPE DcSystemDataDef
          SEQUENCE
          REAL Cnode(iMaxTerminals)               ! Farad,    Parameter
		  REAL Cbnode(iMaxBranches)               ! Farad, constant parameter
          REAL Lbranch(iMaxBranches)              ! Henry,    Parameter
          REAL Rbranch(iMaxBranches)              ! Ohm,      Parameter
          REAL Pdcinject(iMaxTerminals)           ! MW,       Input
          REAL Idcinject(iMaxTerminals)           ! Amp,      Input
          REAL Ud(iMaxTerminals)                  ! kV,       Output, input at initialization
          REAL Id(iMaxBranches)                   ! Amp,      Output, injected to dc system
          REAL UdcNom                             ! kV,       Parameter
          REAL FaultCurrent                       ! Amp,      Output
          REAL Pnom                               ! MW,       Parameter
          REAL fRealStore(iRealStoreMaxDcSys)     ! Real      Output, to tbe stored to next timestep
		  INTEGER  BusNrFrom(iMaxBranches)        ! the bus number that the branches are connected from
          INTEGER  BusNrTo(iMaxBranches)          ! the bus number that the branches are connected to
          
          INTEGER DcInjectAsP(iMaxTerminals)      ! Integer   Input,  to communicate to dc system module
                                                  !           1=inject as Pdc, 0= inject as dc-current
          INTEGER Asymmetrical                    ! Integer   Input,  Fault node in DC system,
                                                  !           0=Symmetric dc-system, 1= Asymmetrical dc system
          INTEGER FaultNode                       ! Integer   Input,  Fault node in DC system, 
                                                  !           if negative= isolated node by dc breaker (and fault)
          INTEGER iUdcCtrlTerminal                ! Terminal number with converter in dc voltage control
          INTEGER iInspect                        ! On/Off
          INTEGER iIntStore(iIntStoreMaxDcSys)    ! Integer   Output, to tbe stored to next timestep
          INTEGER iErrorCount                     ! Integer   Output/Counter
          CHARACTER*32 sDcSystemId                ! Character Parameter/Identifier
          CHARACTER*132 sErrorMessage(iErrorCountMaxDcSys)
      END TYPE DcSystemDataDef
!
      END MODULE HvdcLightDcSystem_Mod

