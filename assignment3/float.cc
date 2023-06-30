#include "float.h"

using namespace std;

// Class constructor
float_t::float_t(void) : data(0) { /* Nothing to do */ }

// Class constructor
float_t::float_t(const float &v) : data(*((unsigned*)&v)) { /* Nothing to do */ }

// Class constructor (private)
float_t::float_t(unsigned d) : data(d) { /* Nothing to do */ }
   
// Class copy constructor
float_t::float_t(const float_t &f) : data(f.data) { /* Nothing to do */ }

// Class destructor
float_t::~float_t(void) { /* Nothing to do */ }

// cout << operator
ostream& operator<<(ostream &os, const float_t &f) { os << *((float*)&f.data); return os; }

// Assignment = operator
float_t& float_t::operator=(const float &v) { data = *((unsigned*)&v); return *this; }

// Assignment = operator
float_t& float_t::operator=(const float_t &f) { data = f.data; return *this; }

// Unary - operator
float_t float_t::operator-(void) { return float_t(data ^ (1 << (exp_bits + frac_bits))); }


/************************************************************
 * EEE3530 Assignment #3: Floating-point numbers            *
 * Implement the floating-point add, subtract, multiply,    *
 * and divide operators below. The functions should perform *
 * bit-level operations to produce the results.             *
 ************************************************************/

// Add + operator
float_t float_t::operator+(const float_t &y) {
    /********************************************************
     * EEE3530 Assignment #3                                *
     * Implement the floating-point add function.           *
     ********************************************************/

    // Extract the sign, exponent, and fraction of x (*this).
    unsigned x_sign = data >> (exp_bits + frac_bits);
    unsigned  x_exp  = (data & exp_mask) >> frac_bits;
    unsigned  x_frac = (data & frac_mask); //| 0x800000;
    int x_type = 0;  // 0,1,2,3 = norm,denorm,nan,inf
    
    // Extract the sign, exponent, and fraction of y (arg).
    unsigned y_sign = y.data >> (exp_bits + frac_bits);
    unsigned  y_exp  = (y.data & exp_mask) >> frac_bits;
    unsigned  y_frac = (y.data & frac_mask);// | 0x800000;
    int y_type = 0;  // 0,1,2,3 = norm,denorm,nan,inf
    
    // declare r and its sign,exp,fraction
    float_t r;
    unsigned r_sign;
    unsigned r_exp;
    unsigned r_frac;

    int expdiff;  // exponential difference

    // classify x and y
    if(x_exp == exp_max && x_frac != 0) x_type = 2; // if x is nan
    else if(x_exp == exp_max && x_frac == 0) x_type = 3; // if x in inf
    else if(x_exp == 0 && x_frac != 0) { // if x is denormalized number
        x_type = 1;
        x_exp = -126;
    }
    else {  // if x is normalized number
        x_type = 0; 
        x_exp -= exp_bias;  // subtract exponent bias
        x_frac |= 0x800000U;
    }

    if(y_exp == exp_max && y_frac != 0) y_type = 2; // if y is nan
    else if(y_exp == exp_max && y_frac == 0) y_type = 3; // if y in inf
    else if(y_exp == 0 && y_frac != 0) { // if y is denormalized number
        y_type = 1;
        y_exp = -126;
    }
    else { // if y is normalized number
        y_type = 0; 
        y_exp -= exp_bias;  // subtract exponent bias
        y_frac |= 0x800000U;
    }
    

    // check if x or y is nan
    if(x_type == 2 || y_type == 2) {
        r_sign = 0;
        r_exp = exp_max;
        r_frac = 1;
	    goto return_result;  // return nan
    }   
    // check if x or y is inf
    else if((x_type == 3) || (y_type == 3)) {
        if(x_type == 3 && y_type == 3) {  // if x and y is inf 
            if(x_sign == y_sign) {  // if (+/-)inf + (+/-)inf
                r_sign = x_sign;
                r_exp = exp_max;
                r_frac = 0;
                goto return_result;  // return (+/-)inf
            }
            else {  // if inf - inf
                r_sign = 0;
                r_exp = exp_max;
                r_frac = 1;
                goto return_result;  // return nan
            }
        }
        else {  // if one of x and y is not inf
            if(x_type == 3) {
                r_sign = x_sign;
                r_exp = exp_max;
                r_frac = 0;
                goto return_result;  // return (+/-)inf
            }
            else {
                r_sign = y_sign;
                r_exp = exp_max;
                r_frac = 0;
                goto return_result;  // return (+/-)inf
            }
        }
    }

    // Execute add operation (denorm,norm cases)
    // Align significands
    expdiff = x_exp - y_exp;  // exponent difference
    if(expdiff > 0) {  // if x_exp > y_exp
        y_exp += expdiff;
        y_frac >>= expdiff;
    }
    else {  // if y_exp > x_exp
        expdiff *= -1;
        x_exp += expdiff;
        x_frac >>= expdiff;
    }

    // exponent of r is equal to exponent of x or y after alignment
    r_exp = x_exp;

    // add significants
    if(x_sign == y_sign) {  // if x+y = (+)+(+) or (-)+(-)
	    r_frac = x_frac + y_frac;
        r_sign = x_sign;
    }
    else {  // x_sign != y_sign
	    if(x_sign) { // x:(-), y:(+)
            if(x_frac <= y_frac) {  // if x + y is positive
                r_frac = y_frac - x_frac;
                r_sign = 0;
            }
            else {  // if x + y is negative
                r_frac = x_frac - y_frac;
                r_sign = 1;
            }
        }
        else { // x:(+), y:(-)
            if(y_frac <= x_frac) {  // if x + y is positive
                r_frac = x_frac - y_frac;
                r_sign = 0;
            }
            else {  // if x + y is negative
                r_frac = y_frac - x_frac;
                r_sign = 1;
            }
        }
    }
    if(r_frac == 0) {  // if result is 0
        r_exp = 0;
        r_sign = 0;
        goto return_result;
    }

    /*if(r_exp < 126U) {  // if result have to be denormalized
        expdiff = -126 - r_exp;
        r_exp += expdiff;
        r_frac >>= 1;
        cout << "\ndenomeaofmosf?????\n";
        goto return_result;
    }*/


    // normalizing the result
    if(r_frac > 0xFFFFFF) {  // if r_frac is bigger or equal than 1
        r_frac >>= 1;
        r_exp++;
    }


    // check for overflow and underflow


    // bias exponent
    r_exp += exp_bias;
    r_frac = r_frac & 0x7FFFFF;  // round the bits

return_result:  // for go statement
    //Put the calculated sign, exponent, and fraction into r.data.
    r.data |= r_sign << 31;
    r.data |= r_exp << 23;
    r.data |= r_frac;
    return r;
}

// Subtract - operator
float_t float_t::operator-(const float_t &y) {
    /********************************************************
     * EEE3530 Assignment #3                                *
     * Implement the floating-point subtract function.      *
     ********************************************************/

    // Put the calculated sign, exponent, and fraction into r.data.

    float_t r;
    unsigned mask = 1U << 31;  // mask for sign bit
    float_t y_neg;  // negated y
    y_neg.data = y.data ^ mask;  // change only the sign bit
    r = *this + y_neg;  // + operation
    return r;
}

// Multiply * operator
float_t float_t::operator*(const float_t &y) {
    /********************************************************
     * EEE3530 Assignment #3                                *
     * Implement the floating-point multiply function.      *
     ********************************************************/
    
    // Extract the sign, exponent, and fraction of x (*this).
    unsigned x_sign = data >> (exp_bits + frac_bits);
    unsigned  x_exp  = (data & exp_mask) >> frac_bits;
    unsigned long  x_frac = (data & frac_mask); 
    int x_type = 0;  // 0,1,2,3 = norm,denorm,nan,inf
    
    // Extract the sign, exponent, and fraction of y (arg).
    unsigned y_sign = y.data >> (exp_bits + frac_bits);
    unsigned  y_exp  = (y.data & exp_mask) >> frac_bits;
    unsigned long  y_frac = (y.data & frac_mask);
    int y_type = 0;  // 0,1,2,3 = norm,denorm,nan,inf
    
    // declare r and its sign,exp,fraction
    float_t r;
    unsigned r_sign;
    unsigned r_exp;
    unsigned long r_frac;
    
    // classify x and y
    if(x_exp == exp_max && x_frac != 0) x_type = 2; // if x is nan
    else if(x_exp == exp_max && x_frac == 0) x_type = 3; // if x in inf
    else if(x_exp == 0 && x_frac != 0) { // if x is denormalized number
        x_type = 1;
        x_exp = -126;
    }
    else if(x_exp == 0 && x_frac == 0);
    else {  // if x is normalized number
        x_type = 0; 
        x_exp -= exp_bias;  // subtract exponent bias
        x_frac |= 0x800000U;
    }

    if(y_exp == exp_max && y_frac != 0) y_type = 2; // if y is nan
    else if(y_exp == exp_max && y_frac == 0) y_type = 3; // if y in inf
    else if(y_exp == 0 && y_frac != 0) { // if y is denormalized number
        y_type = 1;
        y_exp = -126;
    }
    else if(y_exp == 0 && y_frac == 0);
    else { // if y is normalized number
        y_type = 0; 
        y_exp -= exp_bias;  // subtract exponent bias
        y_frac |= 0x800000U;
    }
    
    // check if x or y is nan
    if(x_type == 2 || y_type == 2) {
        r_sign = 0;
        r_exp = exp_max;
        r_frac = 1;
	    goto return_result;  // return nan
    }   
    // check if x or y is inf
    else if((x_type == 3) || (y_type == 3)) {
        if(x_type == 3 && y_type == 3) {  // if x and y is inf 
            if(x_sign == y_sign) {  // if (+/-)inf + (+/-)inf
                r_sign = x_sign;
                r_exp = exp_max;
                r_frac = 0;
                goto return_result;  // return (+/-)inf
            }
            else {  // if inf - inf
                r_sign = 0;
                r_exp = exp_max;
                r_frac = 1;
                goto return_result;  // return nan
            }
        }
        else {  // if one of x and y is not inf
            if(x_type == 3) {
                r_sign = x_sign;
                r_exp = exp_max;
                r_frac = 0;
                goto return_result;  // return (+/-)inf
            }
            else {
                r_sign = y_sign;
                r_exp = exp_max;
                r_frac = 0;
                goto return_result;  // return (+/-)inf
            }
        }
    }

    // Multiply Operation
    r_exp = x_exp + y_exp;
    r_sign = x_sign ^ y_sign;
    r_frac = x_frac * y_frac;
    if(r_frac == 0) {  // if result is 0
        r_exp = 0;
        r_sign = 0;
        goto return_result;
    }
/*
    if(r_exp < -126U) {  // if result have to be denormalized
        expdiff = -126 - r_exp;
        r_exp += expdiff;
        r_frac >>= 1;
        cout << "\ndenormalize??????\n";
        goto return_result;
    }
*/
    
    if(r_exp < -126U && r_frac < 1U) {
        r_exp = 0;
        r_frac = 0;
        goto return_result;
    }

    // normalizing the result
    if(r_frac & (1ULL << 47)) {  // if r_frac is bigger or equal than 1
        r_frac >>= 1;
        r_exp++;
    }

    r_frac >>= 23;
    r_exp += exp_bias;
    r_frac = r_frac & 0x7FFFFFU;

return_result:  // for go statement
    //Put the calculated sign, exponent, and fraction into r.data.
    r.data |= r_sign << 31;
    r.data |= r_exp << 23;
    r.data |= r_frac;
    return r;
}

// Divide / operator
float_t float_t::operator/(const float_t &y) {
    /********************************************************
     * EEE3530 Assignment #3                                *
     * Implement the floating-point divide function.        *
     ********************************************************/

    float_t r;
    // if y is 0 return inf
    if(y.data == 0) {
        return (0x7F800000U);
    }

    // if y is inf return 0
    if(y.data == 0xFF800000U || y.data == 0x7F800000U) {
        return 0x0U;
    }

    float_t y_flip;
    y_flip.data = y.data;//0x7F800000U - y.data;
    r = *this * y_flip;

    return r;
}
