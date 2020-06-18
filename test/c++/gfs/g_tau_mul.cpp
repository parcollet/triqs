#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;

TEST(GfMul, Statistic) {
  double beta = 10.0;

  auto f = gf<imtime>{{beta, Fermion, 100}, {1, 1}};
  auto b = gf<imtime>{{beta, Boson, 100}, {1, 1}};

  EXPECT_THROW(gf<imtime>{f + b}, triqs::runtime_error);

  {
    gf<imtime> p = gf<imtime>{f * f};
    EXPECT_TRUE(p.mesh().domain().statistic == Boson);
  }
  {
    gf<imtime> p = gf<imtime>{f * b};
    EXPECT_TRUE(p.mesh().domain().statistic == Fermion);
  }
  {
    gf<imtime> p = gf<imtime>{b * f};
    EXPECT_TRUE(p.mesh().domain().statistic == Fermion);
  }
  {
    gf<imtime> p = gf<imtime>{b * b};
    EXPECT_TRUE(p.mesh().domain().statistic == Boson);
  }
}

// -------------------------------------

TEST(GfMul, Statistic2Imtime) {
  double beta = 10.0;
  using g_t   = gf<prod<imtime, imtime>>;

  auto ff = g_t{{{beta, Fermion, 100}, {beta, Fermion, 100}}, {1, 1}};
  auto fb = g_t{{{beta, Fermion, 100}, {beta, Boson, 100}}, {1, 1}};

  EXPECT_THROW(g_t{ff + fb}, triqs::runtime_error);

  {
    g_t p = g_t{ff * ff};
    EXPECT_TRUE(std::get<0>(p.mesh()).domain().statistic == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Boson);
  }
  {
    g_t p = g_t{ff * fb};
    EXPECT_TRUE(std::get<0>(p.mesh()).domain().statistic == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Fermion);
  }
  {
    g_t p = g_t{fb * ff};
    EXPECT_TRUE(std::get<0>(p.mesh()).domain().statistic == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Fermion);
  }
  {
    g_t p = g_t{fb * fb};
    EXPECT_TRUE(std::get<0>(p.mesh()).domain().statistic == Boson);
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Boson);
  }
}

// -------------------------------------

TEST(GfMul, Statistic_k_tau) {
  using g_t = gf<prod<b_zone, imtime>, matrix_valued>;

  double beta = 10.0;
  int n_bz    = 20;
  auto bz     = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
  auto g_eps  = gf<b_zone>{{bz, n_bz}, {1, 1}};

  auto f = g_t{{{bz, n_bz}, {beta, Fermion, 100}}, {1, 1}};
  auto b = g_t{{{bz, n_bz}, {beta, Boson, 100}}, {1, 1}};

  EXPECT_THROW(g_t{f + b}, triqs::runtime_error);
  EXPECT_THROW(g_t{f - b}, triqs::runtime_error);

  {
    g_t p = g_t{f + f};
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Fermion);
  }
  {
    g_t p = g_t{b + b};
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Boson);
  }
  {
    g_t p = g_t{f * f};
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Boson);
  }
  {
    g_t p = g_t{f * b};
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Fermion);
  }
  {
    g_t p = g_t{b * f};
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Fermion);
  }
  {
    g_t p = g_t{b * b};
    EXPECT_TRUE(std::get<1>(p.mesh()).domain().statistic == Boson);
  }
}

MAKE_MAIN;
