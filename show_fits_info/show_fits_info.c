#include "stdio.h"
#include "fitsio.h"

void Show_Fits_Info( char *fn ) {
    char Card[FLEN_CARD];
    int i, j, status, hdu_num, hdu_type;
    int keysexist, morekeys;
    fitsfile *fptr;
    status = 0;
    fits_open_file( &fptr, fn, READONLY, &status );
    fits_get_hdu_num( fptr, &hdu_num );
    for ( i=0; i<hdu_num; i++ ) {
         fits_get_hdu_type( fptr, &hdu_type, &status );
         fprintf( stdout, "HDU: %i\n", i );
         fprintf( stdout, "Type: %i\n", hdu_type );
         fits_get_hdrspace( fptr, &keysexist, &morekeys, &status );
         fprintf( stdout, "Keysexist: %i,  morekeys: %i\n", keysexist, morekeys );
         for ( j=0; j<keysexist; j++ ) {
            fits_read_record( fptr, j, Card, &status );
            fprintf( stdout, "%s\n", Card );
         }
    }
    fits_close_file( fptr, &status );
    fits_report_error( stderr, status );
}

void main( int argc, char *argv[] ) {
    Show_Fits_Info ( argv[1] );
}
