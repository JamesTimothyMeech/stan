#ifndef STAN_MCMC_HMC_STATIC_UNIFORM_ADAPT_DENSE_E_STATIC_UNIFORM_HPP
#define STAN_MCMC_HMC_STATIC_UNIFORM_ADAPT_DENSE_E_STATIC_UNIFORM_HPP

#include <stan/mcmc/stepsize_covar_adapter.hpp>
#include <stan/mcmc/hmc/static_uniform/dense_e_static_uniform.hpp>

namespace stan {

  namespace mcmc {

    // Hamiltonian Monte Carlo on a
    // Euclidean manifold with dense metric,
    // static integration time,
    // and adaptive stepsize
    template <typename M, class BaseRNG>
    class adapt_dense_e_static_uniform : public dense_e_static_uniform<M, BaseRNG>,
                                     public stepsize_covar_adapter {
    public:
      adapt_dense_e_static_uniform(M &m, BaseRNG& rng,
                               std::ostream* o,
                               std::ostream* e)
        : dense_e_static_uniform<M, BaseRNG>(m, rng, o, e),
        stepsize_covar_adapter(m.num_params_r()) { }

      ~adapt_dense_e_static_uniform() { }

      sample transition(sample& init_sample) {
        sample s = dense_e_static_uniform<M, BaseRNG>::transition(init_sample);

        if (this->adapt_flag_) {
          this->stepsize_adaptation_.learn_stepsize(this->nom_epsilon_,
                                                    s.accept_stat());

          bool update = this->covar_adaptation_.learn_covariance
            (this->z_.mInv, this->z_.q);

          if (update) {
            this->init_stepsize();
            this->stepsize_adaptation_.set_mu(log(10 * this->nom_epsilon_));
            this->stepsize_adaptation_.restart();
          }
        }
        return s;
      }

      void disengage_adaptation() {
        base_adapter::disengage_adaptation();
        this->stepsize_adaptation_.complete_adaptation(this->nom_epsilon_);
      }
    };

  }  // mcmc

}  // stan

#endif
