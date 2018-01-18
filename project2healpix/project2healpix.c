#include "stdio.h"
#include "fitsio.h"
#include "malloc.h"
#include "chealpix.h"
#include "math.h"
#include "string.h"
#define PI  3.1415926

void get_fits_data(char *fp, double **data, long *dim){
	fitsfile *ffp;
	int status=0, i, j,bitpix,anynull;
	char comment[FLEN_COMMENT];
	long start_pos[2]={1,1},nside,npix,index;
	fits_open_file(&ffp, fp, READONLY, &status);
	fits_get_img_size(ffp, 2, dim, &status);
	*data = (double*)malloc(dim[0]*dim[1]*sizeof(double));
	fits_read_key(ffp, TINT, "BITPIX", &bitpix, comment, &status);
	fits_read_pix(ffp, TDOUBLE, start_pos, dim[0]*dim[1], 0, *data, &anynull, &status);
	fits_close_file(ffp, &status);
	fits_report_error(stderr, status);
}

void save_fits(double *data, long *dim, char *fp){
	fitsfile *ffp;
	int status=0;
	long naxis=2;
	char fn_tmp[100];
	sprintf(fn_tmp, "!%s",fp);
	fits_create_file(&ffp, fn_tmp, &status);
	fits_create_img(ffp, DOUBLE_IMG, naxis, dim, &status);
	fits_write_img(ffp, TDOUBLE, 1, dim[0]*dim[1], data,  &status);
	fits_close_file(ffp, &status);
	fits_report_error(stderr, status);
}

void project2healpix(char *fpi, char *fpo, long nside, char *order, char *coordsys){
	double *data, delta_theta, delta_phi,theta, phi;
	long dim[2],i,j,npix,index;
	float *healpix_data;
	char fn_tmp[100];
	get_fits_data(fpi, &data, dim);
	if (nside == 0){
		nside = (long)(pow(2,floor(log2(dim[0]/4.0))));
	}
	npix = nside2npix(nside);
	healpix_data = (float*)malloc(npix*sizeof(float));
	delta_theta =  PI/(dim[1]);
	delta_phi = 2*PI/(dim[0]);
	for(index=0; index<npix; index++){
			if(strcmp(order, "RING")==0) pix2ang_ring(nside, index, &theta, &phi);
			if(strcmp(order, "NESTED")==0) pix2ang_nest(nside, index, &theta, &phi);
			//Be care
			i = dim[1] - floor(theta / delta_theta);
			j = floor(phi / delta_phi);
			if (j < dim[0]/2) j += (dim[0]/2);
			else j -= (dim[0]/2);
			j = dim[0] - 1 - j;
			//Be care
			healpix_data[index] = data[i*dim[0]+j];
	}
	printf("******************************************\n");
	printf("Project to Healpix\n");
	printf("NSIDE = %li\n",nside);
	printf("NPIX = %li\n",npix);
	printf("ORDER : %s\n", order);
	printf("Coordinate System : %s\n", coordsys);
	printf("Map Size : %li*%li\n", dim[1], dim[0]);
	printf("******************************************\n");
	sprintf(fn_tmp, "!%s",fpo);
	if (strcmp(order, "RING")==0)
		write_healpix_map(healpix_data, nside, fn_tmp, 0, coordsys);
	if (strcmp(order, "NESTED")==0)
		write_healpix_map(healpix_data, nside, fn_tmp, 1, coordsys);
	free(healpix_data);
	free(data);
}

void healpix_inverse(char *fpi, char *fpo, long map_size1, long map_size2){
	long nside,npix,index,i,j,dim[2];
	char order[10],coordsys[5];
	float *healpix_data;
	double *data,delta_phi, delta_theta, theta, phi;
	healpix_data = read_healpix_map(fpi, &nside, coordsys, order);
	if (map_size1 == 0){
		map_size1 = pow(2,floor(log2(nside * 4)));
		map_size2 = map_size1;
	}
	printf("******************************************\n");
	printf("Healpix Inverse\n");
	printf("NSIDE = %li\n",nside);
	printf("NPIX = %li\n",npix);
	printf("ORDER : %s\n", order);
	printf("Coordinate System : %s\n", coordsys);
	printf("Map Size : %li*%li\n",map_size1, map_size2);
	printf("******************************************\n");
	data = (double*)malloc(sizeof(double)*map_size1*map_size2);
	delta_theta = PI/(map_size1-1);
	delta_phi = 2*PI/(map_size2-1);
	for(i=0; i<map_size1; i++)
		for(j=0; j<map_size2; j++){
			theta =PI - i*delta_theta;
			phi = PI - j*delta_phi;
			if (strcmp(order, "RING")==0) ang2pix_ring(nside, theta, phi, &index);
			if (strcmp(order, "NESTED")==0) ang2pix_nest(nside, theta, phi, &index);
			data[i*map_size2+j] = healpix_data[index];
		}
	dim[0] = map_size2;
	dim[1] = map_size1;
	save_fits(data, dim, fpo);
	free(healpix_data);
	free(data);
}

void main(int argc, char *argv[]){
	printf("******************************************\n");
	if (argc < 2){
		printf("Please input configure file!\n");
	}
	FILE *fp;
	char tmp[100],fni[100],fno[100],order[10],coordsys[10];
	fp = fopen(argv[1], "r");
	long flag,nside,map_size1, map_size2;
	fscanf(fp, "%s%li", tmp, &flag);
	fscanf(fp, "%s%s", tmp, fni);
	fscanf(fp, "%s%s", tmp, fno);
	fscanf(fp, "%s%li", tmp, &nside);
	fscanf(fp, "%s%s", tmp, order);
	fscanf(fp, "%s%s", tmp, coordsys);
	fscanf(fp, "%s%li", tmp, &map_size1);
	fscanf(fp, "%s%li", tmp, &map_size2);
	printf("Run Flag : %ld\nInput File : %s\nOutput File : %s\n",
		  flag, fni, fno);
	printf("******************************************\n");
	switch(flag){
		case 1: project2healpix(fni, fno, nside, order,coordsys); break;
		case 2: healpix_inverse(fni, fno, map_size1, map_size2); break;
		default:  printf("Please Check your input arguments!\n");
	}
}
