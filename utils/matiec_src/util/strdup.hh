/*
 *  matiec - a compiler for the programming languages defined in IEC 61131-3
 *
 *  Copyright (C) 2011  Mario de Sousa (msousa@fe.up.pt)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */


/*******************************************/
/* String Duplication Utility Functions... */
/*******************************************/



#ifndef STRDUP__H
#define STRDUP__H

#include <stdlib.h>   /* required for malloc() */
#include <string.h>   /* required for strcat() & strlen() */

/*
 * Join two strings together. Allocate space with malloc(3).
 */
static char *strdup2(const char *a, const char *b) {
  char *res = (char *)malloc(strlen(a) + strlen(b) + 1);

  if (res == NULL) return NULL;
  return strcat(strcpy(res, a), b);
}

/*
 * Join three strings together. Allocate space with malloc(3).
 */
static char *strdup3(const char *a, const char *b, const char *c) {
  char *res = (char *)malloc(strlen(a) + strlen(b) + strlen(c) + 1);

  if (res == NULL) return NULL;
  return strcat(strcat(strcpy(res, a), b), c);
}


#endif /* STRDUP__H */
