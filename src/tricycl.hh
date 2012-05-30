/*----------------------------------------------------------------------------*
 * TriCyCL main class definition.
 *----------------------------------------------------------------------------*/

#ifndef tricycl_hh
#define tricycl_hh

template<typename T>
struct TypeToOpt {
}; // struct TypeToOpt

template<> TypeToOpt<float> {
	const char * option_string() {
		return "-Dreal_t=float";
	} // option_string
}; // struct TypeToOpt

template<> TypeToOpt<double> {
	const char * option_string() {
		return "-Dreal_t=double";
	} // option_string
}; // struct TypeToOpt

class TriCyCL
{
public:

	static TriCyCL & instance() {
		static TriCyCL t;
		return t;
	} // instance

	template<typename T> int32_t solve(size_t system_size, size_t num_systems,
		T * a, T * b, T * c, T * d, T * x);

private:

	TriCyCL() {}
	TriCyCL(const TriCyCL &) {}
	TriCyCL & operator = (const TriCyCL &);

	~TriCyCL() {}

}; // class TriCyCL

template<typename T>
int32_t TriCyCL<T>::solve(size_t system_size, size_t num_systems,
	T * a, T * b, T * c, T * d, T * x) {
} // TriCyCL<>::solve

#endif // tricycl_hh
