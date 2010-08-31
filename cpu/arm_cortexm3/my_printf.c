#include <stdarg.h>

#include "my_uart.h"

/* Default digits number after radix for 'f' conversion*/
#define DEF_FLT_PRECISION 6 
#define DEF_OXD_PRECISION 1 /* precision for 'o','d' and 'x' conversion */

/*
    Purpose:
	Convert positive double value to string of SIGNIFICAND_DIGITS-1 digits
	or less number of digit. It's depended on given precision.
	String start at str[1]. str[0] reserved to carry when round.
	By default str[0]='0', if there is a cary to the most significand
	digits that cause overflow str[0]  set to 1.
    
    Arguments:
	x - number to convert;
	str - pointer to buffer of SIGNIFICAND_DIGITS chars;
	prec - precision;
    Returns:
	the number of digits in integer part of str.
	-1 if infinity.
	-2 if nan.
    Important: A caller function must clear buffer pointed by str. 
*/

#define SIGNIFICAND_DIGITS 9 /* with rounding result. check buffer size */
#define INF 1./0. /* infinity */

/*
    Purpose:
	This is an internal function;
	Convert unsigned integer value of type long to string in reverse oder.
	if value =0 no conversion, and return 0;
    Arguments:
	x - number to convert;
	base - base of convert must be one of the number 8, 10, 16;
	str - pointer to result (in reverse order!);
    Returns:
	the number of digits in str.
*/
static int int2str(unsigned long x, char base, char upper, char *str)
{
    char digit;
    int i; /* counter */
    
    for (i=0; x; i++) {
	if (8 == base) {
	    digit = x & 7;
	    x >>= 3;
	} else if (16 == base) {
	    digit = x & 15;
	    x >>= 4;
	} else {
	    digit = x % 10;
	    x /= 10;
	}
	
	if (digit < 10) {
	    str[i] = '0'+digit;
	} else {
	    str[i] = (upper?'A':'a')-10+digit;
	}
    }
    
    return i;
} /* <-- int int2str( */

/*
    Purpose:
	This function prints char 'c' by specified callback (as in
	vnprintf_base) 'iter' times and controls overall length of string.
    Arguments:
	pact_len - a pointer to actual length of string;
	len - a maximum length of string;
	put - pointer to a callback function (see vnprintf_base);
	data - parameter of a callback functon;
	iter - see Purpose;
	c - see Purpose;
*/
static void put_base(unsigned int *pact_len, unsigned int len,
	void (*put)(char c, void *data), void *data, char iter, char c)
{
    for (; 0 < iter; iter--) {
	if (!len || *pact_len < len) {
	    put(c, data);
	    (*pact_len)++;
	} else {
	    break;
	}
    }    
} /* static void put_base( */

static unsigned long strtoul(const char *nptr, char **endptr, int radix)
{
    char neg_sign = 0;
    unsigned long res;
    unsigned long res_; /* for overflow test */
    char digit;
    char no_digits = 1;
    
    /* 00) Store nptr */
    if (0 != endptr) {
	*endptr = (char*)nptr;
    }
    
    /* 0) check base */
    if (36 < radix || 0 > radix || 1 == radix) {
	return -1;
    }	
    
    /* 1) Skip white space. */
    while ('\t' == (*nptr) || ' ' == (*nptr)) {
	++nptr;
    }
    
    /* 2) check for a sign.*/
    if ('+' == (*nptr)) {
	nptr++;
    }
    if ('-' == (*nptr)) {
	neg_sign = 1;
	nptr++;
    }
    
    /* 3)Recognize number prefix */
    if ('0' == (*nptr)) {
	if ((0 == radix || 16 == radix)&&('X' == (nptr[1]) ||
			'x' == (nptr[1]))) {
	    radix = 16;
	    /* Store new reference point */
	    if (0 != endptr) {
		*endptr = (char*)nptr + 1;
	    }
	    nptr += 2;
	} else if (0 == radix) {
	    radix = 8;
	}
    } else if (0 == radix) {
	radix = 10;
    }

    /* 4) convert */
    res = res_ = 0;
    for (;;nptr++) {
	/* get next digit */
	if ('0' <= (*nptr) && '9' >= (*nptr)) {
	    digit = (*nptr) - '0';
	} else if ('A' <= (*nptr) && 'Z' >= (*nptr)) {
	    digit = (*nptr) - 'A'+10;
	} else if ('a' <= (*nptr) && 'z' >= (*nptr)) {
	    digit = (*nptr) - 'a'+10;
	} else {
	    digit = 36;
	}
	
	/* check digit and radix */
	if (digit >= radix) {
	    if ((0 != endptr) && (!no_digits)) {
		*endptr = (char*)nptr;
	    }
	    return neg_sign ? -res : res;
	} else {
	    if (no_digits) {
		no_digits = 0;
	    }
	}
	
	/* calculate */
	res_ = res * radix + digit;
	
	/* check overflow */
	if ((res_ - digit)/radix != res) {
	    return -1;
	} else {
	    res = res_;
	}
	
    } /* for (;;nptr++) { */

} /* unsigned long strtoul */

/*
    States of statechart of processing format string.
    After processing % pattern, state must equal to one of the supported 
    conversion specifier: c, o, d, x, f, p.
*/
#define AFTER_COMMON	0	/* preceding symbol was processed */
#define AFTER_PERCENT	1	/* preceding symbol is the '%' */
#define AFTER_DOT	2	/* preceding symbol is the '.' */

static int vnprintf_base(unsigned int len, void (*put)(char c, void *data),
	void *data, const char *format, va_list ap)
{
    unsigned int index;	/* position in string */
    unsigned int act_len = 0; /* actualy length of an output */
    
    char state = AFTER_COMMON; /* State of processing format string. */
    
    char alt;		/* alternate form flag */
    char zero_pad;	/* 'The value should be zero padded.' flag */
    char left_align;	/* align to the left */
    char width;		/* The value of 'a minimum field width' */
    char prec;		/* 'precision'. if <0 then wasn't specified */
    
    /* length modifier ('hh','h','l') */
    enum {NONE_MOD, HH_MOD, H_MOD, L_MOD} len_mod = NONE_MOD;
    
    short int si;	/* common iterator */
    char *ptr;		/* needed by strtoul */
    char buffer[11];	/* buffer for conversion and round */
    unsigned long l;	/* temporary store */
    char *s;		/* temporary store */
    char neg;		/* negative number */
    
    alt = zero_pad = width = prec = left_align = -1; /* disable warning */
    
    /* process string */
    for (index = 0; (format[index]) && (!len || act_len < len); index++) {
	
	switch (state) { /* switch on state of line processing */
	case AFTER_COMMON:
	    if ('%' == (format[index])) {
		alt = 0;
		zero_pad = 0;
		left_align = 0;
		width = 0;
		len_mod = NONE_MOD;
		prec = -1;
		state = AFTER_PERCENT;
	    } else {
		put_base(&act_len, len, put, data, 1, (format[index]));
		/* stay in AFTER_COMMON state */
	    }
	    continue; /* process next character */
	    
	case AFTER_DOT:
	    /* precision field */
	    prec = strtoul((char*)&format[index], &ptr, 10);
	    index = ptr - format -1;	    
	    state = AFTER_PERCENT;
	    continue; /* process next character */
	    
	case AFTER_PERCENT:
	    if ('#' == (format[index])) {
		alt = 1;
		/* stay in AFTER_PERCENT state */
	    } else if ('0' == (format[index])) {
		zero_pad = 1;
		/* stay in AFTER_PERCENT state */
	    } else if ('.' == (format[index])) {
		state = AFTER_DOT;
	    } else if ('-' == (format[index])) {
		left_align = 1;
		/* stay in AFTER_PERCENT state */
	    } else if ('0' < (format[index]) && '9' >= (format[index])) {
		/* width field */
		width = strtoul((char*)&format[index], &ptr, 10);
		index = ptr - format - 1;
		/* stay in AFTER_PERCENT state */
	    } else if ('l' == (format[index])) { /*check for length modifiers*/
		len_mod = L_MOD;
		/* stay in AFTER_PERCENT state */
	    } else if ('h' == (format[index])) {
		if ('h' == (format[index+1])) {
		    len_mod = HH_MOD;
		    index++;
		    /* stay in AFTER_PERCENT state */
		} else {
		    len_mod = H_MOD;
		    /* stay in AFTER_PERCENT state */
		}
	    } else if ('c' == (format[index]) || 'o' == (format[index])
		    || 'd' == (format[index]) || 'x' == (format[index])
		    || 'i' == (format[index]) || 'X' == (format[index])
		    || 'f' == (format[index]) || 'p' == (format[index])
		    || 's' == (format[index])) {
		/* checked for a conversion spec */
		state = (format[index]);
		break; /* go to conversions switch */
	    } else { /* unknown character */
		put_base(&act_len, len, put, data, 1, (format[index]));
		state = AFTER_COMMON;
	    }
	    continue; /* process next character */
	} /* switch (state) { 'switch on state of line processing' */
	
	switch (state) { /* switch on conversions type */
	case 'c':
	    if (!left_align)
		put_base(&act_len, len, put, data, width - 1, ' ');
	    /* L_MOD ignored.`char' and `short int' is promoted to `int'. */
	    put_base(&act_len, len, put, data, 1,
		    (unsigned char) va_arg(ap, int));
	    if (left_align)
		put_base(&act_len, len, put, data, width - 1, ' ');
	    break;
	case 'o':
	    /*
		Unsigned argument should be used with 'o' and 'x' conversion.
		But if signed  arg passed we must truncate number to
		appropriate sizeof.
	    */
	    if (L_MOD == len_mod) {
		l = va_arg (ap, unsigned long int);
	    } else if (HH_MOD == len_mod) {
		l = (unsigned char) va_arg (ap, unsigned int);
	    } else  if (H_MOD == len_mod) {
		l = (unsigned short) va_arg (ap, unsigned int);
	    } else {
		l = va_arg (ap, unsigned int);
	    }
	    /* converse */
	    si = int2str(l, 8, 0, buffer);
	    
	    if (0>prec) {
		/* set default precision for o and x conversion */
		prec = DEF_OXD_PRECISION;
	    } else if (zero_pad) {
		zero_pad=0; /* If a precision is given, '0' flag is ignored */
	    }
	    
	    /* calculate output parameters */
	    if (prec > si) {
		alt = 0; /* ignore '#' */
		width -= prec;
		prec -= si;
	    } else {
		prec = 0;
		if (zero_pad && width > si) {
		    alt = 0; /* ignore '#' */
		    width -= si;
		} else {
		    width -= si+((alt)?1:0);
		}
	    }
	    
	    /* Output */
	    /* maintain width */
	    if (!left_align)
		put_base(&act_len, len, put, data, width, zero_pad ? '0' : ' ');
	    
	    if (alt) {
		put_base(&act_len, len, put, data, 1, '0');
	    }
	    
	    put_base(&act_len, len, put, data, prec, '0');
	    
	    for (; 0 < si; si--) {
		put_base(&act_len, len, put, data, 1, buffer[si-1]);
	    }
	    if (left_align)
		put_base(&act_len, len, put, data, width, ' ');
	    break;
	case 's':
	    s = va_arg(ap, char *);
	    for (si = 0; s[si] != 0; ) {
		si++;
		if (width > 0 && --width == 0)
		    break;
	    }
	    if (!left_align)
		put_base(&act_len, len, put, data, width, ' ');
	    while (--si >= 0) {
		put_base(&act_len, len, put, data, 1, *s);
		++s;
	    }
	    if (left_align)
		put_base(&act_len, len, put, data, width, ' ');
	    break;
	case 'x':
	case 'X':
	    /*
		Unsigned argument should be used with 'o' and 'x' conversion.
		But if signed  arg passed we must truncate number to
		appropriate sizeof.
	    */
	    if (L_MOD == len_mod) {
		l = va_arg (ap, unsigned long int);
	    } else if (HH_MOD == len_mod) {
		l = (unsigned char) va_arg (ap, unsigned int);
	    } else  if (H_MOD == len_mod) {
		l = (unsigned short) va_arg (ap, unsigned int);
	    } else {
		l = va_arg (ap, unsigned int);
	    }
	    
	    /* converse */
	    si = int2str(l, 16, (state == 'X'), buffer);
	
	    if (0>prec) {
		/* set default precision for o and x conversion */
		prec = DEF_OXD_PRECISION;
		if (zero_pad && width) {
		    /* in this case width pads after '0x' prefix */
		    prec = width - (alt?2:0);
		    width = 0;
		}
	    } else if (zero_pad) {
		zero_pad=0; /* If a precision is given, '0' flag is ignored */
	    }
	    
	    /* calculate output parameters */
	    if (!si && !prec) {
		alt = 0;
	    }
	    
	    if (prec > si) {
		width -= prec +(alt?2:0);
		prec -= si;
	    } else {
		prec = 0;
		width -= si +(alt?2:0);
	    }
	    
	    /* Output */
	    /* maintain width */
	    if (!left_align)
		put_base(&act_len, len, put, data, width, zero_pad ? '0' : ' ');
	    
	    if (alt) {
		put_base(&act_len, len, put, data, 1, '0');
		put_base(&act_len, len, put, data, 1, 'x');
	    }
	    
	    put_base(&act_len, len, put, data, prec, '0');
	    
	    for (; 0 < si; si--) {
		put_base(&act_len, len, put, data, 1, buffer[si-1]);
	    }
	    if (left_align)
		put_base(&act_len, len, put, data, width, ' ');
	    break;
	case 'd':
	case 'i':
	    /* get signed argument */
	    if (L_MOD == len_mod) {
		l = va_arg (ap, long int);
	    } else {  /* `char' and `short int' will be promoted to `int'. */
		l = va_arg (ap, int);
	    }
	    
	    /* get sign */
	    neg = (0 > (long)l)? 1 : 0;
	    
	    /* converse */
	    if (neg) {
		si = int2str(-(long)l, 10, 0, buffer);
	    } else {
		si = int2str(l, 10, 0, buffer);
	    }
	
	    if (0>prec) {
		/* set default precision for 'o', 'x' and 'd' conversion */
		prec = DEF_OXD_PRECISION;
		if (zero_pad && width) {
		    /* in this case width pads after sign */
		    prec = width - neg;
		    width = 0;
		}
	    } else if (zero_pad) {
		zero_pad=0; /* If a precision is given, '0' flag is ignored */
	    }
	    
	    /* calculate output parameters */
	    if (prec > si) {
		width -= prec+neg;
		prec -= si;
	    } else {
		prec = 0;
		width -= si+neg;
	    }
	    
	    /* Output */
	    /* maintain width */
	    if (!left_align)
		put_base(&act_len, len, put, data, width, zero_pad ? '0' : ' ');
	    
	    if (neg) {
		put_base(&act_len, len, put, data, 1, '-');
	    }
	    
	    put_base(&act_len, len, put, data, prec, '0');
	    
	    for (; 0 < si; si--) {
		put_base(&act_len, len, put, data, 1, buffer[si-1]);
	    }
	    if (left_align)
		put_base(&act_len, len, put, data, width, ' ');
	    break;	    
	case 'p':
	    /*
		The void* pointer argument is printed in hexadecimal
		(as if by %#x or %#lx).
	    */
	    alt = 1;
	    zero_pad = 0;
	    width = 0;
	    len_mod = L_MOD;
	    prec = -1;
	    /* process this pattern in next turn */
	    state = 'x';
	    index--;
	    continue;
    } /* switch (state) { 'switch on conversions type'*/
    
    state = AFTER_COMMON; /* % pattern completly processed */
    
    } /* <-- for ( */

    return act_len;
} /* <-- int vnprintf_base( */
 
static void put_debug(char c, void *data)
{
    my_uart_putc(c);
}

static int strlen(const char *s)
{
    int i;
    for (i = 0; i < 1000 && s[i]; i++)
	;
    return i;
}

int my_puts(const char *s)
{
    int i;
    for (i = 0; s[i]; i++) {
	my_uart_putc(s[i]);
    }
    my_uart_putc('\n');
    return strlen(s);
}

int my_printf(const char *fmt, ...)
{
    va_list ap;
    unsigned int n ;
    
    va_start(ap, fmt); /* initialize list of arguments */
    
    n=vnprintf_base(0, put_debug, 0, fmt, ap);
    
    va_end(ap);/* finalize list of arguments */
    return n;
}

int my_putchar(int c)
{
    my_uart_putc(c);
    return 1;
}
