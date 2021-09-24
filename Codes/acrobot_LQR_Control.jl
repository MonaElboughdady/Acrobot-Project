using Plots: display
using LinearAlgebra
using ControlSystems
using Plots
include("acrobot_linear_model.jl")

   l1 = 0.325
    l2 = 0.2
   lc1 = 0.29
   lc2 = 0.1
   m1 = 0.510
   m2 = 0.203
   I1 = 0.0023
   I2 = 0.0006
   g =  9.80782 

    angle = 0.0
    B = [0;1]
    A   = [zeros(2,2) I+zeros(2,2);-1*inv(H(0.0,l1,lc1,m1,I1,l2,lc2,m2,I2,g))*dG_dQ(angle,0.0,l1,lc1,m1,I1,l2,lc2,m2,I2,g) -1*inv(H(0.0,l1,lc1,m1,I1,l2,lc2,m2,I2,g))*c_q_qDot(angle,0.0,0,0,l1,lc1,m1,I1,l2,lc2,m2,I2,g)]
    B   = [zeros(2,1);inv(H(0,l1,lc1,m1,I1,l2,lc2,m2,I2,g))*B]
    Cm  = [1 0 0 0;0 1 0 0;0 0 1 0;0 0 0 1]

    Q   = I+zeros(4,4)
  Q[1,1] = 32.828#1000
   Q[2,2] =1.621 #10

  Q[3,3] =0.1013#0.01

  Q[4,4] =0.00101#0.001
  R   = 0.3460*I+zeros(1,1) #

    L   =lqr(A,B,Q,R)
    L = 0.2*L
a=L[1]
b=L[3]
c=L[2]
d= L[4]
kq1=a/2
kdq1=b/2
kq2=c/2
kdq2=d/2
kx2=c/(2*lc2)
kvx2=d/(2*lc2)
kx1=(a-2*kx2*(l1+lc2))/(2*lc1)
kvx1=(b-2*kvx2*(l1+lc2))/(2*lc1)
return [kx1 kvx1 kq1 kdq1 kx2 kvx2 kq2 kdq2 L] 