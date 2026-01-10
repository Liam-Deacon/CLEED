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

/**
 * @brief Structure containing information needed for LEED pattern generation.
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

/**
 * @brief Read pattern from file.
 * @param file Input file handle.
 * @return pattern_t* Pointer to pattern_t struct.
 *
 * Read pattern configuration from file and return allocated pattern_t.
 */
pattern_t *pattern_read(FILE *file);

/**
 * @brief Free memory for pattern_t struct.
 * @param pat Pointer to pattern_t struct to free.
 */
void pattern_free(pattern_t *pat);

/**
 * @brief Get pattern radius.
 * @param pat Pointer to pattern_t struct.
 * @return Radius value.
 */
double pattern_get_radius(pattern_t *pat);

/**
 * @brief Check if pattern uses square area.
 * @param pat Pointer to pattern_t struct.
 * @return true if square, false otherwise.
 */
bool pattern_is_square(const pattern_t *pat);

/**
 * @brief Check if domain is commensurate.
 * @param pat Pointer to pattern_t struct.
 * @param domain Domain index to check.
 * @return true if commensurate, false otherwise.
 */
bool pattern_domain_is_commensurate(const pattern_t *pat, size_t domain);

/**
 * @brief Get number of domains.
 * @param pat Pointer to pattern_t struct.
 * @return Number of domains.
 */
size_t pattern_get_n_domains(pattern_t *pat);

/**
 * @brief Get pattern title.
 * @param pat Pointer to pattern_t struct.
 * @return Pointer to title string.
 */
const char *pattern_get_title(pattern_t *pat);

/**
 * @brief Print pattern to stream.
 * @param stream Output file stream.
 * @param pat Pointer to pattern_t struct.
 */
void pattern_printf(FILE *stream, pattern_t *pat);

/**
 * @brief Set a1 basis vector.
 * @param pat Pointer to pattern_t struct.
 * @param a1 Pointer to basis vector.
 */
void pattern_set_a1(pattern_t *pat, basis_vector_t *a1);

/**
 * @brief Set a2 basis vector.
 * @param pat Pointer to pattern_t struct.
 * @param a2 Pointer to basis vector.
 */
void pattern_set_a2(pattern_t *pat, basis_vector_t *a2);

/**
 * @brief Set pattern radius.
 * @param pat Pointer to pattern_t struct.
 * @param radius Radius value to set.
 */
void pattern_set_radius(pattern_t *pat, double radius);

/**
 * @brief Set pattern title.
 * @param pat Pointer to pattern_t struct.
 * @param title Title string to set.
 */
void pattern_set_title(pattern_t *pat, char *title);

/**
 * @brief Set whether pattern uses square area.
 * @param pat Pointer to pattern_t struct.
 * @param is_square true for square, false for circular.
 */
void pattern_set_square(pattern_t *pat, bool is_square);

/**
 * @brief Set maximum number of domains.
 * @param pat Pointer to pattern_t struct.
 * @param n_domains Maximum number of superstructure domains.
 * @return Status code.
 */
int pattern_set_max_domains(pattern_t *pat, size_t n_domains);

/**
 * @brief Set superstructure matrix for a domain.
 * @param pat Pointer to pattern_t struct.
 * @param mat Pointer to 2x2 matrix.
 * @param domain Domain index.
 */
void pattern_set_superstructure_matrix(pattern_t *pat,
        matrix_2x2_t *mat, size_t domain);

/**
 * @brief Get superstructure matrix for a domain.
 * @param pat Pointer to pattern_t struct.
 * @param domain Domain index.
 * @return Pointer to 2x2 matrix.
 */
const matrix_2x2_t *get_superstructure_matrix(const pattern_t *pat, size_t domain);

/**
 * @brief Calculate substrate spots for pattern.
 * @param pat Pointer to pattern_t struct.
 * @return Pointer to spots_t struct containing substrate spots.
 */
spots_t *pattern_calculate_substrate_spots(const pattern_t *pat);

/**
 * @brief Calculate superstructure spots for a domain.
 * @param pat Pointer to pattern_t struct.
 * @param domain Domain index.
 * @return Pointer to spots_t struct containing superstructure spots.
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