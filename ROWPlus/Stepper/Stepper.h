//
// Created by Hao Wu on 11/8/16.
//

#ifndef ROWPLUS_STEPPER_H
#define ROWPLUS_STEPPER_H

#include "ROWPlus/Core/ODEUtility.h"
#include "ROWPlus/Core/ODESolver.h"

namespace ROWPlus {

template<class T, typename JacType, typename FunctorType, typename Scalar = double>
class stepper {
 public:
  typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> VectorType;
  typedef std::unique_ptr<ODESolver<JacType, FunctorType>> SolverPtr;
  void makeAdaptiveStepper(FunctorType *_func,
                           Scalar rel_error, Scalar abs_error = 0.0) {
    if (abs_error == 0.0)
      abs_error = Eigen::NumTraits<Scalar>::epsilon() * 10.0;
    assert(rel_error > 0.0 && abs_error > 0.0);
    this->options.relTol = rel_error;
    this->options.absTol = abs_error;
    this->options.h_max = Eigen::NumTraits<Scalar>::highest();
    this->options.h_min = Eigen::NumTraits<Scalar>::epsilon();
    this->options.stepControl[0] = 0.20;
    this->options.stepControl[1] = 5.00;
    this->options.stepControl[2] = 0.80;
    this->solver =
        SolverPtr(new ODESolver<JacType, FunctorType>(_func, this->options));
  };
  void makeConstantStepper(FunctorType *_func) {
    this->options.relTol = 0.0;
    this->options.absTol = std::sqrt(Eigen::NumTraits<Scalar>::highest());
    this->options.h_max = Eigen::NumTraits<Scalar>::highest();
    this->options.h_min = 0.0;
    this->options.stepControl[0] = 1.0;
    this->options.stepControl[1] = 1.0;
    this->options.stepControl[2] = 1.0;
    this->options.iUserAskedKill = false;
    this->solver =
        SolverPtr(new ODESolver<JacType, FunctorType>(_func, this->options));
  }
  inline ROWPlusSolverSpace::Status step(Eigen::Ref<VectorType> u,
                                         Scalar start_time,
                                         const Scalar end_time,
                                         const Scalar init_time) {
    options.h_init = init_time;
    return solver->step(u, start_time, end_time);
  }

  ODEOptions<Scalar> &getOptions() { return this->options; };
  const ODEOptions<Scalar> &getOptions() const { return options; };
  const SolverPtr &getSolver() const { return solver; };
 protected:
  ODEOptions<Scalar> options;
  SolverPtr solver;
};

template<typename FunctorType, typename Scalar = double>
class rosenbrock_krylov4 : public stepper<rosenbrock_krylov4<Scalar>,
                                          ODEJacSAP<FunctorType, Scalar>,
                                          FunctorType, Scalar> {
 public:
  rosenbrock_krylov4(const Eigen::DenseIndex _k = 4) {
    assert(_k >= 4);
    this->options.TypeScheme = ROK4E;
    this->options.iUserJac = false;
    this->options.iUserFt = false;
    this->options.iUserAskedKill = false;
    this->options.iAuto = true;
    this->options.maxKryDim = _k;
    this->options.kryTol = std::sqrt(Eigen::NumTraits<Scalar>::epsilon());
    this->options.maxSteps = std::numeric_limits<int>::max();
  };
};

template<typename FunctorType, typename Scalar = double>
class rosenbrock4 : public stepper<rosenbrock4<Scalar>,
                                   ODEJacEXA<FunctorType, Scalar>,
                                   FunctorType,
                                   Scalar> {
 public:
  rosenbrock4() {
    this->options.TypeScheme = ROK4E;
    this->options.iUserJac = false;
    this->options.iUserFt = false;
    this->options.iUserAskedKill = false;
    this->options.iAuto = true;
    this->options.maxKryDim = 0;
    this->options.maxSteps = std::numeric_limits<int>::max();
  };
};

template<typename FunctorType, typename Scalar = double>
class runge_kutta_bosh3 : public stepper<runge_kutta_bosh3<Scalar>,
                                         ODEJacZRO<FunctorType, Scalar>,
                                         FunctorType,
                                         Scalar> {
 public:
  runge_kutta_bosh3() {
    this->options.TypeScheme = RK23;
    this->options.iUserJac = false;
    this->options.iUserFt = false;
    this->options.iUserAskedKill = false;
    this->options.iAuto = true;
    this->options.maxKryDim = 0;
    this->options.maxSteps = std::numeric_limits<int>::max();
  };
};

template<typename FunctorType, typename Scalar = double>
class runge_kutta_dopri5 : public stepper<runge_kutta_dopri5<Scalar>,
                                          ODEJacZRO<FunctorType, Scalar>,
                                          FunctorType,
                                          Scalar> {
 public:
  runge_kutta_dopri5() {
    this->options.TypeScheme = RKDP;
    this->options.iUserJac = false;
    this->options.iUserFt = false;
    this->options.iUserAskedKill = false;
    this->options.iAuto = true;
    this->options.maxKryDim = 0;
    this->options.maxSteps = std::numeric_limits<int>::max();
  };
};

}

#endif //ROWPLUS_STEPPER_H
