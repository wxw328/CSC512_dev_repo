#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#ifndef _LBM_H_
#define _LBM_H_

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DFL1 (1.0/ 3.0)
#define DFL2 (1.0/18.0)
#define DFL3 (1.0/36.0)

#define SIZE   (10)
#define SIZE_X (1*SIZE)
#define SIZE_Y (1*SIZE)
#define SIZE_Z (13)
#define OMEGA (1.95)

#define OUTPUT_PRECISION float

#define BOOL int
#define TRUE (-1)
#define FALSE (0)

#endif /* _CONFIG_H_ */

typedef enum {C = 0,
              N, S, E, W, T, B,
              NE, NW, SE, SW,
              NT, NB, ST, SB,
              ET, EB, WT, WB,
              FLAGS, N_CELL_ENTRIES} CELL_ENTRIES;
#define N_DISTR_FUNCS FLAGS

typedef enum {OBSTACLE    = 1 << 0,
              ACCEL       = 1 << 1,
              IN_OUT_FLOW = 1 << 2} CELL_FLAGS;

#ifndef _LBM_MACROS_H_
#define _LBM_MACROS_H_

typedef double LBM_Grid[SIZE_Z*SIZE_Y*SIZE_X*N_CELL_ENTRIES];
typedef LBM_Grid* LBM_GridPtr;

#define CALC_INDEX(x,y,z,e) ((e)+N_CELL_ENTRIES*((x)+ \
                             (y)*SIZE_X+(z)*SIZE_X*SIZE_Y))

#define SWEEP_VAR int i;

#define SWEEP_START(x1,y1,z1,x2,y2,z2) \
	for( i = CALC_INDEX(x1, y1, z1, 0); \
	     i < CALC_INDEX(x2, y2, z2, 0); \
			 i += N_CELL_ENTRIES ) {

#define SWEEP_END }

#define SWEEP_X  ((i / N_CELL_ENTRIES) % SIZE_X)
#define SWEEP_Y (((i / N_CELL_ENTRIES) / SIZE_X) % SIZE_Y)
#define SWEEP_Z  ((i / N_CELL_ENTRIES) / (SIZE_X*SIZE_Y))

#define GRID_ENTRY(g,x,y,z,e)          ((g)[CALC_INDEX( x,  y,  z, e)])
#define GRID_ENTRY_SWEEP(g,dx,dy,dz,e) ((g)[CALC_INDEX(dx, dy, dz, e)+(i)])

#define LOCAL(g,e)       (GRID_ENTRY_SWEEP( g,  0,  0,  0, e ))
#define NEIGHBOR_C(g,e)  (GRID_ENTRY_SWEEP( g,  0,  0,  0, e ))
#define NEIGHBOR_N(g,e)  (GRID_ENTRY_SWEEP( g,  0, +1,  0, e ))
#define NEIGHBOR_S(g,e)  (GRID_ENTRY_SWEEP( g,  0, -1,  0, e ))
#define NEIGHBOR_E(g,e)  (GRID_ENTRY_SWEEP( g, +1,  0,  0, e ))
#define NEIGHBOR_W(g,e)  (GRID_ENTRY_SWEEP( g, -1,  0,  0, e ))
#define NEIGHBOR_T(g,e)  (GRID_ENTRY_SWEEP( g,  0,  0, +1, e ))
#define NEIGHBOR_B(g,e)  (GRID_ENTRY_SWEEP( g,  0,  0, -1, e ))
#define NEIGHBOR_NE(g,e) (GRID_ENTRY_SWEEP( g, +1, +1,  0, e ))
#define NEIGHBOR_NW(g,e) (GRID_ENTRY_SWEEP( g, -1, +1,  0, e ))
#define NEIGHBOR_SE(g,e) (GRID_ENTRY_SWEEP( g, +1, -1,  0, e ))
#define NEIGHBOR_SW(g,e) (GRID_ENTRY_SWEEP( g, -1, -1,  0, e ))
#define NEIGHBOR_NT(g,e) (GRID_ENTRY_SWEEP( g,  0, +1, +1, e ))
#define NEIGHBOR_NB(g,e) (GRID_ENTRY_SWEEP( g,  0, +1, -1, e ))
#define NEIGHBOR_ST(g,e) (GRID_ENTRY_SWEEP( g,  0, -1, +1, e ))
#define NEIGHBOR_SB(g,e) (GRID_ENTRY_SWEEP( g,  0, -1, -1, e ))
#define NEIGHBOR_ET(g,e) (GRID_ENTRY_SWEEP( g, +1,  0, +1, e ))
#define NEIGHBOR_EB(g,e) (GRID_ENTRY_SWEEP( g, +1,  0, -1, e ))
#define NEIGHBOR_WT(g,e) (GRID_ENTRY_SWEEP( g, -1,  0, +1, e ))
#define NEIGHBOR_WB(g,e) (GRID_ENTRY_SWEEP( g, -1,  0, -1, e ))


#define COLLIDE_STREAM
#ifdef COLLIDE_STREAM

#define SRC_C(g)  (LOCAL( g, C  ))
#define SRC_N(g)  (LOCAL( g, N  ))
#define SRC_S(g)  (LOCAL( g, S  ))
#define SRC_E(g)  (LOCAL( g, E  ))
#define SRC_W(g)  (LOCAL( g, W  ))
#define SRC_T(g)  (LOCAL( g, T  ))
#define SRC_B(g)  (LOCAL( g, B  ))
#define SRC_NE(g) (LOCAL( g, NE ))
#define SRC_NW(g) (LOCAL( g, NW ))
#define SRC_SE(g) (LOCAL( g, SE ))
#define SRC_SW(g) (LOCAL( g, SW ))
#define SRC_NT(g) (LOCAL( g, NT ))
#define SRC_NB(g) (LOCAL( g, NB ))
#define SRC_ST(g) (LOCAL( g, ST ))
#define SRC_SB(g) (LOCAL( g, SB ))
#define SRC_ET(g) (LOCAL( g, ET ))
#define SRC_EB(g) (LOCAL( g, EB ))
#define SRC_WT(g) (LOCAL( g, WT ))
#define SRC_WB(g) (LOCAL( g, WB ))

#define DST_C(g)  (NEIGHBOR_C ( g, C  ))
#define DST_N(g)  (NEIGHBOR_N ( g, N  ))
#define DST_S(g)  (NEIGHBOR_S ( g, S  ))
#define DST_E(g)  (NEIGHBOR_E ( g, E  ))
#define DST_W(g)  (NEIGHBOR_W ( g, W  ))
#define DST_T(g)  (NEIGHBOR_T ( g, T  ))
#define DST_B(g)  (NEIGHBOR_B ( g, B  ))
#define DST_NE(g) (NEIGHBOR_NE( g, NE ))
#define DST_NW(g) (NEIGHBOR_NW( g, NW ))
#define DST_SE(g) (NEIGHBOR_SE( g, SE ))
#define DST_SW(g) (NEIGHBOR_SW( g, SW ))
#define DST_NT(g) (NEIGHBOR_NT( g, NT ))
#define DST_NB(g) (NEIGHBOR_NB( g, NB ))
#define DST_ST(g) (NEIGHBOR_ST( g, ST ))
#define DST_SB(g) (NEIGHBOR_SB( g, SB ))
#define DST_ET(g) (NEIGHBOR_ET( g, ET ))
#define DST_EB(g) (NEIGHBOR_EB( g, EB ))
#define DST_WT(g) (NEIGHBOR_WT( g, WT ))
#define DST_WB(g) (NEIGHBOR_WB( g, WB ))

#else /* COLLIDE_STREAM */

#define SRC_C(g)  (NEIGHBOR_C ( g, C  ))
#define SRC_N(g)  (NEIGHBOR_S ( g, N  ))
#define SRC_S(g)  (NEIGHBOR_N ( g, S  ))
#define SRC_E(g)  (NEIGHBOR_W ( g, E  ))
#define SRC_W(g)  (NEIGHBOR_E ( g, W  ))
#define SRC_T(g)  (NEIGHBOR_B ( g, T  ))
#define SRC_B(g)  (NEIGHBOR_T ( g, B  ))
#define SRC_NE(g) (NEIGHBOR_SW( g, NE ))
#define SRC_NW(g) (NEIGHBOR_SE( g, NW ))
#define SRC_SE(g) (NEIGHBOR_NW( g, SE ))
#define SRC_SW(g) (NEIGHBOR_NE( g, SW ))
#define SRC_NT(g) (NEIGHBOR_SB( g, NT ))
#define SRC_NB(g) (NEIGHBOR_ST( g, NB ))
#define SRC_ST(g) (NEIGHBOR_NB( g, ST ))
#define SRC_SB(g) (NEIGHBOR_NT( g, SB ))
#define SRC_ET(g) (NEIGHBOR_WB( g, ET ))
#define SRC_EB(g) (NEIGHBOR_WT( g, EB ))
#define SRC_WT(g) (NEIGHBOR_EB( g, WT ))
#define SRC_WB(g) (NEIGHBOR_ET( g, WB ))

#define DST_C(g)  (LOCAL( g, C  ))
#define DST_N(g)  (LOCAL( g, N  ))
#define DST_S(g)  (LOCAL( g, S  ))
#define DST_E(g)  (LOCAL( g, E  ))
#define DST_W(g)  (LOCAL( g, W  ))
#define DST_T(g)  (LOCAL( g, T  ))
#define DST_B(g)  (LOCAL( g, B  ))
#define DST_NE(g) (LOCAL( g, NE ))
#define DST_NW(g) (LOCAL( g, NW ))
#define DST_SE(g) (LOCAL( g, SE ))
#define DST_SW(g) (LOCAL( g, SW ))
#define DST_NT(g) (LOCAL( g, NT ))
#define DST_NB(g) (LOCAL( g, NB ))
#define DST_ST(g) (LOCAL( g, ST ))
#define DST_SB(g) (LOCAL( g, SB ))
#define DST_ET(g) (LOCAL( g, ET ))
#define DST_EB(g) (LOCAL( g, EB ))
#define DST_WT(g) (LOCAL( g, WT ))
#define DST_WB(g) (LOCAL( g, WB ))

#endif /* COLLIDE_STREAM */

#define MAGIC_CAST(v) ((unsigned int*) ((void*) (&(v))))
#define FLAG_VAR(v) unsigned int* const _aux_ = MAGIC_CAST(v)

#define TEST_FLAG_SWEEP(g,f)     ((*MAGIC_CAST(LOCAL(g, FLAGS))) & (f))
#define SET_FLAG_SWEEP(g,f)      {FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_) |=  (f);}
#define CLEAR_FLAG_SWEEP(g,f)    {FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_) &= ~(f);}
#define CLEAR_ALL_FLAGS_SWEEP(g) {FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_)  =    0;}

#define TEST_FLAG(g,x,y,z,f)     ((*MAGIC_CAST(GRID_ENTRY(g, x, y, z, FLAGS))) & (f))
#define SET_FLAG(g,x,y,z,f)      {FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_) |=  (f);}
#define CLEAR_FLAG(g,x,y,z,f)    {FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_) &= ~(f);}
#define CLEAR_ALL_FLAGS(g,x,y,z) {FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_)  =    0;}

#endif


void LBM_allocateGrid( double** ptr );
void LBM_freeGrid( double** ptr );
void LBM_initializeGrid( LBM_Grid grid );
void LBM_initializeSpecialCellsForLDC( LBM_Grid grid );
void LBM_loadObstacleFile( LBM_Grid grid, const char* filename );
void LBM_initializeSpecialCellsForChannel( LBM_Grid grid );
void LBM_swapGrids( LBM_GridPtr* grid1, LBM_GridPtr* grid2 );
void LBM_performStreamCollideBGK( LBM_Grid srcGrid, LBM_Grid dstGrid );
void LBM_performStreamCollideTRT( LBM_Grid srcGrid, LBM_Grid dstGrid );
void LBM_handleInOutFlow( LBM_Grid srcGrid );
void LBM_showGridStatistics( LBM_Grid Grid );
void LBM_storeVelocityField( LBM_Grid grid, const char* filename,
                           const BOOL binary );
void LBM_compareVelocityField( LBM_Grid grid, const char* filename,
                             const BOOL binary );

#endif /* _LBM_H_ */

#ifndef _MAIN_H_
#define _MAIN_H_


#include <sys/times.h>

typedef struct {
	double timeScale;
	clock_t tickStart, tickStop;
	struct tms timeStart, timeStop;

} MAIN_Time;

typedef enum {NOTHING = 0, COMPARE, STORE} MAIN_Action;
typedef enum {LDC = 0, CHANNEL} MAIN_SimType;

typedef struct {
	int nTimeSteps;
	char* resultFilename;
	MAIN_Action action;
	MAIN_SimType simType;
	char* obstacleFilename;
} MAIN_Param;

/*############################################################################*/

void MAIN_parseCommandLine( int nArgs, char* arg[], MAIN_Param* param );
void MAIN_printInfo( const MAIN_Param* param );
void MAIN_initialize( const MAIN_Param* param );
void MAIN_finalize( const MAIN_Param* param );

void MAIN_startClock( MAIN_Time* time );
void MAIN_stopClock( MAIN_Time* time, const MAIN_Param* param );

/*############################################################################*/

#endif /* _MAIN_H_ */

/*############################################################################*/

static LBM_GridPtr srcGrid, dstGrid;

/*############################################################################*/

int main( int nArgs, char* arg[] ) {
	MAIN_Param param;
	MAIN_Time time;
	int t;

	MAIN_parseCommandLine( nArgs, arg, &param );
	MAIN_printInfo( &param );
	MAIN_initialize( &param );
	MAIN_startClock( &time );


	for( t = 1; t <= param.nTimeSteps; t++ ) {
		if( param.simType == CHANNEL ) {
			LBM_handleInOutFlow( *srcGrid );
		}

		LBM_performStreamCollideTRT( *srcGrid, *dstGrid );
		LBM_swapGrids( &srcGrid, &dstGrid );

		int temp = t & 63;
		if( temp == 0 ) {
			printf( "timestep: %i\n", t );
			LBM_showGridStatistics( *srcGrid );
		}
	}

	MAIN_stopClock( &time, &param );
	MAIN_finalize( &param );

	return 0;
}

/*############################################################################*/

void MAIN_parseCommandLine( int nArgs, char* arg[], MAIN_Param* param ) {
	struct stat fileStat;
	
	if( nArgs < 5) {
		printf( "syntax: lbm <time steps> <result file> <0: nil, 1: cmp, 2: str> <0: ldc, 1: channel flow> [<obstacle file>]\n" );
		exit( 1 );
	}else {
		int null = 0;
	}
    if( nArgs > 6){
        printf( "syntax: lbm <time steps> <result file> <0: nil, 1: cmp, 2: str> <0: ldc, 1: channel flow> [<obstacle file>]\n" );
		exit( 1 );
    }else{
		int null = 0;
	}

	param->nTimeSteps     = atoi( arg[1] );
	param->resultFilename = arg[2];
	param->action         = (MAIN_Action) atoi( arg[3] );
	param->simType        = (MAIN_SimType) atoi( arg[4] );

	if( nArgs == 6 ) {
		param->obstacleFilename = arg[5];

		if( stat( param->obstacleFilename, &fileStat ) != 0 ) {
			printf( "MAIN_parseCommandLine: cannot stat obstacle file '%s'\n",
			         param->obstacleFilename );
			exit( 1 );
		}else{
			int null = 0;
		}
		if( fileStat.st_size != SIZE_X*SIZE_Y*SIZE_Z+(SIZE_Y+1)*SIZE_Z ) {
			printf( "MAIN_parseCommandLine:\n"
			        "\tsize of file '%s' is %i bytes\n"
					    "\texpected size is %i bytes\n",
			        param->obstacleFilename, (int) fileStat.st_size,
			        SIZE_X*SIZE_Y*SIZE_Z+(SIZE_Y+1)*SIZE_Z );
			exit( 1 );
		}else{
			int null = 0;
		}
	}
	else param->obstacleFilename = NULL;

}

/*############################################################################*/

void MAIN_printInfo( const MAIN_Param* param ) {
	const char actionString[3][32] = {"nothing", "compare", "store"};
	const char simTypeString[3][32] = {"lid-driven cavity", "channel flow"};
	printf( "MAIN_printInfo:\n"
	        "\tgrid size      : %i x %i x %i = %.2f * 10^6 Cells\n"
	        "\tnTimeSteps     : %i\n"
	        "\tresult file    : %s\n"
	        "\taction         : %s\n"
	        "\tsimulation type: %s\n"
	        "\tobstacle file  : %s\n\n",
	        SIZE_X, SIZE_Y, SIZE_Z, 1e-6*SIZE_X*SIZE_Y*SIZE_Z,
	        param->nTimeSteps, param->resultFilename, 
	        actionString[param->action], simTypeString[param->simType],
	        (param->obstacleFilename == NULL) ? "<none>" :
	                                            param->obstacleFilename );
}

/*############################################################################*/

void MAIN_initialize( const MAIN_Param* param ) {
	LBM_allocateGrid( (double**) &srcGrid );
	LBM_allocateGrid( (double**) &dstGrid );

	LBM_initializeGrid( *srcGrid );
	LBM_initializeGrid( *dstGrid );

	if( param->obstacleFilename != NULL ) {
		LBM_loadObstacleFile( *srcGrid, param->obstacleFilename );
		LBM_loadObstacleFile( *dstGrid, param->obstacleFilename );
	}else{
		int null = 0;
	}

	if( param->simType == CHANNEL ) {
		LBM_initializeSpecialCellsForChannel( *srcGrid );
		LBM_initializeSpecialCellsForChannel( *dstGrid );
	}
	else {
		LBM_initializeSpecialCellsForLDC( *srcGrid );
		LBM_initializeSpecialCellsForLDC( *dstGrid );
	}

	LBM_showGridStatistics( *srcGrid );
}

/*############################################################################*/

void MAIN_finalize( const MAIN_Param* param ) {
	LBM_showGridStatistics( *srcGrid );

	if( param->action == COMPARE )
		LBM_compareVelocityField( *srcGrid, param->resultFilename, TRUE );
	else {
		int null = 0;
	}
	if( param->action == STORE )
	LBM_storeVelocityField( *srcGrid, param->resultFilename, TRUE );
	else{
		int null = 0;
	}

	LBM_freeGrid( (double**) &srcGrid );
	LBM_freeGrid( (double**) &dstGrid );
}

/*############################################################################*/

void MAIN_startClock( MAIN_Time* time ) {
	time->timeScale = 1.0 / sysconf( _SC_CLK_TCK );
	time->tickStart = times( &(time->timeStart) );
}


/*############################################################################*/

void MAIN_stopClock( MAIN_Time* time, const MAIN_Param* param ) {
	time->tickStop = times( &(time->timeStop) );

	printf( "MAIN_stopClock:\n"
	        "\tusr: %7.2f sys: %7.2f tot: %7.2f wct: %7.2f MLUPS: %5.2f\n\n",
	        (time->timeStop.tms_utime - time->timeStart.tms_utime) * time->timeScale,
	        (time->timeStop.tms_stime - time->timeStart.tms_stime) * time->timeScale,
	        (time->timeStop.tms_utime - time->timeStart.tms_utime +
	         time->timeStop.tms_stime - time->timeStart.tms_stime) * time->timeScale,
	        (time->tickStop           - time->tickStart          ) * time->timeScale,
	        1.0e-6 * SIZE_X * SIZE_Y * SIZE_Z * param->nTimeSteps /
	        (time->tickStop           - time->tickStart          ) / time->timeScale );
}

void LBM_allocateGrid( double** ptr ) {
	const size_t margin = 2*SIZE_X*SIZE_Y*N_CELL_ENTRIES,
	             size   = sizeof( LBM_Grid ) + 2*margin*sizeof( double );

	*ptr = malloc( size );
	if( ! *ptr ) {
		printf( "LBM_allocateGrid: could not allocate %.1f MByte\n",
		        size / (1024.0*1024.0) );
		exit( 1 );
	}else{
		int null = 0;
	}
	printf( "LBM_allocateGrid: allocated %.1f MByte\n",
	        size / (1024.0*1024.0) );
	*ptr += margin;
}

/*############################################################################*/

void LBM_freeGrid( double** ptr ) {
	const size_t margin = 2*SIZE_X*SIZE_Y*N_CELL_ENTRIES;

	free( *ptr-margin );
	*ptr = NULL;
}

/*############################################################################*/

void LBM_initializeGrid( LBM_Grid grid ) {
	SWEEP_VAR

	/*voption indep*/
	SWEEP_START( 0, 0, -2, 0, 0, SIZE_Z+2 )
		LOCAL( grid, C  ) = DFL1;
		LOCAL( grid, N  ) = DFL2;
		LOCAL( grid, S  ) = DFL2;
		LOCAL( grid, E  ) = DFL2;
		LOCAL( grid, W  ) = DFL2;
		LOCAL( grid, T  ) = DFL2;
		LOCAL( grid, B  ) = DFL2;
		LOCAL( grid, NE ) = DFL3;
		LOCAL( grid, NW ) = DFL3;
		LOCAL( grid, SE ) = DFL3;
		LOCAL( grid, SW ) = DFL3;
		LOCAL( grid, NT ) = DFL3;
		LOCAL( grid, NB ) = DFL3;
		LOCAL( grid, ST ) = DFL3;
		LOCAL( grid, SB ) = DFL3;
		LOCAL( grid, ET ) = DFL3;
		LOCAL( grid, EB ) = DFL3;
		LOCAL( grid, WT ) = DFL3;
		LOCAL( grid, WB ) = DFL3;

		CLEAR_ALL_FLAGS_SWEEP( grid );
	SWEEP_END
}

/*############################################################################*/

void LBM_swapGrids( LBM_GridPtr* grid1, LBM_GridPtr* grid2 ) {
	LBM_GridPtr aux = *grid1;
	*grid1 = *grid2;
	*grid2 = aux;
}

/*############################################################################*/

void LBM_loadObstacleFile( LBM_Grid grid, const char* filename ) {
	int x,  y,  z;

	FILE* file = fopen( filename, "rb" );

	for( z = 0; z < SIZE_Z; z++ ) {
		for( y = 0; y < SIZE_Y; y++ ) {
			for( x = 0; x < SIZE_X; x++ ) {
				if( fgetc( file ) != '.' ) {
					SET_FLAG( grid, x, y, z, OBSTACLE );}
				else{
					int null = 0;
				}
			}
			fgetc( file );
		}
		fgetc( file );
	}

	fclose( file );
}

/*############################################################################*/
void LBM_initializeSpecialCellsForLDC(LBM_Grid grid) {
    int x, y, z;

    for (z = -2; z < SIZE_Z + 2; z++) {
        for (y = 0; y < SIZE_Y; y++) {
            for (x = 0; x < SIZE_X; x++) {
                if (x == 0) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (x == SIZE_X - 1) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (y == 0) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (y == SIZE_Y - 1) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (z == 0) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (z == SIZE_Z - 1) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (z == 1) {
					if (x > 1) {
						if (x < SIZE_X - 2) {
							if (y > 1) {
								if (y < SIZE_Y - 2) {
									SET_FLAG(grid, x, y, z, ACCEL);
								}
							}
						}
					}
                    continue;
                }
                if (z == SIZE_Z - 2) {
					if (x > 1) {
						if (x < SIZE_X - 2) {
							if (y > 1) {
								if (y < SIZE_Y - 2) {
									SET_FLAG(grid, x, y, z, ACCEL);
								}
							}
						}
					}
                    continue;
                }
            }
        }
    }
}


/*############################################################################*/

void LBM_initializeSpecialCellsForChannel( LBM_Grid grid ) {
    int x, y, z;

    for (z = -2; z < SIZE_Z + 2; z++) {
        for (y = 0; y < SIZE_Y; y++) {
            for (x = 0; x < SIZE_X; x++) {
                if (x == 0) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (x == SIZE_X - 1) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (y == 0) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (y == SIZE_Y - 1) {
                    SET_FLAG(grid, x, y, z, OBSTACLE);
                    continue;
                }
                if (z == 0) {
                    if (!TEST_FLAG(grid, x, y, z, OBSTACLE))
                        SET_FLAG(grid, x, y, z, IN_OUT_FLOW);
                    continue;
                }
                if (z == SIZE_Z - 1) {
                    if (!TEST_FLAG(grid, x, y, z, OBSTACLE))
                        SET_FLAG(grid, x, y, z, IN_OUT_FLOW);
                }
            }
        }
    }
}


/*############################################################################*/

void LBM_performStreamCollideBGK( LBM_Grid srcGrid, LBM_Grid dstGrid ) {
	SWEEP_VAR

	double ux, uy, uz, u2, rho;
	SWEEP_START( 0, 0, 0, 0, 0, SIZE_Z )
		if( TEST_FLAG_SWEEP( srcGrid, OBSTACLE )) {
			DST_C ( dstGrid ) = SRC_C ( srcGrid );
			DST_S ( dstGrid ) = SRC_N ( srcGrid );
			DST_N ( dstGrid ) = SRC_S ( srcGrid );
			DST_W ( dstGrid ) = SRC_E ( srcGrid );
			DST_E ( dstGrid ) = SRC_W ( srcGrid );
			DST_B ( dstGrid ) = SRC_T ( srcGrid );
			DST_T ( dstGrid ) = SRC_B ( srcGrid );
			DST_SW( dstGrid ) = SRC_NE( srcGrid );
			DST_SE( dstGrid ) = SRC_NW( srcGrid );
			DST_NW( dstGrid ) = SRC_SE( srcGrid );
			DST_NE( dstGrid ) = SRC_SW( srcGrid );
			DST_SB( dstGrid ) = SRC_NT( srcGrid );
			DST_ST( dstGrid ) = SRC_NB( srcGrid );
			DST_NB( dstGrid ) = SRC_ST( srcGrid );
			DST_NT( dstGrid ) = SRC_SB( srcGrid );
			DST_WB( dstGrid ) = SRC_ET( srcGrid );
			DST_WT( dstGrid ) = SRC_EB( srcGrid );
			DST_EB( dstGrid ) = SRC_WT( srcGrid );
			DST_ET( dstGrid ) = SRC_WB( srcGrid );
			continue;
		}

		rho = + SRC_C ( srcGrid ) + SRC_N ( srcGrid )
		      + SRC_S ( srcGrid ) + SRC_E ( srcGrid )
		      + SRC_W ( srcGrid ) + SRC_T ( srcGrid )
		      + SRC_B ( srcGrid ) + SRC_NE( srcGrid )
		      + SRC_NW( srcGrid ) + SRC_SE( srcGrid )
		      + SRC_SW( srcGrid ) + SRC_NT( srcGrid )
		      + SRC_NB( srcGrid ) + SRC_ST( srcGrid )
		      + SRC_SB( srcGrid ) + SRC_ET( srcGrid )
		      + SRC_EB( srcGrid ) + SRC_WT( srcGrid )
		      + SRC_WB( srcGrid );

		ux = + SRC_E ( srcGrid ) - SRC_W ( srcGrid )
		     + SRC_NE( srcGrid ) - SRC_NW( srcGrid )
		     + SRC_SE( srcGrid ) - SRC_SW( srcGrid )
		     + SRC_ET( srcGrid ) + SRC_EB( srcGrid )
		     - SRC_WT( srcGrid ) - SRC_WB( srcGrid );
		uy = + SRC_N ( srcGrid ) - SRC_S ( srcGrid )
		     + SRC_NE( srcGrid ) + SRC_NW( srcGrid )
		     - SRC_SE( srcGrid ) - SRC_SW( srcGrid )
		     + SRC_NT( srcGrid ) + SRC_NB( srcGrid )
		     - SRC_ST( srcGrid ) - SRC_SB( srcGrid );
		uz = + SRC_T ( srcGrid ) - SRC_B ( srcGrid )
		     + SRC_NT( srcGrid ) - SRC_NB( srcGrid )
		     + SRC_ST( srcGrid ) - SRC_SB( srcGrid )
		     + SRC_ET( srcGrid ) - SRC_EB( srcGrid )
		     + SRC_WT( srcGrid ) - SRC_WB( srcGrid );

		ux /= rho;
		uy /= rho;
		uz /= rho;

		if( TEST_FLAG_SWEEP( srcGrid, ACCEL )) {
			ux = 0.005;
			uy = 0.002;
			uz = 0.000;
		}

		u2 = 1.5 * (ux*ux + uy*uy + uz*uz);
		DST_C ( dstGrid ) = (1.0-OMEGA)*SRC_C ( srcGrid ) + DFL1*OMEGA*rho*(1.0                                 - u2);

		DST_N ( dstGrid ) = (1.0-OMEGA)*SRC_N ( srcGrid ) + DFL2*OMEGA*rho*(1.0 +       uy*(4.5*uy       + 3.0) - u2);
		DST_S ( dstGrid ) = (1.0-OMEGA)*SRC_S ( srcGrid ) + DFL2*OMEGA*rho*(1.0 +       uy*(4.5*uy       - 3.0) - u2);
		DST_E ( dstGrid ) = (1.0-OMEGA)*SRC_E ( srcGrid ) + DFL2*OMEGA*rho*(1.0 +       ux*(4.5*ux       + 3.0) - u2);
		DST_W ( dstGrid ) = (1.0-OMEGA)*SRC_W ( srcGrid ) + DFL2*OMEGA*rho*(1.0 +       ux*(4.5*ux       - 3.0) - u2);
		DST_T ( dstGrid ) = (1.0-OMEGA)*SRC_T ( srcGrid ) + DFL2*OMEGA*rho*(1.0 +       uz*(4.5*uz       + 3.0) - u2);
		DST_B ( dstGrid ) = (1.0-OMEGA)*SRC_B ( srcGrid ) + DFL2*OMEGA*rho*(1.0 +       uz*(4.5*uz       - 3.0) - u2);

		DST_NE( dstGrid ) = (1.0-OMEGA)*SRC_NE( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (+ux+uy)*(4.5*(+ux+uy) + 3.0) - u2);
		DST_NW( dstGrid ) = (1.0-OMEGA)*SRC_NW( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (-ux+uy)*(4.5*(-ux+uy) + 3.0) - u2);
		DST_SE( dstGrid ) = (1.0-OMEGA)*SRC_SE( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (+ux-uy)*(4.5*(+ux-uy) + 3.0) - u2);
		DST_SW( dstGrid ) = (1.0-OMEGA)*SRC_SW( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (-ux-uy)*(4.5*(-ux-uy) + 3.0) - u2);
		DST_NT( dstGrid ) = (1.0-OMEGA)*SRC_NT( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (+uy+uz)*(4.5*(+uy+uz) + 3.0) - u2);
		DST_NB( dstGrid ) = (1.0-OMEGA)*SRC_NB( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (+uy-uz)*(4.5*(+uy-uz) + 3.0) - u2);
		DST_ST( dstGrid ) = (1.0-OMEGA)*SRC_ST( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (-uy+uz)*(4.5*(-uy+uz) + 3.0) - u2);
		DST_SB( dstGrid ) = (1.0-OMEGA)*SRC_SB( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (-uy-uz)*(4.5*(-uy-uz) + 3.0) - u2);
		DST_ET( dstGrid ) = (1.0-OMEGA)*SRC_ET( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (+ux+uz)*(4.5*(+ux+uz) + 3.0) - u2);
		DST_EB( dstGrid ) = (1.0-OMEGA)*SRC_EB( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (+ux-uz)*(4.5*(+ux-uz) + 3.0) - u2);
		DST_WT( dstGrid ) = (1.0-OMEGA)*SRC_WT( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (-ux+uz)*(4.5*(-ux+uz) + 3.0) - u2);
		DST_WB( dstGrid ) = (1.0-OMEGA)*SRC_WB( srcGrid ) + DFL3*OMEGA*rho*(1.0 + (-ux-uz)*(4.5*(-ux-uz) + 3.0) - u2);
	SWEEP_END
}


void LBM_performStreamCollideTRT( LBM_Grid srcGrid, LBM_Grid dstGrid ) {
	SWEEP_VAR

	double ux, uy, uz, u2, rho;

	const double lambda0 = 1.0/(0.5+3.0/(16.0*(1.0/OMEGA-0.5)));
	double fs[N_CELL_ENTRIES], fa[N_CELL_ENTRIES],
		     feqs[N_CELL_ENTRIES], feqa[N_CELL_ENTRIES];

	/*voption indep*/
	SWEEP_START( 0, 0, 0, 0, 0, SIZE_Z )
		if( TEST_FLAG_SWEEP( srcGrid, OBSTACLE )) {
			DST_C ( dstGrid ) = SRC_C ( srcGrid );
			DST_S ( dstGrid ) = SRC_N ( srcGrid );
			DST_N ( dstGrid ) = SRC_S ( srcGrid );
			DST_W ( dstGrid ) = SRC_E ( srcGrid );
			DST_E ( dstGrid ) = SRC_W ( srcGrid );
			DST_B ( dstGrid ) = SRC_T ( srcGrid );
			DST_T ( dstGrid ) = SRC_B ( srcGrid );
			DST_SW( dstGrid ) = SRC_NE( srcGrid );
			DST_SE( dstGrid ) = SRC_NW( srcGrid );
			DST_NW( dstGrid ) = SRC_SE( srcGrid );
			DST_NE( dstGrid ) = SRC_SW( srcGrid );
			DST_SB( dstGrid ) = SRC_NT( srcGrid );
			DST_ST( dstGrid ) = SRC_NB( srcGrid );
			DST_NB( dstGrid ) = SRC_ST( srcGrid );
			DST_NT( dstGrid ) = SRC_SB( srcGrid );
			DST_WB( dstGrid ) = SRC_ET( srcGrid );
			DST_WT( dstGrid ) = SRC_EB( srcGrid );
			DST_EB( dstGrid ) = SRC_WT( srcGrid );
			DST_ET( dstGrid ) = SRC_WB( srcGrid );
			continue;
		}

		rho = + SRC_C ( srcGrid ) + SRC_N ( srcGrid )
		      + SRC_S ( srcGrid ) + SRC_E ( srcGrid )
		      + SRC_W ( srcGrid ) + SRC_T ( srcGrid )
		      + SRC_B ( srcGrid ) + SRC_NE( srcGrid )
		      + SRC_NW( srcGrid ) + SRC_SE( srcGrid )
		      + SRC_SW( srcGrid ) + SRC_NT( srcGrid )
		      + SRC_NB( srcGrid ) + SRC_ST( srcGrid )
		      + SRC_SB( srcGrid ) + SRC_ET( srcGrid )
		      + SRC_EB( srcGrid ) + SRC_WT( srcGrid )
		      + SRC_WB( srcGrid );

		ux = + SRC_E ( srcGrid ) - SRC_W ( srcGrid )
		     + SRC_NE( srcGrid ) - SRC_NW( srcGrid )
		     + SRC_SE( srcGrid ) - SRC_SW( srcGrid )
		     + SRC_ET( srcGrid ) + SRC_EB( srcGrid )
		     - SRC_WT( srcGrid ) - SRC_WB( srcGrid );
		uy = + SRC_N ( srcGrid ) - SRC_S ( srcGrid )
		     + SRC_NE( srcGrid ) + SRC_NW( srcGrid )
		     - SRC_SE( srcGrid ) - SRC_SW( srcGrid )
		     + SRC_NT( srcGrid ) + SRC_NB( srcGrid )
		     - SRC_ST( srcGrid ) - SRC_SB( srcGrid );
		uz = + SRC_T ( srcGrid ) - SRC_B ( srcGrid )
		     + SRC_NT( srcGrid ) - SRC_NB( srcGrid )
		     + SRC_ST( srcGrid ) - SRC_SB( srcGrid )
		     + SRC_ET( srcGrid ) - SRC_EB( srcGrid )
		     + SRC_WT( srcGrid ) - SRC_WB( srcGrid );

		ux /= rho;
		uy /= rho;
		uz /= rho;

		if( TEST_FLAG_SWEEP( srcGrid, ACCEL )) {
			ux = 0.005;
			uy = 0.002;
			uz = 0.000;
		}

		u2 = 1.5 * (ux*ux + uy*uy + uz*uz);

		feqs[C ] =            DFL1*rho*(1.0                         - u2);
		feqs[N ] = feqs[S ] = DFL2*rho*(1.0 + 4.5*(+uy   )*(+uy   ) - u2);
		feqs[E ] = feqs[W ] = DFL2*rho*(1.0 + 4.5*(+ux   )*(+ux   ) - u2);
		feqs[T ] = feqs[B ] = DFL2*rho*(1.0 + 4.5*(+uz   )*(+uz   ) - u2);
		feqs[NE] = feqs[SW] = DFL3*rho*(1.0 + 4.5*(+ux+uy)*(+ux+uy) - u2);
		feqs[NW] = feqs[SE] = DFL3*rho*(1.0 + 4.5*(-ux+uy)*(-ux+uy) - u2);
		feqs[NT] = feqs[SB] = DFL3*rho*(1.0 + 4.5*(+uy+uz)*(+uy+uz) - u2);
		feqs[NB] = feqs[ST] = DFL3*rho*(1.0 + 4.5*(+uy-uz)*(+uy-uz) - u2);
		feqs[ET] = feqs[WB] = DFL3*rho*(1.0 + 4.5*(+ux+uz)*(+ux+uz) - u2);
		feqs[EB] = feqs[WT] = DFL3*rho*(1.0 + 4.5*(+ux-uz)*(+ux-uz) - u2);

		feqa[C ] = 0.0;
		feqa[S ] = - (feqa[N ] = DFL2*rho*3.0*(+uy   ));
		feqa[W ] = - (feqa[E ] = DFL2*rho*3.0*(+ux   ));
		feqa[B ] = - (feqa[T ] = DFL2*rho*3.0*(+uz   ));
		feqa[SW] = - (feqa[NE] = DFL3*rho*3.0*(+ux+uy));
		feqa[SE] = - (feqa[NW] = DFL3*rho*3.0*(-ux+uy));
		feqa[SB] = - (feqa[NT] = DFL3*rho*3.0*(+uy+uz));
		feqa[ST] = - (feqa[NB] = DFL3*rho*3.0*(+uy-uz));
		feqa[WB] = - (feqa[ET] = DFL3*rho*3.0*(+ux+uz));
		feqa[WT] = - (feqa[EB] = DFL3*rho*3.0*(+ux-uz));

		fs[C ] =                 SRC_C ( srcGrid );
		fs[N ] = fs[S ] = 0.5 * (SRC_N ( srcGrid ) + SRC_S ( srcGrid ));
		fs[E ] = fs[W ] = 0.5 * (SRC_E ( srcGrid ) + SRC_W ( srcGrid ));
		fs[T ] = fs[B ] = 0.5 * (SRC_T ( srcGrid ) + SRC_B ( srcGrid ));
		fs[NE] = fs[SW] = 0.5 * (SRC_NE( srcGrid ) + SRC_SW( srcGrid ));
		fs[NW] = fs[SE] = 0.5 * (SRC_NW( srcGrid ) + SRC_SE( srcGrid ));
		fs[NT] = fs[SB] = 0.5 * (SRC_NT( srcGrid ) + SRC_SB( srcGrid ));
		fs[NB] = fs[ST] = 0.5 * (SRC_NB( srcGrid ) + SRC_ST( srcGrid ));
		fs[ET] = fs[WB] = 0.5 * (SRC_ET( srcGrid ) + SRC_WB( srcGrid ));
		fs[EB] = fs[WT] = 0.5 * (SRC_EB( srcGrid ) + SRC_WT( srcGrid ));

		fa[C ] = 0.0;
		fa[S ] = - (fa[N ] = 0.5 * (SRC_N ( srcGrid ) - SRC_S ( srcGrid )));
		fa[W ] = - (fa[E ] = 0.5 * (SRC_E ( srcGrid ) - SRC_W ( srcGrid )));
		fa[B ] = - (fa[T ] = 0.5 * (SRC_T ( srcGrid ) - SRC_B ( srcGrid )));
		fa[SW] = - (fa[NE] = 0.5 * (SRC_NE( srcGrid ) - SRC_SW( srcGrid )));
		fa[SE] = - (fa[NW] = 0.5 * (SRC_NW( srcGrid ) - SRC_SE( srcGrid )));
		fa[SB] = - (fa[NT] = 0.5 * (SRC_NT( srcGrid ) - SRC_SB( srcGrid )));
		fa[ST] = - (fa[NB] = 0.5 * (SRC_NB( srcGrid ) - SRC_ST( srcGrid )));
		fa[WB] = - (fa[ET] = 0.5 * (SRC_ET( srcGrid ) - SRC_WB( srcGrid )));
		fa[WT] = - (fa[EB] = 0.5 * (SRC_EB( srcGrid ) - SRC_WT( srcGrid )));

		DST_C ( dstGrid ) = SRC_C ( srcGrid ) - OMEGA * (fs[C ] - feqs[C ])                                ;
		DST_N ( dstGrid ) = SRC_N ( srcGrid ) - OMEGA * (fs[N ] - feqs[N ]) - lambda0 * (fa[N ] - feqa[N ]);
		DST_S ( dstGrid ) = SRC_S ( srcGrid ) - OMEGA * (fs[S ] - feqs[S ]) - lambda0 * (fa[S ] - feqa[S ]);
		DST_E ( dstGrid ) = SRC_E ( srcGrid ) - OMEGA * (fs[E ] - feqs[E ]) - lambda0 * (fa[E ] - feqa[E ]);
		DST_W ( dstGrid ) = SRC_W ( srcGrid ) - OMEGA * (fs[W ] - feqs[W ]) - lambda0 * (fa[W ] - feqa[W ]);
		DST_T ( dstGrid ) = SRC_T ( srcGrid ) - OMEGA * (fs[T ] - feqs[T ]) - lambda0 * (fa[T ] - feqa[T ]);
		DST_B ( dstGrid ) = SRC_B ( srcGrid ) - OMEGA * (fs[B ] - feqs[B ]) - lambda0 * (fa[B ] - feqa[B ]);
		DST_NE( dstGrid ) = SRC_NE( srcGrid ) - OMEGA * (fs[NE] - feqs[NE]) - lambda0 * (fa[NE] - feqa[NE]);
		DST_NW( dstGrid ) = SRC_NW( srcGrid ) - OMEGA * (fs[NW] - feqs[NW]) - lambda0 * (fa[NW] - feqa[NW]);
		DST_SE( dstGrid ) = SRC_SE( srcGrid ) - OMEGA * (fs[SE] - feqs[SE]) - lambda0 * (fa[SE] - feqa[SE]);
		DST_SW( dstGrid ) = SRC_SW( srcGrid ) - OMEGA * (fs[SW] - feqs[SW]) - lambda0 * (fa[SW] - feqa[SW]);
		DST_NT( dstGrid ) = SRC_NT( srcGrid ) - OMEGA * (fs[NT] - feqs[NT]) - lambda0 * (fa[NT] - feqa[NT]);
		DST_NB( dstGrid ) = SRC_NB( srcGrid ) - OMEGA * (fs[NB] - feqs[NB]) - lambda0 * (fa[NB] - feqa[NB]);
		DST_ST( dstGrid ) = SRC_ST( srcGrid ) - OMEGA * (fs[ST] - feqs[ST]) - lambda0 * (fa[ST] - feqa[ST]);
		DST_SB( dstGrid ) = SRC_SB( srcGrid ) - OMEGA * (fs[SB] - feqs[SB]) - lambda0 * (fa[SB] - feqa[SB]);
		DST_ET( dstGrid ) = SRC_ET( srcGrid ) - OMEGA * (fs[ET] - feqs[ET]) - lambda0 * (fa[ET] - feqa[ET]);
		DST_EB( dstGrid ) = SRC_EB( srcGrid ) - OMEGA * (fs[EB] - feqs[EB]) - lambda0 * (fa[EB] - feqa[EB]);
		DST_WT( dstGrid ) = SRC_WT( srcGrid ) - OMEGA * (fs[WT] - feqs[WT]) - lambda0 * (fa[WT] - feqa[WT]);
		DST_WB( dstGrid ) = SRC_WB( srcGrid ) - OMEGA * (fs[WB] - feqs[WB]) - lambda0 * (fa[WB] - feqa[WB]);
	SWEEP_END
}

/*############################################################################*/

void LBM_handleInOutFlow( LBM_Grid srcGrid ) {
	double ux , uy , uz , rho ,
	       ux1, uy1, uz1, rho1,
	       ux2, uy2, uz2, rho2,
	       u2, px, py;
	SWEEP_VAR

	/* inflow */
	SWEEP_START( 0, 0, 0, 0, 0, 1 )
		rho1 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, C  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, N  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, S  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, E  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, W  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, T  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, B  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, NE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, NW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, SE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, SW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, NT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, NB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, ST )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, SB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, ET )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, EB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, WT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 1, WB );
		rho2 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, C  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, N  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, S  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, E  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, W  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, T  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, B  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, NE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, NW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, SE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, SW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, NT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, NB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, ST )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, SB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, ET )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, EB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, WT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, 2, WB );

		rho = 2.0*rho1 - rho2;

		px = (SWEEP_X / (0.5*(SIZE_X-1))) - 1.0;
		py = (SWEEP_Y / (0.5*(SIZE_Y-1))) - 1.0;
		ux = 0.00;
		uy = 0.00;
		uz = 0.01 * (1.0-px*px) * (1.0-py*py);

		u2 = 1.5 * (ux*ux + uy*uy + uz*uz);

		LOCAL( srcGrid, C ) = DFL1*rho*(1.0                                 - u2);

		LOCAL( srcGrid, N ) = DFL2*rho*(1.0 +       uy*(4.5*uy       + 3.0) - u2);
		LOCAL( srcGrid, S ) = DFL2*rho*(1.0 +       uy*(4.5*uy       - 3.0) - u2);
		LOCAL( srcGrid, E ) = DFL2*rho*(1.0 +       ux*(4.5*ux       + 3.0) - u2);
		LOCAL( srcGrid, W ) = DFL2*rho*(1.0 +       ux*(4.5*ux       - 3.0) - u2);
		LOCAL( srcGrid, T ) = DFL2*rho*(1.0 +       uz*(4.5*uz       + 3.0) - u2);
		LOCAL( srcGrid, B ) = DFL2*rho*(1.0 +       uz*(4.5*uz       - 3.0) - u2);

		LOCAL( srcGrid, NE) = DFL3*rho*(1.0 + (+ux+uy)*(4.5*(+ux+uy) + 3.0) - u2);
		LOCAL( srcGrid, NW) = DFL3*rho*(1.0 + (-ux+uy)*(4.5*(-ux+uy) + 3.0) - u2);
		LOCAL( srcGrid, SE) = DFL3*rho*(1.0 + (+ux-uy)*(4.5*(+ux-uy) + 3.0) - u2);
		LOCAL( srcGrid, SW) = DFL3*rho*(1.0 + (-ux-uy)*(4.5*(-ux-uy) + 3.0) - u2);
		LOCAL( srcGrid, NT) = DFL3*rho*(1.0 + (+uy+uz)*(4.5*(+uy+uz) + 3.0) - u2);
		LOCAL( srcGrid, NB) = DFL3*rho*(1.0 + (+uy-uz)*(4.5*(+uy-uz) + 3.0) - u2);
		LOCAL( srcGrid, ST) = DFL3*rho*(1.0 + (-uy+uz)*(4.5*(-uy+uz) + 3.0) - u2);
		LOCAL( srcGrid, SB) = DFL3*rho*(1.0 + (-uy-uz)*(4.5*(-uy-uz) + 3.0) - u2);
		LOCAL( srcGrid, ET) = DFL3*rho*(1.0 + (+ux+uz)*(4.5*(+ux+uz) + 3.0) - u2);
		LOCAL( srcGrid, EB) = DFL3*rho*(1.0 + (+ux-uz)*(4.5*(+ux-uz) + 3.0) - u2);
		LOCAL( srcGrid, WT) = DFL3*rho*(1.0 + (-ux+uz)*(4.5*(-ux+uz) + 3.0) - u2);
		LOCAL( srcGrid, WB) = DFL3*rho*(1.0 + (-ux-uz)*(4.5*(-ux-uz) + 3.0) - u2);
	SWEEP_END

	SWEEP_START( 0, 0, SIZE_Z-1, 0, 0, SIZE_Z )
		rho1 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, C  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, N  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, S  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, E  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, W  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, T  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, B  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, ST )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, ET )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, EB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, WT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, WB );
		ux1 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, E  ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, W  )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NE ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NW )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SE ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SW )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, ET ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, EB )
		      - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, WT ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, WB );
		uy1 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, N  ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, S  )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NE ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NW )
		      - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SE ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SW )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NT ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NB )
		      - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, ST ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SB );
		uz1 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, T  ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, B  )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NT ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, NB )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, ST ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, SB )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, ET ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, EB )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, WT ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -1, WB );

		ux1 /= rho1;
		uy1 /= rho1;
		uz1 /= rho1;

		rho2 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, C  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, N  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, S  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, E  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, W  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, T  )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, B  ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SE )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SW ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, ST )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, ET )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, EB ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, WT )
		       + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, WB );
		ux2 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, E  ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, W  )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NE ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NW )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SE ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SW )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, ET ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, EB )
		      - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, WT ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, WB );
		uy2 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, N  ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, S  )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NE ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NW )
		      - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SE ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SW )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NT ) + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NB )
		      - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, ST ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SB );
		uz2 = + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, T  ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, B  )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NT ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, NB )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, ST ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, SB )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, ET ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, EB )
		      + GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, WT ) - GRID_ENTRY_SWEEP( srcGrid, 0, 0, -2, WB );

		ux2 /= rho2;
		uy2 /= rho2;
		uz2 /= rho2;

		rho = 1.0;

		ux = 2*ux1 - ux2;
		uy = 2*uy1 - uy2;
		uz = 2*uz1 - uz2;

		u2 = 1.5 * (ux*ux + uy*uy + uz*uz);

		LOCAL( srcGrid, C ) = DFL1*rho*(1.0                                 - u2);

		LOCAL( srcGrid, N ) = DFL2*rho*(1.0 +       uy*(4.5*uy       + 3.0) - u2);
		LOCAL( srcGrid, S ) = DFL2*rho*(1.0 +       uy*(4.5*uy       - 3.0) - u2);
		LOCAL( srcGrid, E ) = DFL2*rho*(1.0 +       ux*(4.5*ux       + 3.0) - u2);
		LOCAL( srcGrid, W ) = DFL2*rho*(1.0 +       ux*(4.5*ux       - 3.0) - u2);
		LOCAL( srcGrid, T ) = DFL2*rho*(1.0 +       uz*(4.5*uz       + 3.0) - u2);
		LOCAL( srcGrid, B ) = DFL2*rho*(1.0 +       uz*(4.5*uz       - 3.0) - u2);

		LOCAL( srcGrid, NE) = DFL3*rho*(1.0 + (+ux+uy)*(4.5*(+ux+uy) + 3.0) - u2);
		LOCAL( srcGrid, NW) = DFL3*rho*(1.0 + (-ux+uy)*(4.5*(-ux+uy) + 3.0) - u2);
		LOCAL( srcGrid, SE) = DFL3*rho*(1.0 + (+ux-uy)*(4.5*(+ux-uy) + 3.0) - u2);
		LOCAL( srcGrid, SW) = DFL3*rho*(1.0 + (-ux-uy)*(4.5*(-ux-uy) + 3.0) - u2);
		LOCAL( srcGrid, NT) = DFL3*rho*(1.0 + (+uy+uz)*(4.5*(+uy+uz) + 3.0) - u2);
		LOCAL( srcGrid, NB) = DFL3*rho*(1.0 + (+uy-uz)*(4.5*(+uy-uz) + 3.0) - u2);
		LOCAL( srcGrid, ST) = DFL3*rho*(1.0 + (-uy+uz)*(4.5*(-uy+uz) + 3.0) - u2);
		LOCAL( srcGrid, SB) = DFL3*rho*(1.0 + (-uy-uz)*(4.5*(-uy-uz) + 3.0) - u2);
		LOCAL( srcGrid, ET) = DFL3*rho*(1.0 + (+ux+uz)*(4.5*(+ux+uz) + 3.0) - u2);
		LOCAL( srcGrid, EB) = DFL3*rho*(1.0 + (+ux-uz)*(4.5*(+ux-uz) + 3.0) - u2);
		LOCAL( srcGrid, WT) = DFL3*rho*(1.0 + (-ux+uz)*(4.5*(-ux+uz) + 3.0) - u2);
		LOCAL( srcGrid, WB) = DFL3*rho*(1.0 + (-ux-uz)*(4.5*(-ux-uz) + 3.0) - u2);
	SWEEP_END
}

/*############################################################################*/

void LBM_showGridStatistics( LBM_Grid grid ) {
	int nObstacleCells = 0,
	    nAccelCells    = 0,
	    nFluidCells    = 0;
	double ux, uy, uz;
	double minU2  = 1e+30, maxU2  = -1e+30, u2;
	double minRho = 1e+30, maxRho = -1e+30, rho;
	double mass = 0;

	SWEEP_VAR

	SWEEP_START( 0, 0, 0, 0, 0, SIZE_Z )
		rho = + LOCAL( grid, C  ) + LOCAL( grid, N  )
		      + LOCAL( grid, S  ) + LOCAL( grid, E  )
		      + LOCAL( grid, W  ) + LOCAL( grid, T  )
		      + LOCAL( grid, B  ) + LOCAL( grid, NE )
		      + LOCAL( grid, NW ) + LOCAL( grid, SE )
		      + LOCAL( grid, SW ) + LOCAL( grid, NT )
		      + LOCAL( grid, NB ) + LOCAL( grid, ST )
		      + LOCAL( grid, SB ) + LOCAL( grid, ET )
		      + LOCAL( grid, EB ) + LOCAL( grid, WT )
		      + LOCAL( grid, WB );
		if( rho < minRho ) minRho = rho;
		if( rho > maxRho ) maxRho = rho;
		mass += rho;

		if( TEST_FLAG_SWEEP( grid, OBSTACLE )) {
			nObstacleCells++;
		}
		else {
			if( TEST_FLAG_SWEEP( grid, ACCEL ))
				nAccelCells++;
			else
				nFluidCells++;

			ux = + LOCAL( grid, E  ) - LOCAL( grid, W  )
			     + LOCAL( grid, NE ) - LOCAL( grid, NW )
			     + LOCAL( grid, SE ) - LOCAL( grid, SW )
			     + LOCAL( grid, ET ) + LOCAL( grid, EB )
			     - LOCAL( grid, WT ) - LOCAL( grid, WB );
			uy = + LOCAL( grid, N  ) - LOCAL( grid, S  )
			     + LOCAL( grid, NE ) + LOCAL( grid, NW )
			     - LOCAL( grid, SE ) - LOCAL( grid, SW )
			     + LOCAL( grid, NT ) + LOCAL( grid, NB )
			     - LOCAL( grid, ST ) - LOCAL( grid, SB );
			uz = + LOCAL( grid, T  ) - LOCAL( grid, B  )
			     + LOCAL( grid, NT ) - LOCAL( grid, NB )
			     + LOCAL( grid, ST ) - LOCAL( grid, SB )
			     + LOCAL( grid, ET ) - LOCAL( grid, EB )
			     + LOCAL( grid, WT ) - LOCAL( grid, WB );
			u2 = (ux*ux + uy*uy + uz*uz) / (rho*rho);
			if( u2 < minU2 ) minU2 = u2;else{
				int null = 0;
			}
			if( u2 > maxU2 ) maxU2 = u2;else{
				int null = 0;
			}
		}
	SWEEP_END

	printf( "LBM_showGridStatistics:\n"
	        "\tnObstacleCells: %7i nAccelCells: %7i nFluidCells: %7i\n"
	        "\tminRho: %8.6f maxRho: %8.6f Mass: %e\n"
	        "\tminU  : %8.6f maxU  : %8.6f\n\n",
	        nObstacleCells, nAccelCells, nFluidCells,
	        minRho, maxRho, mass,
	        sqrt( minU2 ), sqrt( maxU2 ) );
}

/*############################################################################*/

static void storeValue( FILE* file, OUTPUT_PRECISION* v ) {
	const int litteBigEndianTest = 1;
	if( (*((unsigned char*) &litteBigEndianTest)) == 0 ) {         /* big endian */
		const char* vPtr = (char*) v;
		char buffer[sizeof( OUTPUT_PRECISION )];
		int i;

		for (i = 0; i < sizeof( OUTPUT_PRECISION ); i++)
			buffer[i] = vPtr[sizeof( OUTPUT_PRECISION ) - i - 1];

		fwrite( buffer, sizeof( OUTPUT_PRECISION ), 1, file );
	}
	else {                                                     /* little endian */
		fwrite( v, sizeof( OUTPUT_PRECISION ), 1, file );
	}
}

/*############################################################################*/

static void loadValue( FILE* file, OUTPUT_PRECISION* v ) {
	const int litteBigEndianTest = 1;
	if( (*((unsigned char*) &litteBigEndianTest)) == 0 ) {         /* big endian */
		char* vPtr = (char*) v;
		char buffer[sizeof( OUTPUT_PRECISION )];

		int i;

		fread( buffer, sizeof( OUTPUT_PRECISION ), 1, file );

		for (i = 0; i < sizeof( OUTPUT_PRECISION ); i++)
			vPtr[i] = buffer[sizeof( OUTPUT_PRECISION ) - i - 1];
	}
	else {                                                     /* little endian */
		fread( v, sizeof( OUTPUT_PRECISION ), 1, file );
	}
}

/*############################################################################*/

void LBM_storeVelocityField( LBM_Grid grid, const char* filename,
                             const int binary ) {
	int x, y, z;
	OUTPUT_PRECISION rho, ux, uy, uz;

	FILE* file = fopen( filename, (binary ? "wb" : "w") );

	for( z = 0; z < SIZE_Z; z++ ) {
		for( y = 0; y < SIZE_Y; y++ ) {
			for( x = 0; x < SIZE_X; x++ ) {
				rho = + GRID_ENTRY( grid, x, y, z, C  ) + GRID_ENTRY( grid, x, y, z, N  )
				      + GRID_ENTRY( grid, x, y, z, S  ) + GRID_ENTRY( grid, x, y, z, E  )
				      + GRID_ENTRY( grid, x, y, z, W  ) + GRID_ENTRY( grid, x, y, z, T  )
				      + GRID_ENTRY( grid, x, y, z, B  ) + GRID_ENTRY( grid, x, y, z, NE )
				      + GRID_ENTRY( grid, x, y, z, NW ) + GRID_ENTRY( grid, x, y, z, SE )
				      + GRID_ENTRY( grid, x, y, z, SW ) + GRID_ENTRY( grid, x, y, z, NT )
				      + GRID_ENTRY( grid, x, y, z, NB ) + GRID_ENTRY( grid, x, y, z, ST )
				      + GRID_ENTRY( grid, x, y, z, SB ) + GRID_ENTRY( grid, x, y, z, ET )
				      + GRID_ENTRY( grid, x, y, z, EB ) + GRID_ENTRY( grid, x, y, z, WT )
				      + GRID_ENTRY( grid, x, y, z, WB );
				ux = + GRID_ENTRY( grid, x, y, z, E  ) - GRID_ENTRY( grid, x, y, z, W  ) 
				     + GRID_ENTRY( grid, x, y, z, NE ) - GRID_ENTRY( grid, x, y, z, NW ) 
				     + GRID_ENTRY( grid, x, y, z, SE ) - GRID_ENTRY( grid, x, y, z, SW ) 
				     + GRID_ENTRY( grid, x, y, z, ET ) + GRID_ENTRY( grid, x, y, z, EB ) 
				     - GRID_ENTRY( grid, x, y, z, WT ) - GRID_ENTRY( grid, x, y, z, WB );
				uy = + GRID_ENTRY( grid, x, y, z, N  ) - GRID_ENTRY( grid, x, y, z, S  ) 
				     + GRID_ENTRY( grid, x, y, z, NE ) + GRID_ENTRY( grid, x, y, z, NW ) 
				     - GRID_ENTRY( grid, x, y, z, SE ) - GRID_ENTRY( grid, x, y, z, SW ) 
				     + GRID_ENTRY( grid, x, y, z, NT ) + GRID_ENTRY( grid, x, y, z, NB ) 
				     - GRID_ENTRY( grid, x, y, z, ST ) - GRID_ENTRY( grid, x, y, z, SB );
				uz = + GRID_ENTRY( grid, x, y, z, T  ) - GRID_ENTRY( grid, x, y, z, B  ) 
				     + GRID_ENTRY( grid, x, y, z, NT ) - GRID_ENTRY( grid, x, y, z, NB ) 
				     + GRID_ENTRY( grid, x, y, z, ST ) - GRID_ENTRY( grid, x, y, z, SB ) 
				     + GRID_ENTRY( grid, x, y, z, ET ) - GRID_ENTRY( grid, x, y, z, EB ) 
				     + GRID_ENTRY( grid, x, y, z, WT ) - GRID_ENTRY( grid, x, y, z, WB );
				ux /= rho;
				uy /= rho;
				uz /= rho;

				if( binary ) {
					storeValue( file, &ux );
					storeValue( file, &uy );
					storeValue( file, &uz );
				} else
					fprintf( file, "%e %e %e\n", ux, uy, uz );

			}
		}
	}

	fclose( file );
}

/*############################################################################*/

void LBM_compareVelocityField( LBM_Grid grid, const char* filename,
                             const int binary ) {
	int x, y, z;
	double rho, ux, uy, uz;
	OUTPUT_PRECISION fileUx, fileUy, fileUz,
	                 dUx, dUy, dUz,
	                 diff2, maxDiff2 = -1e+30;

	FILE* file = fopen( filename, (binary ? "rb" : "r") );

	for( z = 0; z < SIZE_Z; z++ ) {
		for( y = 0; y < SIZE_Y; y++ ) {
			for( x = 0; x < SIZE_X; x++ ) {
				rho = + GRID_ENTRY( grid, x, y, z, C  ) + GRID_ENTRY( grid, x, y, z, N  )
				      + GRID_ENTRY( grid, x, y, z, S  ) + GRID_ENTRY( grid, x, y, z, E  )
				      + GRID_ENTRY( grid, x, y, z, W  ) + GRID_ENTRY( grid, x, y, z, T  )
				      + GRID_ENTRY( grid, x, y, z, B  ) + GRID_ENTRY( grid, x, y, z, NE )
				      + GRID_ENTRY( grid, x, y, z, NW ) + GRID_ENTRY( grid, x, y, z, SE )
				      + GRID_ENTRY( grid, x, y, z, SW ) + GRID_ENTRY( grid, x, y, z, NT )
				      + GRID_ENTRY( grid, x, y, z, NB ) + GRID_ENTRY( grid, x, y, z, ST )
				      + GRID_ENTRY( grid, x, y, z, SB ) + GRID_ENTRY( grid, x, y, z, ET )
				      + GRID_ENTRY( grid, x, y, z, EB ) + GRID_ENTRY( grid, x, y, z, WT )
				      + GRID_ENTRY( grid, x, y, z, WB );
				ux = + GRID_ENTRY( grid, x, y, z, E  ) - GRID_ENTRY( grid, x, y, z, W  ) 
				     + GRID_ENTRY( grid, x, y, z, NE ) - GRID_ENTRY( grid, x, y, z, NW ) 
				     + GRID_ENTRY( grid, x, y, z, SE ) - GRID_ENTRY( grid, x, y, z, SW ) 
				     + GRID_ENTRY( grid, x, y, z, ET ) + GRID_ENTRY( grid, x, y, z, EB ) 
				     - GRID_ENTRY( grid, x, y, z, WT ) - GRID_ENTRY( grid, x, y, z, WB );
				uy = + GRID_ENTRY( grid, x, y, z, N  ) - GRID_ENTRY( grid, x, y, z, S  ) 
				     + GRID_ENTRY( grid, x, y, z, NE ) + GRID_ENTRY( grid, x, y, z, NW ) 
				     - GRID_ENTRY( grid, x, y, z, SE ) - GRID_ENTRY( grid, x, y, z, SW ) 
				     + GRID_ENTRY( grid, x, y, z, NT ) + GRID_ENTRY( grid, x, y, z, NB ) 
				     - GRID_ENTRY( grid, x, y, z, ST ) - GRID_ENTRY( grid, x, y, z, SB );
				uz = + GRID_ENTRY( grid, x, y, z, T  ) - GRID_ENTRY( grid, x, y, z, B  ) 
				     + GRID_ENTRY( grid, x, y, z, NT ) - GRID_ENTRY( grid, x, y, z, NB ) 
				     + GRID_ENTRY( grid, x, y, z, ST ) - GRID_ENTRY( grid, x, y, z, SB ) 
				     + GRID_ENTRY( grid, x, y, z, ET ) - GRID_ENTRY( grid, x, y, z, EB ) 
				     + GRID_ENTRY( grid, x, y, z, WT ) - GRID_ENTRY( grid, x, y, z, WB );
				ux /= rho;
				uy /= rho;
				uz /= rho;

				if( binary ) {
					loadValue( file, &fileUx );
					loadValue( file, &fileUy );
					loadValue( file, &fileUz );
				}
				else {
					if( sizeof( OUTPUT_PRECISION ) == sizeof( double )) {
						fscanf( file, "%lf %lf %lf\n", &fileUx, &fileUy, &fileUz );
					}
					else {
						fscanf( file, "%f %f %f\n", &fileUx, &fileUy, &fileUz );

					}
				}

				dUx = ux - fileUx;
				dUy = uy - fileUy;
				dUz = uz - fileUz;
				diff2 = dUx*dUx + dUy*dUy + dUz*dUz;
				if( diff2 > maxDiff2 ) maxDiff2 = diff2;
			}
		}
	}

	printf( "LBM_compareVelocityField: maxDiff = %e  ==>  %s\n\n",
	        sqrt( maxDiff2 ),
	        sqrt( maxDiff2 ) > 1e-5 ? "##### ERROR #####" : "OK" );
	fclose( file );
}

