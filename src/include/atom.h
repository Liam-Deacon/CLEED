#ifndef ATOM_H
#define ATOM_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*! \typedef atom_t
 *  \brief basic atom struct.
 */
typedef struct {
  char *element;        /*!< Elemental symbol. */
  double x;             /*!< x Cartesian coordinate of atom. */
  double y;             /*!< y Cartesian coordinate of atom. */
  double z;             /*!< z Cartesian coordinate of atom. */
} atom_t;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* ATOM_H */