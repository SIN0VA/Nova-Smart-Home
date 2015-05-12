// TRNG - A C++ class to access the internal hardware random number generator for
//        the Raspberry Pi
//
// Copyright 2014 by Walter Anderson
//
// This class allows for obtaining the random numbers in the format one would like
//
// This is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// It is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Entropy.  If not, see <http://www.gnu.org/licenses/>.
#ifndef trng_h
#define trng_h

#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include "trng.h"
using namespace std;

// The default template class is for those simple types on the Raspberry Pi
// that are of a 32-bit integer class; int, long.  Other types require 
// specialization class
template <class T>
class trng
{
  ifstream hwrng_device;
 public:
  // Constructor - This method will open the hardware random number generator on
  // a raspberry pi for binary input.
  trng(void)
    {
#ifdef DELPHI
      hwrng_device.open("random.bin", ios::binary);
#else
      hwrng_device.open("/dev/hwrng", ios::binary);
#endif
    }

  // Destructor - This method will close the file that was opened in the Constructor
  ~trng(void)
    {
      hwrng_device.close();
    }

  // The Raspeberry Pi hardware random number generator seems to require that it be 
  // read as a four byte value.  Attempting to read smaller size values appear to 
  // cause problems with the reads.
  T random(void)
     {
       T tmp;
       hwrng_device.read( reinterpret_cast <char *> (&tmp), sizeof(tmp));
       return (tmp);
     }

  // This function returns a uniformly distributed random number in the range
  // of [0,max).  The added complexity of this function is required to ensure
  // a uniform distribution since the naive modulus max (% max) introduces
  // bias for all values of max that are not powers of two.
  //
  // The loops below are needed, because there is a small and non-uniform 
  // chance that the division below will yield an answer = max.  So we just grab 
  // the next random value until answer < max
  T random(T max)
     {
       T slice;
       T retVal;

       retVal = 0xFFFFFFFF;
       slice = 0xFFFFFFFF / max;
       while (retVal >= max)
	 retVal = random() / slice;
       return(retVal);
     }

  // This method returns and number in the range of [min, max)
  T random(T min, T max)
     {
       T tmax;
       T retVal;

       tmax = max - min;
       retVal = min + random(tmax);
       return(retVal);
     }

  // This method is a place holder since it is only valid for classes with 
  // real number types
  T rnorm(T mean, T stdDev)
    {
      T tmp = 0;

      return(tmp);
    }
};


// Specialized class for 64-bit integers, unsigned long long
template <>
class trng <long long>
{
  ifstream hwrng_device;
 public:
  // Constructor - This method will open the hardware random number generator on
  // a raspberry pi for binary input.
  trng(void)
    {
      hwrng_device.open("/dev/hwrng", ios::binary);
    }

  // Destructor - This method will close the file that was opened in the Constructor
  ~trng(void)
    {
      hwrng_device.close();
    }

  // Reads 64-bit value
  long long random(void)
     {
       unsigned long long tmp;
       hwrng_device.read( reinterpret_cast <char *> (&tmp), sizeof(tmp));
       return (tmp);
     }

  // This function returns a uniformly distributed random number in the range
  // of [0,max).  The added complexity of this function is required to ensure
  // a uniform distribution since the naive modulus max (% max) introduces
  // bias for all values of max that are not powers of two.
  //
  // The loops below are needed, because there is a small and non-uniform 
  // chance that the division below will yield an answer = max.  So we just grab 
  // the next random value until answer < max
  long long random(long long max)
     {
       long long slice;
       long long retVal;

       retVal = 0xFFFFFFFFFFFFFFFF;
       slice = 0xFFFFFFFFFFFFFFFF / max;
       while (retVal >= max)
	 retVal = random() / slice;
       return(retVal);
     }

  // This method returns and number in the range of [min, max)
  long long random(long long min, long long max)
     {
       long long tmax;
       long long retVal;

       tmax = max - min;
       retVal = min + random(tmax);
       return(retVal);
     }

  // This method is a place holder since it is only valid for classes with 
  // real number types
  long long rnorm(long long mean, long long stdDev)
    {
      long long tmp = 0;

      return(tmp);
    }
};


// Specialized class for 64-bit integers, unsigned long long
template <>
class trng <unsigned long long>
{
  ifstream hwrng_device;
 public:
  // Constructor - This method will open the hardware random number generator on
  // a raspberry pi for binary input.
  trng(void)
    {
      hwrng_device.open("/dev/hwrng", ios::binary);
    }

  // Destructor - This method will close the file that was opened in the Constructor
  ~trng(void)
    {
      hwrng_device.close();
    }

  // Reads 64-bit value
  unsigned long long random(void)
     {
       unsigned long long tmp;
       hwrng_device.read( reinterpret_cast <char *> (&tmp), sizeof(tmp));
       return (tmp);
     }

  // This function returns a uniformly distributed random number in the range
  // of [0,max).  The added complexity of this function is required to ensure
  // a uniform distribution since the naive modulus max (% max) introduces
  // bias for all values of max that are not powers of two.
  //
  // The loops below are needed, because there is a small and non-uniform 
  // chance that the division below will yield an answer = max.  So we just grab 
  // the next random value until answer < max
  unsigned long long random(unsigned long long max)
     {
       unsigned long long slice;
       unsigned long long retVal;

       retVal = 0xFFFFFFFFFFFFFFFF;
       slice = 0xFFFFFFFFFFFFFFFF / max;
       while (retVal >= max)
	 retVal = random() / slice;
       return(retVal);
     }

  // This method returns and number in the range of [min, max)
  unsigned long long random(unsigned long long min, unsigned long long max)
     {
       unsigned long long tmax;
       unsigned long long retVal;

       tmax = max - min;
       retVal = min + random(tmax);
       return(retVal);
     }

  // This method is a place holder since it is only valid for classes with 
  // real number types
  unsigned long long rnorm(unsigned long long mean, unsigned long long stdDev)
    {
      unsigned long long tmp = 0;

      return(tmp);
    }
};


template <>
class trng <float>
{
  ifstream hwrng_device;
 public:
  // Constructor - This method will open the hardware random number generator on
  // a raspberry pi for binary input.
  trng(void)
    {
      hwrng_device.open("/dev/hwrng", ios::binary);
    }

  // Destructor - This method will close the file that was opened in the Constructor
  ~trng(void)
    {
      hwrng_device.close();
    }

  // This method returns a single precision floating point value in the 
  // range of 0-1.  It assumes that the internal binary representation of
  // the float type is IEEE754 single precision.  It then uses bit operations
  // to take a 23-bit random integer and copies it to the mantissa and sets
  // the exponent and sign to produce a float in the range of [1.0 - 2.0).
  // This range has the unique property of containing 2^23 floats, which are
  // equidistant. and this equidistant property is conserved when the resulting
  // values is scaled and translated.
  float random(void)
     {
       int tmp;
       float retVal;
       hwrng_device.read( reinterpret_cast <char *> (&tmp), sizeof(tmp));
       // Since c++ doesn't allow bit manipulations of floating point types, we are
       // using integer type and arrange its bit patter to follow the IEEE754 bit 
       // pattern for single precision floating point value in the range of 1.0 - 2.0
       tmp = (tmp & 0x007FFFFF) | 0x3F800000;
       // We then copy that binary representation from the temporary integer to the 
       // returned floating point value
       memcpy((void *) &retVal, (void *) &tmp, sizeof(retVal));
       // Subtract 1.0 from the constructed random number to get it in the more useful
       // [0.0, 1.0) range.
       retVal = retVal - 1.0;
       return(retVal);
     }

  // This function returns a uniformly distributed random number in the range
  // of [0,max).  The added complexity of this function is required to ensure
  // a uniform distribution since the naive modulus max (% max) introduces
  // bias for all values of max that are not powers of two.
  //
  // The loops below are needed, because there is a small and non-uniform 
  // chance that the division below will yield an answer = max.  So we just grab 
  // the next random value until answer < max
  float random(float max)
     {
       float retVal;
       retVal = random() * max;
       return(retVal);
     }

  // This method returns and number in the range of [min, max)
  float random(float min, float max)
     {
       float tmax;
       float retVal;

       tmax = max - min;
       retVal = min + random(tmax);
       return(retVal);
     }

  // This method is a place holder since it is only valid for classes with 
  // real number types
  float rnorm(float mean, float stdDev)
    {
      static float spare;
      static float u1;
      static float u2;
      static float s;
      static bool isSpareReady = false;

      if (isSpareReady)
	{
	  isSpareReady = false;
	  return ((spare * stdDev) + mean);
	} else {
	do {
	  u1 = (random() * 2.0) - 1.0;
	  u2 = (random() * 2.0) - 1.0;
	  s = (u1 * u1) + (u2 * u2);
	} while (s >= 1.0);
	s = sqrt(-2.0 * log(s) / s);
	spare = u2 * s;
	isSpareReady = true;
	return(mean + (stdDev * u1 * s));
      }
    }
};


// This specialized template class is for the double type on the Raspberry Pi
template <>
class trng <double>
{
  ifstream hwrng_device;
 public:
  // Constructor - This method will open the hardware random number generator on
  // a raspberry pi for binary input.
  trng(void)
    {
      hwrng_device.open("/dev/hwrng", ios::binary);
    }

  // Destructor - This method will close the file that was opened in the Constructor
  ~trng(void)
    {
      hwrng_device.close();
    }

  // This method returns a double precision floating point value in the 
  // range of 0-1.  It assumes that the internal binary representation of
  // the float type is IEEE754 double precision.  It then uses bit operations
  // to take a 52-bit random integer and copies it to the mantissa and sets
  // the exponent and sign to produce a float in the range of [1.0 - 2.0).
  // This range has the unique property of containing 2^52 floats, which are
  // equidistant. and this equidistant property is conserved when the resulting
  // values is scaled and translated.
  double random(void)
     {
       unsigned long long tmp;
       double retVal;

       hwrng_device.read( reinterpret_cast <char *> (&tmp), sizeof(tmp));

       // Since c++ doesn't allow bit manipulations of floating point types, we are
       // using integer type and arrange its bit patter to follow the IEEE754 bit 
       // pattern for single precision floating point value in the range of 1.0 - 2.0
       tmp = (tmp & 0x000FFFFFFFFFFFFF) | 0x3FF0000000000000;
       // We then copy that binary representation from the temporary integer to the 
       // returned floating point value
       memcpy((void *) &retVal, (void *) &tmp, sizeof(retVal));
       // Subtract 1.0 from the constructed random number to get it in the more useful
       // [0.0, 1.0) range.
       retVal = retVal - 1.0;
       return(retVal);
     }

  // This method returns a number in the range of [0,max)
  double random(double max)
     {
       double retVal;

       retVal = random() * max;
       return(retVal);
     }

  // This method returns and number in the range of [min, max)
  double random(double min, double max)
     {
       double tmax;
       double retVal;

       tmax = max - min;
       retVal = min + random(tmax);
       return(retVal);
     }

  // This function uses the polar method to produce a gausian distribution (normal)
  // random number with the characteristics of having a mean and standard deviation
  // provided.
  double rnorm(double mean, double stdDev)
    {
      static double spare;
      static double u1;
      static double u2;
      static double s;
      static bool isSpareReady = false;

      if (isSpareReady)
	{
	  isSpareReady = false;
	  return ((spare * stdDev) + mean);
	} else {
	do {
	  u1 = (random() * 2.0) - 1.0;
	  u2 = (random() * 2.0) - 1.0;
	  s = (u1 * u1) + (u2 * u2);
	} while (s >= 1.0);
	s = sqrt(-2.0 * log(s) / s);
	spare = u2 * s;
	isSpareReady = true;
	return(mean + (stdDev * u1 * s));
      }
    }
};
#endif
