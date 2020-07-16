#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double addO(double arg1, double arg2) {
	return arg1 + arg2;
}

double subO(double arg1, double arg2) {
	return arg1 - arg2;
}

double mulO(double arg1, double arg2) {
	return arg1 * arg2;
}

double divO(double arg1, double arg2) {
	return arg1 / arg2;
}

double remO(double arg1, double arg2) {
	return fmod(arg1, arg2);
}

double powO(double arg1, double arg2) {
	return pow(arg1, arg2);
}

double greO(double arg1, double arg2) {
	return arg1 > arg2;
}

double lesO(double arg1, double arg2) {
	return arg1 < arg2;
}

double equO(double arg1, double arg2) {
	return arg1 == arg2;
}

double maxF(double *args, int n) {
	int i;
	double max = args[0];
	for(i = 1; i < n; i++)
		if(args[i] > max)
			max = args[i];
	return max;
}

double minF(double *args, int n) {
	int i;
	double min = args[0];
	for(i = 1; i < n; i++)
		if(args[i] < min)
			min = args[i];
	return min;
}

double avgF(double *args, int n) {
	int i;
	double sum = 0;
	for(i = 0; i < n; i++)
		sum += args[i];
	return sum / n;
}

double sqrtF(double *args, int n) {
	return sqrt(args[0]);
}

double sinF(double *args, int n) {
	return sin(args[0]);
}

double cosF(double *args, int n) {
	return cos(args[0]);
}

double tanF(double *args, int n) {
	return sin(args[0]) / cos(args[0]);
}

double ctanF(double *args, int n) {
	return cos(args[0]) / sin(args[0]);
}

double sinhF(double *args, int n) {
	return sinh(args[0]);
}

double coshF(double *args, int n) {
	return cosh(args[0]);
}

double tanhF(double *args, int n) {
	return tanh(args[0]);
}

double cothF(double *args, int n) {
	return cosh(args[0]) / sinh(args[0]);
}

double absF(double *args, int n) {
	return abs(args[0]);
}

double lnF(double *args, int n) {
	return log(args[0]);
}

double logF(double *args, int n) {
	return log10(args[0]);
}

double expF(double *args, int n) {
	return exp(args[0]);
}

double ceilF(double *args, int n) {
	return ceil(args[0]);
}

double floorF(double *args, int n) {
	return floor(args[0]);
}

double roundF(double *args, int n) {
	return round(args[0]);
}

double truncF(double *args, int n) {
	return trunc(args[0]);
}
