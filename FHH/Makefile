CC = g++

# Flags for -c compilation and linking
FLAG = -c -g -I/usr/include -I/usr/local/include
LFLAG = -L/usr/lib64 -lgsl -lgslcblas -lm --with-pic
CXXFLAGS = -O2 -Wall -march=native

OBJS  = main.o \
	random.o utility.o \
	gene.o chromosome.o population.o objfunc.o \
	codetable.o subset.o mpm.o cache.o ecga.o \
	benchmark.o \
        F01_shifted_sphere.o \
        F02_shifted_schwefel.o \
        F03_shifted_rotated_high_cond_elliptic.o \
        F04_shifted_schwefel_noise.o \
        F05_schwefel_global_opt_bound.o \
        F06_shifted_rosenbrock.o \
        F07_shifted_rotated_griewank.o \
        F08_shifted_rotated_ackley_global_opt_bound.o \
        F09_shifted_rastrigin.o \
        F10_shifted_rotated_rastrigin.o \
        F11_shifted_rotated_weierstrass.o \
        F12_schwefel.o \
        F13_shifted_expanded_griewank_rosenbrock.o \
        F14_shifted_rotated_expanded_scaffer.o \
        F15_hybrid_composition_1.o \
        F16_rotated_hybrid_composition_1.o \
        F17_rotated_hybrid_composition_1_noise.o \
        F18_rotated_hybrid_composition_2.o \
        F19_rotated_hybrid_composition_2_narrow_basin_global_opt.o \
        F20_rotated_hybrid_composition_2_global_opt_bound.o \
        F21_rotated_hybrid_composition_3.o \
        F22_rotated_hybrid_composition_3_high_cond_num_matrix.o \
        F23_noncontinuous_rotated_hybrid_composition_3.o \
        F24_rotated_hybrid_composition_4.o \
        F25_rotated_hybrid_composition_4_bound.o\
	F26_8_peaks1.o\
	F27_8_peaks2.o

ecga: $(OBJS)
	$(CC) $(LFLAGS) -o rECGA_FHH $(OBJS)

clean:
	rm *.o

#--- UTILITY stuff --------------------------------------------------------

# random number generator
random.o: random.cpp random.hpp
	$(CC) $(FLAG) random.cpp

# utility functions and procedures
utility.o: utility.cpp utility.hpp random.hpp
	$(CC) $(FLAG) utility.cpp


#--- general GA stuff -----------------------------------------------------

# gene
gene.o: gene.cpp gene.hpp random.hpp
	$(CC) $(FLAG) gene.cpp
# chromosome
chromosome.o: chromosome.cpp chromosome.hpp random.hpp gene.hpp parameter.hpp
	$(CC) $(FLAG) chromosome.cpp

# population
population.o: population.cpp population.hpp parameter.hpp utility.hpp
	$(CC) $(FLAG) population.cpp

# objective function
objfunc.o: objfunc.cpp objfunc.hpp
	$(CC) $(FLAG) objfunc.cpp

#--- ECGA stuff ----------------------------------------------------------

codetable.o: codetable.cpp codetable.h
	$(CC) $(FLAG) codetable.cpp

# a set in the mpm
subset.o: subset.cpp subset.hpp parameter.hpp utility.hpp population.hpp 
	$(CC) $(FLAG) subset.cpp

# an MPM is a collection of "subsets"
mpm.o: mpm.cpp mpm.hpp subset.hpp population.hpp parameter.hpp cache.hpp
	$(CC) $(FLAG) mpm.cpp

# cache to store mergers
cache.o: cache.cpp cache.hpp parameter.hpp subset.hpp
	$(CC) $(FLAG) cache.cpp

# ECGA
ecga.o: ecga.cpp ecga.hpp parameter.hpp population.hpp mpm.hpp utility.hpp
	$(CC) $(FLAG) ecga.cpp

#--- MAIN  -----------------------------------------------------------------

main.o: main.cpp utility.hpp random.hpp parameter.hpp ecga.hpp
	$(CC) $(FLAG) main.cpp

benchmark.o: benchmark.cpp benchmark.h test_func.h HCJob.h
	$(CC) $(FLAG) benchmark.cpp

F01_shifted_sphere.o: benchmark.h F01_shifted_sphere.h F01_shifted_sphere.cpp
	$(CC) $(FLAG) F01_shifted_sphere.cpp

F02_shifted_schwefel.o: benchmark.h F02_shifted_schwefel.h F02_shifted_schwefel.cpp
	$(CC) $(FLAG) F02_shifted_schwefel.cpp

F03_shifted_rotated_high_cond_elliptic.o: benchmark.h F03_shifted_rotated_high_cond_elliptic.h F03_shifted_rotated_high_cond_elliptic.cpp
	$(CC) $(FLAG) F03_shifted_rotated_high_cond_elliptic.cpp

F04_shifted_schwefel_noise.o:benchmark.h F04_shifted_schwefel_noise.h F04_shifted_schwefel_noise.cpp
	$(CC) $(FLAG) F04_shifted_schwefel_noise.cpp

F05_schwefel_global_opt_bound.o:benchmark.h F05_schwefel_global_opt_bound.h F05_schwefel_global_opt_bound.cpp
	$(CC) $(FLAG) F05_schwefel_global_opt_bound.cpp

F06_shifted_rosenbrock.o:benchmark.h F06_shifted_rosenbrock.h F06_shifted_rosenbrock.cpp
	$(CC) $(FLAG) F06_shifted_rosenbrock.cpp

F07_shifted_rotated_griewank.o:benchmark.h F07_shifted_rotated_griewank.h F07_shifted_rotated_griewank.cpp
	$(CC) $(FLAG) F07_shifted_rotated_griewank.cpp

F08_shifted_rotated_ackley_global_opt_bound.o:benchmark.h F08_shifted_rotated_ackley_global_opt_bound.h F08_shifted_rotated_ackley_global_opt_bound.cpp
	 $(CC) $(FLAG) F08_shifted_rotated_ackley_global_opt_bound.cpp

F09_shifted_rastrigin.o:benchmark.h F09_shifted_rastrigin.h F09_shifted_rastrigin.cpp
	$(CC) $(FLAG) F09_shifted_rastrigin.cpp

F10_shifted_rotated_rastrigin.o:benchmark.h F10_shifted_rotated_rastrigin.h F10_shifted_rotated_rastrigin.cpp
	$(CC) $(FLAG) F10_shifted_rotated_rastrigin.cpp

F11_shifted_rotated_weierstrass.o: benchmark.h F11_shifted_rotated_weierstrass.h F11_shifted_rotated_weierstrass.cpp
	$(CC) $(FLAG) F11_shifted_rotated_weierstrass.cpp

F12_schwefel.o: benchmark.h F12_schwefel.h F12_schwefel.cpp
	$(CC) $(FLAG) F12_schwefel.cpp

F13_shifted_expanded_griewank_rosenbrock.o: benchmark.h F13_shifted_expanded_griewank_rosenbrock.h F13_shifted_expanded_griewank_rosenbrock.cpp
	$(CC) $(FLAG) F13_shifted_expanded_griewank_rosenbrock.cpp

F14_shifted_rotated_expanded_scaffer.o: benchmark.h F14_shifted_rotated_expanded_scaffer.h F14_shifted_rotated_expanded_scaffer.cpp
	$(CC) $(FLAG) F14_shifted_rotated_expanded_scaffer.cpp

F15_hybrid_composition_1.o: benchmark.h F15_hybrid_composition_1.h F15_hybrid_composition_1.cpp
	$(CC) $(FLAG) F15_hybrid_composition_1.cpp

F16_rotated_hybrid_composition_1.o: benchmark.h F16_rotated_hybrid_composition_1.h F16_rotated_hybrid_composition_1.cpp
	$(CC) $(FLAG) F16_rotated_hybrid_composition_1.cpp

F17_rotated_hybrid_composition_1_noise.o: benchmark.h F17_rotated_hybrid_composition_1_noise.h F17_rotated_hybrid_composition_1_noise.cpp
	$(CC) $(FLAG) F17_rotated_hybrid_composition_1_noise.cpp

F18_rotated_hybrid_composition_2.o: benchmark.h F18_rotated_hybrid_composition_2.h F18_rotated_hybrid_composition_2.cpp
	$(CC) $(FLAG) F18_rotated_hybrid_composition_2.cpp

F19_rotated_hybrid_composition_2_narrow_basin_global_opt.o: benchmark.h F19_rotated_hybrid_composition_2_narrow_basin_global_opt.h F19_rotated_hybrid_composition_2_narrow_basin_global_opt.cpp
	$(CC) $(FLAG) F19_rotated_hybrid_composition_2_narrow_basin_global_opt.cpp

F20_rotated_hybrid_composition_2_global_opt_bound.o: benchmark.h F20_rotated_hybrid_composition_2_global_opt_bound.h F20_rotated_hybrid_composition_2_global_opt_bound.cpp
	$(CC) $(FLAG) F20_rotated_hybrid_composition_2_global_opt_bound.cpp

F21_rotated_hybrid_composition_3.o: benchmark.h F21_rotated_hybrid_composition_3.h F21_rotated_hybrid_composition_3.cpp
	$(CC) $(FLAG) F21_rotated_hybrid_composition_3.cpp

F22_rotated_hybrid_composition_3_high_cond_num_matrix.o: benchmark.h F22_rotated_hybrid_composition_3_high_cond_num_matrix.h F22_rotated_hybrid_composition_3_high_cond_num_matrix.cpp
	$(CC) $(FLAG) F22_rotated_hybrid_composition_3_high_cond_num_matrix.cpp

F23_noncontinuous_rotated_hybrid_composition_3.o: benchmark.h F23_noncontinuous_rotated_hybrid_composition_3.h F23_noncontinuous_rotated_hybrid_composition_3.cpp
	$(CC) $(FLAG) F23_noncontinuous_rotated_hybrid_composition_3.cpp

F24_rotated_hybrid_composition_4.o: benchmark.h F24_rotated_hybrid_composition_4.h F24_rotated_hybrid_composition_4.cpp
	$(CC) $(FLAG) F24_rotated_hybrid_composition_4.cpp

F25_rotated_hybrid_composition_4_bound.o: benchmark.h F25_rotated_hybrid_composition_4_bound.h F25_rotated_hybrid_composition_4_bound.cpp
	$(CC) $(FLAG) F25_rotated_hybrid_composition_4_bound.cpp

F26_8_peaks1.o: benchmark.h F26_8_peaks1.h F26_8_peaks1.cpp
	$(CC) $(FLAG) F26_8_peaks1.cpp

F27_8_peaks2.o: benchmark.h F27_8_peaks2.h F27_8_peaks2.cpp
	$(CC) $(FLAG) F27_8_peaks2.cpp


