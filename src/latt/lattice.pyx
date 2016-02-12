from stdio cimport FILE, fopen, fclose

cdef extern from "latt.h":
    ctypedef struct lattice
    lattice *lattice_read_input(const char *filename)
    void lattice_generate_atoms(lattice *latt)
    lattice *lattice_read_input(const char *filename)
    lattice *lattice_init(size_t n_atoms)
    int lattice_realloc(lattice *lat, size_t size)
    int lattice_set_input_filename(lattice *lat, const char *filename)
    int lattice_set_output_filename(lattice *lat, const char *filename)
    int lattice_set_script(lattice *lat, const char *script)
    void lattice_free(lattice *lat)
    void lattice_free_atom_list(lattice *lat)
    int lattice_read(lattice *lat, coord *a1, coord *a2, coord *a3,
                     coord *nor, coord *bas, char *bas_name, size_t *n_bas)
    int lattice_set_atom(lattice *lat, const atom *atom, size_t index)
    int lattice_set_atom_list(lattice *lat, const atom *atoms, size_t n_atoms)
    void lattice_printf(FILE *output, const lattice *lat)
    void lattice_debug(const lattice *lat)
    
    double lattice_get_a(const lattice *lat)
    double lattice_get_b(const lattice *lat)
    double lattice_get_c(const lattice *lat)
    double lattice_get_max_disp(const lattice *lat)
    double lattice_get_max_disp_z(const lattice *lat)
    size_t lattice_get_max_layers(const lattice *lat)
    size_t lattice_get_max_cells(const lattice *lat)
    double lattice_get_h(const lattice *lat)
    double lattice_get_k(const lattice *lat)
    double lattice_get_l(const lattice *lat)
    const char *lattice_get_input_filename(const lattice *lat)
    const char *lattice_get_output_filename(const lattice *lat)
    size_t lattice_get_n_atoms(const lattice *lat)
    const atom *lattice_get_atom_list(const lattice *lat)
    atom *lattice_get_atom(const lattice *lat, size_t index)
    void lattice_atom_index_swap(lattice *lat, size_t i, size_t j)
    
    coord *lattice_get_surface_normal(const lattice *lat, const coord *a1,
                                        const coord *a2, const coord *a3)
    
    double **lattice_normal_get_rotation_matrix(const coord *normal)
    
    void lattice_setup(lattice *lat, coord *a1, coord *a2, coord *a3, coord *nor,
                       coord *bas, char *bas_name, size_t *n_bas)
             
    miller_hkl *lattice_get_miller_hkl(const lattice *lat)

    
cdef class Lattice:
    cdef lattice *_thisptr
    
    def __cinit__(self, unsigned long n_atoms):
        self._thisptr = lattice_init(n_atoms)
        if self._thisptr == NULL:
            raise MemoryError("Insufficient memory")
        
    def __dealloc__(self):
        if self._thisptr != NULL:
            lattice_free(self._thisptr)
            
    cpdef load_file(const char *filename):
        if self._thisptr != NULL:
            lattice_free(self._thisptr)
        self._thisptr = lattice_read_input(filename)
    
    cpdef save_file(const char *filename):
        FILE *f = fopen(filename, "w")
        if f == NULL:
            raise FileNotFoundError(2, 
                    "No such file or directory: '%s'" % filename)
        lattice_printf(f, self._thisptr)
        fclose(f)