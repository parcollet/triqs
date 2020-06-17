#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;

TEST(Gf, SimpleAssign) {


  static_assert(std::is_constructible<std::complex<double>, matsubara_freq>::value, "oops");

  triqs::clef::placeholder<0> om_;
  auto g  = gf<imfreq>{{10, Fermion, 10}, {2, 2}};
  auto g2 = g;
  auto g3 = g;

  g(om_) << om_ + 0.0;
  g2(om_) << om_;
  g3(om_) << om_ + om_;

  EXPECT_ARRAY_NEAR(g.data(), g2.data());
  EXPECT_ARRAY_NEAR(g.data(), g3.data() / 2);

  auto g4 = gf<imfreq, tensor_valued<3>>{};
  auto g5 = gf<imfreq, tensor_valued<3>>{{10, Fermion}, {3, 3, 3}};

  rw_h5(g5, "g5");
}
MAKE_MAIN;
