      SUBROUTINE HvdcLightDcSystem(fDcStep, iUsedTerminals,iUsedBranches, iCalcMode, Dta, Time)
!     This subroutine is to call the DC system in every time step.
!
!     Rules for this subroutine:
!     The subroutine can use any number of terminal, but so far a max limit is set with the parameter iMaxTerminals
!
      USE HvdcLightDcSystem_Mod
!
      IMPLICIT  NONE
!
      INCLUDE 'HvdcLightDcSystem-declare-insert.f'
!
!
      CnodeMin=0.1E-6                      ! Set a temporary value, will be recalculated later down
      fPadjust= 0.0                        ! Give a defined value, only used in Load Flow and Initialization
      Lmin= 1.E-3                          ! Set a temporary value, will be recalculated later down
      iMaxIterations= 1000
	  fUdiff = 1.0E+6                      ! iterative tolerance
	  iCntUdLoop = 0                       ! loops within load flow
	  acc = 1.2                            ! accelerated factor
!
!     Avoid false values
      If (Dta%iInspect .Gt. 1) Then
          Dta%iInspect=0
      Elseif (Dta%iInspect .Lt. 0) Then
          Dta%iInspect=0
      Endif
!
      If (Dta%Asymmetrical .Eq. 1) Then
        fSymMultUdBy2= 1.0  ! Do not multiply UdNode and Ud by 2 in calculations
      Else
        fSymMultUdBy2= 2.0  ! Multiply UdNode and Ud by 2 in calculations
      Endif
!
      If ((iUsedTerminals .Lt. 2) .Or. (iUsedTerminals .Gt. iMaxTerminals)) Then

          Dta%iErrorCount= Dta%iErrorCount + 1
          If (Dta%iErrorCount .Le. iErrorCountMaxDcSys) Then
              Write(sValue,*) iUsedTerminals
!                 sErrorMessage limited to 132 characters
              Dta%sErrorMessage(Dta%iErrorCount)= 'The number of terminals are outside the allowed range, '
     &                                        //'the given value is iUsedTerminals= '//Trim(sValue)
          Endif
!
          Return
!
      Endif
!
!     ###############################
!
      Dta%FaultCurrent= 0.0 ! Set the fault current to zero, first, if a fault is activated, the fault current will be set later
!
      If (Dta%FaultNode .Lt. 0) Then
        iDcBreakerOn= 1
      Else
        iDcBreakerOn= 0
      Endif
!
!     Use an internal variable for FaultNode, always positive. Dta%FaultNode can be negative in case of DC-breaker
      iFaultNode= Abs(Dta%FaultNode)
!      
      If (iCalcMode .Eq. -1) Then ! Load flow, initiate the dc-voltage vector
!
          Do iTerm= 1, iUsedTerminals
              UdNode(iTerm)= Dta%fRealStore(iTerm+0)  ! "Zeropoint" in vector is 0
!
!             Make an initial check of dc voltages
              If (UdNode(iTerm) .Lt. 0.7*Dta%UdcNom ) Then    ! The dc voltage is too low, assumed to be false at initiation
                  UdNode(iTerm)= Dta%UdcNom                   ! Set the voltage to UdcNom
              Endif
!
          Enddo
!
          UdcRef= Dta%Ud(Dta%iUdcCtrlTerminal)        ! kV
!
      Elseif (iCalcMode .Eq. 0) Then ! Initialize from external load flow, initiate the dc-voltage vector
!
          Do iTerm= 1, iUsedTerminals
              UdNode(iTerm)= Dta%UdcNom   ! "Zeropoint" in vector is 0, initiate to UdcNom
			   Dta%Ud(iTerm) = Dta%UdcNom
          Enddo
!
          UdcRef= Dta%Ud(Dta%iUdcCtrlTerminal)        ! kV
!
      Else        ! "Normal" dynamic solution
!
          Do iTerm= 1, iUsedTerminals
              UdNode(iTerm)= Dta%fRealStore(iTerm+0)  ! "Zeropoint" in vector is 0
          Enddo
!
          Do iBranch= 1, iUsedBranches
!             ! "Zeropoint" is previous number of nodes=iUsedTerminals
              IdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch))= Dta%fRealStore(iBranch+iUsedTerminals)   
          Enddo
!
          UdcRef= 0.0   ! Not used in "Normal" dynamic solution
      Endif
!
      If (iCalcMode .Eq. 0) Then ! Initialize from external load flow, initiate the dc-voltage vector
!
          iInspectInit= 0
      Else
!
          iInspectInit= Dta%iIntStore(1)
      Endif
!
!     Formulate meshed dc grid
!****************************************
             RdBranch = 0
             LdBranch = 0	        

             Do iBranch = 1, iUsedBranches
!     setting each branch resistance as intermediate value in resistance matrix, it's symmetrical
             RdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch)) = Dta%Rbranch(iBranch)
             RdBranch(Dta%BusNrTo(iBranch),Dta%BusNrFrom(iBranch)) = Dta%Rbranch(iBranch)

!     setting each branch inductance as intermediate value in inductance matrix
            LdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch)) = Dta%Lbranch(iBranch)
            LdBranch(Dta%BusNrTo(iBranch),Dta%BusNrFrom(iBranch)) = Dta%Lbranch(iBranch)

             Enddo
!
!     Admittance matrix
!*************************			 
			YdSys = 0
		    ydc = 0
		    ydc_line = 0
    	   
      Do iNode = 1, iUsedTerminals         
          Do jNode = 1,iUsedTerminals
               If ((iNode .NE. jNode) .And. RdBranch(iNode,jNode) .Gt. 0) Then
                   ydc(iNode,jNode) = 1/max(RdBranch(iNode,jNode),0.01)
                   YdSys(iNode,jNode) = -ydc(iNode,jNode)
                   ydc_line(iNode)= ydc_line(iNode)+ ydc(iNode,jNode)
               Endif
          Enddo
      Enddo

      Do iTerm = 1,iUsedTerminals
         ydc(iTerm,iTerm) = ydc_line(iTerm)
         YdSys(iTerm,iTerm) = ydc(iTerm,iTerm)		 
      Enddo
!
!     Gauss-Seidel iterations
!***************************************
      If ((iCalcMode .Eq. -1) .Or. (iCalcMode .Eq. 0)) Then	 
!	  
          Do while ((fUdiff .Gt. 1e-4) .and. (iCntUdLoop .Le. iMaxIterations))		  
               Do iNode = 1,iUsedTerminals					  
                  temp1 = Dta%Pdcinject(iNode)/max(UdNode(iNode), 0.01)
                  temp2 = 0
				  If (iNode .Ne. Dta%iUdcCtrlTerminal) Then
                      Do jNode = 1,iUsedTerminals
                           If (jNode .Eq. iNode) Then
                               temp2 = temp2 + 0
                           Else
                               temp2 = temp2 + YdSys(iNode,jNode)*Dta%Ud(jNode)
                           Endif
                      Enddo							
                         UdNode(iNode) = (temp1-temp2)/max(YdSys(iNode,iNode),0.01)						   
						 delx = UdNode - Dta%Ud
!     accelerated factor
                         Dta%Ud(iNode) = Dta%Ud(iNode) + acc*(UdNode(iNode)- Dta%Ud(iNode))                    
                  Endif
			   Enddo	       
			   fUdiff = maxval(abs(delx))
			   iCntUdLoop = iCntUdLoop+1				   
          Enddo		

!     power of Udc Controlled terminal
!*******************************************
             temp0 = 0
          Do jNode = 1,iUsedTerminals
             If (jNode .Ne. Dta%iUdcCtrlTerminal) Then
                 temp0 = YdSys(Dta%iUdcCtrlTerminal,jNode)*(Dta%Ud(Dta%iUdcCtrlTerminal)-Dta%Ud(jNode))+ temp0
             Endif 				 
		  Enddo
             Dta%Pdcinject(Dta%iUdcCtrlTerminal)= -Dta%Ud(Dta%iUdcCtrlTerminal)*temp0	

!     Calculate the IdBranch in steady state
!************************************************
          Do iNode = 1,iUsedTerminals
             Do jNode = 1,iUsedTerminals
                If ((iNode .Ne. jNode) .And. (RdBranch(iNode,jNode) .Ne.0)) Then
                   IdBranch(iNode,jNode) = 1000*(UdNode(iNode)-UdNode(jNode))/max(RdBranch(iNode,jNode),0.01)
                Else
                   IdBranch(iNode,jNode) = 0.0
                Endif
             Enddo
          Enddo				 
!
          Do iTerm= 1, iUsedTerminals                                     
             Dta%Ud(iTerm)= UdNode(iTerm)
          Enddo
!
!     Set terminal values, loop nodeS
!************************************************
          Do iTerm= 1, iUsedTerminals
             Iinject(iTerm)= 1000.*Dta%Pdcinject(iTerm)/Max(UdNode(iTerm)*fSymMultUdBy2,0.1)
!
             If (iTerm .Ne. 0 ) Then ! This is a node related to  a terminal, save the voltage as "measured" voltage 
                 Dta%Ud(iTerm)= UdNode(iTerm)
             Endif
          Enddo
!
      Else        
!     "Normal" dynamic solution
!************************************************
!     fUdError= 0.0
!     fPdiff= 0.0     ! Not used in "Normal" dynamic solution
!
!     Check converter capacitance
!     UdcNom (kV) is defined as:
!           Symmetric configuration: pole to midpoint (half the pole-pole voltage)
!           Asymmetrical configuration: pole to ground
!     Pnom is the "full" dc power (MW)
!     Calculate a CnodeMin for converter capacitance, minimum time constant shall be 6 times the timestep
!     T=6.0*fDcStep= 1.0/2.0*CnodeMin * Dta%UdcNom**2.0 / (Dta%Pnom/2.0)
!     CnodeMin is Pole-ground (midpoint) for a symmetric configuration
!     For an Asymmetrical configuration, the factor 2.0 shall be multiplied (Pnom shall not be divided by two in the equation above)
          If (Dta%Asymmetrical .Eq. 1) Then
              CnodeMin= Max(6.0*fDcStep * 2.0 * Dta%Pnom / Dta%UdcNom**2.0, CnodeMin)         ! (Farad)
          Else
            CnodeMin= Max(6.0*fDcStep * Dta%Pnom / Dta%UdcNom**2.0, CnodeMin)         ! (Farad)
          Endif
!
!       
          Do iTerm= 1, iUsedTerminals   ! Step througt the Terminals, starting from terminal=1
              If (Dta%Cnode(iTerm) .Lt. CnodeMin) Then    ! To small converter capacitance
                  Dta%Cnode(iTerm)= CnodeMin
                  Dta%iErrorCount= Dta%iErrorCount + 1
                  If (Dta%iErrorCount .Le. iErrorCountMaxDcSys) Then
                      Write(sValue,*) iTerm
!                     sErrorMessage limited to 132 characters
                      Dta%sErrorMessage(Dta%iErrorCount)= 'Converter capacitance is too small, '
     &                                            //'Modified to a minimum value, DC-node No='//Trim(sValue)
                  Endif
              Endif
          Enddo
!
!       Lbranch is the half total inductance of the branch, i.e. the inductance of each leg in the dc system T-link
!       The calculation of the minimum inductance value is performed by
!       using a scaled value of M5 "smoothing" reactor of 5 mH as minimum Lbranch (5 mH in each station, per pole)
!       Assuming inductance time constant is constant, Tinductance=1.0/2.0*Lbranch*Idnom^2/Pnom= 1.0/2.0*Lbranch*(Pnom/(2.*Udnom))^2/Pnom= Lbranch*Pnom/(8.*Udnom^2)
!       Tinductance= 1.0/2.0*5.E-3*369.90/(4.*150.^2)= 10.275 us ' M5 (s), Udnom is pole-ground (pole-midpoint)
!       Lbranch= Tinductance*8.*Udnom^2/Pnom (H), Udnom is pole-ground (pole-midpoint, half bridge voltage)
!       Lbranch is the inductance in one pole in one station
!       Use a reduction to 0.95 times the calculated value, as margin
!       For an Asymmetrical configuration, Tinductance= 20.55 us (corresponding to two 5 mH inductances), and Udc is not doubled.
!       i.e. for Asymmetrical: Lbranch= Tinductance*2.*Udnom^2/Pnom (H), Udnom is pole-ground (full bridge voltage)
!
          If (Dta%Asymmetrical .Eq. 1) Then
              Lmin= Max(0.95 * 20.5E-6 * 2.0 * Dta%UdcNom**2.0 / Dta%Pnom, Lmin)          ! (Henry)
          Else
              Lmin= Max(0.95 * 10.275E-6 * 8.0 * Dta%UdcNom**2.0 / Dta%Pnom, Lmin)          ! (Henry)
          Endif
!
!       Check if branch impedance have valid values, each inductance must be above Lmin
          Do iBranch= 1, iUsedBranches
              If (LdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch)) .Lt. Lmin) Then
                  LdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch))= Lmin
                  Dta%iErrorCount= Dta%iErrorCount + 1
                  If (Dta%iErrorCount .Le. iErrorCountMaxDcSys) Then
                      Write(sValue,*) iBranch
!                     sErrorMessage limited to 132 characters
                      Dta%sErrorMessage(Dta%iErrorCount)= 'Branch inductance is close to zero, '
     &                                            //'a small value is auto-added, Branch No= '//Trim(AdjustL(sValue))
                  Endif
!                 
              Endif
			  If (LdBranch(Dta%BusNrTo(iBranch),Dta%BusNrFrom(iBranch)) .Lt. Lmin) Then
                  LdBranch(Dta%BusNrTo(iBranch),Dta%BusNrFrom(iBranch)) = Lmin
                  If (Dta%iErrorCount .Le. iErrorCountMaxDcSys) Then
                       Write(sValue,*) iNode
 !                     sErrorMessage limited to 132 characters
                       Dta%sErrorMessage(Dta%iErrorCount)= 'Converter inductance is too small, '
     &                                            //'Modified onto a minimum value, DC-node No='//Trim(sValue)
                  Endif
              Endif
!
              If (RdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch)) .Lt. -1.E-6) Then 
!                 Use a small negative value as "zero check"
                  RdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch))= 0.0
                  Dta%iErrorCount= Dta%iErrorCount + 1
                  If (Dta%iErrorCount .Le. iErrorCountMaxDcSys) Then
                      Write(sValue,*) iBranch
!                     sErrorMessage limited to 132 characters
                      Dta%sErrorMessage(Dta%iErrorCount)= 'Branch resistance is below zero, '
     &                                            //'auto-set to zero, Branch No= '//Trim(AdjustL(sValue))
                  Endif
              Endif

			  If (RdBranch(Dta%BusNrTo(iBranch),Dta%BusNrFrom(iBranch)) .Lt. -1.E-6)  Then
                  RdBranch(Dta%BusNrTo(iBranch),Dta%BusNrFrom(iBranch)) = 0
                  If (Dta%iErrorCount .Le. iErrorCountMaxDcSys) Then
                      write(sValue,*) iNode
 !                    sErrorMessage limited to 132 characters
                          Dta%sErrorMessage(Dta%iErrorCount)= 'Converter resistance is below zero, '
     &                 //'Modified to a minimum value, DC-node No='//Trim(sValue)
                  Endif
              Endif
          Enddo
!
!         Make a reset/definition of bCnodeEliminate
          Do iTerm= 1, iUsedTerminals
            bCnodeEliminate(iTerm)= .False.
          Enddo
!     
!
!         Apply dc fault, if any
          If ((iFaultNode .Ge. 1) .And. (iFaultNode .Le. iUsedTerminals)) Then
              UdNode(iFaultNode)= 0.0
!             Reset bCnodeEliminate (in case already set to be eliminated)
              bCnodeEliminate(iFaultNode)= .False.
          Endif
!
!
           If (iDcBreakerOn .Eq. 1) Then  !Not used Yet
!          Rsegm= 1.E6 ! Set Rsegm to a high value, to emulate a dc breaker
           Endif
!
!     Apply KVL on each cable and KCL on each node
!     This is the proper branch current calculation
!************************************************
           Do iNode =1,iUsedTerminals
               Do jNode = 1,iUsedTerminals
                   If (RdBranch(iNode,jNode) .Ne. 0 .And. LdBranch(iNode,jNode) .Ne. 0) Then
!     first: lossless calculation
                      IdBranch(iNode,jNode) = IdBranch(iNode,jNode) + 1000.*(UdNode(iNode)-UdNode(jNode))/LdBranch(iNode,jNode)*fDcStep
!     second: make loss adjustment
                      IdBranch(iNode,jNode) = IdBranch(iNode,jNode)/(1+RdBranch(iNode,jNode)/LdBranch(iNode,jNode)*fDcStep)
                   Endif
               Enddo
           Enddo              
!  
!     Calculate node voltages, loop nodes
!************************************************
           Do iTerm= 1, iUsedTerminals                 ! This is branch to next node ("after" node)
!            
                If (Dta%DcInjectAsP(iTerm) .Eq. 1) Then
!                   "Communicated as Pdc to inject
!                   The "Max(UdNode(iNode),0.01)" have the following explanation:
!                       The first term, is the normal use
!                       The second term is to 100% make sure avoid division with zero, and to avoid negative dc-voltage which could give 
!                       bizarr results (negatve dc-voltage gives negative current for positive active power, and further discharge)
                    Iinject(iTerm)= 1000.*Dta%Pdcinject(iTerm)/Max(UdNode(iTerm)*fSymMultUdBy2,0.01)     
            					
!                   Make a check to avoid wrong current at a close dc fault
                    If (Abs(Iinject(iTerm)) .Gt. Abs(1.5*Dta%Idcinject(iTerm))) Then
                        Iinject(iTerm)= Dta%Idcinject(iTerm)             ! Ampere
                    Endif
                 Else
!                   "Communicated as Idc to inject
                    Iinject(iTerm)= Dta%Idcinject(iTerm)             ! Ampere
                 Endif
!
!     Calculate the total current on each node, it depends on the injected current and the branch current
!************************************************
                 Ibranch_tot = 0
                 Do iNode = 1, iUsedTerminals
                     Do jNode = 1,iUsedTerminals
                        Ibranch_tot(iNode) = Ibranch_tot(iNode) + IdBranch(iNode,jNode)
                     Enddo
                 Enddo
 
            Itot(iTerm) = Iinject(iTerm)*fSymMultUdBy2 - Ibranch_tot(iTerm)            
            UdNode(iTerm)= UdNode(iTerm) + Itot(iTerm)/1000./Dta%Cnode(iTerm)*fDcStep
!
                If (iTerm .Eq. iFaultNode) Then
                    UdNode(iTerm)= 0.0
                    Dta%FaultCurrent= Itot(iTerm)
                Endif            
!
                If (UdNode(iTerm) .Gt. 2.0*Dta%UdcNom ) Then    ! The dc voltage is extremely high
                                                                ! This is a numerical limitation, but can also be 
                                                                ! explained as a "ruff" dc arrester approximation
                    UdNode(iTerm)= 2.0*Dta%UdcNom
                    Dta%iErrorCount= Dta%iErrorCount + 1
                    If (Dta%iErrorCount .Le. iErrorCountMaxDcSys) Then
                        Write(sValue,*) 2.0*Dta%UdcNom
!                       sErrorMessage limited to 132 characters
                        Dta%sErrorMessage(Dta%iErrorCount)='The dc voltage is extremely high, '//'limited to 2*UdcNom='//Trim(sValue)
                    Endif
                Endif
!
!           
!
                If (iTerm .Ne. 0 ) Then ! This is a node related to  a terminal, save the voltage as "measured" voltage
                    Dta%Ud(iTerm)= UdNode(iTerm)
                    Dta%Id(iTerm)= Iinject(iTerm)
                Endif
!
           Enddo      
!
      Endif
!
      open(87,FILE='test1.dat',ACcESS='APPEND')
      write(87,*) 'IdBranch', IdBranch
	  write(87,*) 'Dta%Pdcinject', Dta%Pdcinject
	  close(87)


!     Reset unused array elements
!     ***************************
!
      Do iNode= iUsedTerminals+1, iMaxTerminals
          UdNode(iNode)= 0.0
          Dta%Cnode(iNode)= 0.0
      Enddo
!
      Do iBranch= iUsedBranches+1, iMaxBranches
          IdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch))= 0.0
          Dta%Lbranch(iBranch)= 0.0
          Dta%Rbranch(iBranch)= 0.0
      Enddo
!     
      Do iTerm= iUsedTerminals+1, iMaxTerminals
          Dta%Pdcinject(iTerm)= 0.0
          Dta%Ud(iTerm)= 0.0
      Enddo
!
!     Include code for logfile (optional)
      INCLUDE 'HvdcLightDcSystem-logfile-insert.f'
!
!
!     Prepare to exit
!     ***************
!
!     Save fRealStore
      Do iNode= 1, iUsedTerminals
          Dta%fRealStore(iNode+0)= UdNode(iNode)  ! "Zeropoint" in vector is 0
      Enddo
!
      Do iBranch= 1, iUsedBranches
!         ! "Zeropoint" is previous number of nodes=2*iUsedTerminals-1
          Dta%fRealStore(iBranch+iUsedTerminals)= IdBranch(Dta%BusNrFrom(iBranch),Dta%BusNrTo(iBranch))
      Enddo
!
      Dta%iIntStore(1)= iInspectInit
!     
      RETURN
!
      END

