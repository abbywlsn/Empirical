//  This file is part of Empirical, https://github.com/devosoft/Empirical
//  Copyright (C) Michigan State University, 2016.
//  Released under the MIT Software license; see doc/LICENSE
//
//  This file demonstrates the functionality of QuorumOrganisms and their associated
//  subsystems

#include <iostream>
#include <string>

#include "../../config/ArgManager.h"
#include "../../evo/World.h"
#include "../../tools/Random.h"
#include "../../evo/EvoStats.h"
#include "../../evo/StatsManager.h"
#include "../../evo/QuorumOrg.h"
#include "../../evo/QuorumManager.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

using QOrg = emp::evo::QuorumOrganism;

template <template<class> class BASE_PM>
using QM = emp::evo::QuorumManager<QOrg, BASE_PM>;

template <class QOrg, template<class> class BASE_PM>
using QWorld = emp::evo::World<QOrg, QM<BASE_PM>>;

// define the underlying population manager here
template <class QOrg>
using FOUNDATION = emp::evo::PopulationManager_Grid<QOrg>;

//TODO: find a way to enforce that POP_MANAGER is POP_MANAGER<QOrg>
// Consult Emily's stats stuff for reference...?

EMP_BUILD_CONFIG( QuorumConfig,
    VALUE(HI_AI_WEIGHT, int, 4, "What value should the AI production be for hi-density?"),
    VALUE(LO_AI_WEIGHT, int, 1, "What value should the AI production be for lo-density?"),
    VALUE(AI_RADIUS, int, 10, "What's the radius of AI dispersal?"),
    VALUE(NUM_TO_DONATE, int, 45, "Number of points a public good is 'worth'"),
    VALUE(NEEDED_TO_REPRODUCE, int, 50, "Number of points needed for an organism to reproduce"),
    VALUE(COST_TO_DONATE, int, 25, "Number of points a public good costs to produce"),
    VALUE(MUTATION_AMOUNT, double, 0.1, "Standard deviation of distribution to mutate by"),
    VALUE(GRID_X, int, 60, "Width of the grid"),
    VALUE(GRID_Y, int, 60, "Height of the grid"),
    VALUE(TICKS, int, 1000, "Length of simulation"),
    VALUE(INITIAL_SIZE, unsigned int, 30, "Starting population size"),
    VALUE(INITIAL_CONFIG, int, 0, "Which predefined organism to pit against the defector?"),
    VALUE(PERCENT_DEFECTOR, double, 0.5, "Portion of the starting population to seed as defector"),
    VALUE(ENABLE_MUTATION, bool, 1, "If mutation should be enabled"),
    VALUE(RAND_SEED, int, 238947, "Seed for the random generator"),
    VALUE(PREFIX, std::string, "", "Prefix for filenames")
)

std::string init_config_names[4] = {"balanced", "defector", "donator", "scrooge"};

int main(int argc, char* argv[]) {
    
    QuorumConfig config;
    config.Read("quorum.cfg");

    emp::Random dice;
    std::string prefix;

    auto args = emp::cl::ArgManager(argc, argv);
    if (args.ProcessConfigOptions(config, std::cout, "quorum.cfg") == false) {exit(0);}
    if (args.TestUnknown() == false) {exit(0);}
    
    if (config.PREFIX() != "") {prefix = config.PREFIX() + "-";}
    QWorld<QOrg, FOUNDATION> Qpop(&dice);
    Qpop.ConfigPop(config.GRID_X(), config.GRID_Y());

    emp::evo::StatsManager_FunctionsOnUpdate<QM<FOUNDATION>> Qstats(&Qpop, prefix + "quorum.csv"); 

    // Set all the class variables
    QM<FOUNDATION>::hi_weight = config.HI_AI_WEIGHT();
    QM<FOUNDATION>::lo_weight = config.LO_AI_WEIGHT();
    QM<FOUNDATION>::ai_radius = config.AI_RADIUS();
    QOrg::num_to_donate = config.NUM_TO_DONATE();
    QOrg::needed_to_reproduce = config.NEEDED_TO_REPRODUCE();
    QOrg::cost_to_donate = config.COST_TO_DONATE();
    QOrg::mutation_amount = config.MUTATION_AMOUNT();
    QOrg::random = &dice; // our random is gotten from the world
    unsigned int runtime = config.TICKS();
    unsigned int pop_size = config.INITIAL_SIZE();
    dice.ResetSeed(config.RAND_SEED());

    if (pop_size > (unsigned) (config.GRID_X() * config.GRID_Y()) ) {
      std::cerr << "** ERROR: Initial population size is larger than the grid!!" << std::endl;
      std::cerr << "** Aboring--!!" << std::endl;
      return 2;
    }

    config.Write("quorum.cfg");

    unsigned int i = 0;
    for(; (double) i / pop_size < config.PERCENT_DEFECTOR(); i++) {
      QOrg * org = new QOrg(QOrg::initial_configurations[1].co_op_prob, // defector
                            QOrg::initial_configurations[1].ai_radius, 
                            QOrg::initial_configurations[1].quorum_threshold, 
                            config.ENABLE_MUTATION(), 0,
                            QOrg::initial_configurations[1].lineage,
                            QOrg::initial_configurations[1].can_make_HiAI); // liniage tag
      org->set_id(Qpop.SequentialInsert(org));
    }

    unsigned chosen = config.INITIAL_CONFIG();
    for(; i < pop_size; i++) {
      QOrg * org = new QOrg(QOrg::initial_configurations[chosen].co_op_prob, // donator
                            QOrg::initial_configurations[chosen].ai_radius, 
                            QOrg::initial_configurations[chosen].quorum_threshold, 
                            config.ENABLE_MUTATION(), 0,
                            QOrg::initial_configurations[chosen].lineage,
                            QOrg::initial_configurations[chosen].can_make_HiAI);
      org->set_id(Qpop.SequentialInsert(org));
    }

    // mutation is handled automatically by the population QPop_Manager, currently
    // I'm getting the sense that it probably shouldn't be.
    Qpop.SetDefaultMutateFun([](QOrg * org, emp::Random & random) {
      return org->mutate(random);
    });

    std::function<double(QOrg * )>fit_func=[](QOrg* org) {
      if (org != nullptr ) {return (double) org->get_fitness();}
      else {return (double) -1;} // seriously, I have to cast this to make you happy?
    };

    
    QM<FOUNDATION> * underlying = &Qpop.popM;
    std::function<double()>age_func=[underlying]
      () {
      double age = 0;
      int num_orgs = 0;
      for (auto org : (*underlying)) {
        if (org != nullptr) {
          age += org->get_age();
          num_orgs++;
        }
      }
      if (num_orgs == 0) {return (double) -1;}
      return age / (double) num_orgs;
    };

    std::function<double()>max_age_func=[underlying]
      () {
      double max = 0;

      for(auto org: (*underlying) ){
        if (org != nullptr) {
          if (org->get_age() > max) {max = org->get_age();};
        }
      }
      return max;
    };

    std::function<double()>avg_coop_chance=[underlying] () {
      double pop_coop_prob = 0;
      int num_orgs = 0;

      for(auto org : (*underlying)) {
        if(org != nullptr) {
          pop_coop_prob += org->state.genome.co_op_prob;
          num_orgs++;
        }
      }

      return pop_coop_prob / (double) num_orgs;
    };

    
   std::function<double()>avg_points=[underlying] () {
      double points = 0;
      int num_orgs = 0;

      for(auto org : (*underlying)) {
        if(org != nullptr) {
          points += org->state.get_points();
          num_orgs++;
        }
      }

      return points / (double) num_orgs;
    };

   std::function<double()>percent_defector_lin=[underlying] () {
      int count = 0;
      int num_orgs = 0;

      for(auto org : (*underlying)) {
        if(org != nullptr) {
          if (org->state.genome.get_lineage() == 1) {count++;}
          num_orgs++;
        }
      }
      return (double) count / (double) num_orgs;
   };

    auto config_ptr = &config; 
    std::function<double()>percent_donator_lin=[underlying, config_ptr] () {
    int count = 0;
      int num_orgs = 0;

      for(auto org : (*underlying)) {
        if(org != nullptr) {
          if (org->state.genome.get_lineage() == config_ptr->INITIAL_CONFIG()) {count++;}
          num_orgs++;
        }
      }
      return (double) count / (double) num_orgs;
   };

    Qstats.AddFunction(age_func, "avg_age");
    Qstats.AddFunction(max_age_func, "max_age");
    Qstats.AddFunction(avg_coop_chance, "avg_coop");
    Qstats.AddFunction(avg_points, "avg_points"); 
    Qstats.AddFunction(percent_defector_lin, "percent_defector");
    Qstats.AddFunction(percent_donator_lin, init_config_names[config.INITIAL_CONFIG()]);

    unsigned int checkpoint = 0;
    
    std::cerr << "Progress:   0% [ . . . . . . . . . .]";
    // loop through the specified number of updates && run the evolution
    for (unsigned int update_num = 0; update_num < runtime; update_num++) {
      Qpop.Update();
      if(( (double) update_num / (double) runtime) * 20 > checkpoint) {
        ++checkpoint;
        
        std::cerr << "\rProgress: " << std::setw(3) << checkpoint * 5 << "% [";
       
        unsigned i = 0;
        for(; i < checkpoint; i++) {
          std::cerr << "+"; 
        }
        for(; i < 20; i++) {
          if (i % 2 == 0) { std::cerr << " ";}
          else { std::cerr << ".";}
        }
        std::cerr << "]";
      }
    }
}
