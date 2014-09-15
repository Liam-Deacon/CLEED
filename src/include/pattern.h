#ifndef PATTERN_H
#define PATTERN_H

#include "basis_vector.h"
#include "matrix_2x2.h"
#include "spots.h"
#include <stdbool.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  #include <list>

  extern "C" {
#endif

/*! \struct pattern_t
 *  \brief Structure containing information needed for LEED pattern generation.
 */
typedef struct pattern_t 
{
  char *title;              /*!< pointer to title string */
  basis_vector_t a1;        /*!< a1 basis vector */
  basis_vector_t a2;        /*!< a2 basis vector */
  matrix_2x2_t *M_SS;       /*!< array of superstructure matrices */
  size_t n_domains;         /*!< number of superstructures */
  double radius;            /*!< maximum radius for valid spots */
  bool square;              /*!< switch for circular or square valid area */
} pattern_t;

extern pattern_t pattern_default; 

/* pattern_t functions */
/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
pattern_t *pattern_alloc(size_t n_domains);

/*! \fn pattern_t *pattern_init(size_t n_domains)
 *  \brief Allocate memory and initialise pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * and initialise members to default values.
 */
pattern_t *pattern_init(size_t n_domains);

/*! \fn pattern_t *pattern_read(FILE *file)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
pattern_t *pattern_read(FILE *file);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_free(pattern_t *pat);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
double pattern_get_radius(pattern_t *pat);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
bool pattern_is_square(const pattern_t *pat);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
bool pattern_domain_is_commensurate(const pattern_t *pat, size_t domain);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
size_t pattern_get_n_domains(pattern_t *pat);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
const char *pattern_get_title(pattern_t *pat);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_printf(FILE *stream, pattern_t *pat);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_set_a1(pattern_t *pat, basis_vector_t *a1);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_set_a2(pattern_t *pat, basis_vector_t *a2);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_set_radius(pattern_t *pat, double radius);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_set_title(pattern_t *pat, char *title);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_set_square(pattern_t *pat, bool is_square);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
int pattern_set_max_domains(pattern_t *pat, size_t n_domains);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
void pattern_set_superstructure_matrix(pattern_t *pat, 
        matrix_2x2_t *mat, size_t domain);
        
/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
const matrix_2x2_t *get_superstructure_matrix(const pattern_t *pat, size_t domain);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
spots_t *pattern_calculate_substrate_spots(const pattern_t *pat);

/*! \fn pattern_t *pattern_alloc(size_t n_domains)
 *  \brief Allocates memory for pattern_t struct.
 *  \param n_domains The maximum number of superstructure domains to allocate.
 *  \return pattern_t* Pointer to pattern_t struct.
 *
 * Allocate memory for pattern_t structure with \a n_domains, 
 * but do not initialise any members of pattern_t other than \a n_domains.
 */
spots_t *pattern_calculate_superstructure_spots(const pattern_t *pat, size_t domain);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

/*! \class Pattern
 *  \brief C++ wrapper class for pattern_t struct and associated functions.
 */
class Pattern {
  public:
    Pattern();
    ~Pattern();
    
    void setTitle(std::string title);
    void setBasisVectorA1(double x, double y);
    void setBasisVectorA2(double x, double y);
    int setNumberOfDomains(std::size_t ndom);
    void setRadius(double radius);
    void setSuperStructureMatrix(Matrix_2x2 mat, std::size_t domain);
    void setSquare(bool isSquare);
    
    const string getTitle();
    size_t getNumberOfDomains();
    double getRadius();
    const std::list<double> getBasisVectorA1();
    const std::list<double> getBasisVectorA2();
    const std::list<Matrix2x2> getSuperStructureMatrices();
    bool isSquare();
    
  protected:
    pattern_t pat;
};

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* PATTERN_H */