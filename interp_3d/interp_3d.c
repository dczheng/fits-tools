#include"stdio.h"
#include "fitsio.h"
#include "stdlib.h"
#include "string.h"
#include "gsl/gsl_spline.h"

int StartIndex, EndIndex, NumIndex;
long naxes[3];
char FilePrefix[200], OutFile[200];
float *img3d;

void endrun( int err ) {
    fprintf( stderr, "EXIT CODE: %i\n", err );
    exit( err );
}

void read_parameters( char *fn ) {
#define MAXTAGS 300
#define REAL 1
#define STRING 2
#define INT 3
    FILE *fd;
    void *addr[MAXTAGS];
    char tag[MAXTAGS][50], buf[200], buf1[200], buf2[200], buf3[200];
        int id[MAXTAGS], nt, i, j, errflag=0;;
        printf( "read parameter...\n" );
        fd = fopen( fn, "r" );
        if ( NULL == fd ) {
            fprintf( stderr, "Faile to Open Parameter file %s\n", fn );
            endrun( 1 );
        }

        if ( sizeof( long long ) != 8 ) {
            printf( "Type `long long` is no 64 bit on this platform. Stopping. \n" );
            endrun( 20171207 );
        }

        nt = 0;

        strcpy( tag[nt], "FilePrefix" );
        addr[nt] = &FilePrefix;
        id[nt++] = STRING;

        strcpy( tag[nt], "OutFile" );
        addr[nt] = &OutFile;
        id[nt++] = STRING;

        strcpy( tag[nt], "StartIndex" );
        addr[nt] = &StartIndex;
        id[nt++] = INT;

        strcpy( tag[nt], "EndIndex" );
        addr[nt] = &EndIndex;
        id[nt++] = INT;

        strcpy( tag[nt], "InterpNum" );
        addr[nt] = &naxes[2];
        id[nt++] = INT;

        while( !feof( fd ) ) {
            *buf = 0;
            fgets( buf, 200, fd );
            if ( sscanf( buf, "%s%s%s", buf1, buf2, buf3 ) < 2 )
                continue;
            if ( buf1[0] == '%' )
                continue;
            for ( i=0, j=-1; i<nt; i++ )
                if ( strcmp( buf1, tag[i] ) == 0 ) {
                    j = i;
                    tag[i][0] = 0;
                    break;
                }
            if ( j>=0 ) {
                switch ( id[j] ) {
                    case REAL:
                        *( (double*)addr[j] ) = atof( buf2 );
                        printf( "%-35s: %g\n", buf1, *((double*)addr[j]) );
                        break;
                    case INT:
                        *( (int*)addr[j] ) = atoi( buf2 );
                        printf( "%-35s: %d\n", buf1, *((int*)addr[j]) );
                        break;
                    case STRING:
                        strcpy( (char*)addr[j], buf2 );
                        printf( "%-35s: %s\n", buf1, buf2 );
                        break;
                }
            }
            else {
                printf( "Error in file %s:  Tag: '%s', not allowed or multiple define.\n", fn, buf1 );
                errflag = 1;
            }
        }
        for ( i=0; i<nt; i++ ) {
            if ( *tag[i] ) {
                printf( "Error. I miss a value for tag '%s' in parameter file '%s'.\n", tag[i], fn );
                errflag = 1;
            }
        }
        if ( errflag )
            endrun( 0 );
        fclose( fd );
}

void get_fits_naxes( char *fn ) {
    int status, i;
    char comment[FLEN_COMMENT];
    fitsfile *fptr;
    status = 0;
    fits_open_file( &fptr, fn, READONLY, &status );
    fits_get_img_size( fptr, 2, naxes, &status );
    fits_close_file( fptr, &status );
    fits_report_error( stderr, status );
    printf( "naxes[0]: %li, naxes[1]: %li\n", naxes[0], naxes[1] );
}

void read_fits_data( char *fn, float *img ) {
    fitsfile *fptr;
    int status, anynul;
    status = 0;
    fits_open_file( &fptr, fn, READONLY, &status );
    anynul = 0;
    fits_read_img( fptr, TFLOAT, 1, (LONGLONG)(naxes[0]*naxes[1]), 0, img, &anynul, &status );
    fits_close_file( fptr, &status );
    fits_report_error( stderr, status );
}

void save_fits() {
    fitsfile *fptr;
    int status;
    char buf[200];
    printf( "save fits file to %s ...\n", OutFile );
    sprintf( buf, "!%s", OutFile );
    status = 0;
    fits_create_file( &fptr, buf, &status );
    fits_create_img( fptr, FLOAT_IMG, 3, naxes, &status);
    fits_write_img( fptr, TFLOAT, 1, (LONGLONG)(naxes[0]*naxes[1]*naxes[2]), img3d, &status );
    fits_close_file( fptr, &status );
    fits_report_error( stderr, status );
    printf( "save fits file to %s ... done.\n", OutFile );
}

void test() {
    FILE *fd;
    int i, j, k, N, start_i, end_i, start_j, end_j;
    char buf[100];
    N = naxes[0] * naxes[1];
    printf( "test output...\n" );
    start_i = naxes[0] / 2 - 500;
    end_i = naxes[0] / 2 + 500;
    start_j = naxes[1] / 2 - 500;
    end_j = naxes[1] / 2 + 500;
    for ( k=0; k<naxes[2]; k++ ){
        printf( "%i\n", k );
        sprintf( buf, "./dat/%i.dat", k );
        fd = fopen( buf, "w" );
        for ( j=start_j; j<end_j; j++ ){
            for ( i=start_i; i<end_i; i++ )
                fprintf( fd, "%f ", img3d[ k*N + j * naxes[0] + i ] );
            fprintf( fd, "\n" );
        }
        fclose( fd );
    }
    printf( "test output... done.\n" );
}

void output_fits( float *img, int k ) {
    int i, j, start_i, start_j, end_i, end_j, N;
    char buf[100];
    FILE *fd;
    N = naxes[0] * naxes[1];
    start_i = naxes[1] / 2 - 500;
    end_i = naxes[1] / 2 + 500;
    start_j = naxes[0] / 2 - 500;
    end_j = naxes[0] / 2 + 500;
    sprintf( buf, "./txt/%i.txt", k );
    fd = fopen( buf, "w" );
    for ( i=start_i; i<end_i; i++ ){
        for ( j=start_j; j<end_j; j++ )
            fprintf( fd, "%f ", img[i*naxes[0]+j] );
        fprintf( fd, "\n" );
    }
    fclose( fd );
}

void main( int argc, char **argv ) {
    char buf[200];
    int i, ii, j, k, N, zN, *index;
    float dx;
    double *x, *y;
    gsl_spline *spline;
    gsl_interp_accel *acc;
    if ( argc < 2 ) {
        fprintf( stderr, "parameter file is required on command line!\n" );
        endrun( 0 );
    }
    read_parameters( argv[1] );
    NumIndex = EndIndex - StartIndex + 1;
    sprintf( buf, "%s%3d.fits", FilePrefix, StartIndex );
    get_fits_naxes( buf );
    N = naxes[0]* naxes[1];
    zN = naxes[2];
    dx = (float)(-StartIndex+EndIndex) / ( naxes[2] - 1 );
    printf( "dx: %f\n", dx );
    img3d = malloc( sizeof( float ) * N * zN );
    //memset( img3d, 0, sizeof( float ) * N * zN );
    index = malloc( sizeof(int) * NumIndex );
    x = malloc( sizeof( double) * NumIndex );
    y = malloc( sizeof( double) * NumIndex );
    for ( i=StartIndex; i<=EndIndex; i++ ) {
        ii = i - StartIndex;
        sprintf( buf, "%s%3d.fits", FilePrefix, i );
        printf( "read file %s\n", buf );
        index[ii] = (int)(( ii ) / ( dx ));
        x[ii] = index[ii];
        printf( "3d x index: %d\n", index[ii] );
        read_fits_data( buf, &img3d[index[ii]*N] );
        //output_fits( &img3d[index[ii]*N], ii );
    }
    printf( "interpolate ...\n" );
    spline = gsl_spline_alloc( gsl_interp_cspline, NumIndex );
    acc = gsl_interp_accel_alloc();
    for ( i=0; i<naxes[1]; i++ )
        for ( j=0; j<naxes[0]; j++ ) {
            for ( k=0; k<NumIndex; k++ ) {
                y[k] = img3d[ index[k]*N + i*naxes[0] + j ];
                //printf( "%f\n", y[k] );
            }
            gsl_spline_init( spline, x, y, NumIndex );
            for ( k=0; k<naxes[2]; k++ )
                img3d[ k*N + i*naxes[0] + j ] = gsl_spline_eval( spline, k, acc );
        }
    printf( "interpolate ... done.\n" );
    save_fits();
    //test();
    free( img3d );
    free( index );
    free( x );
    free( y );
    gsl_interp_accel_free( acc );
    gsl_spline_free( spline );
}

