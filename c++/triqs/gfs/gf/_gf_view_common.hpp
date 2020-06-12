
// common to many classes

// ------------- All the call operators arguments -----------------------------

template <typename... Args> decltype(auto) operator()(Args &&... args) const & {
  if constexpr (sizeof...(Args) == 0)
    return const_view_type{*this};
  else {
    static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
    if constexpr ((... or clef::is_any_lazy<Args>)) // any argument is lazy ?
      return clef::make_expr_call(*this, std::forward<Args>(args)...);
    else
      return evaluator_t()(*this, std::forward<Args>(args)...);
  }
}
template <typename... Args> decltype(auto) operator()(Args &&... args) & {
  if constexpr (sizeof...(Args) == 0)
    return view_type{*this};
  else {
    static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
    if constexpr ((... or clef::is_any_lazy<Args>)) // any argument is lazy ?
      return clef::make_expr_call(*this, std::forward<Args>(args)...);
    else
      return evaluator_t()(*this, std::forward<Args>(args)...);
  }
}
template <typename... Args> decltype(auto) operator()(Args &&... args) && {
  if constexpr (sizeof...(Args) == 0)
    return view_type{std::move(*this)};
  else {
    static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
    if constexpr ((... or clef::is_any_lazy<Args>)) // any argument is lazy ?
      return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
    else
      return evaluator_t()(std::move(*this), std::forward<Args>(args)...);
  }
}

// ------------- All the [] operators without lazy arguments -----------------------------

private:
//inline static constexpr std::index_sequence<arity> _seq;

FORCEINLINE decltype(auto) call_data(long i) {
  if constexpr (data_rank == 1)
    return _data(i); // FIXME avoid the current bug with zero size ellipsis. But it is also simpler to compile in simple cases..
  else
    return _data(i, ellipsis{});
}

// FIXME C++20 lambda
template <auto... Is, typename Tu> FORCEINLINE decltype(auto) call_data_impl(std::index_sequence<Is...>, Tu const &tu) {
  if constexpr (data_rank == sizeof...(Is))
    return _data(std::get<Is>(tu)...);
  else
    return _data(std::get<Is>(tu)..., ellipsis{});
}

template <typename... T> FORCEINLINE decltype(auto) call_data(std::tuple<T...> const &tu) {
  return call_data_impl(std::make_index_sequence<sizeof...(T)>{}, tu);
}

FORCEINLINE decltype(auto) call_data(long i) const {
  if constexpr (data_rank == 1)
    return _data(i); // FIXME avoid the current bug with zero size ellipsis. But it is also simpler to compile in simple cases..
  else
    return _data(i, ellipsis{});
}

// FIXME C++20 lambda
template <auto... Is, typename Tu> FORCEINLINE decltype(auto) call_data_impl(std::index_sequence<Is...>, Tu const &tu) const {
  if constexpr (data_rank == sizeof...(Is))
    return _data(std::get<Is>(tu)...);
  else
    return _data(std::get<Is>(tu)..., ellipsis{});
}

template <typename... T> FORCEINLINE decltype(auto) call_data(std::tuple<T...> const &tu) const {
  return call_data_impl(std::make_index_sequence<sizeof...(T)>{}, tu);
}

public:
// pass a index_t of the mesh
decltype(auto) operator[](mesh_index_t const &arg) {
  EXPECTS(_mesh.is_within_boundary(arg));
  return call_data(_mesh.index_to_linear(arg));
}

decltype(auto) operator[](mesh_index_t const &arg) const {
  EXPECTS(_mesh.is_within_boundary(arg));
  return call_data(_mesh.index_to_linear(arg));
}

// pass a mesh_point of the mesh
decltype(auto) operator[](mesh_point_t const &x) {
#ifdef TRIQS_DEBUG
  if (!mesh_point_compatible_to_mesh(x, _mesh)) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
  return call_data(x.linear_index());
}

decltype(auto) operator[](mesh_point_t const &x) const {
#ifdef TRIQS_DEBUG
  if (!mesh_point_compatible_to_mesh(x, _mesh)) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
  return call_data(x.linear_index());
}

private:
using cp_worker = mesh::closest_point<Mesh, Target>;

public:
// pass an abtract closest_point. We extract the value of the domain from p, call the gf_closest_point trait
template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) {
  return call_data(_mesh.index_to_linear(cp_worker::invoke(_mesh, p)));
}
template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) const {
  return call_data(_mesh.index_to_linear(cp_worker::invoke(_mesh, p)));
}

// -------------- operator [] with tuple_com. Distinguich the lazy and non lazy case
public:
template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) & {
  static_assert(sizeof...(U) == get_n_variables<Mesh>::value, "Incorrect number of argument in [] operator");
  if constexpr ((... or clef::is_any_lazy<U>)) // any argument is lazy ?
    return clef::make_expr_subscript(*this, tu);
  else {
    static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
    auto l = [this](auto &&... y) -> decltype(auto) { return details::slice_or_access_general(*this, y...); };
    return triqs::tuple::apply(l, tu._t);
  }
}

template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) const & {
  static_assert(sizeof...(U) == get_n_variables<Mesh>::value, "Incorrect number of argument in [] operator");
  if constexpr ((... or clef::is_any_lazy<U>)) // any argument is lazy ?
    return clef::make_expr_subscript(*this, tu);
  else {
    static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
    auto l = [this](auto &&... y) -> decltype(auto) { return details::slice_or_access_general(*this, y...); };
    return triqs::tuple::apply(l, tu._t);
  }
}

template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) && {
  static_assert(sizeof...(U) == get_n_variables<Mesh>::value, "Incorrect number of argument in [] operator");
  if constexpr ((... or clef::is_any_lazy<U>)) // any argument is lazy ?
    return clef::make_expr_subscript(std::move(*this), tu);
  else {
    static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
    auto l = [this](auto &&... y) -> decltype(auto) { return details::slice_or_access_general(*this, y...); };
    return triqs::tuple::apply(l, tu._t);
  }
}

// ------------- [] with lazy arguments -----------------------------

template <typename Arg> auto operator[](Arg &&arg) const &REQUIRES(clef::is_any_lazy<Arg>) {
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

template <typename Arg> auto operator[](Arg &&arg) & REQUIRES(clef::is_any_lazy<Arg>) {
  return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
}

template <typename Arg> auto operator[](Arg &&arg) && REQUIRES(clef::is_any_lazy<Arg>) {
  return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
}

// --------------------- A direct access to the grid point --------------------------

template <typename... Args> FORCEINLINE decltype(auto) on_mesh(Args &&... args) {
  return call_data(_mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
}

template <typename... Args> FORCEINLINE decltype(auto) on_mesh(Args &&... args) const {
  return call_data(_mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
}

template <typename... Args> FORCEINLINE decltype(auto) on_mesh_from_linear_index(Args &&... args) {
  return call_data(_mesh.index_to_linear(linear_mesh_index_t(std::forward<Args>(args)...)));
}

template <typename... Args> FORCEINLINE decltype(auto) on_mesh_from_linear_index(Args &&... args) const {
  return call_data(_mesh.index_to_linear(linear_mesh_index_t(std::forward<Args>(args)...)));
}

//----------------------------- HDF5 -----------------------------

/// HDF5 name
static std::string hdf5_format() { return "Gf"; }

friend struct gf_h5_rw<Mesh, Target>;

/// Write into HDF5
friend void h5_write(h5::group fg, std::string const &subgroup_name, this_t const &g) {
  auto gr = fg.create_group(subgroup_name);
  write_hdf5_format(gr, g);
  gf_h5_rw<Mesh, Target>::write(gr, g);
}

/// Read from HDF5
friend void h5_read(h5::group fg, std::string const &subgroup_name, this_t &g) {
  auto gr       = fg.open_group(subgroup_name);
  auto tag_file = read_hdf5_format(gr);
  if (!(tag_file[0] == 'G' and tag_file[1] == 'f'))
    TRIQS_RUNTIME_ERROR << "h5_read : For a Green function, the type tag should be Gf (or Gfxxxx for old archive) "
                        << " while I found " << tag_file;
  gf_h5_rw<Mesh, Target>::read(gr, g);
}

//-----------------------------  BOOST Serialization -----------------------------
private:
friend class boost::serialization::access;

public:
/// The serialization as required by Boost
template <class Archive> void serialize(Archive &ar, const unsigned int version) {
  ar &_data;
  ar &_mesh;
  ar &_indices;
}

//----------------------------- print  -----------------------------

/// IO
friend std::ostream &operator<<(std::ostream &out, this_t const &x) { return out << "this_t"; }

//----------------------------- MPI  -----------------------------

// mako <%def name="mpidoc(OP)">
/**
     * Initiate (lazy) MPI ${OP}
     *
     * When the returned object is used at the RHS of operator = or in a constructor of a gf,
     * the MPI ${OP} operation is performed.
     *
     * @group MPI
     * @param g The Green function
     * @param c The MPI communicator (default is world)
     * @param root The root of the broadcast communication in the MPI sense.
     * @return Returns a lazy object describing the object and the MPI operation to be performed.
     *
     */
// mako </%def>

// mako ${mpidoc("Bcast")}
friend void mpi_broadcast(this_t &g, mpi::communicator c = {}, int root = 0) {
  // Shall we bcast mesh ?
  mpi::broadcast(g.data(), c, root);
}

// mako ${mpidoc("Reduce")}
friend mpi_lazy<mpi::tag::reduce, const_view_type> mpi_reduce(this_t const &a, mpi::communicator c = {}, int root = 0, bool all = false,
                                                              MPI_Op op = MPI_SUM) {
  return {a(), c, root, all, op};
}

// mako ${mpidoc("Scatter")}
friend mpi_lazy<mpi::tag::scatter, const_view_type> mpi_scatter(this_t const &a, mpi::communicator c = {}, int root = 0) {
  return {a(), c, root, true};
}

// mako ${mpidoc("Gather")}
friend mpi_lazy<mpi::tag::gather, const_view_type> mpi_gather(this_t const &a, mpi::communicator c = {}, int root = 0, bool all = false) {
  return {a(), c, root, all};
}
