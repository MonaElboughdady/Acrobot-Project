using LinearAlgebra
using ControlSystems
include("SinglePendulumLinearModel.jl")
l = 0.2                             # length [m]
    lcm = 0.1
    #m = 0.0334                             # mass[m]
    g = 9.80782                         # gravitational acceleration [m/s²]
   # Im = (1/12)*m*l^2+0.5*m*r^2                   # Mass Moment of Inertia
    #Im = 0.0008446                    # Mass Moment of Inertia
    m = 0.203
    Im = 0.0006
    angle = 0.0
    A = [0 1;-((H_q(angle,m,l,Im,g))^-1)*dG_by_dQ(angle,m,l,Im,g) 0]
    B = [0;(H_q(angle,m,l,Im,g))^-1]
    Q   = I+zeros(2,2)
    Q[1] =10
    R   = I+zeros(1,1)
    L   =lqr(A,B,Q,R)