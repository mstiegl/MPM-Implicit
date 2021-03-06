//------------------------------------------------------------------------------
                       // initialise the container

template<typename MPM_ITEMS>
void mpm::MatrixMpm<MPM_ITEMS>::initialise () {
    columnMatrix.clear();
    rowMatrix.clear();
    AxU.clear();
    M_.resize(0);
    K_xx.resize(0); K_yy.resize(0); K_zz.resize(0);
    K_xy.resize(0); K_xz.resize(0); K_yz.resize(0);
    force_x.resize(0); force_y.resize(0); force_z.resize(0);
    residual.resize(0); velocity_.resize(0,0);
    P_.resize(0);
}

//------------------------------------------------------------------------------
                       // Resize all matrices

template<typename MPM_ITEMS>
void mpm::MatrixMpm<MPM_ITEMS>::resizeMatrix (unsigned& size) {

        unsigned num = columnMatrix.size();
        M_.resize(num);
        K_xx.resize(num); K_yy.resize(num);
        K_xy.resize(num);
        force_x.resize(size);
        force_y.resize(size); 

        if (dim == 3) {
            K_zz.resize(num); K_xz.resize(num); K_yz.resize(num);
            force_z.resize(size);
        }

        residual.resize(2*size);
        P_.resize(2*size);
        velocity_.resize(dim,size);
}

//------------------------------------------------------------------------------
template<typename MPM_ITEMS>
bool mpm::MatrixMpm<MPM_ITEMS>::solveLinearMatrixSystem(NodeVec_& activeNodes) {
    unsigned numActiveNodes = activeNodes.size();
    bool converge = 1;

// initial guess for the nodal value at each time step is the current velocity
    for (unsigned i = 0; i < numActiveNodes; i++) {
        VecDof nVelocity = activeNodes.at(i)->giveSoilVelocity();
        velocity_(0,i) = nVelocity(0);
        velocity_(1,i) = nVelocity(1);
    }

// find the initial residual
    this->findResidual(numActiveNodes);

    if (this->checkErrorTolerance())
        converge = this->CGIterativeSolver(numActiveNodes, activeNodes);
    
    return converge;
}

//------------------------------------------------------------------------------
template<typename MPM_ITEMS>
void mpm::MatrixMpm<MPM_ITEMS>::findResidual(unsigned& numNodes) {
    VecDof MatxVector;
    for (unsigned i = 0; i < numNodes; i++) {
        this->productMatrixVector(i, velocity_);
        residual(i) = force_x(i) - MatxVector(0);
        residual(i+numNodes) = force_y(i) - MatxVector(1); 
    }

    P_ = residual;
    Rho_ = this->findNorm(residual);

}

//------------------------------------------------------------------------------
template<typename MPM_ITEMS>
void mpm::MatrixMpm<MPM_ITEMS>::productMatrixVector(unsigned& rowId, MatrixD& vector) {   

    AxU.clear();
    unsigned n = 0;
    for (unsigned i : rowMatrix) {
        if (i == rowId) {
            AxU(0) += (K_xx(n) + M_(n))*vector(0,columnMatrix.at(n)) + K_xy(n)*vector(1,columnMatrix.at(n));
        }
        n++;
    }

    unsigned m = 0;
    for (unsigned j : columnMatrix) {
        if (j == rowId) {
            AxU(1) += K_xy(m)*vector(0,rowMatrix.at(m)) + (K_xx(m) + M_(m))*vector(1,rowMatrix.at(m));
        }
        m++;
    }

}

//------------------------------------------------------------------------------
template<typename MPM_ITEMS>
double mpm::MatrixMpm<MPM_ITEMS>::findNorm(VectorD& vector) {

    long double sum = 0;
    for (unsigned i = 0; i < vector.size(); i++) {
        sum += vector(i)*vector(i);
    }
    return std::sqrt(sum);
}

//------------------------------------------------------------------------------
template<typename MPM_ITEMS>
bool mpm::MatrixMpm<MPM_ITEMS>::checkErrorTolerance() {
    bool status = 0;

    for (unsigned i = 0; i < residual.size(); i++) {
        if (std::fabs(residual(i)) > 0.01) {
            status = 1;
            break;
        }
    }
    return status;
}

//------------------------------------------------------------------------------
template<typename MPM_ITEMS>
bool mpm::MatrixMpm<MPM_ITEMS>::CGIterativeSolver(unsigned& activeNodes, NodeVec_ ptrActiveNodes) {

    unsigned iterMax = 1000000; // this should be given as an input
    unsigned iter = 0;
    unsigned status = 0;
    VectorD w_(2*activeNodes); 


// main CG iterative loop
    for (unsigned i = 0; i < iterMax; i++) {

        MatrixD pMatrix(dim,P_.size()/2);
        pMatrix.row(0) = P_.head(P_.size()/2);
        pMatrix.row(1) = P_.tail(P_.size()/2);

        for (unsigned j = 0; j < activeNodes; j++) {
            this->productMatrixVector(j,pMatrix);
            w_(j) = AxU(0);
            w_(j+activeNodes) = AxU(1);
        }

        double alpha_ = Rho_ / (P_.transpose()*w_);
        // std::cout << "alpha: " << alpha_ << std::endl;

        velocity_.row(0) += (alpha_ * P_.head(activeNodes));
        velocity_.row(1) += (alpha_ * P_.tail(activeNodes));

        residual -= (alpha_ * w_);

        double rho = this->findNorm(residual);
        double beta_ = Rho_/rho;
        Rho_ = rho;

        P_ = residual + (beta_ * P_);
        iter++;

        //std::cout << "Residual: " << residual << std::endl;
        //std::cout << "velocity: " << velocity_ <<std::endl;
 
        if (!this->checkErrorTolerance()){
           std::cout << "Residual: " << iter << std::endl;
            status = 1;
            VecDof nodeVelocity;
            unsigned num = 0;
            for (auto k : ptrActiveNodes) {
                nodeVelocity(0) = velocity_(0,num);
                nodeVelocity(1) = velocity_(1,num+activeNodes);
                k->updateNodalVelocity(nodeVelocity);
                num++;
            }
           return status;

            break;
        }

    }

    return status;

}

//------------------------------------------------------------------------------
